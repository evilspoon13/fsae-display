#include "shared.h"

int main(int argc, char *argv[]) {
    printf("graphics engine starting\n");

    int shared_mem_fd = shm_open(TELEMETRY_SHM_NAME, O_CREAT | O_RDWR, 0666);

    ftruncate(shared_mem_fd, sizeof(Telemetry));

    // map it, grab the pointer
    Telemetry* shared_telemetry = mmap(
        NULL,
        sizeof(Telemetry),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shared_mem_fd,
        0
    );

    return 0;
}