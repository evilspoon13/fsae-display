#include <csignal>
#include <cstdio>

#include "broadcast_queue.hpp"
#include "config_parser.hpp"
#include "shared_memory.hpp"
#include "can_socket.hpp"
#include "frame_parser.hpp"

static volatile sig_atomic_t running = 1;
static volatile sig_atomic_t reload_flag = 0;

static void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) running = 0;
    if (sig == SIGHUP) reload_flag = 1;
}

int main() {
    // TODO: implement
    return 0;
}
