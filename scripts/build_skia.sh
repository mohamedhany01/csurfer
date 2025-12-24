#!/usr/bin/env bash
set -e

# Define directories
SKIA_DIR="$(cd "$(dirname "$0")/../external/skia" && pwd)"
OUT_DIR="$SKIA_DIR/out/Static"

echo "[INFO] Skia source directory: $SKIA_DIR"
echo "[INFO] Skia output directory: $OUT_DIR"

# # Clone Skia if the directory does not exist
# echo "[INFO] Checking if Skia is already cloned..."
# if [ ! -d "$SKIA_DIR/.git" ]; then
#     echo "[INFO] Skia not found. Cloning repository..."
#     git clone https://skia.googlesource.com/skia.git "$SKIA_DIR"
# else
#     echo "[INFO] Skia already cloned."
# fi

# Change to Skia directory
echo "[INFO] Changing directory to Skia source: $SKIA_DIR"
cd "$SKIA_DIR"

# # Update Skia repository
# echo "[INFO] Fetching latest changes from Skia repository..."
# git fetch
# git checkout "chrome/m144"
# echo "[INFO] Checked out branch chrome/m144"

# Sync dependencies
echo "[INFO] Syncing Skia dependencies..."
python3 tools/git-sync-deps
echo "[INFO] Dependencies synced."

# Install required system dependencies
echo "[INFO] Installing Skia system dependencies..."
set +e
bash ./tools/install_dependencies.sh
set -e
echo "[INFO] System dependencies installed."

# Generate build files using GN
echo "[INFO] Generating build files with GN..."
bin/gn gen "$OUT_DIR" --args='is_official_build=true is_component_build=false'
echo "[INFO] GN build files generated in $OUT_DIR"

# Build Skia using Ninja
echo "[INFO] Starting Skia build with Ninja..."
ninja -C "$OUT_DIR"
echo "[INFO] Skia build complete at $OUT_DIR"
