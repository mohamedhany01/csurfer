---
trigger: always_on
---

# Core System Refactoring Rules

## Goal

Safely refactor the core after stabilizing outer layers.

---

# Rules

## 1. Refactor Core Last

Core systems must remain untouched until:

- Outer modules are stable
- Test coverage exists
- Dependencies are isolated

---

## 2. Protect Existing Behavior

Core changes require:

- Integration tests
- Regression tests
- Compatibility validation

---

## 3. Isolate Business Logic

Separate:

- Orchestration
- Infrastructure
- Domain logic

---

## 4. Avoid Large Refactors

Do not rewrite entire core systems at once.

Use incremental extraction.

---

## 5. Maintain Compatibility Layers

Temporary adapters are acceptable during migration.

---

## 6. Validate Stability Continuously

Every step must preserve runtime stability.