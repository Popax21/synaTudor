#!/usr/bin/env bash
#
# tudor-probe.sh — automate the synaTudor "diagnose the next missing thing" loop.
#
# Builds the relinker with debug stubs (DBGWDF/DBGIMPORT), runs tudor_cli against the
# 06cb:00fd sensor, and reports the FIRST unresolved WDF index or Win32 import, plus a
# backtrace if it crashes.
#
# Usage:   sudo ./tools/tudor-probe.sh [path-to-synaTudor-src]
# Needs:   meson ninja innoextract gdb; root (opens USB then drops caps).
#
set -u

SRC="${1:-$HOME/synaTudor}"
B="$SRC/build"
PID_USB="00fd"
STORE="/tmp/tudor-store"
YES="/tmp/tudor-yes.txt"
OUT="/tmp/tudor-probe.out"

cd "$SRC" || { echo "no such source dir: $SRC"; exit 1; }
for t in meson ninja innoextract gdb; do
    command -v "$t" >/dev/null || { echo "missing tool: $t"; exit 1; }
done

echo "[*] configuring debug build ..."
if [ -d "$B" ]; then
    meson configure "$B" -DDBGWDF=true -DDBGIMPORT=true -DUNMOUNTFS=false >/dev/null
else
    meson setup "$B" -DDBGWDF=true -DDBGIMPORT=true -DUNMOUNTFS=false >/dev/null
fi

echo "[*] building ..."
ninja -C "$B" >/dev/null 2>&1 || { echo "[!] build failed:"; ninja -C "$B"; exit 1; }
ninja -C "$B" install >/dev/null || { echo "[!] install failed"; exit 1; }
systemctl daemon-reload 2>/dev/null || true

printf 'y\nq\n' > "$YES"

echo "[*] running tudor_cli -P$PID_USB (root, gdb) ..."
gdb -batch -q \
    -ex "run < $YES" \
    -ex "bt" \
    --args /sbin/tudor/tudor_cli "$STORE" -P"$PID_USB" > "$OUT" 2>&1

echo
echo "==================== RESULT ===================="
FWDF=$(grep -aoE "Unresolved WDF function [0-9]+" "$OUT" | head -1)
FIMP=$(grep -aoE "Unresolved import [^ ]+" "$OUT" | head -1)
if [ -n "$FWDF" ]; then
    echo "[WALL] $FWDF"
    echo "       -> map the index in microsoft/.../wdffuncenum.h, then:"
    echo "          __winfnc NTSTATUS Impl(WDF_DRIVER_GLOBALS*, ...) { ... }"
    echo "          WDFFUNC(Impl, <index>)      # in libtudor/src/winapi/wdf/*.c"
elif [ -n "$FIMP" ]; then
    echo "[WALL] $FIMP"
    echo "       -> add to libtudor/src/winapi/ :"
    echo "          __winfnc <ret> Name(<args>) { ... }"
    echo "          WINAPI(Name)"
else
    if grep -q "Commands:" "$OUT"; then
        echo "[OK] driver reached the command prompt — enrollment stage!"
    else
        echo "[?] no unresolved call found; inspect output (maybe a crash/hang):"
    fi
fi

echo
echo "---------- last 15 lines ----------"
grep -vE "unsupported (resource|exception) data directory" "$OUT" | tail -15
echo "-----------------------------------"
