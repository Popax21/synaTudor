#ifndef TUDOR_HOST_HANDLER_H
#define TUDOR_HOST_HANDLER_H

#include "ipc.h"

bool handle_ipc_msg(struct tudor_device *dev, int sock, enum ipc_msg_type type);

#endif