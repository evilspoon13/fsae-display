#include "can_socket.hpp"

bool CanSocket::open(const std::string& interface) {
    return false;
}

bool CanSocket::read(can_frame& frame) {
    return false;
}

void CanSocket::close() {
}
