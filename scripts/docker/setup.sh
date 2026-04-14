#!/bin/bash

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.."

echo "--- CSurfer Docker Environment Setup ---"
echo "[Info] This will build the container environment without modifying your host system."

# Get current user details for correct permissions in container
export USER_ID=$(id -u)
export GROUP_ID=$(id -g)

echo "[Docker] Building image (this may take a few minutes)..."
docker-compose build

echo "--- Docker Setup Complete! ---"
echo "You can now run: ./scripts/docker/run.sh"
