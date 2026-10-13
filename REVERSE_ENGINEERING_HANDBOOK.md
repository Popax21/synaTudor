# Linux Driver Reverse-Engineering Handbook
## Synaptics `06cb:00fd` (Lenovo IdeaPad 3 14ITL6) via the synaTudor relinker

How to bring up a proprietary Windows UMDF biometric driver on Linux. For the list of
what was fixed, see `STATUS_WORKING.md`; for system integration, see `contrib/README.md`.

---

## 1. Mission

Make the built-in Synaptics sensor work under `fprintd`/GNOME. Upstream `libfprint` does
not support this PID and there is no open driver, so the route is `synaTudor`, which
**relinks Synaptics' own Windows DLLs at runtime** and exposes them through a
`libfprint-tod` module.

## 2. Architecture

The Synaptics driver is a **UMDF** driver: a pair of PE DLLs
(`synaFpAdapterNNN.dll`, `synaWudfBioUsbNNN.dll`) that

- call Win32 APIs **by name** (ntdll/kernel32/user32/advapi32/crypt32/…), and
- call **WDF functions by index** into a 261-entry table (`WdfFunctions[]`).

synaTudor re-implements that environment in userspace:

```
libtudor/            PE loader + Win32/WDF/WINBIO emulation
  src/pe/            PE parsing (imports, exports, relocs, TLS)
  src/loader.c       maps the image, resolves imports by name
  src/winapi/        kernel32/advapi32/crypt32/user32/... emulation
  src/winapi/wdf/    WDF function table + UMDF object model
  src/tudor/         WINBIO glue (device.c, storage.c, reg.c)
tudor-host/          sandboxed host process running libtudor
tudor-host-launcher/ systemd + dbus service that spawns hosts
libfprint-tod/       the .so libfprint loads from .../libfprint-2/tod-1/
cli/                 tudor_cli — the debugging harness
```

Two failure modes you will hit repeatedly:

1. `Unresolved WDF function N called!`  → index `N` is unimplemented.
2. `Unresolved import NAME@LIB.dll called!` → a Win32 import hit the abort stub.

Both are solvable: map the index via Microsoft's `wdffuncenum.h`, or write a stub and
register it.

## 3. Method: the diagnose → implement loop

```bash
# Build with debug stubs so the driver names what it wants:
meson configure build -DDBGWDF=true -DDBGIMPORT=true -DUNMOUNTFS=false
ninja -C build && sudo ninja -C build install
sudo systemctl restart tudor-host-launcher
printf 'y\nq\n' > /tmp/yes.txt
sudo /sbin/tudor/tudor_cli /tmp/store -P00fd < /tmp/yes.txt 2>&1 | tail
# read the FIRST "Unresolved ..." line, implement it, repeat.
```

- `-DDBGWDF` → unimplemented **WDF indices** print their number.
- `-DDBGIMPORT` → missing **imports** print `name@lib.dll`.
- `-DUNMOUNTFS=false` → keeps `/proc` intact so `gdb` works.

**Implement a WDF function** (`libtudor/src/winapi/wdf/*.c`):

```c
__winfnc NTSTATUS MyWdfFunc(WDF_DRIVER_GLOBALS *globals, WDFOBJECT obj, ...) { ... }
WDFFUNC(MyWdfFunc, THE_INDEX)      /* registers into wdf_functions[THE_INDEX] */
```

**Implement an import** (`libtudor/src/winapi/`); convert UTF-16 with `winstr_to_str()`:

```c
__winfnc BOOL RegisterClassExW(const void *wcx) { return 1; }
WINAPI(RegisterClassExW)           /* registers by name for the loader */
```

Notes:
- `GetModuleHandleW(NULL)` must return the **current module** (`cur_module`) — never `strcmp(NULL)`.
- Crashes instead of an "Unresolved" line? Use `gdb -batch -ex 'run < /tmp/yes.txt' -ex bt --args /sbin/tudor/tudor_cli /tmp/store -P00fd`, plus `dmesg`/`journalctl`.
- Data path look wrong (e.g. `usbfs: ... did not claim interface 0`)? `strace -f -e trace=ioctl` to watch the URB activity.

Automate the loop with `tools/tudor-probe.sh`.

## 4. WDF function-table indices (subset, from `wdffuncenum.h`)

```
25  WdfDeviceCreate
27  WdfDeviceCreateDeviceInterface
31  WdfDeviceQueryProperty
33  WdfDeviceSetPnpCapabilities             <- stub added
40  WdfDeviceConfigureRequestDispatching
41  WdfDeviceGetSystemPowerAction
50  WdfDeviceAssignInterfaceProperty        <- IMPLEMENTED (the long-standing wall)
51  WdfDeviceAllocAndQueryInterfaceProperty <- IMPLEMENTED
52  WdfDeviceQueryInterfaceProperty         <- IMPLEMENTED
57  WdfDriverCreate
85  WdfIoQueueCreate            86 WdfIoQueueGetState        87 WdfIoQueueStart
117 WdfMemoryCreate            118 WdfMemoryCreatePreallocated  119 WdfMemoryGetBuffer
123 WdfObjectGetTypedContextWorker   124 WdfObjectAllocateContext   129 WdfObjectDelete
131 WdfRegistryOpenKey         133 WdfRegistryClose         136 WdfRegistryQueryValue
142 WdfRegistryAssignValue     147 WdfRegistryAssignULong
148 WdfRequestCreate … 185     188 WdfStringCreate … 194
248 WdfDeviceStopIdleActual   249 WdfDeviceResumeIdleActual
… (261 total)
Full list: microsoft/Windows-Driver-Frameworks → src/publicinc/wdf/umdf/<ver>/wdffuncenum.h
```

## 5. References

- `Popax21/synaTudor` — the relinker (issue **#20**: the same index-33→50 wall).
- `vixalien/synaTudor` (branch `00c9`) — an earlier attempt that reached enrollment with stubs.
- `jedbillyb/linux-fingerprint-drivers` — community device catalogue.
- `microsoft/Windows-Driver-Frameworks` — `wdffuncenum.h` (index → name/signature).
- `vojtapl/synaTudorMiS`, `uunicorn/python-validity` — sibling Synaptics families.
- `fprint.freedesktop.org/supported-devices.html` — what upstream libfprint supports.

## 6. Caveats

- Runs Synaptics' **proprietary Windows binaries** — the project warns of bricked sensors /
  corrupted firmware. x86-64 only.
- Keep vendored sources and patches in git so progress is reproducible.
- Clean rollback: `sudo ninja -C build uninstall`, disable `tudor-host-launcher`, remove
  `/sbin/tudor` and `/usr/lib/x86_64-linux-gnu/libfprint-2/tod-1/`.
