#ifndef FSAE_CONFIG_PARSER_HPP
#define FSAE_CONFIG_PARSER_HPP

#include "config_types.hpp"

constexpr const char* DEFAULT_CONFIG_PATH = "/tmp/display.json";

FrameMap load_can_config(const std::string& path);

DisplayConfig load_display_config(const std::string& path);

#endif
