# C Surfer

A minimal web browser written in C++ using SDL2. Parse HTML, fetch pages, and render text in a simple window.

## Overview

**Get started quickly** - C Surfer demonstrates core browser concepts: HTML parsing, HTTP requests, and text rendering. 
**Now with Multi-Tab support!** Built with modern C++23 and SDL2.

## Installation

**Build from source** - Currently tested on **GNU/Linux** only. Requires CMake 3.28+ and standard development libraries.

### Dependencies

```bash
# Debian/Mint/Ubuntu
sudo apt install build-essential cmake libssl-dev libsdl2-ttf-dev libfreetype-dev
```

### Build

```bash
mkdir build && cd build
cmake ..
make -j8
```

The executable `c_surfer` will be in the `build/` directory.

## Usage

**Run anytime** - Launch the browser without arguments to see the welcome page, or pass a URL.

```bash
./c_surfer                     # Opens about:welcome
./c_surfer https://example.com  # Opens a specific site
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
    *   **UP / DOWN Arrows**: Scroll the page vertically.
    *   **Left Click**: Interact with links on web pages.

## Configuration

Fonts are loaded from `assets/fonts/`. Modify `Browser.cpp` to change window size or font path.

## Examples

**Test with local files** - Use the included script to start a test server.

```bash
# Start test server (default port 8000)
./scripts/run_test_server.sh

# Or specify a custom port
./scripts/run_test_server.sh 8080

# Then browse
./c_surfer http://localhost:8000/simple.html
```

See `pages/` directory for sample HTML files.

## License

**GPL v3** - See [LICENSE](LICENSE) for details. Free software under the GNU General Public License version 3.
