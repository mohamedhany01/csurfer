#!/usr/bin/env bash
set -e

# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default values
PORT=${1:-8000}
PAGES_DIR="$PROJECT_ROOT/pages"

# Check if pages directory exists
if [ ! -d "$PAGES_DIR" ]; then
    echo "[INFO] Creating pages directory: $PAGES_DIR"
    mkdir -p "$PAGES_DIR"
fi

# Change to the pages directory
cd "$PAGES_DIR"

echo "[INFO] Starting Python HTTP server on port $PORT"
echo "[INFO] Serving directory: $PAGES_DIR"
echo "[INFO] Server URL: http://127.0.0.1:$PORT"
echo ""
echo "Press Ctrl+C to stop the server"

# Run Python's built-in HTTP server
python3 -m http.server "$PORT" --bind 127.0.0.1
