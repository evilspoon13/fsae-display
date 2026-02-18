import { useState, useEffect } from "react";
import DraggableWidget from "./DraggableWidget";
import ScreenTabs from "./ScreenTabs";
import { useEditorState, useEditorDispatch } from "../state/EditorContext";
import { listScreens, loadScreen, saveScreen, deleteScreen } from "../utils/layoutIO";
import type { WidgetType } from "../types";

const widgetTypes: WidgetType[] = [
  "gauge",
  "number",
  "bar",
  "graph",
  "indicator",
];

export default function Navbar() {
  const state = useEditorState();
  const dispatch = useEditorDispatch();
  const [availableScreens, setAvailableScreens] = useState<string[]>([]);
  const [saveStatus, setSaveStatus] = useState<string>("");
  const [showCommitModal, setShowCommitModal] = useState<boolean>(false);
  const [showClearModal, setShowClearModal] = useState<boolean>(false);
  const [showDeleteModal, setShowDeleteModal] = useState<boolean>(false);

  const activeScreen = state.screens.find((s) => s.id === state.activeScreenId);

  const refreshScreens = () => {
    setAvailableScreens(listScreens());
  };

  useEffect(() => {
    refreshScreens();
  }, []);

  // Refresh dropdown when screen names change
  useEffect(() => {
    refreshScreens();
  }, [state.screens.map((s) => s.name).join(",")]);

  const handleLoad = (name: string) => {
    // Check if screen with this name is already open
    const existing = state.screens.find((s) => s.name === name);
    if (existing) {
      dispatch({ type: "SET_ACTIVE_SCREEN", payload: { id: existing.id } });
      return;
    }

    // Load from storage
    const screen = loadScreen(name);
    if (!screen) return;
    dispatch({ type: "LOAD_SCREEN", payload: screen });
  };

  const handleSave = () => {
    if (!activeScreen) return;

    saveScreen({
      name: activeScreen.name,
      widgets: activeScreen.widgets,
    });

    // Update originalName to current name after save
    dispatch({
      type: "UPDATE_ORIGINAL_NAME",
      payload: { id: activeScreen.id, originalName: activeScreen.name },
    });

    // Mark screen as clean
    dispatch({
      type: "MARK_CLEAN",
      payload: { id: activeScreen.id },
    });

    setSaveStatus("Saved!");
    setTimeout(() => setSaveStatus(""), 2000);
    refreshScreens();
  };

  const handleClearWidgets = () => {
    setShowClearModal(true);
  };

  const handleConfirmClear = () => {
    setShowClearModal(false);
    dispatch({ type: "CLEAR_SCREEN" });
  };

  const handleDeleteScreen = () => {
    if (!activeScreen) return;
    setShowDeleteModal(true);
  };

  const handleConfirmDelete = () => {
    if (!activeScreen) return;
    setShowDeleteModal(false);

    // Delete from localStorage if it was saved
    if (activeScreen.originalName) {
      deleteScreen(activeScreen.originalName);
      refreshScreens();
    }

    dispatch({ type: "REMOVE_SCREEN", payload: { id: activeScreen.id } });
  };

  const handleCommit = () => {
    setShowCommitModal(true);
  };

  const handleConfirmCommit = () => {
    setShowCommitModal(false);
    // TODO: Backend integration - export to graphics engine config
    setSaveStatus("Committed!");
    setTimeout(() => setSaveStatus(""), 2000);
  };

  return (
    <>
      {/* Clear Widgets Confirmation Modal */}
      {showClearModal && (
        <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/60">
          <div className="mx-4 w-full max-w-md rounded-lg border border-gray-600 bg-gray-800 p-8 shadow-2xl">
            <h2 className="mb-4 text-xl font-bold text-white">
              Clear Widgets?
            </h2>
            <p className="mb-8 text-gray-300">
              This will remove all widgets from the current screen.
            </p>
            <div className="flex gap-4">
              <button
                onClick={() => setShowClearModal(false)}
                className="flex-1 rounded bg-gray-700 px-6 py-4 text-lg font-medium text-white hover:bg-gray-600"
              >
                Cancel
              </button>
              <button
                onClick={handleConfirmClear}
                className="flex-1 rounded bg-orange-600 px-6 py-4 text-lg font-medium text-white hover:bg-orange-500"
              >
                Clear
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Delete Screen Confirmation Modal */}
      {showDeleteModal && (
        <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/60">
          <div className="mx-4 w-full max-w-md rounded-lg border border-gray-600 bg-gray-800 p-8 shadow-2xl">
            <h2 className="mb-4 text-xl font-bold text-white">
              Delete Screen?
            </h2>
            <p className="mb-8 text-gray-300">
              This will permanently delete the current screen and all its widgets.
            </p>
            <div className="flex gap-4">
              <button
                onClick={() => setShowDeleteModal(false)}
                className="flex-1 rounded bg-gray-700 px-6 py-4 text-lg font-medium text-white hover:bg-gray-600"
              >
                Cancel
              </button>
              <button
                onClick={handleConfirmDelete}
                className="flex-1 rounded bg-red-600 px-6 py-4 text-lg font-medium text-white hover:bg-red-500"
              >
                Delete
              </button>
            </div>
          </div>
        </div>
      )}

      {/* Commit Confirmation Modal */}
      {showCommitModal && (
        <div className="fixed inset-0 z-50 flex items-center justify-center bg-black/60">
          <div className="mx-4 w-full max-w-md rounded-lg border border-gray-600 bg-gray-800 p-8 shadow-2xl">
            <h2 className="mb-4 text-xl font-bold text-white">
              Commit Configuration?
            </h2>
            <p className="mb-8 text-gray-300">
              This will update the driver display configuration.
            </p>
            <div className="flex gap-4">
              <button
                onClick={() => setShowCommitModal(false)}
                className="flex-1 rounded bg-gray-700 px-6 py-4 text-lg font-medium text-white hover:bg-gray-600"
              >
                Cancel
              </button>
              <button
                onClick={handleConfirmCommit}
                className="flex-1 rounded bg-green-600 px-6 py-4 text-lg font-medium text-white hover:bg-green-500"
              >
                Confirm
              </button>
            </div>
          </div>
        </div>
      )}

      <div className="flex h-screen w-72 flex-col border-r border-gray-700 bg-gray-800">
      {/* Header */}
      <div className="border-b border-gray-700 p-4">
        <h1 className="text-lg font-bold text-white">FSAE Display</h1>
        <p className="text-xs text-gray-400">Configurator</p>
      </div>

      {/* Load Screen Dropdown */}
      <div className="border-b border-gray-700 p-4">
        <label className="mb-2 block text-xs font-medium text-gray-400">
          Load Screen
        </label>
        <select
          value=""
          onChange={(e) => e.target.value && handleLoad(e.target.value)}
          className="w-full appearance-none rounded border border-gray-600 bg-gray-900 px-3 py-2 pr-8 text-sm text-white focus:border-blue-500 focus:outline-none"
          style={{
            backgroundImage: `url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' fill='none' viewBox='0 0 20 20'%3E%3Cpath stroke='%239CA3AF' stroke-linecap='round' stroke-linejoin='round' stroke-width='1.5' d='M6 8l4 4 4-4'/%3E%3C/svg%3E")`,
            backgroundPosition: 'right 0.5rem center',
            backgroundRepeat: 'no-repeat',
            backgroundSize: '1.5em 1.5em',
          }}
        >
          <option value="">Select saved screen...</option>
          {availableScreens.map((name) => (
            <option key={name} value={name}>
              {name}
            </option>
          ))}
        </select>
      </div>

      {/* Screen Tabs */}
      <div className="overflow-y-auto border-b border-gray-700 p-4 scrollbar-hide" style={{ maxHeight: '200px' }}>
        <label className="mb-2 block text-xs font-medium text-gray-400">
          Screens
        </label>
        <div className="flex flex-col gap-2">
          <ScreenTabs />
        </div>
      </div>

      {/* Widget Palette */}
      <div className="border-b border-gray-700 p-4">
        <label className="mb-2 block text-xs font-medium text-gray-400">
          Components
        </label>
        <div className="grid grid-cols-2 gap-2 overflow-y-auto scrollbar-hide" style={{ maxHeight: '400px' }}>
          {widgetTypes.map((t) => (
            <DraggableWidget key={t} widgetType={t} />
          ))}
        </div>
      </div>

      {/* Spacer */}
      <div className="flex-1" />

      {/* Action Buttons */}
      <div className="space-y-2 p-4">
        <button
          onClick={handleSave}
          className={`w-full rounded px-4 py-2 text-sm font-medium text-white transition ${
            activeScreen?.isDirty
              ? "bg-orange-600 hover:bg-orange-500"
              : "bg-blue-600 hover:bg-blue-500"
          }`}
        >
          {saveStatus ||
            (activeScreen?.isDirty
              ? "Save Configuration *"
              : "Save Configuration")}
        </button>

        <button
          onClick={handleCommit}
          className="w-full rounded bg-green-600 px-4 py-2 text-sm font-medium text-white hover:bg-green-500"
        >
          Commit Configuration
        </button>

        <button
          onClick={handleClearWidgets}
          className="w-full rounded bg-orange-700 px-4 py-2 text-sm font-medium text-white hover:bg-orange-600"
        >
          Clear Widgets
        </button>

        <button
          onClick={handleDeleteScreen}
          disabled={state.screens.length <= 1}
          className={`w-full rounded px-4 py-2 text-sm font-medium text-white ${
            state.screens.length <= 1
              ? "cursor-not-allowed bg-gray-700 text-gray-500"
              : "bg-red-700 hover:bg-red-600"
          }`}
        >
          Delete Screen
        </button>
      </div>
    </div>
    </>
  );
}
