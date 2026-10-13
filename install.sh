#!/usr/bin/env bash
#
# install.sh - one-command install for the Synaptics 06cb:00fd fingerprint reader
# on Linux (Ubuntu/Debian based).
#
# It builds synaTudor (the runtime relinker for Synaptics' Windows driver) from the
# 06cb-00fd branch, installs the libfprint TOD module + host launcher + udev rule,
# installs a boot-time warm-up service, and enables fingerprint auth for login /
# lock screen / sudo.
#
# Usage:   sudo ./install.sh
#
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="${REPO:-https://github.com/Manasi-ish/synaTudor.git}"
BRANCH="${BRANCH:-06cb-00fd}"
SRC="${SRC:-/root/synaTudor-00fd}"

[ "$(id -u)" -eq 0 ] || { echo "Please run as root:  sudo ./install.sh"; exit 1; }

echo "==> Installing build dependencies"
apt-get update -qq
apt-get install -y --no-install-recommends \
    git build-essential meson ninja-build pkg-config innoextract wget ca-certificates \
    libglib2.0-dev libusb-1.0-0-dev libssl-dev libcap-dev libseccomp-dev \
    libdbus-1-dev libsystemd-dev libudev-dev libjson-glib-dev libgusb-dev \
    libfprint-2-dev libfprint-2-tod-dev fprintd

echo "==> Fetching synaTudor ($BRANCH)"
if [ -d "$SRC/.git" ]; then
    git -C "$SRC" fetch origin "$BRANCH"
    git -C "$SRC" checkout "$BRANCH"
    git -C "$SRC" pull --ff-only
else
    git clone -b "$BRANCH" "$REPO" "$SRC"
fi

echo "==> Building (downloads the Lenovo IdeaPad driver package to extract the DLLs)"
cd "$SRC"
rm -rf build
meson setup build -DUNMOUNTFS=true
ninja -C build

echo "==> Installing (TOD module, host launcher, udev rule)"
ninja -C build install
udevadm control --reload
udevadm trigger

echo "==> Boot-time warm-up service (driver needs ~4s to load before GDM/lock ask)"
install -m 0644 "$HERE/tudor-warm.service" /etc/systemd/system/tudor-warm.service
systemctl daemon-reload
systemctl enable --now tudor-warm.service || true

echo "==> Enabling fingerprint for login / lock screen / sudo (PAM)"
if [ -f /usr/share/pam-configs/fprintd ]; then
    DEBIAN_FRONTEND=noninteractive pam-auth-update --enable fprintd
fi

systemctl restart fprintd 2>/dev/null || true

echo
echo "Done."
echo "Now enroll a finger:"
echo "    fprintd-enroll"
echo "(touch the sensor ~8-10 times, centred and flat)"
