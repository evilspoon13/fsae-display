#include "raylib.h"
#include "Widgets.h"

int main(void)
{
    const int W = 800, H = 480;
    InitWindow(W, H, "BarGraphWidget Raw Demo");
    SetTargetFPS(60);

    Font uiFont = LoadFontEx("assets/fonts/InterVariable.ttf", 256, 0, 0);
    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);

    BarGraphWidget bar;          // <-- raw defaults from the struct
    bar.gx = 4;                  // place on screen (10 tiles wide -> centered for 2 tiles)
    bar.gy = 1;                  // centered vertically for 3 tiles (6 tiles tall)

    // Minimal data setup so it's meaningful
    bar.minValue = 0;
    bar.maxValue = 100;
    bar.value = 27;
    bar.units = "°C";
    bar.decimals = 0;

    // Minimal thresholds so color changes are visible
    bar.thresholdCount = 3;
    bar.thresholds[0] = GaugeThreshold{ 60.0f, GREEN };
    bar.thresholds[1] = GaugeThreshold{ 85.0f, YELLOW };
    bar.thresholds[2] = GaugeThreshold{ 100.0f, RED };

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        float slowRate = 40.0f;
        float fastRate = 160.0f;
        bool fast = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        float rate = fast ? fastRate : slowRate;

        if (IsKeyDown(KEY_UP))   bar.value += rate * dt;
        if (IsKeyDown(KEY_DOWN)) bar.value -= rate * dt;

        if (IsKeyPressed(KEY_HOME)) bar.value = bar.minValue;
        if (IsKeyPressed(KEY_END))  bar.value = bar.maxValue;
        if (IsKeyPressed(KEY_R))    bar.value = bar.minValue;

        if (bar.value < bar.minValue) bar.value = bar.minValue;
        if (bar.value > bar.maxValue) bar.value = bar.maxValue;

        BeginDrawing();
        ClearBackground(BLACK);

        bar.Draw(uiFont);

        EndDrawing();
    }

    UnloadFont(uiFont);
    CloseWindow();
    return 0;
}