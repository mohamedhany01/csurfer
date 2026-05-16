---
trigger: always_on
---

# Threading Preparation Rules

## Goal

Prepare the architecture for safe multithreaded execution.

---

# Rules

## 1. Identify Shared State

Document:

- Shared services
- Shared caches
- Shared runtime resources

---

## 2. Define Thread Ownership

Every shared resource must define:

- Ownership
- Synchronization responsibility
- Access rules

---

## 3. Prefer Immutability

Immutable state is preferred for thread safety.

---

## 4. Avoid Hidden State Mutation

Functions should not mutate shared state implicitly.

---

## 5. Encapsulate Synchronization

Synchronization logic must remain localized.

Avoid scattered locking behavior.

---

## 6. Audit Thread Safety

Identify:

- Race conditions
- Unsafe static variables
- Unsafe shared memory access