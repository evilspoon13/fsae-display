#ifndef FSAE_CONFIG_TYPES_HPP
#define FSAE_CONFIG_TYPES_HPP

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>

// --------------- can frame config types ----------------

enum class SignalType {
    UINT8,
    INT8,
    UINT16,
    INT16,
    UINT32,
    INT32,
    FLOAT,
    DOUBLE
};

struct ChannelConfig {
    uint8_t start_byte;
    uint8_t length;
    SignalType type;
    double scale;
    double offset;
};


// --------------- display config types ----------------


// todo: do
struct WidgetConfig {
    std::string id;
    int x;
    int y;
    // todo
};


// maps can ID to a list of channels that exist in that frame
using FrameMap = std::unordered_map<uint32_t, std::vector<ChannelConfig>>;

struct ScreenConfig {
    // list of widgets and their positions?
};

struct DisplayConfig {
    // list of screens?
};

#endif
