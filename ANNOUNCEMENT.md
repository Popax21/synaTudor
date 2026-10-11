# Synaptics `06cb:00fd` fingerprint readers now run on Linux (Lenovo IdeaPad)

**TL;DR** — The Synaptics `06cb:00fd` sensor (Lenovo IdeaPad 3 14ITL6 and friends) now
initializes and is detected by `fprintd` on Linux, via the [synaTudor][synatudor] UDP
runtime-relink approach. Previously this PID sat in the "no known fix" pile.

```
$ fprintd-list $USER
found 1 devices
Device at /net/reactivated/Fprint/Device/0
```

## Why this is a big deal

- Upstream `libfprint` does **not** support `06cb:00fd` (see fprint's supported-devices list).
- The community catalogue listed it as *"No known fix"*.
- Synaptics' "Tudor" match-in-sensor family needs the vendor Windows driver; there is no
  open protocol.

The only route is [synaTudor][synatudor]: it **relinks Synaptics' own Windows UMDF driver
DLLs at runtime on x86-64 Linux** and exposes the result to `fprintd` as a libfprint TOD
module. We made that work for `00fd` with the IdeaPad driver package (DLLs v139).

## What was needed (the fun part)

The relinked driver kept hitting unimplemented Windows APIs. We implemented each wall, one
at a time, using the project's debug stubs (`-DDBGWDF -DDBGIMPORT`) to reveal the next one:

1. **WDF index 50 `WdfDeviceAssignInterfaceProperty`** (+ 51/52) — the wall that stops every
   attempt (also documented in synaTudor issue #20). Implemented as a keyed property store.
2. **`GetModuleHandleW(NULL)`** — a NULL-dereference crash in the relinker (fixed to return
   the current module).
3. A long tail of window/message/power/WTS/registry imports the driver pulls in for its
   notification machinery (`CreateWindowExW`, `RegisterPowerSettingNotification`,
   `WTSRegisterSessionNotification`, `RegQueryInfoKeyW`, ...), all stubbed.

Result: the driver opens the sensor, loads device records, and sits at its enroll prompt.

## How to use it

```bash
git clone -b relink https://github.com/<your-fork>/synaTudor
cd synaTudor
meson setup build -DUNMOUNTFS=true
ninja -C build && sudo ninja -C build install
sudo udevadm control --reload && sudo udevadm trigger
sudo systemctl restart fprintd
fprintd-list $USER          # -> found 1 devices
fprintd-enroll              # then touch the sensor
```

Then enable fingerprint login: `sudo pam-auth-update --enable fprintd`
(or GNOME Settings -> Users -> Fingerprint Login).

## Reproducibility / tooling included

- `REVERSE_ENGINEERING_HANDBOOK.md` — the complete methodology and findings.
- `tools/tudor-probe.sh` — rebuilds with debug stubs and prints the **next** missing WDF
  index or Win32 import, with a gdb backtrace.
- `tools/wdffuncenum.h` — the WDF function-index table, vendored.

## Credits

- [Popax21/synaTudor][synatudor] — the relinker that makes any of this possible.
- `n00b-t00b/synaTudor` — the `06cb:00fd` fork pointing at the IdeaPad driver package.
- Microsoft's `wdffuncenum.h` for the index map.
- The `#20` thread for the property-store insight.

## Caveats

- This runs Synaptics' proprietary Windows binaries; the project itself warns about
  bricked sensors / corrupted firmware. Keep a Windows fallback in mind.
- Enrollment/verification must be tested per-unit; report results back to the tracker.

[synatudor]: https://github.com/Popax21/synaTudor
