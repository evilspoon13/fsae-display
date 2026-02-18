import { useDraggable } from "@dnd-kit/core";
import type { WidgetType } from "../types";

const typeLabels: Record<WidgetType, string> = {
  gauge: "Gauge",
  number: "Number",
  bar: "Bar",
  graph: "Graph",
  indicator: "Indicator",
};

const typeIcons: Record<WidgetType, string> = {
  gauge: "⊙",
  number: "#",
  bar: "▬",
  graph: "📈",
  indicator: "●",
};

interface Props {
  widgetType: WidgetType;
}

export default function DraggableWidget({ widgetType }: Props) {
  const { attributes, listeners, setNodeRef, isDragging } = useDraggable({
    id: `palette-${widgetType}`,
    data: { fromPalette: true, widgetType },
  });

  return (
    <div
      ref={setNodeRef}
      {...listeners}
      {...attributes}
      className={`flex w-full cursor-grab flex-col items-center justify-center select-none rounded border border-gray-600 bg-gray-700 px-4 py-5 text-sm font-medium text-gray-200 transition hover:border-gray-500 hover:bg-gray-600 ${
        isDragging ? "opacity-50 scale-95" : ""
      }`}
    >
      <span className="text-2xl">{typeIcons[widgetType]}</span>
      <span className="mt-1 text-center">{typeLabels[widgetType]}</span>
    </div>
  );
}
