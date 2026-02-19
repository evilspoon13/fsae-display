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
- **lucide-react** — icon library (consistent, tree-shakeable React icons)
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
│   ├── Header: "T.R.A.C.K. - Configurator"
│   ├── DBC Upload Section (file input + status display)
│   ├── Load Screen dropdown (select from localStorage)
│   ├── ScreenTabs (editable tabs)
│   │   ├── Screen buttons (double-click to rename inline)
│   │   └── + New Screen button
│   ├── DraggableWidget palette (2-column grid)
│   │   └── 5 widget types (lucide-react icons + labels)
│   └── Action Buttons (horizontal icon bar with tooltips)
│       ├── Save (floppy disk icon, orange dot when dirty)
│       ├── Commit (checkmark icon, with modal)
│       ├── Clear (file-edit icon, with modal)
│       └── Delete Screen (X icon, with modal, disabled if last screen)
├── GridCanvas (drop target, renders 10x6 grid, centered)
│   └── PlacedWidget[] — labeled rectangles, draggable + selectable
├── ConfigPanel (right sidebar, 224px wide)
│   ├── Type display (read-only)
│   ├── Label input (max 30 chars with counter)
│   ├── CAN Signal selector (dropdown: DBC signals or custom ID)
│   ├── Size selector (grid of buttons, disabled if collision)
│   └── Delete Widget button
└── DragOverlay (accurate size/color preview during drag)
```

### Component Details

**Navbar.tsx** (vertical left sidebar):
- Width: 288px (w-72)
- Sections from top to bottom:
  1. **Header**: "T.R.A.C.K." title + "Configurator" subtitle
  2. **DBC Upload** (file input section):
     - Before upload: "Upload DBC File" button
     - After upload: Green-bordered box showing filename, signal count, and clear (✕) button
     - Parsed DBC stored in state, signals available for widget binding
     - Currently uses hardcoded sample data (5 signals) — backend integration pending
  3. **Load Screen**: Dropdown with custom arrow SVG, loads from localStorage
  4. **Screens** (max-height 200px, scrollable, hidden scrollbar):
     - Screen tabs as buttons (blue when active, gray otherwise)
     - Show asterisk (*) if screen has unsaved widget changes
     - Double-click to edit name inline
     - X button to delete (hidden if only 1 screen)
     - + New Screen button
  5. **Components** (max-height 400px, scrollable, hidden scrollbar):
     - 2-column grid layout
     - Each widget: lucide-react icon (32×32px) + name (below, centered)
     - Icons: Gauge, Hash, BarChart3, LineChart, CircleDot
     - Taller boxes (py-5) for touch interaction
  6. **Action Buttons** (at bottom, pushed down by spacer):
     - Horizontal icon bar (4 square buttons, 48×48px each)
     - Base color: gray-700, semantic colors on hover
     - Icons from lucide-react: Save, Check, FileEdit, X
     - CSS tooltips appear above on hover
     - Save button shows orange dot (top-right) when screen is dirty
     - Space efficient: ~97px vertical (vs ~176px with old text buttons)

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
- Icon on top (centered, 32×32px)
- Label on bottom (centered)
- Hover effects for touch feedback
- **Icons** (lucide-react components):
  - Gauge → `<Gauge />` (speedometer)
  - Number → `<Hash />` (# symbol)
  - Bar → `<BarChart3 />` (bar chart)
  - Graph → `<LineChart />` (line graph)
  - Indicator → `<CircleDot />` (circle with dot)
- **Design iteration**: Originally used mixed emoji/symbols (📈 for graph, ⊙ for gauge, etc.). Changed to consistent lucide-react icons for professional appearance and visual consistency.

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
- **Type**: read-only, capitalized
- **Label**: text input, max 30 chars, shows counter
- **CAN Signal** (new feature):
  - Dropdown selector with three modes:
    1. "No signal assigned" (default)
    2. DBC signals (if DBC file uploaded) — shown as optgroup with name + unit
    3. "Custom CAN ID..." — reveals text input for manual entry
  - Tracks source: `canIdSource` = "dbc" | "custom"
  - Widget stores: `canId` (string) and `canIdSource`
  - Backwards compatible (optional fields on PlacedWidget)
- **Size**: 2-column grid of buttons
  - Active size: blue background
  - Available sizes: gray, hover effect
  - Blocked sizes (collision): gray, disabled, cursor-not-allowed
- **Delete**: red button at bottom

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
  dbcFile: DbcFile | null; // Parsed DBC data
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
  canId?: string; // CAN signal ID (from DBC or custom)
  canIdSource?: "dbc" | "custom"; // Tracks input method
}

interface DbcFile {
  filename: string;
  signals: DbcSignal[];
  uploadedAt: string; // ISO timestamp
}

interface DbcSignal {
  id: string; // e.g., "ENGINE_RPM", "WHEEL_SPEED_FL"
  name: string; // Human-readable name
  unit?: string; // e.g., "rpm", "km/h"
}
```

### Actions
- `ADD_WIDGET` — Add new widget to active screen, mark dirty
- `MOVE_WIDGET` — Update widget position, mark dirty
- `RESIZE_WIDGET` — Update widget size, mark dirty
- `REMOVE_WIDGET` — Delete widget, mark dirty, clear selection if removed
- `UPDATE_WIDGET_LABEL` — Update label, mark dirty
- `UPDATE_WIDGET_CAN` — Update CAN signal binding (canId + canIdSource), mark dirty
- `SELECT_WIDGET` — Set selected widget ID
- `CLEAR_SCREEN` — Remove all widgets from active screen, mark dirty
- `ADD_SCREEN` — Create new screen, switch to it
- `REMOVE_SCREEN` — Delete screen, switch to first remaining
- `SET_ACTIVE_SCREEN` — Switch active screen, clear selection
- `SET_SCREEN_NAME` — Update screen name
- `UPDATE_ORIGINAL_NAME` — Update originalName after save
- `MARK_CLEAN` — Clear isDirty flag after save
- `LOAD_SCREEN` — Load screen from localStorage as new tab
- `LOAD_DBC` — Load parsed DBC file into state
- `CLEAR_DBC` — Remove DBC file from state

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

## Design Iterations & Decisions

This section documents the evolution of the frontend UI and the rationale behind key design choices.

### Summary of Changes

**Current state (as of latest iteration):**

1. ✅ **Action buttons redesigned** — Vertical text buttons → horizontal icon bar with tooltips
   - Space savings: 79px vertical space (45% reduction)
   - Icons from lucide-react with CSS tooltips
   - Muted color palette (gray base, semantic colors on hover)
   - Orange dot indicator for dirty state

2. ✅ **Widget palette icons standardized** — Mixed emoji/symbols → consistent lucide-react icons
   - All icons now render as React components (32×32px)
   - Professional, scalable SVG icons
   - Gauge, Hash, BarChart3, LineChart, CircleDot

3. ✅ **CAN signal binding implemented** — Widgets can now be bound to data sources
   - DBC file upload in navbar (currently uses sample data)
   - CAN signal selector in ConfigPanel (DBC signals or custom ID)
   - PlacedWidget extended with `canId` and `canIdSource` fields
   - Backwards compatible with existing layouts

4. ✅ **Header branding updated** — "FSAE Display" → "T.R.A.C.K."

**Key dependencies added:**
- `lucide-react` (icon library, tree-shakeable, 0 vulnerabilities)

---

### Iteration 1: Action Button Redesign (Icon Bar)

**Problem**: Original navbar had 4 full-width text buttons stacked vertically at the bottom, consuming ~176px of vertical space. This limited space for the widget palette and future additions.

**Solution**: Replaced vertical text buttons with a horizontal row of 4 compact icon buttons (48×48px each) with CSS tooltips.

**Design choices:**
- **Icon library**: lucide-react (tree-shakeable, consistent design, React-first)
  - Save → `<Save />` (floppy disk)
  - Commit → `<Check />` (checkmark)
  - Clear → `<FileEdit />` (edit/clear icon)
  - Delete → `<X />` (close/delete)
- **Tooltips**: Pure CSS implementation using Tailwind's `group` utility
  - Zero dependencies, performant, consistent with existing patterns
  - Positioned above buttons, centered with `left-1/2 -translate-x-1/2`
  - Hidden by default (`opacity-0`), shown on hover (`group-hover:opacity-100`)
  - `pointer-events-none` to prevent tooltip from interfering with mouse
- **Color strategy**: Muted, low-stress design
  - Base state: All buttons `bg-gray-700` (neutral)
  - Hover states reveal semantic colors:
    - Save: `hover:bg-blue-700` (when clean) or `hover:bg-orange-700` (when dirty)
    - Commit: `hover:bg-green-700` (positive action)
    - Clear: `hover:bg-amber-700` (warning)
    - Delete: `hover:bg-red-800` (danger, extra muted)
  - Chose 700/800 shades (not 600) for less eye strain and modern aesthetic
  - Smooth transitions: `transition-colors duration-200`
- **Dirty state indicator**: Orange dot (8×8px) in top-right corner of Save button
  - Replaces asterisk in button text
  - More compact, visually distinct
  - Shows at-a-glance whether screen has unsaved changes
- **Space savings**: Reduced from ~176px to ~97px (79px saved, 45% reduction)
  - Freed space available for future navbar sections

**Alternative considered**: MUI icons, Heroicons
- **Rejected**: lucide-react had better variety for our specific needs (dedicated Save icon)

**Alternative considered**: JavaScript tooltip library (e.g., tippy.js)
- **Rejected**: Pure CSS tooltips are zero-dependency, performant, and sufficient for our use case

**Header title change**: "FSAE Display" → "T.R.A.C.K." to match project branding (Telemetry Rendering And Capture Kit)

---

### Iteration 2: Widget Palette Icon Consistency

**Problem**: Widget palette had inconsistent icon styles — graph used emoji (📈) while others used Unicode symbols (⊙, #, ▬, ●). Visual inconsistency and lack of scalability.

**Solution**: Replaced all icons with lucide-react components for consistency.

**Icon choices:**
- **Gauge**: `<Gauge />` — literal speedometer/gauge icon
- **Number**: `<Hash />` — # symbol, clear representation of numeric data
- **Bar**: `<BarChart3 />` — grouped bars, professional bar chart
- **Graph**: `<LineChart />` — line chart, replaces 📈 emoji
- **Indicator**: `<CircleDot />` — circle with center dot, clear indicator representation

**Design rationale:**
- All icons now render as React components (consistent sizing: 32×32px via `h-8 w-8`)
- Clean, professional aesthetic matches navbar icon buttons
- lucide-react already installed (no additional dependency)
- Scalable and crisp at all sizes (SVG-based)
- Consistent stroke width and design language across all icons

**Alternative considered**: Keep emoji for graph only
- **Rejected**: Inconsistency was jarring, icons look more professional

**Alternative considered**: Use different icon types (e.g., `Activity` for graph, `Zap` for indicator)
- **Rejected**: User preference was for `Hash` and `CircleDot` for clarity

---

### Iteration 3: CAN Signal Binding

**Feature**: Added ability to bind widgets to CAN signals from DBC file or custom IDs.

**Problem**: Widgets had no connection to data sources. Graphics engine needs to know what CAN signal each widget displays.

**Solution**: Three-mode CAN signal selector in ConfigPanel:
1. **No signal** (default): Widget not bound to any data
2. **DBC signals**: Dropdown populated from uploaded DBC file (shows name + unit)
3. **Custom ID**: Manual text input for custom CAN identifiers

**Design choices:**
- **DBC upload location**: Top of navbar (after header, before load screen)
  - Logical flow: upload CAN database → load/create screen → add widgets → bind signals
  - Upload before screen selection ensures DBC is available for all screens
- **DBC file format**: Currently uses hardcoded sample data (5 signals)
  - Backend integration pending: `POST /api/dbc/upload`
  - Sample signals: ENGINE_RPM, WHEEL_SPEED_FL, BATTERY_VOLTAGE, MOTOR_TEMP, THROTTLE_POS
- **Storage format**: Added `canId` and `canIdSource` to PlacedWidget
  - `canId`: Signal identifier (e.g., "ENGINE_RPM" or custom value)
  - `canIdSource`: "dbc" | "custom" (tracks input method for validation)
  - Optional fields (backwards compatible with existing layouts)
- **Dropdown design**: Uses custom SVG arrow (matches Load Screen dropdown)
  - DBC signals shown as optgroup for visual separation
  - "Custom CAN ID..." option reveals text input below
  - Shows signal name and unit in dropdown (e.g., "Engine RPM (rpm)")
- **State management**: DBC stored globally in `EditorState.dbcFile`
  - Shared across all screens (one DBC file for entire session)
  - Clear button (✕) removes DBC and resets all widget bindings
- **Visual feedback**: Green-bordered box after upload (matches success state)
  - Shows filename and signal count
  - Clear button in top-right for quick removal
- **Dirty state**: Updating CAN binding marks screen dirty (triggers save indicator)

**Alternative considered**: Separate DBC file per screen
- **Rejected**: DBC file represents vehicle's CAN database — single source of truth for all screens

**Alternative considered**: Auto-bind widgets by label matching
- **Rejected**: Too magical, error-prone. Explicit binding is safer and clearer.

**Alternative considered**: Text input only (no DBC upload)
- **Rejected**: DBC provides signal names, units, and validation. Manual entry is error-prone.

**Future enhancement**: Backend DBC parsing
- Currently uses hardcoded sample data for prototyping
- Backend will parse real .dbc files and return signal list via API
- Frontend ready for integration (just swap sample data with API response)

---

### Design Philosophy

**General principles applied throughout:**
1. **Touch-first design**: Large buttons, generous padding, clear hit targets
2. **Visual hierarchy**: Important actions (save, commit) more prominent via color
3. **Muted color palette**: Reduce eye strain, only show color on hover/interaction
4. **Zero dependencies when possible**: CSS tooltips, custom dropdowns (no UI library bloat)
5. **Backwards compatibility**: Optional fields, graceful degradation for old layouts
6. **Explicit over implicit**: User confirms destructive actions, manual signal binding
7. **Consistent design language**: lucide-react icons throughout, Tailwind utilities only

**Color semantics (consistent across UI):**
- **Blue**: Safe actions (save, primary buttons)
- **Green**: Confirmation/commit (finalizing changes)
- **Orange/Amber**: Warning (unsaved changes, clear actions)
- **Red**: Danger (delete, destructive actions)
- **Gray**: Neutral/disabled states

**Why Tailwind + lucide-react (not a UI library like MUI or Chakra):**
- Full control over design without fighting library defaults
- Lightweight (tree-shaking, only import what's used)
- No learning curve for library-specific APIs
- Fast iteration (no need to learn component props)
- Consistent with project guidelines (no external state libs, minimal dependencies)

---

## Verification Steps

1. `cd web-server/frontend && npm run dev` — dev server starts at http://localhost:5173
2. **Initial state**: See vertical sidebar (left), empty 10x6 grid (center), config panel (right)
3. **Header & Icons**: Header shows "T.R.A.C.K.", widget palette shows lucide-react icons (Gauge, Hash, BarChart3, LineChart, CircleDot)
4. **DBC Upload**:
   - Click "Upload DBC File" → file picker opens
   - Select any .dbc file → green box appears with filename and "5 signals"
   - Click ✕ → DBC cleared, upload button returns
5. **Widget placement**: Drag "Gauge" from palette → snaps to grid, shows labeled 2x2 block
4. **Collision detection**: Try placing overlapping widget → blocked
5. **Smart fallback**: Drop widget in tight space → uses smaller size if available
6. **Widget movement**: Drag placed widget → snaps to new position
7. **Widget selection**: Click widget → config panel opens
8. **Widget config**:
   - Type shown (read-only)
   - Label input (max 30 chars)
   - **CAN Signal** (new):
     - Upload DBC file first
     - Dropdown shows: "No signal assigned" (default)
     - Select DBC signal → shows "Engine RPM (rpm)", "Battery Voltage (V)", etc.
     - Select "Custom CAN ID..." → text input appears below
     - Enter custom value → saved with canIdSource: "custom"
     - Changing CAN signal marks screen dirty (orange save button)
   - Size buttons (2x2, 3x3 for gauge) — active size highlighted blue
   - Try changing size → updates if no collision, disabled if collision
9. **Delete widget**: Click Delete → widget removed
10. **Screen management**:
    - Click "+ New Screen" → new tab appears, switches to it
    - Switch between screens → widgets independent per screen
    - Double-click screen tab → enter edit mode
    - Type new name → click out → auto-saves, dropdown updates
11. **Save/Load**:
    - Add widgets to screen → screen tab shows *, save button shows orange dot
    - Hover over save button → turns orange, tooltip shows "Save Configuration *"
    - Click save button → dot disappears, tooltip changes to "Save Configuration"
    - Hover again → turns blue (clean state)
    - Reload page → screen still in dropdown
    - Select from dropdown → loads screen
12. **Action button tooltips**:
    - Hover over each icon button → tooltip appears above
    - Save: "Save Configuration" (or "Save Configuration *" when dirty)
    - Commit: "Commit"
    - Clear: "Clear Widgets"
    - Delete: "Delete Screen" (no tooltip when disabled)
13. **Action button colors**:
    - Base state: all gray
    - Save hover: blue (clean) or orange (dirty)
    - Commit hover: green
    - Clear hover: amber
    - Delete hover: red (when enabled)
14. **Clear Widgets**: Click clear icon → modal appears → confirm → all widgets removed
15. **Delete Screen**: Click delete icon → modal appears (disabled if last screen) → confirm → screen deleted
16. **Commit**: Click commit icon → modal appears → confirm → "Committed!" status (placeholder)

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

#### 6. Upload DBC File (NEW)
```
POST /api/dbc/upload

Request:
- Content-Type: multipart/form-data
- Field: "file" (DBC file upload)

Response:
{
  "filename": "vehicle_can.dbc",
  "signals": [
    {
      "id": "ENGINE_RPM",
      "name": "Engine RPM",
      "unit": "rpm"
    },
    {
      "id": "BATTERY_VOLTAGE",
      "name": "Battery Voltage",
      "unit": "V"
    },
    ...
  ],
  "uploadedAt": "2025-01-15T10:30:00.000Z"
}

Status Codes:
- 200: Success (DBC parsed and signals extracted)
- 400: Invalid file format or parse error
- 413: File too large
- 500: Server error

Purpose:
- Accept DBC file upload from configurator
- Parse DBC file using cantools or equivalent library
- Extract signal definitions (ID, name, unit)
- Return structured signal list to frontend
- Store DBC for later reference (optional)

Notes:
- Frontend expects DbcFile format (see types above)
- Signal IDs must be unique and valid identifiers
- Unit is optional (e.g., some signals are dimensionless)
- Current implementation uses hardcoded sample data — replace with real parser
- Consider caching parsed DBC to avoid re-parsing on each upload
- May want to validate against vehicle-specific DBC schema

Implementation:
- Use cantools Python library or equivalent for parsing
- Extract BO_ (message) and SG_ (signal) definitions
- Return only relevant signals (filter out internal/diagnostic messages)
- Handle malformed DBC files gracefully (return 400 with error message)
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

- ~~**Widget data binding**: Assign CAN signals to widgets~~ ✅ IMPLEMENTED (see Iteration 3)
- **Live preview mode**: Show real telemetry data in configurator (requires backend integration)
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
