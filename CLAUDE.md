# Claude Code Guidelines

## Project Overview

T.R.A.C.K. (Telemetry Rendering And Capture Kit) — a real-time driver display and telemetry system for Formula SAE Electric.

## General Principles

- **Separation of concerns** — keep modules focused and independent
- **No over-engineering** — solve the current problem, not hypothetical future ones
- **Keep it simple** — prefer straightforward solutions over abstractions

## Git Workflow

- **Never commit directly to main**
- Branch naming: `<category>/<short-description>` (e.g., `feature/auth`, `fix/parser`)
- Commits:
  - Keep focused — one logical change per commit
  - Messages: short, imperative mood (e.g., "Add collision detection")
  - No co-author footers
- Always push to a branch, never force push to main

## What NOT to Do

- Don't add features that weren't requested
- Don't refactor code you're not actively modifying
- Don't add comments/docstrings to unchanged code
- Don't add error handling for impossible cases
- Don't create abstractions for one-time operations

## Edge Cases & Basic Behavior

When implementing features, always consider:

**State management:**
- What happens on reload/refresh?
- What if user switches tabs/screens without saving?
- What if user enters duplicate/invalid input?

**User actions:**
- Undo/redo (if applicable)
- Edge cases: empty inputs, special characters, very long strings
- Conflict resolution: duplicate names, overlapping data

**Basic UX expectations:**
- Validation feedback (immediate, clear errors)
- Prevent invalid states (disable buttons, block actions)
- Confirm destructive actions (delete, overwrite)
- Preserve work (autosave, warn before losing changes)

**Before marking a feature "done":**
1. Test the happy path
2. Test edge cases (empty, null, duplicate, max length)
3. Test error states (network failure, invalid input)
4. Test user mistakes (typos, wrong clicks, navigation)

If unsure about behavior, ask before implementing.

## Domain-Specific Guidelines

Some directories contain their own `CLAUDE.md` files with domain-specific conventions. Check subdirectories for additional guidelines.
