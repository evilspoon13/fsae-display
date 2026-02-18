# Frontend Plan — Drag-and-Drop Display Configurator

## Context

Pit crew members connect to the Raspberry Pi and open a web app to configure the driver's dashboard layout. The frontend is a **touch-friendly grid editor** — a vertical sidebar on the left with widget palette and screen management, a snap-to-grid canvas in the center, and a config panel on the right.

**The frontend does NOT render actual widgets** (no gauges, graphs, etc.). It places labeled rectangles on a grid — the graphics engine (C++ / raylib) handles the real rendering on the driver display. This is purely a layout configurator.

**Current persistence**: localStorage (per-browser). Backend API contract is detailed in "Backend Dependencies" section below for when shared persistence is needed.

---

## Tech Stack

- **React + TypeScript** — Vite scaffold
- **@dnd-kit/core** — drag-and-drop with DragOverlay
- **Tailwind CSS** — styling
- **uuid** — widget ID generation
- **localStorage** — client-side persistence (will be replaced with API calls)

---

## Grid System

- **10 columns x 6 rows** — matches driver display aspect ratio (~800x480)
- Widgets snap to cells, can span multiple cells
- Collision detection prevents overlapping
- Canvas maintains fixed aspect ratio, scales responsively to browser
- Smart size fallback: if default size doesn't fit, tries other allowed sizes

| Widget Type | Default Size | Allowed Sizes |
|-------------|-------------|---------------|
| gauge       | 2x2         | 2x2, 3x3     |
| number      | 2x1         | 1x1, 2x1, 3x1 |
| bar         | 3x1         | 2x1, 3x1, 4x1, 1x2, 1x3 |
| graph       | 4x2         | 3x2, 4x2, 5x3 |
| indicator   | 1x1         | 1x1           |

---

## Layout & Component Architecture

### Overall Layout
```
┌─────────────────────────────────────────────────────┐
│ Navbar (288px)  │  GridCanvas    │  ConfigPanel    │
│ (vertical left) │  (centered)    │  (224px right)  │
└─────────────────────────────────────────────────────┘
```

### Component Tree
```
App (DndContext + EditorContext provider)
├── Navbar (vertical left sidebar, 288px wide)
│   ├── Header: "FSAE Display - Configurator"
│   ├── Load Screen dropdown (select from localStorage)
│   ├── ScreenTabs (editable tabs)
│   │   ├── Screen buttons (double-click to rename inline)
│   │   └── + New Screen button
│   ├── DraggableWidget palette (2-column grid)
│   │   └── 5 widget types (stacked icon+label)
│   └── Action Buttons (4 buttons, full width)
│       ├── Save Configuration
│       ├── Commit Configuration (with modal)
│       ├── Clear Widgets (with modal)
│       └── Delete Screen (with modal, disabled if last screen)
├── GridCanvas (drop target, renders 10x6 grid, centered)
│   └── PlacedWidget[] — labeled rectangles, draggable + selectable
├── ConfigPanel (right sidebar, 224px wide)
│   ├── Type display (read-only)
│   ├── Label input (max 30 chars with counter)
│   ├── Size selector (grid of buttons, disabled if collision)
│   └── Delete Widget button
└── DragOverlay (accurate size/color preview during drag)
```

### Component Details

**Navbar.tsx** (vertical left sidebar):
- Width: 288px (w-72)
- Sections from top to bottom:
  1. **Header**: Title + subtitle
  2. **Load Screen**: Dropdown with custom arrow SVG, loads from localStorage
  3. **Screens** (max-height 200px, scrollable, hidden scrollbar):
     - Screen tabs as buttons (blue when active, gray otherwise)
     - Show asterisk (*) if screen has unsaved widget changes
     - Double-click to edit name inline
     - X button to delete (hidden if only 1 screen)
     - + New Screen button
  4. **Components** (max-height 400px, scrollable, hidden scrollbar):
     - 2-column grid layout
     - Each widget: stacked symbol (top, text-2xl) + name (bottom, text-center)
     - Taller boxes (py-5) for touch interaction
  5. **Action Buttons** (at bottom, pushed down by spacer):
     - Save Configuration (blue/orange if dirty)
     - Commit Configuration (green)
     - Clear Widgets (orange)
     - Delete Screen (red, disabled if last screen)

**ScreenTabs.tsx**:
- Renders list of screen buttons
- Double-click any screen tab to enter edit mode
- Shows input field in place when editing
- Auto-saves on blur (clicking out) or Enter key
- Validates: no empty names, no duplicate names
- On rename: deletes old localStorage entry, saves with new name, updates dropdown
- Escape key cancels edit

**DraggableWidget.tsx**:
- Palette items with vertical stacked layout
- Symbol on top (centered, text-2xl)
- Label on bottom (centered)
- Hover effects for touch feedback
- Icons: ⊙ (gauge), # (number), ▬ (bar), 📈 (graph), ● (indicator)

**GridCanvas.tsx**:
- Centered with flex layout (h-full w-full)
- 10x6 grid with visible gray gridlines
- Click background to deselect widget
- Drop target for both palette and existing widgets

**PlacedWidget.tsx**:
- Labeled rectangle on grid
- Shows type and label
- Colored borders and backgrounds (unique per type)
- Draggable and selectable
- Selected state: yellow border

**ConfigPanel.tsx**:
- Always rendered (shows placeholder when no widget selected)
- Width: 224px (w-56)
- Type: read-only, capitalized
- Label: text input, max 30 chars, shows counter
- Size: 2-column grid of buttons
  - Active size: blue background
  - Available sizes: gray, hover effect
  - Blocked sizes (collision): gray, disabled, cursor-not-allowed
- Delete: red button at bottom

**App.tsx**:
- DndContext setup with PointerSensor (5px activation distance)
- Smart size fallback algorithm:
  1. Try default size at drop position
  2. If collision, iterate through allowed sizes
  3. Use first size that fits
  4. If no size fits, bail out (don't place widget)
- DragOverlay with spring animation (300ms, cubic-bezier)
- Responsive cell size calculation based on container dimensions

---

## State Management

**Context**: `useReducer` + React Context

### State Structure
```typescript
interface EditorState {
  screens: ScreenState[];
  activeScreenId: string;
  selectedWidgetId: string | null;
}

interface ScreenState {
  id: string;
  name: string;
  originalName?: string; // For rename/replace in localStorage
  widgets: PlacedWidget[];
  isDirty?: boolean; // True if widgets changed since last save
}

interface PlacedWidget {
  id: string;
  type: WidgetType;
  label: string;
  col: number;
  row: number;
  cols: number;
  rows: number;
}
```

### Actions
- `ADD_WIDGET` — Add new widget to active screen, mark dirty
- `MOVE_WIDGET` — Update widget position, mark dirty
- `RESIZE_WIDGET` — Update widget size, mark dirty
- `REMOVE_WIDGET` — Delete widget, mark dirty, clear selection if removed
- `UPDATE_WIDGET_LABEL` — Update label, mark dirty
- `SELECT_WIDGET` — Set selected widget ID
- `CLEAR_SCREEN` — Remove all widgets from active screen, mark dirty
- `ADD_SCREEN` — Create new screen, switch to it
- `REMOVE_SCREEN` — Delete screen, switch to first remaining
- `SET_ACTIVE_SCREEN` — Switch active screen, clear selection
- `SET_SCREEN_NAME` — Update screen name
- `UPDATE_ORIGINAL_NAME` — Update originalName after save
- `MARK_CLEAN` — Clear isDirty flag after save
- `LOAD_SCREEN` — Load screen from localStorage as new tab

### Dirty State Tracking
- Screen marked dirty on: add, move, resize, remove widget, update label, clear
- Screen marked clean on: save to localStorage
- Save button shows orange + asterisk when dirty
- Screen tabs show asterisk when dirty
- **Name changes do NOT mark dirty** — auto-saved on blur

---

## Persistence Layer

**File**: `utils/layoutIO.ts`

**Current Implementation**: localStorage with key `fsae_screens`

**Storage Format**:
```typescript
{
  screens: {
    "Screen 1": {
      name: "Screen 1",
      widgets: PlacedWidget[]
    },
    "Dashboard": {
      name: "Dashboard",
      widgets: PlacedWidget[]
    }
  }
}
```

**Functions** (synchronous, will become async for backend):
- `listScreens(): string[]` — Returns array of saved screen names
- `loadScreen(name: string): SavedLayout | null` — Load screen by name
- `saveScreen(screen: SavedLayout): void` — Save screen (upsert by name)
- `deleteScreen(name: string): void` — Delete screen by name

**Backend Migration Notes**:
- All persistence logic isolated in this file
- Functions will need to become async (return Promises)
- Components will need async/await updates
- Need to add loading states and error handling

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
    ├── index.css                  # Tailwind directives + scrollbar-hide utility
    ├── types.ts                   # All types
    ├── state/
    │   ├── editorReducer.ts       # Reducer with all actions
    │   └── EditorContext.tsx      # Context provider + hooks
    ├── components/
    │   ├── Navbar.tsx             # Vertical left sidebar
    │   ├── DraggableWidget.tsx    # Palette item
    │   ├── GridCanvas.tsx         # Drop target
    │   ├── PlacedWidget.tsx       # Widget on grid
    │   ├── ConfigPanel.tsx        # Right sidebar
    │   └── ScreenTabs.tsx         # Editable screen tabs
    └── utils/
        ├── gridHelpers.ts         # pixelToGrid, collision, clamp
        ├── widgetDefaults.ts      # Sizes and constants
        └── layoutIO.ts            # Persistence (localStorage → future API)
```

Flat component structure — one file per component, no nesting.

---

## UX Features

### Touch-Friendly Design
- Large buttons (px-6 py-4 for modals, px-4 py-2 for navbar)
- 2-column grid layout for widgets (easy to tap)
- Grid of buttons for size selection (no dropdown)
- Big confirmation modals with clear Cancel/Confirm
- Hover states for all interactive elements

### Confirmation Modals
All modals use same pattern:
- Dark overlay (bg-black/60)
- Centered modal (max-w-md)
- Large heading (text-xl font-bold)
- Descriptive message (text-gray-300)
- Two buttons: Cancel (gray) + Action (colored)
- Touch-friendly sizing (px-6 py-4 text-lg)

**Modals**:
1. **Clear Widgets**: "This will remove all widgets from the current screen." (Orange button)
2. **Delete Screen**: "This will permanently delete the current screen and all its widgets." (Red button, disabled if last screen)
3. **Commit Configuration**: "This will update the driver display configuration." (Green button)

### Inline Screen Renaming
- Double-click any screen tab to edit
- Input appears in place
- Auto-saves on blur (click out) or Enter
- Escape to cancel
- Validation: no empty, no duplicates
- On save: updates localStorage immediately, refreshes dropdown
- Name changes tracked with originalName for rename/replace

### Smart Size Fallback
When dropping widget from palette:
1. Try default size at drop position
2. If collision detected:
   - Iterate through all allowed sizes
   - Use first size that fits
3. If no size fits, don't place widget

### Unsaved Changes Tracking
- Asterisk (*) on screen tab if widgets changed
- Save button turns orange if dirty
- Button text shows "Save Configuration *"
- Changes tracked per-screen
- Name changes auto-save (don't trigger dirty state)

### Drag & Drop
- 5px activation distance (prevents accidental drags)
- DragOverlay shows accurate preview:
  - Correct size (cols × rows × cellSize)
  - Correct color (matches widget type)
  - Shows label if dragging existing widget
- Smooth animation (300ms cubic-bezier)
- Snap to grid on drop
- Collision prevention

---

## Verification Steps

1. `cd web-server/frontend && npm run dev` — dev server starts at http://localhost:5173
2. **Initial state**: See vertical sidebar (left), empty 10x6 grid (center), config panel (right)
3. **Widget placement**: Drag "Gauge" from palette → snaps to grid, shows labeled 2x2 block
4. **Collision detection**: Try placing overlapping widget → blocked
5. **Smart fallback**: Drop widget in tight space → uses smaller size if available
6. **Widget movement**: Drag placed widget → snaps to new position
7. **Widget selection**: Click widget → config panel opens
8. **Widget config**:
   - Type shown (read-only)
   - Label input (max 30 chars)
   - Size buttons (2x2, 3x3 for gauge) — active size highlighted blue
   - Try changing size → updates if no collision, disabled if collision
9. **Delete widget**: Click Delete → widget removed
10. **Screen management**:
    - Click "+ New Screen" → new tab appears, switches to it
    - Switch between screens → widgets independent per screen
    - Double-click screen tab → enter edit mode
    - Type new name → click out → auto-saves, dropdown updates
11. **Save/Load**:
    - Add widgets to screen
    - Click "Save Configuration" → asterisk disappears, button turns blue
    - Reload page → screen still in dropdown
    - Select from dropdown → loads screen
12. **Clear Widgets**: Click → modal appears → confirm → all widgets removed
13. **Delete Screen**: Click → modal appears (disabled if last screen) → confirm → screen deleted
14. **Commit**: Click → modal appears → confirm → "Committed!" status (placeholder)

---

## Backend Dependencies

### Current State
- Screens stored in browser localStorage (key: `fsae_screens`)
- Per-browser, not shared across devices
- Synchronous operations

### Required API Endpoints

When backend is ready, replace `utils/layoutIO.ts` with API calls to these endpoints:

#### 1. List Screens
```
GET /api/screens

Response:
{
  "screens": ["Screen 1", "Dashboard", "Telemetry"]
}

Status Codes:
- 200: Success
- 500: Server error
```

#### 2. Load Screen
```
GET /api/screens/:name

Response:
{
  "name": "Dashboard",
  "widgets": [
    {
      "id": "uuid",
      "type": "gauge",
      "label": "RPM",
      "col": 0,
      "row": 0,
      "cols": 2,
      "rows": 2
    },
    ...
  ]
}

Status Codes:
- 200: Success
- 404: Screen not found
- 500: Server error
```

#### 3. Save Screen
```
PUT /api/screens/:name

Request Body:
{
  "name": "Dashboard",
  "widgets": [
    {
      "id": "uuid",
      "type": "gauge",
      "label": "RPM",
      "col": 0,
      "row": 0,
      "cols": 2,
      "rows": 2
    },
    ...
  ]
}

Response:
{
  "success": true
}

Status Codes:
- 200: Success (created or updated)
- 400: Invalid request body
- 500: Server error

Notes:
- Upsert operation (create if doesn't exist, update if exists)
- Validate widget schema on backend
- Validate grid positions (0-9 cols, 0-5 rows)
- Validate no overlapping widgets
```

#### 4. Delete Screen
```
DELETE /api/screens/:name

Response:
{
  "success": true
}

Status Codes:
- 200: Success
- 404: Screen not found
- 500: Server error
```

#### 5. Commit Configuration
```
POST /api/config/commit

Request Body:
{
  "screenName": "Dashboard"
}

Response:
{
  "success": true
}

Status Codes:
- 200: Success (config exported and graphics engine signaled)
- 404: Screen not found
- 500: Server error

Purpose:
- Load the named screen from storage
- Transform to graphics engine format
- Write to shared config file (e.g., /var/fsae/driver_display.json)
- Signal graphics engine to reload config (e.g., SIGHUP, IPC message)

Notes:
- This is the "deploy to driver display" action
- Should validate screen exists before committing
- May need to transform widget format for C++ consumption
- Consider locking mechanism if graphics engine is actively rendering
```

---

### Backend Implementation Notes

**Storage**:
- Store screens as JSON files in persistent directory (e.g., `/var/fsae/screens/`)
- Filename: `{name}.json` (sanitize name to avoid path traversal)
- Each file contains: `{ name: string, widgets: PlacedWidget[] }`
- Use file-based storage for simplicity (no database needed)

**Validation**:
- Widget types: "gauge", "number", "bar", "graph", "indicator"
- Grid bounds: col ∈ [0, 9], row ∈ [0, 5]
- Widget sizes must match allowed sizes (see Grid System table)
- No overlapping widgets (implement collision check)
- Label max length: 30 characters
- Screen name max length: 50 characters
- Screen name cannot be empty

**CORS**:
- Frontend runs on `http://localhost:5173` in dev
- Backend should enable CORS for development
- Production: frontend served from same origin (no CORS needed)

**Error Handling**:
- Return proper HTTP status codes
- Include error messages in response body:
  ```json
  {
    "success": false,
    "error": "Screen name already exists"
  }
  ```

**Graphics Engine Integration**:
- Define transformation from frontend format to graphics engine format
- Frontend PlacedWidget → Graphics engine config structure
- May need to map widget types to C++ classes
- Consider scaling factors if grid size ≠ display resolution
- Document expected graphics engine config format

---

### Frontend Migration Checklist (when backend ready)

**File**: `utils/layoutIO.ts`
1. ✅ Add fetch/axios for HTTP requests
2. ✅ Convert functions to async:
   - `async function listScreens(): Promise<string[]>`
   - `async function loadScreen(name: string): Promise<SavedLayout | null>`
   - `async function saveScreen(screen: SavedLayout): Promise<void>`
   - `async function deleteScreen(name: string): Promise<void>`
3. ✅ Add error handling (try/catch, throw on failure)
4. ✅ Add base URL constant (e.g., `/api`)

**Component Updates**:
1. `Navbar.tsx`:
   - Make `handleLoad` async
   - Make `handleSave` async
   - Make `handleConfirmDelete` async
   - Make `refreshScreens` async
   - Add loading states for buttons
   - Add error toast/notification
2. `ScreenTabs.tsx`:
   - Make `handleSaveName` async
   - Add loading indicator during save
   - Add error handling
3. `App.tsx`:
   - No changes needed (doesn't call persistence directly)
4. `ConfigPanel.tsx`:
   - No changes needed (doesn't call persistence directly)

**New Dependencies**:
- Error toast library (e.g., react-hot-toast)
- Loading spinner components
- Error boundary (optional)

**Estimated Effort**: 4-6 hours
- layoutIO.ts rewrite: 30 min
- Component async updates: 1 hour
- Loading states: 1-2 hours
- Error handling: 1-2 hours
- Testing: 1 hour

---

### Graphics Engine Expected Format (TBD)

**To be defined by backend/graphics team:**

Example transformation:
```typescript
// Frontend format
{
  name: "Dashboard",
  widgets: [
    { id: "...", type: "gauge", label: "RPM", col: 0, row: 0, cols: 2, rows: 2 }
  ]
}

// Graphics engine format (example, TBD)
{
  display: {
    resolution: { width: 800, height: 480 },
    widgets: [
      {
        type: "radial_gauge",
        label: "RPM",
        position: { x: 0, y: 0 },
        size: { width: 160, height: 160 },
        data_source: "can.engine.rpm",
        min: 0,
        max: 12000,
        redline: 11000
      }
    ]
  }
}
```

**Questions for backend team:**
1. What is the exact JSON schema for graphics engine config?
2. How are widget types mapped? (gauge → radial_gauge, etc.)
3. How are grid coordinates transformed to pixels?
4. Where do data sources come from? (CAN signals, computed values)
5. Where are min/max/units/thresholds defined?
6. How is config reload triggered? (file watch, signal, API call)
7. Is there a validation step before reload?
8. How are errors reported back to frontend?

---

## Future Enhancements (Not in Current Scope)

- **Widget data binding**: Assign CAN signals to widgets
- **Live preview mode**: Show real telemetry data in configurator
- **Widget library**: Save/load reusable widget templates
- **Undo/Redo**: Action history for layout changes
- **Keyboard shortcuts**: Arrow keys for widget movement, Delete key, etc.
- **Copy/Paste widgets**: Duplicate widget configurations
- **Alignment tools**: Snap to other widgets, distribute evenly
- **Export/Import**: Download/upload screen configs as JSON
- **Multi-user editing**: Websocket for real-time collaboration
- **Version history**: Track config changes over time
- **Widget validation**: Check data sources exist before save
- **Themes**: Light/dark mode, color schemes
- **Accessibility**: Screen reader support, keyboard navigation
