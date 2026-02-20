#ifndef FSAE_BROADCAST_QUEUE_HPP
#define FSAE_BROADCAST_QUEUE_HPP

#include <atomic>
#include <cstdint>
#include <cstring>


template <std::size_t Capacity = 4096>
struct BroadcastQueue {

    // write idx
    // buffer
    // push
    // consume with callback
    // current write idx for each consumer
};

#endif
