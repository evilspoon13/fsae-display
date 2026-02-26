#ifndef FSAE_DBC_PARSER_HPP
#define FSAE_DBC_PARSER_HPP

#include "config_types.hpp"

constexpr const char* DEFAULT_DBC_PATH = "/tmp/display.dbc";

FrameMap load_dbc_config(const std::string& path);

#endif
