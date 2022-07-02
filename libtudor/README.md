# Tudor driver relinking library
This folder contains the common library which relinks and interfaces with the
native Windows driver. It is used by the [CLI wrapper](../cli/README.md) and the
[libfprint host](../libfprint-host/README.md).

## Building
First, download and extract the driver DLLs by executing the command `make
extract` (you'll need a working internet connection). This requires
`innoextract` to be installed (https://pkgs.org/download/innoextract). Then,
compile the library by running `make`. 

Currently, the following build flags are defined:

Flag | Description
----- | ---------------------------
`DEBUG=1` | Enable debug build options, like `-O0` and `-g3`
`DBGIMPORT=1` | Enable import debugging, which prints the name of unresolved function imports when they're called
`DBGWDF=1` | Enable WDF debugging, which prints the index of unresolved WDF functions when they're called

## Documentation
**TODO**