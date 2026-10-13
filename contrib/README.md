# contrib — making a relinked Synaptics sensor actually usable day-to-day

This folder contains the small system-integration pieces that turn a *working* synaTudor
device into something you can use for **login, lock screen and sudo** — validated on a
Synaptics `06cb:00fd` (Lenovo IdeaPad 3 14ITL6, Ubuntu 26.04, Regolith/sway, GDM).

## TL;DR

```bash
# 1) build & install the driver (TOD module, launcher, udev rule)
meson setup build -DUNMOUNTFS=true
ninja -C build && sudo ninja -C build install
sudo udevadm control --reload && sudo udevadm trigger

# 2) enroll a finger
fprintd-enroll            # touch the sensor ~8-10x, centred, flat

# 3) enable login / lock-screen / sudo integration
sudo ./contrib/enable-fingerprint-login.sh
```

## Why these extra pieces are needed

The relinked driver works, but two integration gaps make it *feel* broken:

1. **Boot-time race (fixed in the driver).** The Tudor host aborted with
   `Attempted multiple different sensor pairing data loads!` whenever the *same* sensor's
   pairing data was loaded twice — which happens as soon as the host is **reused** for a
   second login (boot warm-up → GDM, or first login → next login). Symptom: fingerprint
   works **once**, then just stops. Fix: only abort for a *different* sensor.

2. **Device not ready when the login/lock screen asks.** The driver needs ~4 s to load
   its Windows DLLs and pair with the sensor. GDM/gtklock query the device the moment
   they appear, so you get “no fingerprint device”. Fix: `tudor-warm.service` pre-warms
   the device at boot, before the display manager.

3. **Lock screen (gtklock) had no fingerprint.** gtklock authenticates via PAM
   (`gtklock → login → common-auth`). If `pam_fprintd` isn't in the stack, the lock
   screen only offers a password. Fix: enable the `fprintd` PAM profile
   (`pam-auth-update --enable fprintd`).

## Files

- `tudor-warm.service` — boot-time warm-up (install to `/etc/systemd/system/`).
- `enable-fingerprint-login.sh` — enables the fprintd PAM profile and installs the warm
  service. Run as root.

## Verifying

```bash
fprintd-list "$USER"        # -> found 1 devices
fprintd-enroll              # enroll (or add another finger)
# then: lock the screen / log out -> "Place your finger"
```

## Notes / caveats

- `pam_fprintd` uses `max-tries=1 timeout=10` — if your finger doesn't match it falls
  back to the password automatically. Remove/reorder in `/etc/pam.d/common-auth` if you
  prefer a different balance.
- The relink approach runs Synaptics' proprietary Windows binaries; see the project
  disclaimer. It is x86-64 only.
