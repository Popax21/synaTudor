# STATUS: WORKING — Synaptics `06cb:00fd` on Linux

Verified end-to-end on a **Lenovo IdeaPad 3 14ITL6** — Ubuntu 26.04 (Resolute),
Regolith/sway + GDM, fprintd 1.94.5, libfprint 1.95.1+tod1, using the IdeaPad
consumer driver package (Synaptics DLLs **v139**).

Working today: **enrollment**, **boot/login**, **lock/suspend screen (gtklock)** and **sudo**.

## Proof

The relinked driver reaches its interactive menu and the enrollment stage:

```
[INF] Found sensor USB device [bus 3 addr 4 vid 0x06cb pid 0x00fd]
[INF] Loaded driver DLL 'synaFpAdapter139.dll' [192496 bytes]
[INF] Loaded driver DLL 'synaWudfBioUsb139.dll' [1635320 bytes]
[INF] Opening tudor device...
[INF] Loading device records...

Commands:
  e - enroll finger
  v - verify finger
  i - identify finger
  q - query information about enrolled fingers
  w - wipe enrolled finger(s)
  s - shutdown driver
> e
Enter identity index: test
Enter finger index (1-5 = right hand ...): 2
Put your finger on the sensor          <- enrollment ready
```

fprintd/libfprint see it, and enrollment is stored:

```
$ fprintd-list $USER
found 1 devices
Device at /net/reactivated/Fprint/Device/0

$ ls /var/lib/fprint/$USER/syna_tudor_relink/99C1E62F6F950000/
2   ...            # enrolled fingers
```

## Complete list of fixes

### Driver bring-up (emulation walls)

1. **WDF index 50 `WdfDeviceAssignInterfaceProperty`** (+ **51** `WdfDeviceAllocAndQueryInterfaceProperty`, **52** `WdfDeviceQueryInterfaceProperty`) — implemented as a keyed (interface GUID + DEVPROPKEY) property store. This is the wall that blocked every prior attempt (see issue #20).
2. **WDF index 33 `WdfDeviceSetPnpCapabilities`** — safe no-op stub.
3. **`GetModuleHandleW(NULL)` / `GetModuleHandleExW(FROM_ADDRESS)`** — the driver calls these; the relinker dereferenced NULL (`strcmp(NULL)` → SIGSEGV). Now returns the current module.
4. **USER32 imports** — `RegisterClassExW`, `RegisterClassW`, `UnregisterClassW`, `CreateWindowExW`, `DestroyWindow`, `DefWindowProcW`, `GetMessageW`, `PeekMessageW`, `TranslateMessage`, `DispatchMessageW`, `PostQuitMessage`, `RegisterDeviceNotificationW`, `UnregisterDeviceNotification`.
5. **Power/session** — `RegisterPowerSettingNotification`, `UnregisterPowerSettingNotification`, `RegisterSuspendResumeNotification`, `UnregisterSuspendResumeNotification`, `PowerSettingRegisterNotification`, `PowerSettingUnregisterNotification`.
6. **WTSAPI32** — `WTSRegisterSessionNotification`, `WTSUnRegisterSessionNotification`, `WTSQuerySessionInformationW/A`, `WTSFreeMemory`.
7. **ADVAPI32** — `RegQueryInfoKeyW` / `RegQueryInfoKeyA` (empty-key semantics).

### Data path

8. **USB interface not claimed** (`usbfs: process ... did not claim interface 0 before use` → capture fails / hangs). Fixed by claiming all interfaces up-front in `WdfUsbTargetDeviceCreate`.

### Size limits

9. **`Enrollment record size exceeding maximum size: 198680 > 98304`** — the v139 driver builds larger (~194 KB) enrollment records than the older drivers. Raised `IPC_MAX_RECORD_SIZE` (96 KB → 512 KB) and `IPC_MAX_MESSAGE_SIZE` (128 KB → 1 MB).

### Host reuse (the "works once, then stops" bug)

10. **`Attempted multiple different sensor pairing data loads!`** — the host aborted when the **same** sensor's pairing data was loaded twice, which happens the moment the lingering host is **reused** for a later login (boot warm-up → GDM, first login → next login, lock → unlock). Changed the guard to abort only for a **different** sensor.

### System integration (`contrib/`)

11. **Device plumbing** — `06cb:00fd` added to the libfprint-tod id table + udev rule; driver DLL names bumped to v139.
12. **Boot warm-up** (`contrib/tudor-warm.service`) — the driver needs ~4 s to load its DLLs and pair; this pre-warms the device at boot, before the display manager, so login/lock find it ready.
13. **Login / lock-screen / sudo** (`contrib/enable-fingerprint-login.sh`) — enables the `fprintd` PAM profile (`pam_fprintd` in `common-auth`), which gtklock / login / gdm inherit.

## Files changed in the fork

- `libtudor/src/winapi/wdf/device.c` — WDF 33/50/51/52; `06cb:00fd` id table; v139 DLL names.
- `libtudor/src/winapi/module.c` — `GetModuleHandle` fix + USER32/power/WTS stubs.
- `libtudor/src/winapi/reg.c` — `RegQueryInfoKey`.
- `libtudor/src/winapi/wdf/usb.c` — claim interfaces up-front.
- `tudor-host/src/main.c` — same-sensor pairing-data reload fix.
- `tudor-host/inc/tudor/libfprint-proto.h` — raised record/message size caps.
- `libfprint-tod/60-tudor-libfprint-tod.rules` — added `06cb:00fd`.
- `libtudor/meson.build`, `libtudor/download_driver.sh`, `libtudor/installer.sha` — IdeaPad driver package, build deps, flags.
- Docs/tools: `REVERSE_ENGINEERING_HANDBOOK.md`, `ANNOUNCEMENT.md`, `contrib/`, `tools/tudor-probe.sh`, `tools/wdffuncenum.h`.

## Reproduce from scratch

```bash
# 1) Get the 06cb:00fd fork (switches download_driver.sh to the IdeaPad package)
git clone -b relink https://github.com/n00b-t00b/synaTudor.git
cd synaTudor

# 2) Build & install (TOD module, launcher, udev rule)
meson setup build -DUNMOUNTFS=true
ninja -C build && sudo ninja -C build install
sudo udevadm control --reload && sudo udevadm trigger
sudo systemctl restart fprintd

# 3) Enroll a finger
fprintd-list "$USER"        # -> found 1 devices
fprintd-enroll              # touch the sensor ~8-10x, centred, flat

# 4) Make it usable at login / lock screen / sudo
sudo ./contrib/enable-fingerprint-login.sh
```

When iterating on the driver itself, use `tools/tudor-probe.sh` (builds with
`-DDBGWDF -DDBGIMPORT` and prints the next missing WDF index / import, with a backtrace).

## Notes

- Runs Synaptics' proprietary Windows binaries (see project disclaimer); x86-64 only.
- `contrib/README.md` explains *why* fixes 12–13 exist and how to verify.
- `REVERSE_ENGINEERING_HANDBOOK.md` has the full method and findings.
