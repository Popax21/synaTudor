#ifndef TUDOR_TUDOR_LIBFPRINT_PROTO_H
#define TUDOR_TUDOR_LIBFPRINT_PROTO_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <tudor/log.h>
#include <tudor/tudor.h>

enum ipc_msg_type {
    IPC_MSG_INIT,
    IPC_MSG_READY,
    IPC_MSG_SHUTDOWN,
    IPC_MSG_ACK,
    IPC_MSG_CANCEL,

    IPC_MSG_LOAD_PDATA,
    IPC_MSG_RESP_LOAD_PDATA,
    IPC_MSG_STORE_PDATA,

    IPC_MSG_ADD_RECORD,
    IPC_MSG_DEL_RECORD,
    IPC_MSG_CLEAR_RECORDS,

    IPC_MSG_ENROLL,
    IPC_MSG_RESP_ENROLL,
    IPC_MSG_VERIFY,
    IPC_MSG_RESP_VERIFY,
    IPC_MSG_IDENTIFY,
    IPC_MSG_RESP_IDENTIFY
};

#define IPC_SENSOR_NAME_SIZE 128
#define IPC_MAX_PDATA_SIZE 16384

#define IPC_MAX_RECORD_SIZE 98304
#define IPC_MAX_MESSAGE_SIZE 131072

struct ipc_msg_init {
    enum ipc_msg_type type;

    enum log_level log_level;
    uint8_t usb_bus, usb_addr;
};

struct ipc_msg_load_pdata {
    enum ipc_msg_type type;

    char sensor_name[IPC_SENSOR_NAME_SIZE];
};

struct ipc_msg_resp_load_pdata {
    enum ipc_msg_type type;

    bool has_pdata;
    uint8_t pdata[];
};

struct ipc_msg_store_pdata {
    enum ipc_msg_type type;

    char sensor_name[IPC_SENSOR_NAME_SIZE];
    uint8_t pdata[];
};

struct ipc_msg_add_record {
    enum ipc_msg_type type;

    RECGUID guid;
    enum tudor_finger finger;
    uint8_t record_data[];
};

struct ipc_msg_del_record {
    enum ipc_msg_type type;

    RECGUID guid;
    enum tudor_finger finger;
};

struct ipc_msg_enroll {
    enum ipc_msg_type type;

    RECGUID guid;
    enum tudor_finger finger;
};

struct ipc_msg_resp_enroll {
    enum ipc_msg_type type;

    bool retry, done;
    uint8_t record_data[];
};

struct ipc_msg_verify {
    enum ipc_msg_type type;

    RECGUID guid;
    enum tudor_finger finger;
};

struct ipc_msg_resp_verify {
    enum ipc_msg_type type;

    bool retry, did_match;
};

struct ipc_msg_resp_identify {
    enum ipc_msg_type type;

    bool retry, did_match;
    RECGUID guid;
    enum tudor_finger finger;
};

#endif