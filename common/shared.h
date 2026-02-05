#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

// shared memory for all processes

typedef struct _Telemetry {
  uint16_t rpm;
  float battery_percent;

  // ... ...

} Telemetry;

#define TELEMETRY_SHM_NAME "/fsae_telemetry"

#endif