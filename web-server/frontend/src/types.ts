export type WidgetType = "gauge" | "number" | "bar" | "graph" | "indicator";

export interface WidgetSize {
  cols: number;
  rows: number;
}

export interface PlacedWidget {
  id: string;
  type: WidgetType;
  label: string;
  col: number; // 0-based grid column
  row: number; // 0-based grid row
  cols: number; // width in cells
  rows: number; // height in cells
}

export interface ScreenState {
  id: string;
  name: string;
  originalName?: string; // Track original name from localStorage for rename/replace
  widgets: PlacedWidget[];
  isDirty?: boolean; // Track if screen has unsaved changes
}

export interface EditorState {
  screens: ScreenState[];
  activeScreenId: string;
  selectedWidgetId: string | null;
}

export interface SavedLayout {
  name: string;
  widgets: PlacedWidget[];
}

export type EditorAction =
  | { type: "ADD_WIDGET"; payload: PlacedWidget }
  | { type: "MOVE_WIDGET"; payload: { id: string; col: number; row: number } }
  | { type: "RESIZE_WIDGET"; payload: { id: string; cols: number; rows: number } }
  | { type: "REMOVE_WIDGET"; payload: { id: string } }
  | { type: "SELECT_WIDGET"; payload: { id: string | null } }
  | { type: "UPDATE_WIDGET_LABEL"; payload: { id: string; label: string } }
  | { type: "CLEAR_SCREEN" }
  | { type: "ADD_SCREEN" }
  | { type: "REMOVE_SCREEN"; payload: { id: string } }
  | { type: "SET_ACTIVE_SCREEN"; payload: { id: string } }
  | { type: "SET_SCREEN_NAME"; payload: { id: string; name: string } }
  | { type: "UPDATE_ORIGINAL_NAME"; payload: { id: string; originalName: string } }
  | { type: "MARK_CLEAN"; payload: { id: string } }
  | { type: "LOAD_SCREEN"; payload: SavedLayout };
