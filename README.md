# C Surfer

A minimal web browser written in C++ using SDL2. Parse HTML, fetch pages, and render text in a simple window.

## Overview

**Get started quickly** - C Surfer demonstrates core browser concepts: HTML parsing, HTTP requests, and text rendering. 
**Now with Multi-Tab support!** Built with modern C++23 and SDL2.

## Installation

**Build from source** - Currently tested on **GNU/Linux** only. Requires CMake 3.28+ and standard development libraries.

### Quick Start (Linux)

Get your environment ready with one command. Supports **Debian/Ubuntu** and **Fedora**:

```bash
./scripts/setup.sh
```

### Docker (Hermetic Build)

If you prefer a completely isolated environment, you can build and run CSurfer using Docker. This handles all dependencies and GUI passthrough for you:

```bash
./scripts/docker-run.sh
```

### Manual Installation
If your distro is not supported by the script, you will need:
*   **Compiler**: Clang++ (Standard C++23 support)
*   **Build Tools**: CMake 3.28+, Ninja
*   **Libraries**: OpenSSL, SDL2, SDL2_ttf, FreeType

### Build

**Recommended method (using scripts):**
```bash
./scripts/rebuild.sh   # Configures and builds using Clang++ & Ninja
```

**Manual method (using Presets):**
```bash
cmake --preset default
cmake --build --preset default
```

The executable `csurfer` will be located in the `build/` directory.

## Usage

**Run anytime** - Launch the browser without arguments to see the welcome page, or pass a URL.

```bash
./csurfer                     # Opens about:welcome
./csurfer https://example.com  # Opens a specific site
```

The browser opens an 800x600 window.

**Controls & Shortcuts:**
*   **Address Bar**: Click to focus, then type a URL and press **Enter** to navigate.
*   **Tab Management**:
    *   **+ Button**: Open a new tab (`about:welcome`).
    *   **Tab Click**: Switch between open tabs.
    *   **x Button**: Close the specific tab.
*   **Navigation**:
    *   **< Button**: Go back in the current tab's history.
    *   **UP / DOWN Arrows** or **Mouse Wheel**: Scroll the page vertically.
    *   **Left Click**: Interact with links AND form elements (inputs/buttons).
    *   **Keyboard**: Type into focused input fields; use backspace to edit.

## Configuration

Fonts are loaded from `assets/fonts/`. Modify `Browser.cpp` to change window size or font path.

## Examples

**Test with local files** - Use the Express v5 test server to verify form submissions and static pages.

```bash
# Start Express test server (default port 8000)
# Requires Node.js and npm
./scripts/run_server.sh

# Then browse
./build/csurfer http://localhost:8000/ch8-login.html
```

See `pages/` directory for sample HTML files, including Chapter 8 interactive tests.

## Development

### Development Scripts
*   **Root Scripts**:
    *   **`./scripts/setup.sh`**: Installs dependencies on your **HOST machine** (Linux Mint, Ubuntu, Fedora).
    *   **`./scripts/rebuild.sh`**:     Incremental build with automatic LSP support.
    *   **`./scripts/dev.sh`**:         Builds and launches the project on the host machine.
    *   **`./scripts/release.sh`**:     Tags the current commit and triggers a GitHub Release.
*   **Docker Tools** (`./scripts/docker/`):
    *   **`./setup.sh`**: Installs dependencies inside the **isolated Docker container**.
    *   **`./run.sh`**: Hermetic build and run with GUI passthrough.
    *   **`./clean.sh`**: Removes container, images, and cached volumes.
*   **Quality Tools** (`./scripts/quality/`):
    *   **`./format.sh`**: Runs `clang-format` on all source files.
    *   **`./lint.sh`**: Runs `clang-tidy` (requires `compile_commands.json`).
    *   **`./fix.sh`**: Automatically applies safe lint fixes.
*   **Test Tools** (`./scripts/test/`):
    *   **`./unit.sh`**: Runs all unit tests.
    *   **`./scripts/test/server.sh`**: Starts the Express test server.

### Releases

This project uses GitHub Actions to automate binary releases. To create a new release:

1.  **Run the release script**:
    ```bash
    ./scripts/release.sh
    ```
2.  **Enter the version**: When prompted, enter a version tag starting with `v` (e.g., `v1.0.0`).
3.  **Check GitHub**: A new release will be created automatically with the optimized `csurfer` binary attached.

### LSP / IDE Support
The root `compile_commands.json` is automatically managed by the build scripts. If you use `clangd` or VS Code, it should work out of the box.

## License

**GPL v3** - See [LICENSE](LICENSE) for details. Free software under the GNU General Public License version 3.
