#include <csignal>
#include <cstdio>

#include "broadcast_queue.hpp"
#include "config_parser.hpp"
#include "config_types.hpp"
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

    // std::signal(SIGINT, signal_handler);
    // std::signal(SIGTERM, signal_handler);
    std::signal(SIGHUP, signal_handler);

    FrameMap frame_map = load_can_config(DEFAULT_CONFIG_PATH);

    CanSocket sock;
    if( !sock.open("vcan0")) {
        std::perror("Failed to open CAN socket");
        return 1;
    }

    can_frame frame;
    while(running) {
        if (sock.read(frame)) {

            printf("Received CAN frame with ID: %03x\n", frame.can_id);
            auto it = frame_map.find(frame.can_id);
            if (it == frame_map.end()) {
                continue;
            }

            const auto& channels = it->second;

            for (const auto& cfg : channels) {
                double value = parse_value(frame, cfg);
                // TODO write value to shared memory or broadcast queue

                printf("Parsed value for CAN ID %03x at byte %d: %f\n", frame.can_id, cfg.start_byte, value);
            }
        }
        if (reload_flag) {
            reload_flag = 0;
            frame_map = load_can_config(DEFAULT_CONFIG_PATH);
            printf("Reloaded config\n");
        }
    }
    return 0;
}
