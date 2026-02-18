#ifndef FSAE_CAN_SOCKET_HPP
#define FSAE_CAN_SOCKET_HPP

#include <linux/can.h>
#include <string>

class CanSocket {
public:
    bool open(const std::string& interface);

    bool read(can_frame& frame);

    void close();

private:
    int fd_ = -1;
};

#endif
