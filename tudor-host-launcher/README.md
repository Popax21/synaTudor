# tudor-host-launcher
This folder contains a simple DBus service capable of launching and managing
tudor_host processes. This is necessary because fprintd is sandboxed in such a
way that the host's own sandbox fails to properly initialize. Because the host
launcher is outside of this sandbox (it does still employ systemd unit
sandboxing, but only to the extent possible when maintaining the host's
functionality), it can properly launch these host processes, and because it
provides its services using the DBus, the libfprint-tod module can interact with
it and take over IPC once the process has been started.

## Systemd State
The installed service uses:

```ini
StateDirectory=tudor
StateDirectoryMode=0700
```

systemd creates this as `/var/lib/tudor`. Pairing data and the v132 native
storage database are kept there.

## Native Storage Path
When launching `tudor_host`, the launcher passes:

```sh
TUDOR_NATIVE_STORAGE_PATH=/var/lib/tudor/native-storage.dat
```

This keeps the native Synaptics template database durable across reboots and
prevents fprintd metadata from outliving a tmp-backed template store.

## Logs
Use systemd logs when debugging fprintd integration:

```sh
journalctl -u tudor-host-launcher.service
journalctl -u fprintd
```
