export enum WidgetType {
    Gauge = "gauge",
    Bar = "bar",
    Number = "number",
    Indicator = "indicator"
}

export enum DataFieldType {
    Temperature = "temperature",
    Pressure = "pressure",
    RPM = "rpm"
}

export interface WidgetInfo {
    type: WidgetType,
    x: number,
    y: number,
    radius: number,
    data_field: DataFieldType,
    min: number,
    max: number,
    redline: number,
    color: string,
    alert_threshold: number,
    warning_color: string
}

export interface ScreenInfo {
    name: string,
    widgets: WidgetInfo[]
}

export interface GraphicsConfig {
    screens: ScreenInfo[]
}

export interface FrameParserConfig {
  frames: Record<`0x${string}`, FrameDefinition>;
}

export enum SignalType {
    UINT8 = "uint8",
    INT8 = "int8",
    UINT16 = "uint16",
    INT16 = "int16",
    UINT32 = "uint32",
    INT32 = "int32",
    FLOAT = "float",
    DOUBLE = "double",
}


export interface FrameSignal {
  name: string;
  start_byte: number;
  length: number;
  type: SignalType;
  scale: number;
  offset: number;
}

export interface FrameDefinition {
  name: string;
  signals: FrameSignal[];
}
