---
trigger: always_on
---

# C Surfer Agent Directives

Welcome, AI Agent! When assisting with the `csurfer` repository, you MUST adhere to the following rules and guidelines. This ensures consistency, code quality, and a supportive learning environment.

## 1. Project Context & Mission
*   **Description**: C Surfer is a minimal web browser written in modern C++23 using SDL2. It parses HTML, fetches pages, and renders text in a simple window.
*   **Tech Stack**: C++23, SDL2, SDL2_ttf, OpenSSL, FreeType, CMake, Ninja, Clang++.

## 2. Architecture & Directory Layout
*   `src/`: Main source code divided by modules (`html`, `css`, `layout`, `browser`, `js`, etc.).
*   `arch/`: Architecture markdown diagrams and documentation.
*   `scripts/`: All bash automation scripts (`build`, `setup`, `test`, `quality`).
*   `unit-tests/`: CTest/GTest suite.
*   `assets/`: Fonts and static assets.
*   `pages/`: Sample HTML/CSS for testing.

## 3. Build & Development Workflow
*   **Always use the provided scripts**. Do not run raw `cmake` or `make` commands unless explicitly asked.
    *   **Build**: `./scripts/rebuild.sh`
    *   **Format**: `./scripts/quality/format.sh`
    *   **Lint**: `./scripts/quality/lint.sh`
    *   **Test**: `./scripts/test/unit.sh`

## 4. Mentorship & Communication Style (JS to C++ Transition)
*   **The User**: The user is a JavaScript expert transitioning to and learning modern C++23. **Treat this as a major advantage**.
*   **Draw Parallels**: Explain C++ concepts using JS equivalents where applicable (e.g., `std::shared_ptr` for predictable garbage collection, `std::ranges` as JS array methods like `.map()`/`.filter()`, `std::future` as Promises).
*   **Explain the "Why"**: Don't just dump C++ code. Briefly explain *why* a pattern (like RAII, value semantics) is used and how it differs from JS mechanics.
*   **Avoid C-isms**: Keep the user away from legacy C patterns (raw pointers, `new`/`delete`, manual memory management). Guide them towards modern, safe C++23 idioms.

## 5. Coding Conventions & Clean Code
*   **Global Naming Convention**:
    *   Classes/Structs/Enums: `PascalCase`
    *   Functions/Methods: `snake_case`
    *   Variables: `snake_case`
    *   Private Members: `snake_case_` (with a trailing underscore)
    *   Constants/Macros: `UPPER_SNAKE_CASE`
*   **Clean Code & Complexity**:
    *   **Small logic per file**: Keep files focused and concise.
    *   **Encapsulate**: Any complex flow must be encapsulated inside a class to hide its complexity.
    *   **Simplicity**: Use simple, readable logic over "clever" but complex logic.
    *   **Story Comments**: Write a comment block describing the "story" of each function/method (Why, How, and What) above its declaration/definition.
    *   **No Inner Comments**: Do not use inline comments inside the body of a method or function unless absolutely necessary to explain a non-obvious workaround.
    *   **No Namespace Pollution**: Never use `using namespace std;` in header files, and avoid it in source files. Always use explicit `std::` prefixes.

## 6. Error Management
*   **Exceptions**: Use exceptions (`try`, `catch`, `throw`) to control code flow and manage errors globally, rather than returning error codes or booleans.

## 7. Git & Branching Rules
*   **Branch Naming**: All new branches MUST follow the format `issue-number-change-name-or-what` using lowercase and hyphens (`-`) instead of spaces. (e.g., `12-add-tab-manager`).
*   **Commit Constraints**:
    *   Keep commits atomic and small.
    *   The commit header must be all lowercase and concise.
    *   **Body is REQUIRED**: Do not commit without a body. The body must contain the "story" of the commit: what was changed, why it was changed, and how the enhancement was implemented.
*   **File Operations**:
    *   When moving files, use `git mv` instead of `mv`.
    *   When deleting files, use `git rm` instead of `rm`.

## 8. AI-Specific Rules
*   **Do not break the build**: Compile and run tests after significant changes.
*   **Incremental changes**: Prefer small, atomic edits.
*   **No placeholders**: Write complete, working code. Never output `// ... implement here ...`.
*   **Mandatory Brainstorming Phase**: Always start any task with a brainstorming session. DO NOT write or modify any code until the user explicitly confirms and says "YES". If they haven't explicitly said "YES", assume you are still in the thinking/brainstorming phase and no concrete solution is authorized yet.
*   **Keep Architecture Docs Updated**: If a change significantly alters the flow, data structures, or introduces a new component, update the relevant markdown files in the `arch/` directory to keep the documentation in sync with the codebase.
*   **CMake Integration**: When adding new source files (`.cpp`), always ensure they are added to the corresponding `CMakeLists.txt` file so they are included in the build.