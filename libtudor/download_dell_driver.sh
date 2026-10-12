#!/bin/bash -e

HASH_FILE="$1"
TMP_DIR="$2"
OUT_DIR="$3"
DLLS=${@:4}

mkdir -p "$TMP_DIR"

#Download Dell's driver package (Synaptics 6.0.18.1103 A06, release 3PFJG) and check hash
INSTALLER="$TMP_DIR/installer.exe"
wget https://downloads.dell.com/FOLDER12308597M/1/Synaptics-Fingerprint-Sensor-Driver_3PFJG_WIN64_6.0.18.1103_A06.EXE -O "$INSTALLER"
sha256sum "$INSTALLER" | cut -d" " -f1 | cmp - "$HASH_FILE"

#Extract the driver (the package is a self-extracting zip)
WINDRV="$TMP_DIR/windrv"
mkdir -p "$WINDRV"
unzip -q -o "$INSTALLER" -d "$WINDRV"

#Copy outputs
mkdir -p "$OUT_DIR"
for dll in $DLLS
do
    cp $(find "$WINDRV" -name "$dll") "$OUT_DIR/$dll"
done
