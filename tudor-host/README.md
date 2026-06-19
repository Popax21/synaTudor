# tudor-host
This folder contains the Tudor host process. It's launched by the
[tudor-host-launcher](../tudor-host-launcher/README.md), and its job is to
provide [libtudor](../libtudor/README.md)'s functionality over a secured and
sandboxed IPC connection.

## Build Options
Currently, the following build options are defined:

Flag | Description
----- | ---------------------------
`UNMOUNTFS=true` | Enable unmounting of the root file system in the sandbox, which prevents the driver from accessing any files. Enabled by default, disable when debugging using e.g. GDB.

## Native Storage
For v132 devices, `tudor-host` lets the native Synaptics storage adapter own the
template database. The libfprint-tod side sends metadata-only records for
already-enrolled prints, and the host uses the GUID/finger pair for verify,
identify, delete, and clear-storage operations.

The native database path is supplied by `TUDOR_NATIVE_STORAGE_PATH`, normally
set by the host launcher.

`IPC_MSG_CLEAR_RECORDS` is a real storage operation and deletes native records.
`IPC_MSG_CLEAR_HOST_RECORDS` is only for clearing transient host-side state when
libfprint-tod orphans a host process.
