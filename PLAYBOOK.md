# How to do this yourself — a step-by-step playbook

> This is the exact method used to take Synaptics `06cb:00fd` from "no known fix" to a
> working fingerprint sensor (enroll + boot login + lock screen + sudo) on Ubuntu/Regolith.
> The *skills* generalise to almost any "some hardware/software doesn't work on Linux" task.

The whole thing is **one loop**, repeated:

> **run it → read the exact error → find where that error comes from → implement the missing
> piece → rebuild → repeat.** Then, once it *works*, **wire it into the system** (udev, PAM,
> systemd) so it survives reboots and is actually usable.

---

## 0. Mindset (the important part)

- **Never guess — make the program tell you what it wants.** Enable debug modes, read the
  real error string, grep the source for it.
- **Work in small commits.** Each fix is one commit, so you can always `git log`/revert.
- **Leave the system safe.** If a change can break something (PAM, boot), test and have a
  rollback ready.
- **Prior art first.** Someone has usually been 90% of the way; find them before writing code.

---

## 1. Identify the hardware (5 minutes)

```bash
lsusb                       # find the fingerprint reader's vendor:product
# e.g. "ID 06cb:00fd Synaptics, Inc."
lspci -nnk | grep -iA3 -E 'vga|3d|display'   # GPU, for the gaming/graphics side
```

Write down the USB ID (`06cb:00fd`). That hex pair is your search key for everything.

## 2. Check what's supposed to support it

```bash
# What does upstream libfprint support?
xdg-open https://fprint.freedesktop.org/supported-devices.html
# Community catalogue (by laptop / by USB id)
xdg-open https://github.com/jedbillyb/linux-fingerprint-drivers
```

If your ID is in the "unsupported / no known fix" list: you're about to change that.

## 3. Find prior art (the highest-leverage step)

Search the **exact ID** and its *siblings* (`06cb:00be`, `06cb:00c9`, `06cb:00e7`,
`06cb:00ff`, `27c6:...` for Goodix, etc.):

- GitHub search: `<id> linux`, `<vendor> fingerprint libfprint`
- Issues in `fprintd`, `libfprint`, and the project you find
- Reddit / forums ("no known fix" threads show the *symptom* others hit)

For Synaptics "Tudor" sensors this led to **synaTudor** — a project that *relinks*
Synaptics' own Windows driver DLLs at runtime and exposes them via `libfprint-tod`.

> Key concept: **fingerprint drivers live in userspace (`libfprint`), not the kernel.**
> So "making it work" = getting a `libfprint` driver, either open (clean-room) or a relinker.

## 4. Set up the toolchain

```bash
sudo apt update
sudo apt install -y git build-essential meson ninja-build pkg-config \
    innoextract gdb strace libglib2.0-dev libusb-1.0-0-dev libssl-dev \
    libcap-dev libseccomp-dev libdbus-1-dev libsystemd-dev libudev-dev \
    libjson-glib-dev libgusb-dev libfprint-2-dev libfprint-2-tod-dev
```

Clone upstream **and** the fork that already targets your sensor (so you start from the
closest state):

```bash
git clone -b relink https://github.com/Popax21/synaTudor.git        synaTudor-up
git clone -b relink https://github.com/<00fd-fork>/synaTudor.git    synaTudor
cd synaTudor
diff -rq . ../synaTudor-up | grep -v '\.git/'    # see what your fork changed
```

## 5. Build and run the harness

```bash
meson setup build -DUNMOUNTFS=true          # UNMOUNTFS=false makes gdb work
ninja -C build
sudo ninja -C build install                 # TODO module -> /usr/lib/.../libfprint-2/tod-1/
sudo udevadm control --reload && sudo udevadm trigger
sudo systemctl restart fprintd
```

The project ships a debug CLI. Run it against *your* PID:

```bash
printf 'y\nq\n' > /tmp/yes.txt
sudo /sbin/tudor/tudor_cli /tmp/store -P00fd < /tmp/yes.txt
```

## 6. THE LOOP — diagnose → implement

### 6a. Turn on the debug stubs so it names the missing piece

```bash
meson configure build -DDBGWDF=true -DDBGIMPORT=true
ninja -C build && sudo ninja -C build install
# run the CLI -> it now prints e.g.:
#   [ERR] Unresolved WDF function 50 called!
#   [ERR] Unresolved import CreateWindowExW@USER32.dll called!
```

- `-DDBGWDF` → prints the **WDF function-table index** that's missing.
- `-DDBGIMPORT` → prints the missing **Win32 import** (name@dll).

### 6b. Map a WDF index to a name/signature

Grab Microsoft's `wdffuncenum.h` (the ordering of the function table):

```bash
curl -fsSL -o wdffuncenum.h \
  https://raw.githubusercontent.com/microsoft/Windows-Driver-Frameworks/main/src/publicinc/wdf/umdf/2.25/wdffuncenum.h
grep -n 'TableIndex = 50,' wdffuncenum.h     # -> WdfDeviceAssignInterfaceProperty
```

### 6c. Implement it

**WDF functions** are registered by index in `libtudor/src/winapi/wdf/*.c`:

```c
__winfnc NTSTATUS WdfDeviceAssignInterfaceProperty(
        WDF_DRIVER_GLOBALS *globals, WDFOBJECT dev, WDF_DEVICE_INTERFACE_PROPERTY_DATA *pd,
        DEVPROPTYPE type, ULONG len, void *buf) {
    /* ...store the property... */
    return STATUS_SUCCESS;
}
WDFFUNC(WdfDeviceAssignInterfaceProperty, 50)   /* <-- registers index 50 */
```

**Win32 imports** are registered by name in `libtudor/src/winapi/*.c`:

```c
__winfnc unsigned short RegisterClassExW(const void *wcx) { (void)wcx; return 1; }
WINAPI(RegisterClassExW)                          /* <-- registers by name */
```

### 6d. Rebuild, rerun, repeat

```bash
ninja -C build && sudo ninja -C build install
sudo systemctl restart tudor-host-launcher
sudo /sbin/tudor/tudor_cli /tmp/store -P00fd < /tmp/yes.txt
# read the NEXT error, implement, repeat.
```

Commit after each green step:

```bash
git add -A && git commit -m "feat: implement WdfDeviceAssignInterfaceProperty (index 50)"
```

## 7. When it crashes (segfault/abort) instead of printing a number

```bash
dmesg | tail                              # kernel says where/an address
journalctl -b -u fprintd -u tudor-host-launcher | tail
```

Get a backtrace with gdb (needs `-DUNMOUNTFS=false`):

```bash
sudo gdb -batch -ex 'run /tmp/store -P00fd < /tmp/yes.txt' -ex 'bt' --args /sbin/tudor/tudor_cli
# -> e.g. "#2 GetModuleHandleW ... #1 winmodule_find(name=0x0)  strcmp(NULL)"
```

Then read the function and fix the *actual* bug (`strcmp(NULL)` → return current module
when name is NULL). That's it — the tools point straight at the line.

## 8. "Plays but silent" / data-path bugs → use `strace` + `dmesg`

Symptom: enrollment hangs, or `usbfs: process ... did not claim interface 0 before use`.

```bash
dmesg | grep -iE 'usb|usbfs|3-7'
sudo strace -f -e trace=ioctl -o /tmp/st.out /sbin/tudor/tudor_cli /tmp/store -P00fd
grep -c USBDEVFS_SUBMITURB /tmp/st.out     # are USB transfers even happening?
```

Fix: do the missing step (e.g. `libusb_claim_interface()` before transfers).

## 9. Size/limit bugs → read the host log, raise the right constant

Symptom: `Enrollment record size exceeding maximum size: 198680 > 98304`.

```bash
grep -rn "exceeding maximum size" .        # -> tudor-host/src/handler.c uses IPC_MAX_RECORD_SIZE
grep -rn "IPC_MAX_RECORD_SIZE" . | grep define   # -> the #define to raise
```

The v139 driver just makes *bigger* records than the older drivers; bump the cap.

## 10. Now make it *usable* (system integration)

A working device that dies at boot/reboot is not "done". Wire it in:

### 10a. udev + id table
- Add your USB ID to the libfprint-tod module's id table and the udev rule
  (`libfprint-tod/*.rules`), so libfprint recognises and tags the device.

### 10b. Enrollment
```bash
fprintd-list "$USER"        # -> found 1 devices
fprintd-enroll              # touch the sensor ~8-10x, centred & flat
```

### 10c. Login + lock screen + sudo (PAM)
```bash
sudo pam-auth-update --enable fprintd      # adds pam_fprintd to /etc/pam.d/common-auth
```
`gtklock`/`login`/`gdm` include `common-auth`, so this gives you fingerprint on the lock
screen, the login screen and `sudo` — with password fallback.

### 10d. Boot race (device not ready when the login screen appears)
The driver needs ~4 s to load. Pre-warm it before the display manager:
```bash
sudo install -m0644 contrib/tudor-warm.service /etc/systemd/system/
sudo systemctl daemon-reload && sudo systemctl enable --now tudor-warm.service
```

### 10e. Two-clients race ("works once, then dies")
If the host aborts on *reuse* ("Attempted multiple different sensor pairing data loads"),
that's a bug in the host's guard — make it abort only for a *different* sensor, not the
same one.

## 11. Debugging "not this project" problems (transferable)

When a *voice call* only failed between two specific machines, the method was identical:
**isolate the variable.**

```bash
# does it work with a third party?        -> PC1 -> friend: WORKS
# does it work with a different app?      -> Meet/WhatsApp: WORKS  => not the network
# what is different about the failing case? -> both peers on Tailscale
sudo tailscale down     # test: call works  => CONFIRMED cause
```

Then inspect the *mechanics*:
```bash
pactl info; pactl list short sinks sources sink-inputs   # audio routing (was iec958 vs analog)
ss -uapn | grep vesktop                                   # which interfaces the app binds
```

Fix: pin the app to the right interface/profile
(`--force-webrtc-ip-handling-policy=default_public_interface_only`,
`pactl set-card-profile <card> output:analog-stereo+...`), and make it permanent with a
launcher/desktop-entry override.

## 12. Habits that make this fast

1. `grep -rn "the exact error text"` in the source — it always leads to the code.
2. Prefer *making the tool talk* (debug flags, logs, gdb, strace) over reading lots of code.
3. Change **one** thing, rebuild, retest.
4. Commit each win; note the exact command that produced each error.
5. When stuck, look for **siblings** (nearby USB IDs, similar projects) — solutions usually
   generalise.
6. Always leave a **rollback path** (`sudo ninja -C build uninstall`, disable services).

## 13. One-page cheat sheet

```bash
# identify
lsusb; lspci -nnk
# build
meson setup build -DUNMOUNTFS=true && ninja -C build && sudo ninja -C build install
# debug build
meson configure build -DDBGWDF=true -DDBGIMPORT=true
# run harness
sudo /sbin/tudor/tudor_cli /tmp/store -P00fd < /tmp/yes.txt
# backtrace
sudo gdb -batch -ex 'run /tmp/store -P00fd < /tmp/yes.txt' -ex 'bt' --args /sbin/tudor/tudor_cli
# usb io
dmesg | grep -i usb; sudo strace -f -e trace=ioctl -o /tmp/st.out <program>
# find code from an error
grep -rn "<error text>" .
# system integration
sudo udevadm control --reload && sudo udevadm trigger
sudo pam-auth-update --enable fprintd
sudo systemctl enable --now tudor-warm.service
```

That's the entire craft: **make it tell you what's broken, implement the missing piece,
repeat, then integrate it so it survives life.**
