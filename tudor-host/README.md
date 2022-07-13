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

## Documentation
**TODO**