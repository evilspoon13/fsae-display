#ifndef FSAE_TELEMETRY_MESSAGE_HPP
#define FSAE_TELEMETRY_MESSAGE_HPP

#include <cstdint>

struct TelemetryMessage {
    uint32_t can_id;
    uint32_t timestamp_ms;
    double value;
};

#endif
