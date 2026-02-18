#ifndef FSAE_CONFIG_TYPES_HPP
#define FSAE_CONFIG_TYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

struct ChannelConfig {
    uint32_t can_id;
    std::string name;
    // ... ... todo add all fields for config from web app
};

struct WidgetConfig {
    std::string id;
    int x;
    int y;
    // todo
};

struct ScreenConfig {
    // list of widgets and their positions?
};

struct DisplayConfig {
    // list of screens?
};

#endif
