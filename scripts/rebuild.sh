#!/bin/bash

# Exit on error
set -e

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

# Detect if we are running in Docker
if [ -f /.dockerenv ]; then
    BUILD_DIR="build-docker"
    echo "[Info] Running in Docker. Using build directory: $BUILD_DIR"
else
    BUILD_DIR="build"
fi

# We are not removing the build directory here to preserve the CMake cache.
# If you need a fully clean state, use 'cmake --build --preset default --target clean' instead.

echo "Configuring with preset 'default'..."
cmake --preset default -B "$BUILD_DIR"

echo "Building with preset 'default'..."
cmake --build "$BUILD_DIR"

# Create symlink for compile_commands.json to help LSP (clangd, etc.)
if [ -f "$BUILD_DIR/compile_commands.json" ]; then
    ln -sf "$BUILD_DIR/compile_commands.json" .
    echo "Symlinked $BUILD_DIR/compile_commands.json to root."
fi

echo "Rebuild complete!"
