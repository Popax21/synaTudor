# Synaptics Tudor Linux Driver

This project makes Synaptics Tudor fingerprint readers usable on Linux by
relinking the vendor Windows driver at run time and exposing it through
fprintd. It is a bridge driver: Linux talks to a libfprint TOD module, the TOD
module talks to a sandboxed host process, and the host process loads the
Synaptics Windows DLLs that know how to pair with the sensor, enroll fingers,
and perform matching.

The immediate goal is practical fprintd support for hardware that otherwise has
no useful Linux driver. The longer-term goal is to replace the relinked Windows
path with a clean native implementation.

THIS PROJECT IS PROVIDED AS IS, WITHOUT WARRANTY OR LIABILITY OF ANY KIND, OR
FOR ANY RISKS OR SIDE EFFECTS WHICH MIGHT OCCUR FROM USAGE OF ANYTHING PROVIDED
AS PART OF THIS PROJECT, INCLUDING, BUT NOT LIMITED TO, BRICKED SENSORS,
CORRUPTED FIRMWARE, BYPASSES OF HOST SECURITY, AND VULNERABILITIES IN THE CODE.
USE AT YOUR OWN RISK.

## Status

The v132 path can enroll, verify, identify, and authenticate through fprintd
when the matching Synaptics Windows driver DLLs are available. The flow has been
tested live with a Kensington VeriMark Desktop reader.

This is still reverse-engineered driver work. Expect hardware variation, noisy
driver logs, and occasional sharp edges around fprintd, PAM, and polkit
integration.

## Supported Hardware

The TOD driver currently advertises these USB IDs:

| VID:PID | Notes |
| --- | --- |
| `047d:00f2` | Kensington VeriMark Desktop, tested in the current v132 path |
| `06cb:00be` | Original Synaptics Tudor ID |
| `06cb:00de` | Synaptics ID seen on the same sensor family |

The default CLI target is `047d:00f2`. Other Tudor-family devices may need
additional captures, shims, or driver-DLL versions before they work.

## How It Works

The project is split into small pieces:

- [libtudor](libtudor/README.md): loads and relinks the Synaptics Windows DLLs,
  implements the required Win32, WinUSB, WDF, and COM shims, and drives the
  WinBio pipeline.
- [tudor-host](tudor-host/README.md): a sandboxed helper process that owns the
  relinked driver and talks to the sensor.
- [tudor-host-launcher](tudor-host-launcher/README.md): a DBus-activated
  system service that launches host processes outside fprintd's own sandbox.
- [libfprint-tod](libfprint-tod/README.md): the libfprint TOD module loaded by
  fprintd.
- [cli](cli/README.md): a debugging CLI for direct host testing.
- [tools/windows-capture](tools/windows-capture/README.md): helper scripts for
  collecting Windows USB traces.

For v132 devices, matching depends on the native Synaptics storage adapter from
the Windows DLL. fprintd stores metadata only: the template GUID and finger. The
real biometric template lives in `/var/lib/tudor/native-storage.dat`, managed by
the host launcher through `TUDOR_NATIVE_STORAGE_PATH`.

## Build And Install

Install the native build dependencies, libfprint TOD fork, and fprintd. This
project is x86-64 only because it relinks x86-64 Windows driver DLLs.

On Arch/CachyOS:

```sh
sudo pacman -S --needed base-devel meson ninja pkgconf git \
    libusb glib2 json-glib libgusb libcap libseccomp openssl dbus systemd \
    fprintd
```

Install `libfprint-tod-git` from the AUR instead of regular `libfprint`.

The build currently embeds the DLLs from `libtudor/driver_dlls/`. If you are
bringing up another driver version, replace those DLLs or update the extraction
flow before building. Check the driver license before redistributing packages
that contain vendor DLLs.

```sh
meson setup build -Dbuild_tod=enabled
meson compile -C build
sudo meson install -C build
sudo systemctl daemon-reload
sudo udevadm control --reload-rules
sudo udevadm trigger
sudo systemctl restart tudor-host-launcher fprintd
```

The launcher is DBus-activated and uses:

```ini
StateDirectory=tudor
StateDirectoryMode=0700
```

systemd creates this as `/var/lib/tudor`.

## Use With fprintd

Enroll and verify before wiring PAM:

```sh
sudo fprintd-delete "$USER"   # optional, removes stale prints
sudo fprintd-enroll "$USER"
fprintd-verify "$USER"
```

Successful verification should print `verify-match`, and the host log should
show:

```text
Verify GUID ... -> does match
```

After plain fprintd works, PAM can use fingerprint auth. See
[docs/linux-fprintd-setup.md](docs/linux-fprintd-setup.md) for sudo, polkit,
GNOME/GDM, troubleshooting, and recovery commands.

## Development Roadmap

The current code proves the hardware path and gives Linux users a working
bridge. The next major step is turning that knowledge into a native driver that
does not depend on the Windows DLLs. See
[docs/rust-driver-roadmap.md](docs/rust-driver-roadmap.md) for the proposed
Rust architecture and migration plan.

Near-term work:

- keep the v132 fprintd path stable across restarts, suspend/resume, sudo, and
  polkit flows
- package this cleanly for Arch/CachyOS users
- collect reports for more Tudor-family USB IDs
- reduce debug log noise and document useful failure signatures
- build replay tests from Windows and Linux USB captures

## Troubleshooting

Useful logs:

```sh
journalctl -u tudor-host-launcher.service -u fprintd -b
```

Common patterns:

- `Verify GUID ... -> doesn't match`: plain matching failed. Re-enroll before
  debugging PAM.
- `Tudor host process died`: check for the actual error above it, then confirm
  the installed `/usr/bin/tudor/libtudor.so` matches the current build.
- `Not Authorized: net.reactivated.fprint.device.enroll`: polkit blocked
  enroll/delete. Use `sudo fprintd-delete "$USER"` and
  `sudo fprintd-enroll "$USER"` during repair.
- `pam_faillock` or `account temporarily locked`: reset the lockout with
  `sudo faillock --user "$USER" --reset`.

## Security Notes

The fprintd path is safer than the debugging CLI because the relinked driver is
kept in a sandboxed host process launched by a system service. It still runs
reverse-engineered compatibility code around a proprietary Windows driver. Do
not treat this as audited security-sensitive infrastructure yet.
