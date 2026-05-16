---
trigger: always_on
---

# Foundation and Safety Rules

## Goal

Prepare the codebase for safe incremental refactoring without changing behavior.

---

# Rules

## 1. Do Not Refactor Core Logic First

Start with:

- Leaf modules
- Utility modules
- Wrappers
- Infrastructure code

Avoid touching:

- Core orchestration
- Critical workflows
- Shared runtime state

Until outer layers become stable.

---

## 2. Establish Baseline Safety

Before changing any module:

- Add baseline tests.
- Capture existing behavior.
- Protect legacy behavior.

---

## 3. Standardize Logging

All logging must:

- Use the centralized logging abstraction.
- Follow unified formatting.
- Avoid direct console output.

---

## 4. Standardize Error Handling

Errors must:

- Be actionable.
- Explain the failure clearly.
- Avoid vague messages.

Bad:

```cpp
Error occurred
```

Good:

```cpp
Failed to load configuration file: config.json
```

---

## 5. Validation Pipeline Is Mandatory

Every change must pass:

```bash
./scripts/quality/format.sh
./scripts/rebuild.sh
./scripts/test/unit.sh
```

No exceptions.