#!/bin/bash

set -e

# This is not dynamic. I'm hardcoding this for my pico
PICO_DEV="/dev/disk/by-label/RPI-RP2"

UF2_FILE="./build/picoweather.uf2"
MNT_DIR="/mnt"

if [ ! -f "$UF2_FILE" ]; then
    echo "Could not find UF2 file '$UF2_FILE'"
    exit 1
fi

if [[ -n "$1" ]]; then
    MNT_DIR = "$1"
fi

mount "$PICO_DEV" "$MNT_DIR"
cp "$UF2_FILE" "$MNT_DIR"
umount "$MNT_DIR"
