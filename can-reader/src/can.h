#ifndef CAN_H
#define CAN_H

#include "shared.h"

int CanInit();
int CanRead(Telemetry* telemetry);
int CanClose();


#endif