#!/bin/bash
# Script to run clang-tidy on CSurfer source files

if [ ! -f build/compile_commands.json ]; then
    echo "[Error] build/compile_commands.json not found."
    echo "Please run: cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build"
    exit 1
fi

echo "[Lint] Running clang-tidy..."
# Find all cpp files and run clang-tidy using the compile commands database.
# Headers will be checked automatically via HeaderFilterRegex.
find src unit-tests -type f -name "*.cpp" -exec clang-tidy -p build -quiet {} +
