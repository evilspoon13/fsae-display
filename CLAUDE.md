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

## Domain-Specific Guidelines

Some directories contain their own `CLAUDE.md` files with domain-specific conventions. Check subdirectories for additional guidelines.
