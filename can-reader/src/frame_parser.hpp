#ifndef FSAE_FRAME_PARSER_HPP
#define FSAE_FRAME_PARSER_HPP

#include <linux/can.h>

#include "config_types.hpp"
#include "telemetry_message.hpp"

// parse a raw CAN frame into a TelemetryMessage using channel config
double parse_value(const can_frame& frame, const ChannelConfig& cfg);

#endif
