#ifndef FSAE_CONFIG_PARSER_HPP
#define FSAE_CONFIG_PARSER_HPP

#include <string>

#include "config_types.hpp"

inline constexpr const char* DEFAULT_CONFIG_PATH = "/tmp/display.json";

// load and parse json config
DisplayConfig load_config(const std::string& path);

#endif
