#!/bin/bash
# Script to run the Express test server for CSurfer

cd "$(dirname "$0")/../../pages/server" || exit

if [ ! -d "node_modules" ]; then
    echo "[Scripts] node_modules not found. Installing dependencies..."
    npm install
fi

echo "[Scripts] Starting Express server..."
node server.js
