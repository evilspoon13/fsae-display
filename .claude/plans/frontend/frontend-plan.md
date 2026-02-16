# Frontend Plan — Drag-and-Drop Display Configurator

## Context

Pit crew members connect to the Raspberry Pi and open a web app to configure the driver's dashboard layout. The frontend is a **Canva-style grid editor** — a toolbar at the top with widget types, a snap-to-grid canvas below. Users drag widget blocks onto the grid, label them, and save the layout.

**The frontend does NOT render actual widgets** (no gauges, graphs, etc.). It places labeled rectangles on a grid — the graphics engine (C++ / raylib) handles the real rendering on the driver display. This is purely a layout configurator.

Backend API contract (JSON schema, endpoints) is **TBD** — not part of this plan. For now the frontend works standalone with local state.

---

## Tech Stack

- **React + TypeScript** — Vite scaffold
- **@dnd-kit/core + @dnd-kit/modifiers** — drag-and-drop with snap-to-grid
- **Tailwind CSS** — styling
- **uuid** — widget ID generation

---

## Grid System

- **10 columns x 6 rows** — matches driver display aspect ratio (~800x480)
- Widgets snap to cells, can span multiple cells
- Collision detection prevents overlapping
- Canvas maintains fixed aspect ratio, scales to browser

| Widget Type | Default Size | Allowed Sizes |
|-------------|-------------|---------------|
| gauge       | 2x2         | 2x2, 3x3     |
| number      | 2x1         | 1x1, 2x1, 3x1 |
| bar         | 3x1         | 2x1, 3x1, 4x1, 1x2, 1x3 |
| graph       | 4x2         | 3x2, 4x2, 5x3 |
| indicator   | 1x1         | 1x1           |

---

## Component Architecture

```
App (DndContext + EditorContext provider)
├── Navbar
│   ├── WidgetPalette — draggable type icons (gauge, number, bar, graph, indicator)
│   ├── ScreenTabs — switch/add/remove screens
│   └── SaveButton — exports layout JSON (local download for now)
├── GridCanvas (drop target, renders 10x6 grid)
│   └── PlacedWidget[] — labeled rectangles, draggable + selectable
└── ConfigPanel (sidebar on widget select)
    ├── Type label display
    ├── Name/label text input
    ├── Size selector (dropdown of allowed sizes)
    └── Delete button
```

Each `PlacedWidget` is a simple labeled box on the grid — e.g., "Gauge" or "Number" with its name underneath. No visual widget rendering.

---

## State Management

`useReducer` + React Context.

**Actions:** `ADD_WIDGET`, `MOVE_WIDGET`, `RESIZE_WIDGET`, `REMOVE_WIDGET`, `SELECT_WIDGET`, `UPDATE_WIDGET_LABEL`, `ADD_SCREEN`, `REMOVE_SCREEN`, `SET_ACTIVE_SCREEN`, `LOAD_CONFIG`

---

## Folder Structure

```
web-server/frontend/
├── index.html
├── package.json
├── tsconfig.json
├── vite.config.ts
├── tailwind.config.js
├── postcss.config.js
└── src/
    ├── main.tsx
    ├── App.tsx
    ├── index.css
    ├── types.ts                  # All types: WidgetType, PlacedWidget, ScreenState, EditorState, EditorAction
    ├── state/
    │   ├── editorReducer.ts      # Reducer logic
    │   └── EditorContext.tsx     # Context provider
    ├── components/
    │   ├── Navbar.tsx            # Top bar: palette + screen tabs + save
    │   ├── DraggableWidget.tsx   # Palette item (useDraggable)
    │   ├── GridCanvas.tsx        # Drop target, 10x6 grid
    │   ├── PlacedWidget.tsx      # Widget block on the grid
    │   ├── ConfigPanel.tsx       # Sidebar: label input, size select, delete
    │   └── ScreenTabs.tsx        # Multi-screen tab bar
    └── utils/
        ├── gridHelpers.ts        # pixelToGrid, collision detection
        └── widgetDefaults.ts     # Default sizes and allowed sizes per type
```

Flat component structure — no nested folders per component. Everything is a single file.

---

## Implementation Phases

### Phase 1: Scaffold
- Vite + React + TypeScript project setup
- Install deps: `@dnd-kit/core`, `@dnd-kit/modifiers`, `@dnd-kit/utilities`, `uuid`, `tailwindcss`
- Define types in `types.ts`
- Build `editorReducer.ts` and `EditorContext.tsx`
- Basic `App.tsx` layout (navbar top, grid center)

### Phase 2: Grid + Drag-and-Drop
- `GridCanvas` — 10x6 CSS grid with visible gridlines
- `DraggableWidget` in palette using `useDraggable`
- `DndContext` with `createSnapModifier` for snap-to-grid
- Drop handler: pixel → grid coords → `ADD_WIDGET`
- `PlacedWidget` — labeled rectangle, re-draggable on grid
- Collision detection to block overlapping

### Phase 3: Config Panel + Multi-Screen
- `ConfigPanel` sidebar on widget select — name input, size dropdown, delete
- `ScreenTabs` — add/remove/switch screens
- Widgets are per-screen

### Phase 4: Export
- Save button exports layout as JSON (local download or console log)
- JSON structure TBD — will be defined when backend contract is established
- Load from JSON to hydrate editor state

---

## Files to Modify Outside Frontend

- `README.md` — update `web-app/` → `web-server/`
- `.gitignore` — fix `web-server/client/build/` → `web-server/frontend/dist/`

---

## Verification

1. `cd web-server/frontend && npm run dev` — dev server starts
2. See navbar with 5 widget type icons + empty 10x6 grid
3. Drag "Gauge" from palette → snaps to grid cell, shows labeled block
4. Drag placed widget to new position — snaps correctly, blocks overlaps
5. Click widget → sidebar opens with name input and size dropdown
6. Switch screens — widgets are independent per screen
7. Save → JSON output in console/download with grid positions
