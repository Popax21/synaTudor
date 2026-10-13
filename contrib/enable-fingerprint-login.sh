#!/usr/bin/env bash
#
# enable-fingerprint-login.sh
#
# Wire fprintd into PAM so fingerprint works for:  login, sudo, the display manager
# (GDM) and the lock/suspend screen (gtklock on Regolith). Plus, on systems using the
# synaTudor relinked Synaptics "Tudor" driver, install a boot-time warm-up service.
#
# Run as root:   sudo ./enable-fingerprint-login.sh
#
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

[ "$(id -u)" -eq 0 ] || { echo "Please run as root (sudo)."; exit 1; }

echo "==> Enabling fprintd in the PAM stack (common-auth)"
# Adds 'auth [success=3 default=ignore] pam_fprintd.so max-tries=1 timeout=10' to
# /etc/pam.d/common-auth, which is included by login, gdm-password/handling, gtklock, etc.
if [ -f /usr/share/pam-configs/fprintd ]; then
    DEBIAN_FRONTEND=noninteractive pam-auth-update --enable fprintd
    echo "    fprintd PAM profile enabled."
else
    echo "    WARNING: /usr/share/pam-configs/fprintd not found (is libpam-fprintd installed?)"
fi

echo "==> Installing boot-time warm-up service (Synaptics Tudor relink)"
if [ -f "$HERE/tudor-warm.service" ]; then
    install -m 0644 "$HERE/tudor-warm.service" /etc/systemd/system/tudor-warm.service
    systemctl daemon-reload
    systemctl enable --now tudor-warm.service
    echo "    tudor-warm.service installed and started."
else
    echo "    (tudor-warm.service not found next to this script; skipping)"
fi

echo
echo "Done."
echo "Test:"
echo "  - log out / reboot -> finger prompt at the login screen"
echo "  - lock the screen (or suspend/resume) -> finger prompt on the lock screen"
echo "  - terminal: 'sudo -k; sudo true' -> finger prompt"
echo "Password still works as a fallback on every prompt."
