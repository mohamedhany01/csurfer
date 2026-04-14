#!/bin/bash
# Script to run clang-tidy and apply safe fixes

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.."

# Detect build directory
if [ -f /.dockerenv ]; then
    BUILD_DIR="build-docker"
else
    BUILD_DIR="build"
fi

if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    echo "[Error] $BUILD_DIR/compile_commands.json not found. Run ./scripts/rebuild.sh first."
    exit 1
fi

echo "[Lint] Applying automatic fixes using $BUILD_DIR..."
find src unit-tests -type f -name "*.cpp" -exec clang-tidy -p "$BUILD_DIR" --fix --fix-errors -quiet {} +

echo "[Done] Automatic fixes applied. Please review changes before committing."
