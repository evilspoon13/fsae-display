#include <csignal>
#include <cstdio>
#include <vector>

#include "broadcast_queue.hpp"
#include "shared_memory.hpp"
#include "log_writer.hpp"
#include "compressor.hpp"

static volatile sig_atomic_t running = 1;

static void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) running = 0;
}

int main() {
    return 0;
}
