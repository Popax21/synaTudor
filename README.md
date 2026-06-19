# Synaptics Tudor Driver Relinking Project
This project dynamically relinks the Windows driver for Synaptics Tudor
fingerprint sensors at run time so they can be used from Linux on x86-64
systems. It includes a debug CLI, a sandboxed host process, a DBus host
launcher, and a libfprint-tod module for fprintd integration.

THIS PROJECT IS PROVIDED AS IS, WITHOUT WARRANTY OR LIABILITY OF ANY KIND, OR
FOR ANY RISKS OR SIDE EFFECTS WHICH MIGHT OCCUR FROM USAGE OF ANYTHING PROVIDED
AS PART OF THIS PROJECT, INCLUDING, BUT NOT LIMITED TO, BRICKED SENSORS,
CORRUPTED FIRMWARE, BYPASSES OF HOST SECURITY, AND VULNERABILITIES IN THE CODE.
USE AT YOUR OWN RISK.

## Status
The v132 path can enroll, verify, and identify through fprintd when the matching
Synaptics Windows driver DLLs are available. Matching uses the native Synaptics
storage adapter from the Windows driver; fprintd records store only metadata
that points at templates in the driver-managed storage database.

This is still a reverse-engineered driver path. Expect rough edges, hardware
variation, and logs that contain noisy Windows-driver trace messages.

## Structure
This project is split over multiple folders, all providing different parts of
the functionality:
- [libtudor](libtudor/README.md): Contains the common library code handling
  relinking and interfacing with the driver.
- [cli](cli/README.md): Contains a simple CLI wrapper for the relinked driver.
- [tudor-host](tudor-host/README.md): Contains the host application for
  libtudor, used by the libfprint module.
- [tudor-host-launcher](tudor-host-launcher/README.md): Contains the systemd
  service which launches the tudor host processes. This extra step is needed to
  bypass the strict fprintd sandboxing, which messes with the host's even
  stricter sandboxing.
- [libfprint-tod](libfprint-tod/README.md): Contains the libfprint module, to be
  loaded by the libfprint TOD fork.

## Requirements
- x86-64 Linux
- `meson`, `ninja`, `pkg-config`, `innoextract`
- `libusb`, `glib2`, `gio-unix-2.0`, `json-glib`, `udev`
- `libfprint-tod` and `fprintd`

On Arch Linux, install `libfprint-tod-git` instead of regular `libfprint`.

## Build
During the first build, the Windows driver is downloaded and extracted
automatically. `innoextract` must be installed before the first build.

```sh
meson setup build -Dbuild_tod=enabled
ninja -C build
```

For Arch Linux packaging conventions, `arch-meson build -Dbuild_tod=enabled`
also works.

## Install
Install the host, launcher, TOD module, DBus policy, systemd unit, and udev
rules:

```sh
sudo ninja -C build install
sudo systemctl daemon-reload
sudo udevadm control --reload-rules
sudo udevadm trigger
sudo systemctl restart fprintd
```

The host launcher is DBus-activated. Its state directory is managed by systemd
as `/var/lib/tudor` with mode `0700`.

## Use With fprintd
Enroll a finger:

```sh
fprintd-enroll
```

Verify a finger:

```sh
fprintd-verify
```

List or delete enrolled prints:

```sh
fprintd-list "$USER"
fprintd-delete "$USER"
```

## Native Storage
The v132 Windows engine expects the Synaptics storage adapter from the same DLL
to own the template database. The libfprint-tod module therefore stores
metadata-only records containing the template GUID and finger. The real template
database lives in the launcher state directory as `native-storage.dat`, and the
host process receives that path through `TUDOR_NATIVE_STORAGE_PATH`.

Deleting prints through fprintd removes the matching native storage record.

## Troubleshooting
- `fprintd-enroll` cannot see the device: reload udev rules, reconnect the
  sensor, then restart `fprintd`.
- The host launcher does not start: check
  `journalctl -u tudor-host-launcher.service`.
- fprintd returns protocol errors: check `journalctl -u fprintd` and the host
  launcher logs together.
- CLI testing is available through [cli](cli/README.md), but fprintd is the
  safer integration path.
