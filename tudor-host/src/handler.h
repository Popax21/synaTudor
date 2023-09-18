#ifndef TUDOR_HOST_HANDLER_H
#define TUDOR_HOST_HANDLER_H

#include "ipc.h"

extern char probe_sensor_name[IPC_SENSOR_NAME_SIZE+1];

void run_handler_loop(struct tudor_device *dev, int sock);

#endif