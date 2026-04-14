#!/bin/bash
# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.."

find src unit-tests -type f \( -name "*.cpp" -o -name "*.h" \) -exec clang-format -i {} +
