#!/bin/bash -e

HASH_FILE="$1"
TMP_DIR="$2"
OUT_DIR="$3"
DLLS=${@:4}

mkdir -p "$TMP_DIR"

#Download the driver executable and check hash
INSTALLER="$TMP_DIR/installer.exe"
wget https://download.lenovo.com/consumer/mobiles/xalc0l0fqu9mwjc0.exe -O "$INSTALLER"
sha256sum "$INSTALLER" | cut -d" " -f1 | cmp - "$HASH_FILE"

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