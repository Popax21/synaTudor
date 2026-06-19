# libtudor
This folder contains the common library which relinks and interfaces with the
native Windows driver. It is used by the [CLI wrapper](../cli/README.md) and the
[libfprint host](../tudor-host/README.md).

## Driver DLLs

The current build embeds the v132 Windows driver DLLs from
`libtudor/driver_dlls/`:

- `synaFpAdapter132.dll`
- `synaWudfBioUsb132.dll`

`download_driver.sh` is kept as a helper for fetching the Lenovo package used
while reverse engineering, but the Meson build copies local DLLs rather than
downloading them automatically.

## v132 Native Storage
v132 matching depends on the native Synaptics storage adapter exported by the
Windows adapter DLL. libtudor queries `WbioQueryStorageInterface` when available
and uses that adapter for the WinBio pipeline. If the export is missing, it
falls back to the host-side storage adapter.

The native database path is selected in this order:

1. `TUDOR_NATIVE_STORAGE_PATH`
2. `$STATE_DIRECTORY/native-storage.dat`
3. `/var/lib/tudor/native-storage.dat`

The installed host launcher sets `TUDOR_NATIVE_STORAGE_PATH` explicitly.

## Build Options
Currently, the following build options are defined:

Flag | Description
----- | ---------------------------
`DBGIMPORT=true` | Enable import debugging, which prints the name of unresolved function imports when they're called
`DBGWDF=true` | Enable WDF debugging, which prints the index of unresolved WDF functions when they're called
