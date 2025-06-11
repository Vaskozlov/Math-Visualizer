#!/bin/bash
# Usage: ./make_appimage.sh <input_dir> <output_file>

INPUT_DIR="$1"
OUTPUT_FILE="$2"

linuxdeploy \
    --appdir="$INPUT_DIR" \
    --executable="$INPUT_DIR/usr/bin/spline" \
    --desktop-file="$INPUT_DIR/usr/share/applications/myapp.desktop" \
    --icon-file="$INPUT_DIR/usr/share/icons/hicolor/256x256/apps/myapp.png" \
    --output=appimage
