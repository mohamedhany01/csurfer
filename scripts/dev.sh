#!/bin/bash

# Exit on error
set -e

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

# Run the rebuild script
./scripts/rebuild.sh

# Detect build directory
if [ -f /.dockerenv ]; then
    BUILD_DIR="build-docker"
else
    BUILD_DIR="build"
fi

echo "Launching csurfer..."
"./$BUILD_DIR/csurfer" "$@"
