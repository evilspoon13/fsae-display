#ifndef FSAE_SHARED_MEMORY_HPP
#define FSAE_SHARED_MEMORY_HPP

#include <cstddef>

#include "broadcast_queue.hpp"
#include "config_types.hpp"

inline constexpr const char* SHM_NAME = "/fsae_telemetry";

using TelemetryQueue = BroadcastQueue<TelemetryMessage, 4096>;

// open queue, return pointer to shared mem
TelemetryQueue* open_shared_queue(bool is_writer);

// unmap and close shared mem queue
void close_shared_queue(TelemetryQueue* queue, bool is_writer);

#endif
