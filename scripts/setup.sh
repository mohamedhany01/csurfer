#!/bin/bash

# Exit on error
set -e

echo "--- CSurfer Host Environment Setup ---"

# Detect Distro
if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$NAME
else
    DISTRO="Unknown"
fi

echo "[Info] Detected: $DISTRO"

# Skip confirmation if running in CI
if [ "$CI" = "true" ]; then
    echo "[Info] CI detected, skipping confirmation."
else
    echo "[Caution] This script will modify your physical HOST machine."
    echo "If you want isolation, use Docker instead: ./scripts/docker/run.sh"
    read -p "Do you want to proceed with Host Installation? (y/N) " confirm
    if [[ $confirm != [yY] && $confirm != [yY][eE][sS] ]]; then
        echo "[Abort] No changes made to your host system."
        exit 0
    fi
fi

# Detect OS
if [ -f /etc/debian_version ]; then
    OS="debian"
    INSTALL_CMD="sudo apt update || echo '[Warning] apt update had some issues, trying to proceed...'; sudo apt install -y"
    PKGS="build-essential cmake ninja-build clang clang-tidy clang-format libssl-dev libsdl2-dev libsdl2-ttf-dev libfreetype-dev"
elif [ -f /etc/fedora-release ]; then
    OS="fedora"
    INSTALL_CMD="sudo dnf install -y"
    PKGS="gcc-c++ cmake ninja-build clang clang-tools-extra openssl-devel SDL2-devel SDL2_ttf-devel freetype-devel"
else
    echo "[Error] Unsupported OS. Please install dependencies manually."
    exit 1
fi

echo "[Info] Detected $OS system."
echo "[Info] Installing: $PKGS"

# Run install
eval "$INSTALL_CMD $PKGS"

echo "--- Setup Complete! ---"
echo "You can now run: ./scripts/rebuild.sh"
