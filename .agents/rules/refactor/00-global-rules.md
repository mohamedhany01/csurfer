---
trigger: always_on
---

# Global Refactoring Rules

## Goal

This document defines the mandatory global rules for all refactoring operations across the codebase.

These rules must always be followed regardless of module or refactoring scope.

---

# Stability Rules

- The system must remain functional after every refactoring step.
- Avoid behavioral changes unless explicitly planned and documented.
- Maintain backward compatibility whenever possible.
- Refactor incrementally using small isolated changes.
- Refactor from leaves and branches toward the core system.

---

# Commit Rules

Every refactoring change must:

- Be implemented in a small isolated commit.
- Include related test coverage.
- Include documentation updates.
- Include architecture updates when needed.

---

# Naming Rules

Abbreviations are forbidden in:

- Classes
- Methods
- Functions
- Variables
- Comments
- File names

Use explicit descriptive naming.

---

# Function Rules

- Functions should have a maximum of two parameters.
- If more parameters are needed:
  - Introduce a parameter object.
  - Introduce a context model.
  - Split the logic into smaller functions.

Functions must:

- Avoid hidden side effects.
- Be deterministic whenever possible.
- Prefer explicit behavior over implicit behavior.

---

# Documentation Rules

Every class, function, helper, utility, and module must include a story comment explaining:

- Why it exists
- What it does
- How it works

Inline comments are allowed only for complex logic.

---

# Testing Rules

Every refactoring step must add or update tests.

Tests must:

- Follow AAA style:
  - Arrange
  - Act
  - Assert
- Be isolated per module.
- Include:
  - Core cases
  - Corner cases

---

# Ownership Rules

When using ownership models:

- Document why ownership exists.
- Document why ownership type was selected.

Examples:

- unique_ptr
- shared_ptr
- weak_ptr

---

# Code Quality Rules

- Prefer composition over inheritance.
- Reduce coupling.
- Increase cohesion.
- Follow Single Responsibility Principle.
- Remove dead code continuously.
- Remove obsolete dependencies continuously.
- No magic numbers
- Prefer built-in standard library methods over custom logic when available (leverage modern C++).
- Replace deprecated or unsafe methods with modern, safe equivalents.

---

# Mandatory Validation Before Commit

Before every commit run:

```bash
./scripts/quality/format.sh
./scripts/rebuild.sh
./scripts/test/unit.sh
```

Then verify:

- /docs
- /arch
- README.md

Update them when necessary.