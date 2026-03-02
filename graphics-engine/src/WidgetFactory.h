#pragma once
#include "Widgets.h"
#include "config_types.hpp"
#include <variant>
#include <vector>
#include <string>

struct LiveWidget {
    std::variant<NumberWidget, IndicatorLight, GaugeWidget, BarGraphWidget> widget;
    uint32_t can_id;
    std::string signal;

    void set_value(double v);
    void draw(const Font& font) const;
};

std::vector<LiveWidget> build_widgets(const DisplayConfig& config);
