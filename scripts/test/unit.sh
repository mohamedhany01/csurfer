#!/bin/bash

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.."

echo "[Tests] Running unit tests via CTest..."

if [ ! -d "build" ]; then
    echo "[Tests] Build directory not found. Running rebuild.sh first..."
    ./scripts/rebuild.sh
fi

cd build
ctest --output-on-failure

if [ $? -eq 0 ]; then
    echo "[Tests] ALL TESTS PASSED!"
    exit 0
else
    echo "[Tests] SOME TESTS FAILED!"
    exit 1
fi
