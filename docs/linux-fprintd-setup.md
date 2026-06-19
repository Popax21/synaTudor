# Linux fprintd setup

This guide covers the practical path from a built synaTudor tree to working
fingerprint auth on an Arch or CachyOS-style system.

## What This Is For

Use this integration when Linux sees a Synaptics Tudor fingerprint reader but
regular libfprint cannot use it. The stack provides:

- a libfprint TOD module named `Tudor TOD`
- a system DBus launcher service for sandboxed `tudor_host` processes
- durable native template storage in `/var/lib/tudor/native-storage.dat`
- normal fprintd commands for enroll, verify, identify, delete, and list

## Install

Install build/runtime dependencies first. On Arch/CachyOS:

```sh
sudo pacman -S --needed base-devel meson ninja pkgconf git \
    libusb glib2 json-glib libgusb libcap libseccomp openssl dbus systemd \
    fprintd
```

Install `libfprint-tod-git` from the AUR instead of regular `libfprint`.

Build and install from the repository root:

```sh
meson setup build -Dbuild_tod=enabled
meson compile -C build
sudo meson install -C build
sudo systemctl daemon-reload
sudo udevadm control --reload-rules
sudo udevadm trigger
sudo systemctl restart tudor-host-launcher fprintd
```

Check that the services are healthy:

```sh
systemctl status tudor-host-launcher fprintd --no-pager
```

Check that fprintd sees the reader:

```sh
fprintd-list "$USER"
```

## Enroll And Verify

Use `sudo` for enroll/delete while setting the system up. That avoids a polkit
loop where polkit tries to authenticate fprintd operations using fprintd.

```sh
sudo faillock --user "$USER" --reset
sudo fprintd-delete "$USER"  # optional, useful after driver changes
sudo fprintd-enroll "$USER"
fprintd-verify "$USER"
```

Do not wire PAM until `fprintd-verify "$USER"` returns `verify-match`.

Useful success receipts:

```text
Verify result: verify-match (done)
Verify GUID ... -> does match
```

## sudo PAM

Add `pam_fprintd.so` before `system-auth` in `/etc/pam.d/sudo`:

```pam
#%PAM-1.0
auth       sufficient   pam_fprintd.so timeout=10
auth       include      system-auth
account    include      system-auth
session    include      system-auth
```

If sudo fails several times while testing, reset faillock:

```sh
sudo faillock --user "$USER" --reset
```

## polkit PAM

Fingerprint auth can also be enabled for polkit:

```pam
#%PAM-1.0
auth       sufficient   pam_fprintd.so timeout=10
auth       include      system-auth
account    include      system-auth
password   include      system-auth
session    include      system-auth
```

If a polkit dialog stops accepting your password while the fingerprint driver is
broken, temporarily remove the `pam_fprintd.so` line, reset faillock, and
restart polkit:

```sh
sudo faillock --user "$USER" --reset
sudo systemctl restart polkit
```

After plain `fprintd-verify` works again, the fingerprint-first polkit config is
safe to restore.

## GNOME And GDM

Many GNOME installations already ship `/etc/pam.d/gdm-fingerprint`. If GNOME
Settings can enroll and list fingerprints after this driver is installed, no
extra GDM configuration should be required.

Use the command line first. It gives cleaner errors than the GNOME UI:

```sh
fprintd-list "$USER"
fprintd-verify "$USER"
```

## Native Storage

For v132 devices, fprintd records are not full templates. They contain only:

- the template GUID
- the finger/subfactor
- an empty payload

The actual template database is owned by the Synaptics Windows storage adapter
and lives at:

```text
/var/lib/tudor/native-storage.dat
```

If that file is removed or wiped, fprintd metadata can still list enrolled
fingers, but matching will fail. Delete and re-enroll:

```sh
sudo fprintd-delete "$USER"
sudo fprintd-enroll "$USER"
```

## Troubleshooting

Watch fprintd and host logs together:

```sh
journalctl -u tudor-host-launcher -u fprintd -b --no-pager
```

No match:

```text
Verify GUID ... -> doesn't match
```

First test plain fprintd. If `fprintd-verify "$USER"` no-matches, this is not a
PAM issue. Re-enroll and check the native storage database.

Match:

```text
Verify GUID ... -> does match
```

If this appears during sudo or polkit auth, the driver path worked and any
remaining failure is probably PAM policy or account lockout.

Host crash:

```text
Tudor host process died
pthread_create ... Resource temporarily unavailable
```

Install the current build and restart both services. The current code bounds
Windows-driver thread stacks so the host stays below the systemd sandbox data
limit.

Polkit authorization failure:

```text
Not Authorized: net.reactivated.fprint.device.enroll
```

Use `sudo fprintd-delete "$USER"` and `sudo fprintd-enroll "$USER"` for repair
work, or temporarily remove fingerprint auth from `/etc/pam.d/polkit-1`.

Faillock:

```sh
faillock --user "$USER"
sudo faillock --user "$USER" --reset
```

## Clean Reinstall Loop

When testing a new build:

```sh
meson compile -C build
sudo meson install -C build
sudo systemctl restart tudor-host-launcher fprintd
sudo faillock --user "$USER" --reset
sudo fprintd-delete "$USER"
sudo fprintd-enroll "$USER"
fprintd-verify "$USER"
```

Only test sudo or polkit after plain verify matches.
