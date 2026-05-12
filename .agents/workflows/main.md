---
description: Main workflow
---

# Agent Main Workflow

This document outlines the step-by-step workflow for the AI agent when assisting with the implementation of learning chapters into the CSurfer project.

## Phase 1: Discovery & Comprehension

1. **Initialize Context:** 
   Read `agents.md` to load the project's core rules, architectural constraints, and coding standards. This ensures all following steps align with the project's foundation.
2. **Identify the Target:** 
   Read the markdown chapter in the `/learn` folder specified by the user. If none is specified, ask the user which chapter they want to tackle next.
3. **Deep Dive:** 
   Read the chapter to deeply understand the core concepts (*what, why, how, when*). Review any accompanying code files (e.g., `*.cpp`, `*.h`, `*.py`) to see how the concepts operate in practice.
4. **Concept Check:** 
   Brainstorm with the user. Ask them questions about the chapter's concepts to ensure they are learning and understanding the underlying "why", rather than just memorizing the steps.

## Phase 2: Codebase Assessment & Planning

5. **Scan Codebase:** 
   Check the current codebase status using `git status`, `git diff`, or `git diff --staged` to understand what is currently pending or modified.
6. **Refactoring Check:** 
   Evaluate the current code architecture. Notify the user if we can apply the new logic easily, or if we need to perform refactoring first to prepare the codebase for the new chapter's features.
7. **Task Breakdown:** 
   Break the chapter's implementation down into small, atomic, and actionable steps.
8. **User Alignment:** 
   Present the plan to the user. Ask for their approval, suggestions, enhancements, or concerns regarding the proposed step-by-step implementation.

## Phase 3: Execution & Verification

9. **Atomic Implementation:** 
   Implement the logic step-by-step based on the approved plan.
10. **Build & Verify:** 
    After each atomic change, compile the project (e.g., `cmake --build build`) and run any relevant tests to ensure the implementation works correctly and hasn't broken existing features. If it fails, enter a collaborative debugging loop with the user.
11. **Atomic Commits:** 
    Commit each working atomic change with a clear and descriptive commit message. **Do not commit huge, sweeping changes all at once.**
12. **Wrap-up:** 
    Once the chapter is fully implemented, update any relevant project documentation to reflect the newly added logic and celebrate the progress!