---
trigger: always_on
---

# Testability and Dependency Refactoring Rules

## Goal

Improve isolation, testability, and decoupling.

---

# Rules

## 1. Depend on Abstractions

Prefer interfaces over concrete implementations.

Bad:

```cpp
Database database;
```

Good:

```cpp
IDatabase database;
```

Note: apply Liskov Substitution Principle (LSP) if possible
---

## 2. Use Dependency Injection

Dependencies should be injected explicitly.

Avoid hidden construction inside classes.

---

## 3. Separate Side Effects

Separate:

- Pure logic
- IO operations
- State mutation

---

## 4. Improve Determinism

Functions should produce predictable results.

Avoid hidden runtime dependencies.

---

## 5. Isolate Tests

Each module must have isolated tests.

Avoid giant shared test files.

---

## 6. Cover Core and Corner Cases

Every new abstraction requires:

- Normal flow tests
- Edge case tests
- Failure scenario tests