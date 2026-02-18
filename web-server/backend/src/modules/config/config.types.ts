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

export interface ConfigJSON {
  "screens": 
    {
      "name": string,
      "widgets": {
          "type": WidgetType,
          "x": number,
          "y": number,
          "radius": number,
          "data_field": DataFieldType,
          "min": number,
          "max": number,
          "redline": number,
          "color": string,
          "alert_threashold": string,
          "warning_color": string
        } [];
    } [];
}