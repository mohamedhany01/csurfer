---
trigger: always_on
---

# Structural Modularization Rules

## Goal

Create clear module boundaries and improve maintainability.

---

# Rules

## 1. Define Clear Ownership Boundaries

Every module must clearly define:

- Responsibility
- Public API
- Internal implementation
- Dependencies

---

## 2. Split Large Modules

Large modules must be divided into:

- Smaller cohesive modules
- Dedicated directories

Update CMake after restructuring.

---

## 3. Reduce Coupling

Avoid unnecessary dependencies between modules.

Modules should communicate through:

- Interfaces
- Contracts
- Explicit APIs

---

## 4. Increase Cohesion

Each class should have one responsibility only.

Avoid "god objects".

---

## 5. Centralize Configuration

Move configuration logic into dedicated configuration modules.

Avoid scattered environment logic.

---

## 6. Prefer Composition

Prefer:

```cpp
class Service {
    Dependency dependency;
};
```

Instead of inheritance unless inheritance is justified.