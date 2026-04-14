#!/bin/bash

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.."

# Get current user details
export USER_ID=$(id -u)
export GROUP_ID=$(id -g)

echo "[Docker] Building image..."
docker-compose build

echo "[Docker] Enabling X11 passthrough..."
xhost +local:docker > /dev/null

echo "[Docker] Launching container..."
docker-compose run --rm csurfer-service ./scripts/dev.sh
