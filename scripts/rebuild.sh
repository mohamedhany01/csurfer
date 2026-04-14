#!/bin/bash

# Exit on error
set -e

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

# We are not removing the build directory here to preserve the CMake cache.
# If you need a fully clean state, use 'cmake --build --preset default --target clean' instead.

echo "Configuring with preset 'default'..."
cmake --preset default

echo "Building with preset 'default'..."
cmake --build --preset default

# Create symlink for compile_commands.json to help LSP (clangd, etc.)
if [ -f "build/compile_commands.json" ]; then
    ln -sf build/compile_commands.json .
    echo "Symlinked compile_commands.json to root."
fi

echo "Rebuild complete!"
