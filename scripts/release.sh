#!/bin/bash
# Script to automate GitHub tagging and trigger the release workflow

# Get the script directory and move to project root
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR/.."

# Check for uncommitted changes
if ! git diff-index --quiet HEAD --; then
    echo "[Error] You have uncommitted changes. Please commit or stash them first."
    exit 1
fi

# Ask for version
read -p "Enter version (e.g., v1.0.0): " version

if [[ ! $version =~ ^v[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "[Error] Version must follow vX.X.X format."
    exit 1
fi

# Check if tag already exists
if git rev-parse "$version" >/dev/null 2>&1; then
    echo "[Error] Tag $version already exists."
    exit 1
fi

echo "[Release] Creating tag $version..."
git tag -a "$version" -m "Release $version"

echo "[Release] Pushing tag to origin..."
git push origin "$version"

echo "[Done] Workflow triggered! Check your GitHub Actions tab."
