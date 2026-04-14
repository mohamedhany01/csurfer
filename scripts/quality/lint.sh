#!/bin/bash
# Script to run clang-tidy on CSurfer source files

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
    echo "[Error] $BUILD_DIR/compile_commands.json not found."
    echo "Please run: ./scripts/rebuild.sh"
    exit 1
fi

if ! command -v clang-tidy &> /dev/null; then
    echo "[Error] clang-tidy not found."
    echo "Please install it using: sudo apt install clang-tidy"
    exit 1
fi

echo "[Lint] Running clang-tidy on $BUILD_DIR..."
find src unit-tests -type f -name "*.cpp" -exec clang-tidy -p "$BUILD_DIR" -quiet {} +
