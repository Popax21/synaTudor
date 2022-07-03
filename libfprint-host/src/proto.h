#ifndef LIBFPRINT_HOST_PROTO_H
#define LIBFPRINT_HOST_PROTO_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

enum ipc_msg_type {
    IPC_MSG_INIT,
    IPC_MSG_SANDBOX,
    IPC_MSG_READY,
    IPC_MSG_SHUTDOWN
};

enum ipc_log_level {
    IPC_LOG_VERBOSE,
    IPC_LOG_DEBUG,
    IPC_LOG_INFO,
    IPC_LOG_WARN,
    IPC_LOG_ERROR
};

#define IPC_SENSOR_NAME_SIZE 32
#define IPC_MAX_PAIR_DATA_SIZE 1024

struct ipc_msg_init {
    enum ipc_msg_type type;

    enum ipc_log_level log_level;
    uint8_t usb_bus, usb_addr;

    char sensor_name[IPC_SENSOR_NAME_SIZE];
    uint8_t pair_data[];
};

struct ipc_msg_ready {
    enum ipc_msg_type type;

    char sensor_name[IPC_SENSOR_NAME_SIZE];
    uint8_t pair_data[];
};

#endif