#!/bin/bash -e

HASH_FILE="$1"
TMP_DIR="$2"
OUT_DIR="$3"
DLLS=${@:4}

mkdir -p "$TMP_DIR"

#Download the driver executable and check hash
INSTALLER="$TMP_DIR/installer.exe"
DRIVER_URL="https://download.lenovo.com/pccbbs/mobiles/r19fp02w.exe"
if command -v wget >/dev/null 2>&1; then
    wget "$DRIVER_URL" -O "$INSTALLER"
else
    curl -fL "$DRIVER_URL" -o "$INSTALLER"
fi
sha1sum "$INSTALLER" | cut -d" " -f1 | cmp - "$HASH_FILE"

#Extract the driver
WINDRV="$TMP_DIR/windrv"
mkdir -p "$WINDRV"
innoextract -d "$WINDRV" "$INSTALLER"

#Copy outputs
mkdir -p "$OUT_DIR"
for dll in $DLLS
do
    cp $(find "$WINDRV" -name "$dll") "$OUT_DIR/$dll"
done