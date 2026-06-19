# libfprint-tod
This folder contains the libfprint-tod module. It talks to the
[tudor-host-launcher](../tudor-host-launcher/README.md) over DBus, asks it to
launch a [tudor-host](../tudor-host/README.md) process for the active USB
device, then sends IPC commands to that host for enroll, verify, identify,
delete, and clear-storage operations.

## Runtime Model
The host process loads the relinked Synaptics Windows driver through
[libtudor](../libtudor/README.md). On v132 devices, the Windows engine only
matches correctly when its native Synaptics storage adapter owns the template
database. For that reason, TOD print data stores only:

- the template GUID
- the finger/subfactor
- an empty record payload for native-storage devices

The actual biometric template is stored in the native driver database managed by
the host process.

## fprintd Commands
Enroll:

```sh
fprintd-enroll
```

Verify:

```sh
fprintd-verify
```

List and delete prints:

```sh
fprintd-list "$USER"
fprintd-delete "$USER"
```

## Notes
- Identify results are resolved only against the candidate print list provided
  by libfprint.
- Deleting a TOD print also asks the host to delete the native storage record.
- Clearing storage asks the host to delete all native storage records.
