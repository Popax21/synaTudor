# libtudor
This folder contains the common library which relinks and interfaces with the
native Windows driver. It is used by the [CLI wrapper](../cli/README.md) and the
[libfprint host](../tudor-host/README.md).

## Driver Download / Extraction
During your first build, the Windows drivers will automatically be downloaded and
extracted (you'll need a working internet connection). This requires
`innoextract` (https://pkgs.org/download/innoextract) and `unzip` to be
installed. Both Lenovo's driver (for `06cb:00be`) and Dell's driver (for
`06cb:00bc`) are embedded; `tudor_init` picks one by the sensor's USB ID.

## Build Options
Currently, the following build options are defined:

Flag | Description
----- | ---------------------------
`DBGIMPORT=true` | Enable import debugging, which prints the name of unresolved function imports when they're called
`DBGWDF=true` | Enable WDF debugging, which prints the index of unresolved WDF functions when they're called

## Documentation
**TODO**