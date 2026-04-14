#!/bin/bash

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/../.."

echo "--- CSurfer Docker Cleanup ---"

# 1. Stop and remove containers, networks, and volumes defined in compose
echo "[1/3] Stopping and removing containers/networks..."
docker-compose down --volumes --remove-orphans

# 2. Remove the specific dev image
echo "[2/3] Removing csurfer-dev-image..."
docker rmi csurfer-dev-image 2>/dev/null || echo "Image already removed."

# 3. Clean up the Docker build directory
echo "[3/3] Removing build-docker directory..."
rm -rf build-docker

echo "--- Cleanup Complete! ---"
