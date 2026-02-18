# Frontend Guidelines

## Overview

Drag-and-drop layout configurator for the driver display. Users place labeled widget blocks on a 10x6 grid — no actual widget rendering (gauges, graphs, etc.). The graphics engine handles real rendering.

## Tech Stack

- React + TypeScript (Vite)
- @dnd-kit for drag-and-drop
- Tailwind CSS
- Reducer + Context for state (no external state libs)

## Code Style

- Strict TypeScript — no `any`
- Functional components with hooks only
- Prefer `const` over `let`
- Explicit types on all functions and state

## Project Structure

- **Flat components** — all components in `src/components/`, no nested folders
- **State in `src/state/`** — reducer and context provider
- **Utils in `src/utils/`** — grid helpers, widget defaults
- **Types in `src/types.ts`** — all TypeScript types in one file

## Styling

- Use Tailwind utilities
- Avoid custom CSS classes
- Keep component styles inline with className

## State Management

- Single reducer with typed actions
- Context provider wraps the app
- No prop drilling — use `useEditorState()` and `useEditorDispatch()`

## What NOT to Do

- Don't add widget rendering (gauges, graphs, etc.) — this is a configurator only
- Don't add backend integration yet — API contract is TBD
- Don't create separate files per component type
- Don't add external state libraries (Redux, Zustand, etc.)

## Build & Dev

```bash
npm run dev    # Start dev server
npm run build  # Production build
npx tsc -b     # Type-check
```

Always type-check before committing.
