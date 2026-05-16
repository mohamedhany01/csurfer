---
trigger: always_on
---

# Agentic Refactoring Workflow

## Goal

Define the required workflow for AI-assisted refactoring operations.

---

# Workflow

## Step 1 — Understand the Module

The agent must:

- Read related modules
- Understand dependencies
- Understand ownership boundaries
- Identify risks

Do not refactor before understanding the module.

---

## Step 2 — Create Refactoring Scope

The agent must define:

- Goal
- Constraints
- Risks
- Expected outcome

Keep scope small.

---

## Step 3 — Add Safety Tests

Before refactoring:

- Add baseline tests
- Protect existing behavior

---

## Step 4 — Refactor Incrementally

Apply small isolated changes only.

Never mix:

- Cleanup
- Behavioral changes
- Architecture changes

Inside one commit.

---

## Step 5 — Validate Continuously

After every change:

```bash
./format.sh
./rebuild.sh
./unit.sh
```

---

## Step 6 — Update Documentation

Update:

- Story comments
- Architecture docs
- README
- Diagrams

---

## Step 7 — Review Refactoring Quality

Ensure:

- Reduced coupling
- Increased cohesion
- Better naming
- Better ownership clarity
- Improved testability

---

## Step 8 — Prepare for Next Increment

Leave the system stable and ready for the next small refactoring step.