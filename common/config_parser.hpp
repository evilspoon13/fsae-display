#ifndef FSAE_CONFIG_PARSER_HPP
#define FSAE_CONFIG_PARSER_HPP

#include "config_types.hpp"

constexpr const char* DEFAULT_CONFIG_PATH = "/tmp/display.json";

SignalType parse_signal_type(std::string_view s);

FrameMap load_can_config(const std::string& path);

std::vector<WidgetConfig> load_display_config(const std::string& path);

#endif
