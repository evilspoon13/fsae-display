#include "raylib.h"
#include "Widgets.h"
#include "WidgetFactory.h"
#include "config_parser.hpp"
#include "shared_memory.hpp"

int main(int argc, char* argv[])
{
    const char* config_path = (argc > 1) ? argv[1] : "data.json";

    DisplayConfig display_cfg = load_display_config(config_path);
    auto widgets = build_widgets(display_cfg);

    const int W = 800, H = 480;
    InitWindow(W, H, "FSAE Display");
    SetTargetFPS(60);

    Font uiFont = LoadFontEx("assets/fonts/InterVariable.ttf", 256, 0, 0);
    SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);

    TelemetryQueue* queue = open_shared_queue(false);
    std::size_t consumer_pos = queue ? queue->current_pos() : 0;

    while (!WindowShouldClose())
    {
        if (queue) {
            queue->consume(consumer_pos, [&widgets](const TelemetryMessage& msg) {
                for (auto& lw : widgets) {
                    if (lw.can_id == msg.can_id && lw.signal == msg.signal_name)
                        lw.set_value(msg.value);
                }
            });
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (const auto& lw : widgets)
            lw.draw(uiFont);

        EndDrawing();
    }

    if (queue)
        close_shared_queue(queue, false);

    UnloadFont(uiFont);
    CloseWindow();
    return 0;
}
