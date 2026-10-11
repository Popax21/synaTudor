# Linux Driver Reverse-Engineering Handbook & AI Prompt
## Target: Synaptics `06cb:00fd` fingerprint sensor (Lenovo IdeaPad 3 14ITL6)

> A field manual for driving a proprietary Windows UMDF biometric driver on Linux,
> built from the **synaTudor** relinker project. Everything here was executed live
> on the IdeaPad; the "Progress" section is real, not hypothetical.

---

## 1. Mission

Get the built-in Synaptics sensor (`06cb:00fd`) usable under Linux (`fprintd` +
GNOME Settings fingerprint enrollment). Upstream `libfprint` does **not** support
this PID, and no official driver exists. The only viable route is **runtime relinking
of Synaptics' own Windows driver** via `synaTudor`.

---

## 2. Why this is hard (the architecture)

Synaptics' Windows driver is a **UMDF (User-Mode Driver Framework)** driver. It:

- is a pair of PE DLLs (`synaFpAdapterNNN.dll`, `synaWudfBioUsbNNN.dll`);
- talks to the sensor over USB via WinUSB/UMDF;
- calls Windows kernel/user APIs by **name** (ntdll/kernel32/user32/advapi32/crypt32/...);
- calls **WDF functions by index** into a 261-entry table (`WdfFunctions[]`), bound at init
  via the UMDF loader interface.

`synaTudor` re-implements enough of that environment in userspace:

```
libtudor/                 PE loader + Win32/WDF/WINBIO emulation
  src/pe/                 PE parsing (imports, exports, relocs, TLS)
  src/loader.c            maps image, resolves imports by name
  src/winapi/             kernel32/advapi32/crypt32/user32/... emulation
  src/winapi/wdf/         WDF function table + UMDF object model
  src/tudor/              WINBIO glue: device.c, storage.c, reg.c
tudor-host/               sandboxed process that hosts libtudor
tudor-host-launcher/      systemd + dbus service that spawns hosts
libfprint-tod/            the .so loaded by libfprint from .../libfprint-2/tod-1/
cli/                      tudor_cli — debugging harness (this is your scalpel)
```

Two failure modes you will hit repeatedly:

1. **`Unresolved WDF function N called!`** — index `N` in the WDF table is not implemented.
2. **`Unresolved import NAME@LIB.dll called!`** — a Win32 import resolved to the abort stub.

Both are *solvable*: (1) map `N` via Microsoft's `wdffuncenum.h`; (2) write a thin stub
or a real implementation and register it with the `WINAPI()` macro.

---

## 3. The diagnose → implement loop (METHOD)

```bash
# Build with the debugging stubs so the driver tells you exactly what it wants:
meson configure build -DDBGWDF=true -DDBGIMPORT=true -DUNMOUNTFS=false
ninja -C build && sudo ninja -C build install
sudo systemctl restart tudor-host-launcher
printf 'y\nq\n' > /tmp/yes.txt
sudo /sbin/tudor/tudor_cli /tmp/store -P00fd < /tmp/yes.txt 2>&1 | tail
# -> read the FIRST "Unresolved ..." line, then implement it and repeat.
```

- `-DDBGWDF` makes unimplemented **WDF indices** print their number.
- `-DDBGIMPORT` makes unimplemented **imports** print `name@lib.dll`.
- `-DUNMOUNTFS=false` keeps `/proc` intact so `gdb` works (`run < /tmp/yes.txt`, then `bt`).

**WDF implementation** (in `libtudor/src/winapi/wdf/*.c`):

```c
__winfnc NTSTATUS MyWdfFunc(WDF_DRIVER_GLOBALS *globals, WDFOBJECT obj, ...) { ... }
WDFFUNC(MyWdfFunc, THE_INDEX)      /* registers into wdf_functions[THE_INDEX] */
```

**Import implementation** (in `libtudor/src/winapi/`) — return a sane value, or proxy
to an existing implementation; convert UTF-16 with `winstr_to_str()` when needed:

```c
__winfnc BOOL RegisterClassExW(const void *wcx) { return 1; }
WINAPI(RegisterClassExW)           /* registers by name for the loader */
```

`GetModuleHandleW(NULL)` must return the **current module** (`cur_module`); do not
`strcmp(NULL)`.

---

## 4. Progress achieved (live, this session)

| # | Wall hit | Fix | File |
|---|----------|-----|------|
| 1 | `Unresolved WDF function 50` | Implemented `WdfDeviceAssignInterfaceProperty` (index 50), `WdfDeviceAllocAndQueryInterfaceProperty` (51), `WdfDeviceQueryInterfaceProperty` (52) as a keyed property store; stubbed `WdfDeviceSetPnpCapabilities` (33) | `libtudor/src/winapi/wdf/device.c` (appended block) |
| 2 | SIGSEGV in `strcmp` via `GetModuleHandleW(NULL)` | NULL-guard + return current module | `libtudor/src/winapi/module.c` |
| 3 | `Unresolved import RegisterClassExW@USER32.dll` | stub | `libtudor/src/winapi/module.c` |
| 4 | `Unresolved import CreateWindowExW@USER32.dll` | stubbed window + message-loop set | `libtudor/src/winapi/module.c` |

**Index 50 = `WdfDeviceAssignInterfaceProperty`** confirmed against Microsoft's
`wdffuncenum.h` and the maintainer's own note in `Popax21/synaTudor` issue #20.

**Current state:** the relinked v139 driver now
`Loaded driver DLL's → Initialized tudor driver → Opened USB device` and passes the
whole WDF wall. It then enters a USER32 **window/message path** (it creates a hidden
window, presumably for device-arrival notification). With our stubs, `fprintd` activation
now **blocks** instead of crashing — i.e. the notification thread/host hangs rather than
aborts. **This is the exact next frontier.**

**Next steps (in order):**
1. Get a backtrace: build with `-DUNMOUNTFS=false`, run `gdb -batch -ex 'run < /tmp/yes.txt' -ex bt --args /sbin/tudor/tudor_cli /tmp/store -P00fd` (as root).
2. Decide whether the window/message thread is *required* or can be bypassed:
   - If required: implement a minimal, non-blocking window (fake `HWND`, and make
     `GetMessageW`/`PeekMessageW` return immediately, or run the message thread detached).
   - If not: check whether the driver only needs the window to receive
     `WM_DEVICECHANGE`; if the host already owns the USB handle, the notify thread can be
     neutralised.
3. Iterate the loop until `tudor_cli` reaches its `Commands:` prompt. Then enroll/verify.
4. Wire the TOD module back in, enable `tudor-host-launcher`, add the udev rule + id table
   (`06cb:00fd`) and test `fprintd-enroll` / GNOME Settings.

**Rollback/safety:** `sudo ninja -C build uninstall`, disable `tudor-host-launcher`,
remove `/sbin/tudor` and `/usr/lib/x86_64-linux-gnu/libfprint-2/tod-1/`; stock `fprintd`
returns to "No devices available". (Done and verified.)

---

## 5. THE AI PROMPT (copy-paste this to an AI agent)

```
You are a senior systems/reverse-engineering engineer. Goal: make the Synaptics
fingerprint sensor with USB ID 06cb:00fd work on Linux by completing the synaTudor
runtime-relink driver (github.com/Popax21/synaTudor), targeting Ubuntu 26.04 / libfprint
1.95.1 TOD. Work directly on the machine.

CONTEXT
- The sensor is unsupported by upstream libfprint. synaTudor relinks Synaptics' Windows
  UMDF DLLs (from the Lenovo IdeaPad consumer driver package, current DLLs are v139) and
  exposes them as a libfprint TOD module.
- The driver calls WDF functions BY INDEX into a 261-entry table (see Microsoft
  Windows-Driver-Frameworks wdffuncenum.h). Unimplemented indices abort with
  "Unresolved WDF function N called!" when built -DDBGWDF.
- Win32 imports are resolved by name; missing ones abort with
  "Unresolved import name@lib.dll called!" when built -DDBGIMPORT.
- Already implemented this session (in the working tree): WDF indices 50/51/52
  (WdfDevice*InterfaceProperty store) + index 33 stub; GetModuleHandleW(NULL) fix;
  USER32 RegisterClassEx*/CreateWindowExW + message-loop stubs. The driver now reaches
  "Opened USB device" and the window/message path; fprintd activation blocks.

METHOD (repeat strictly)
1. Rebuild with: meson configure build -DDBGWDF=true -DDBGIMPORT=true -DUNMOUNTFS=false
2. Run the harness: printf 'y\nq\n' > /tmp/yes.txt; sudo /sbin/tudor/tudor_cli /tmp/store
   -P00fd < /tmp/yes.txt
3. Read the FIRST unresolved WDF index / import.
4. Implement it (WDF: __winfnc + WDFFUNC(fn, index); import: __winfnc + WINAPI(name)).
   Use Microsoft's wdffuncenum.h to map indices to names/signatures. For imports, prefer
   a minimal correct semantic stub; proxy to existing A-variants converted via
   winstr_to_str().
5. Rebuild, reinstall, goto 2. If it crashes, run under gdb
   (gdb -batch -ex 'run < /tmp/yes.txt' -ex bt --args /sbin/tudor/tudor_cli /tmp/store -P00fd)
   and fix the top frame.
6. Success = tudor_cli reaches its "Commands:" prompt, then `e` enroll / `v` verify work.
   Then re-enable the TOD module + launcher + udev id 06cb:00fd and confirm
   `fprintd-list` shows the device and GNOME Settings can enroll.

CONSTRAINTS
- Never leave the system in a state where fprintd hangs (it can break PAM auth).
  If an iteration leaves fprintd stuck, revert the TOD integration before stopping.
- Keep changes minimal and in the style of the existing code.
- Supplying the vendor Windows driver binaries is required (download_driver.sh does it).
  The project warns about bricked sensors / corrupted firmware: proceed carefully.

DELIVERABLES
- Working sensor (fprintd + GNOME enrollment), OR a precise backtrace + the next
  concrete fix if a wall remains.
- A short changelog of every index/import implemented.
```

---

## 6. Reference: WDF function-table indices (subset, from `wdffuncenum.h`)

```
16  WdfDeviceInitSetS0IdleSettings? (device init setters 16..29)
25  WdfDeviceCreate
27  WdfDeviceCreateDeviceInterface
28  WdfDeviceSetDeviceInterfaceState
29  WdfDeviceRetrieveDeviceInterfaceString
31  WdfDeviceQueryProperty
33  WdfDeviceSetPnpCapabilities            <- stub added
40  WdfDeviceConfigureRequestDispatching
41  WdfDeviceGetSystemPowerAction
50  WdfDeviceAssignInterfaceProperty       <- IMPLEMENTED (was the wall)
51  WdfDeviceAllocAndQueryInterfaceProperty<- IMPLEMENTED
52  WdfDeviceQueryInterfaceProperty        <- IMPLEMENTED
57  WdfDriverCreate
85  WdfIoQueueCreate
86  WdfIoQueueGetState
87  WdfIoQueueStart
88  WdfDeviceStopIdleNoTrack
89  WdfDeviceResumeIdleNoTrack
117 WdfMemoryCreate
118 WdfMemoryCreatePreallocated
119 WdfMemoryGetBuffer
123 WdfObjectGetTypedContextWorker
124 WdfObjectAllocateContext
129 WdfObjectDelete
131 WdfRegistryOpenKey
133 WdfRegistryClose
136 WdfRegistryQueryValue
142 WdfRegistryAssignValue
147 WdfRegistryAssignULong
148 WdfRequestCreate ... 185 request set
188 WdfStringCreate ... 194 wait locks
192+ locks/timers
248 WdfDeviceStopIdleActual
249 WdfDeviceResumeIdleActual
... (261 total)
Full source: microsoft/Windows-Driver-Frameworks -> src/publicinc/wdf/umdf/<ver>/wdffuncenum.h
```

---

## 7. Relevant repositories / references

- `Popax21/synaTudor` — the relinker (issue **#20** documents this exact wall: 33 → 50).
- `vixalien/synaTudor` (branch `00c9`) — another attempt that got enroll working with stubs.
- `jedbillyb/linux-fingerprint-drivers` — community device catalogue (lists `06cb:00fd` as "no known fix" *as of now*).
- `microsoft/Windows-Driver-Frameworks` — `wdffuncenum.h` (index→name/signature).
- `vojtapl/synaTudorMiS`, `uunicorn/python-validity` — sibling Synaptics families (Tudor/Prometheus), useful patterns.
- `fprint.freedesktop.org/supported-devices.html` — what upstream libfprint supports.

---

## 8. Test cases

| ID | Test | Command | Pass criteria |
|----|------|---------|---------------|
| TC1 | Driver loads + opens device | `tudor_cli /tmp/s -P00fd` | reaches `Commands:` prompt, no abort/segfault |
| TC2 | Enroll | at prompt, `e`, identity, finger, touch sensor | `Successfully enrolled finger` |
| TC3 | Verify | `v`, touch sensor | `verify result: ... MATCH` |
| TC4 | libfprint sees device | `fprintd-list $USER` | `found 1 devices` |
| TC5 | System enrollment | GNOME Settings → Users → Fingerprint | enrollment completes |
| TC6 | Auth (optional) | `pam-auth-update` enable fprintd + lock screen | fingerprint unlocks |
| TC7 | Regression | remove module → `fprintd-list` | `No devices available` (clean revert) |

Automate TC1–TC3 with `tools/tudor-probe.sh` (below).

---

## 9. Caveats

- This executes **Synaptics' proprietary Windows binaries**; upstream warns of bricked
  sensors / corrupted firmware. Low real-world downside here (sensor is already unusable
  on Linux) but call it out.
- The v139 driver expects a newer WDF surface than the relinker was written for; the
  remaining work is emulating the window/notification + a few more Win32 calls, not
  re-deriving the protocol.
- Keep vendored sources + our patches in git so progress is reproducible.
