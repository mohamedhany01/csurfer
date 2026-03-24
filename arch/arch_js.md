# JavaScript Integration Architecture

This document describes the architecture of the JavaScript execution layer in `csurfer`.

## Overview
The JavaScript engine is integrated as a library (**Duktape**) that runs within the browser's process but in a separate execution context. The communication between C++ and JavaScript is handled via a bridge layer (`JSContext`).

## Components

### 1. JSContext (C++ Bridge)
The `JSContext` class is the main interface for the browser to interact with the JS engine.
- **Context Management**: Initializes and lifecycle-manages the `duk_context`.
- **Function Registration**: Exposes C++ functions to the JS global scope.
- **Handle Registry**: Maps C++ `Element` objects to integer IDs (handles) that JS can use.
- **Event Dispatching**: Orchestrates the flow of events from C++ to JS.

### 2. Runtime Script (JS side)
A built-in script (`assets/runtime.js`) is executed upon context initialization to provide a standard-ish DOM environment.
- **Polyfills**: Provides ES5-compatible `Node`, `Element`, and `HTMLDocument` prototypes (Duktape 2.x compatibility).
- **Encapsulation**: Wraps raw native calls like `innerHTML_set` into standard JS properties.

### 3. DOM Interaction Flow
1. **Query**: JS calls `document.querySelectorAll`.
2. **Native Call**: The bridge executes the browser's `CSSParser` and `tree_to_list` in C++.
3. **Identification**: C++ returns a list of handles (integers).
4. **Proxy**: JS wraps these handles in `Element` objects for the scripter.

### 4. HTML Parsing for Scripts
The `HTMLParser` treats content inside `<script>` and `<style>` tags as raw text until the closing tag, preventing syntax errors caused by HTML-like strings within JavaScript.

## Security & Isolation
- JS code never receives raw pointers to C++ memory.
- All DOM access is mediated through handles and type-safe bridge functions.
- The `JSContext` is cleared and re-initialized on page loads.
