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

