---
trigger: always_on
---

# Codebase Cleanup Rules

## Goal

Reduce noise, duplication, and structural inconsistency before modularization.

---

# Rules

## 1. Remove Dead Code Immediately

Delete:

- Unused methods
- Unused classes
- Obsolete abstractions
- Deprecated utilities

Do not keep unused code "for later".

---

## 2. Remove Unused Dependencies

Continuously remove:

- Unused headers
- Unused includes
- Unused libraries

---

## 3. Replace Abbreviations

Bad:

```cpp
CfgMgr
AuthSvc
tmp
```

Good:

```cpp
ConfigurationManager
AuthenticationService
temporaryFile
```

---

## 4. Eliminate Repetition

Extract duplicated logic into:

- Utilities
- Helpers
- Services

Do not duplicate business logic.

---

## 5. Simplify Function Signatures

Avoid:

```cpp
foo(a, b, c, d, e)
```

Prefer:

```cpp
foo(RequestContext)
```

or:

```cpp
foo(ConfigurationModel)
```

---

## 6. Keep Refactoring Small

Each cleanup operation should remain isolated and reviewable.