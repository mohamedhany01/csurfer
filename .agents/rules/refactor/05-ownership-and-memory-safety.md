---
trigger: always_on
---

# Ownership and Memory Safety Rules

## Goal

Prepare the system for safe concurrency and lifetime management.

---

# Rules

## 1. Ownership Must Be Explicit

All ownership must be visible and intentional.

Avoid hidden ownership transfer.

---

## 2. Document Smart Pointer Usage

Every smart pointer must explain why it exists.

Example:

```cpp
// Shared ownership because multiple services access the cache lifetime.
std::shared_ptr<CacheService> cacheService;
```

---

## 3. Prefer unique_ptr

Use unique ownership unless shared ownership is required.

---

## 4. Reduce Shared Mutable State

Shared mutable state increases threading risks.

Prefer:

- Immutable state
- Scoped ownership
- Localized mutation

---

## 5. Remove Global Mutable State

Global mutable state should be eliminated gradually.

---

## 6. Audit Lifetime Dependencies

Identify:

- Dangling ownership
- Circular ownership
- Unsafe references