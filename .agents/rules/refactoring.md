---
trigger: always_on
---

# Refactoring Rules

These rules govern ALL refactoring work. They prepare the codebase for threading while keeping the system stable.

## 1. Strategy: Leaves-to-Core

Refactor from modules with zero dependents inward. Never refactor core while leaves are messy.

**Phase 1 — Leaves:** `utils/`, `Rect`, `LayoutConstants`, `gfx/Color.h`, `url/Url`, `css/DefaultStyles.h`, `css/CSSRule.h`
**Phase 2 — Middle:** `lexer/` → `dom/`, `css/CSSSelector`, `css/CSSParser`, `request/`, `gfx/Font`, `HttpRequest`
**Phase 3 — Integration:** `StyleComputation`, `StyleEngine`, `DisplayItem`, `TextLayout`/`LineLayout`/`InputLayout`, `BlockLayout`, `HTMLParser`
**Phase 4 — Core:** `JSContext`, `DocumentLayout`/`LayoutTree`, `CSurferUI`, `Tab`, `Browser`
**Phase 5 — Infrastructure:** Unified logging, centralized config, per-module CMakeLists, full GTest suite, arch docs

- Never skip phases. Phase N+1 starts only after Phase N is complete and verified.
- Each phase must end with all tests passing and the system fully working.
- Backward compatibility is mandatory within each phase.

## 2. Commits

- **Atomic and small.** One logical change per commit. Never mix rename + extract in one commit.
- **System must work** after every commit. Build and tests must pass.
- **No behavioral changes** unless explicitly planned and documented.
- **Header**: all lowercase, concise. **Body (REQUIRED)**: Overview, Why, How, Outcome.
- **Before every commit**: run `./scripts/quality/format.sh` and `./scripts/test/unit.sh`.

## 3. Documentation

- Update `arch/` diagrams when module boundaries, data flow, or components change.
- Add story comments (Why, How, What) above every new or modified function.
- Document every `unique_ptr`/`shared_ptr`/`weak_ptr` with a comment explaining ownership choice.
- Inner comments only when logic is genuinely complex.

## 4. Module Boundaries

- Each `src/` subdirectory is a module with one domain responsibility.
- Modules depend on abstractions (interfaces), not concrete implementations.
- Prefer composition over inheritance unless polymorphic dispatch is needed.
- Target layout: `browser/`, `config/`, `css/`, `dom/` (renamed from `lexer/`), `gfx/`, `html/`, `js/`, `layout/`, `net/` (renamed from `request/`), `url/`, `utils/`.

## 5. Class Design

- **SRP**: Each class has exactly one reason to change.
- **Max 2 parameters** per function. Wrap extras in a config struct.
- **Deterministic**: Same inputs → same outputs whenever possible.
- **No hidden side effects.** Document any unavoidable ones.
- **Prefer immutability** for shared/thread-sensitive state.
- **Modern C++**: Use modern C++23 features. Remove deprecated or non-safe C-style functions (e.g., prefer `std::format` or `std::stringstream` over `snprintf`).
- **Simplify Logic**: Always simplify the logic of a utility/helper. Prefer small, focused methods over monolithic ones.
- **Built-in First**: If a standard library method exists for our logic, use it instead of writing custom code.

## 6. Naming Conventions

| Element | Convention | Example |
|---|---|---|
| Classes/Structs/Enums | `PascalCase` | `BlockLayout` |
| Functions/Methods | `snake_case` | `parse_csp()` |
| Variables | `snake_case` | `scroll_step` |
| Private Members | `snake_case_` | `url_` |
| Constants/Macros | `UPPER_SNAKE_CASE` | `SCROLL_STEP` |
| Namespaces | `snake_case` | `gfx` |

### Explicit Naming Rules

- **No Abbreviations**: Never use abbreviations in class names, function names, or variable names (e.g., `is_chinese_japanese_korean` instead of `isCJK`, `character` instead of `ch`, `index` instead of `i`).
- **Local Readability**: Variable names inside functions must be full words to ensure readability (e.g., `buffer` instead of `buf`).
- **Story Requirement**: Every helper, utility, and function must have a "Story" comment block above its declaration explaining its purpose, use-case, and why it's necessary.
- **Exceptions for Abbreviations**: If an abbreviation is a widely recognized technical standard (e.g., `UTF8`, `URL`, `HTML`), it may be used, but a Story must accompany its first appearance.

## 7. Duplication

- **Zero tolerance.** Extract any repeated logic into a utility.
- Before writing new code, check if the logic already exists.
- Known duplicates to extract: `to_lower()` (3 copies), `trim()` (3 copies), BFS/DFS traversal (4 copies), font-size px parsing (3 copies), `duk_push_global_stash` pattern (7 copies).

## 8. Dependency Injection

- Program against interfaces, not implementations.
- Use constructor injection. Dependencies passed in, not created internally.
- Break circular dependencies with interfaces or event callbacks.
- Known cycles to break: `CSurferUI ↔ Browser` (add `IBrowserCommands`), `JSContext ↔ Tab` (add `IJSHost`).

## 9. Error Handling & Logging

- Use exceptions for error flow, not return codes.
- Never `catch(...)` silently — log, re-throw, or handle meaningfully.
- Never use `assert()` for runtime validation.
- Use the unified logging macros (`ENABLE_LOGGING`/`LOG_LEVEL_*`), not raw `std::cout`/`std::cerr`.

## 10. Testing

- Every refactored module MUST have GTest unit tests.
- Follow AAA pattern: Arrange, Act, Assert — clearly separated.
- Tests must be isolated (no shared state between tests).
- Cover: core behavior, edge cases, error paths, regressions.

## 11. Thread-Safety Preparation

- Prefer immutability for any potentially shared data.
- Use `std::atomic` for flags (e.g., `running_`).
- Use `std::weak_ptr` instead of raw pointer cycles across ownership boundaries.
- Isolate side effects: reading should not also write to disk.

## 12. Magic Numbers

- Zero tolerance. Every numeric literal → a named `inline constexpr` in `src/config/`.
- Known: `800` (width), `600` (height), `16` (font size), `100` (scroll step), `200` (input width), `12` (scrollbar width), `4096` (buffer size), `150` (tab width).

## 13. Cleanup

- Remove unused methods, commented-out code, and stale comments ("Chapter 8", "Stage 1.2").
- Move debug utilities (`debug_print_layout_tree`) to debug-only targets or delete.
- Wire up or remove unused CMake macros (`ENABLE_LOGGING`/`LOG_LEVEL_*`).
