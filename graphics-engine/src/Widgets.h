#pragma once
#include "raylib.h"
#include <string>

// Base grid tile (your screen is divisible into these)
static constexpr float BASE_TILE = 80.0f;

struct NumberWidget
{
    int gx = 0, gy = 0;
    int wTiles = 1, hTiles = 1;

    std::string label = "VALUE";
    int value = 0;
    Color valueColor = GREEN;

    float scale = 1.0f;

    Color panelFill   = Color{0, 0, 0, 180};
    Color panelBorder = Color{255, 255, 255, 80};

    float labelSize = 14.0f;
    float valueSize = 30.0f;
    float pad = 6.0f;
    float gapBelowLabel = 6.0f;
    float border = 2.0f;

    void Draw(const Font& font) const;
};

struct IndicatorLight
{
    int gx = 0, gy = 0;
    int wTiles = 1, hTiles = 1;

    std::string label = "WARN";
    bool on = false;

    float scale = 1.0f;

    Color onColor  = RED;
    Color offColor = DARKGRAY;

    Color panelFill   = Color{0, 0, 0, 180};
    Color panelBorder = Color{255, 255, 255, 80};

    float labelSize = 14.0f;
    float pad = 6.0f;
    float gapBelowLabel = 6.0f;
    float border = 2.0f;

    void Draw(const Font& font) const;
};

struct GaugeThreshold
{
    float value = 0.0f;   // threshold upper bound
    Color color = GREEN;  // used when widget.value <= value
};

struct GaugeWidget
{
    int gx = 0, gy = 0;
    int wTiles = 2, hTiles = 2;

    float value = 0.0f;
    float minValue = 0.0f;
    float maxValue = 100.0f;
    std::string units;
    int decimals = 0;

    int tickCount = 6;
    bool showTickLabels = true;

    GaugeThreshold thresholds[8];
    int thresholdCount = 0;

    float scale = 1.0f;

    Color panelFill   = Color{0, 0, 0, 180};
    Color panelBorder = Color{255, 255, 255, 80};

    Color ringBackColor = Color{60, 60, 60, 255};
    Color tickColor     = Color{255, 255, 255, 160};
    Color textColor     = RAYWHITE;

    float border = 2.0f;
    float pad = 6.0f;

    float startDeg = 135.0f;
    float endDeg   = 405.0f;

    float ringThickness = 9.0f;

    float valueTextSize = 26.0f;
    float unitsTextSize = 11.0f;

    void Draw(const Font& font) const;
};

// -----------------------------
// BarGraphWidget (minimum 2x3 tiles)
// Vertical bar with ticks + labels + thresholds + center value + units
// -----------------------------
struct BarGraphWidget
{
    int gx = 0, gy = 0;
    int wTiles = 2, hTiles = 3;   // minimum 160x240

    // Data
    float value = 0.0f;
    float minValue = 0.0f;
    float maxValue = 100.0f;
    std::string units;
    int decimals = 0;

    // Ticks
    int tickCount = 6;            // e.g. 6 => min..max evenly spaced
    bool showTickLabels = true;

    // Threshold colors (sorted ascending)
    GaugeThreshold thresholds[8];
    int thresholdCount = 0;

    float scale = 1.0f;

    // Style
    Color panelFill   = Color{0, 0, 0, 180};
    Color panelBorder = Color{255, 255, 255, 80};

    Color barBackColor = Color{60, 60, 60, 255};
    Color tickColor    = Color{255, 255, 255, 160};
    Color textColor    = RAYWHITE;

    float border = 2.0f;

    // ---- Permanent layout changes you wanted ----
    float pad = 10.0f;            // more padding (top/bottom/left/right)

    // Wider bar
    float barWidth = 26.0f;       // was 18

    // Put bar more centered (still leaving room for tick labels)
    float barXFrac = 0.45f;       // was 0.30

    // Bigger tick labels
    float tickLabelSize = 12.0f;  // was 9

    // Text sizing (bottom value + units)
    float valueTextSize = 24.0f;
    float unitsTextSize = 11.0f;

    void Draw(const Font& font) const;
};