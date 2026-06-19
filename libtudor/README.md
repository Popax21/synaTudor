# libtudor
This folder contains the common library which relinks and interfaces with the
native Windows driver. It is used by the [CLI wrapper](../cli/README.md) and the
[libfprint host](../tudor-host/README.md).

## Driver Download / Extraction
During your first build, the Windows driver will automatically be downloaded and
extracted (you'll need a working internet connection). This requires
`innoextract` to be installed (https://pkgs.org/download/innoextract).

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
