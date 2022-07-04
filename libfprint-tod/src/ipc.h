#ifndef LIBFPRINT_TOD_IPC_H
#define LIBFPRINT_TOD_IPC_H

#include <tudor/libfprint-proto.h>
#include "device.h"

typedef struct _IPCMessageBuf {
    int transfer_fd;
    size_t size;
    union {
        guint8 data[IPC_MAX_MESSAGE_SIZE];
        enum ipc_msg_type type;
        struct ipc_msg_init init;
        struct ipc_msg_add_record add_record;
        struct ipc_msg_del_record del_record;
        struct ipc_msg_enroll enroll;
        struct ipc_msg_resp_enroll resp_enroll;
        struct ipc_msg_verify verify;
        struct ipc_msg_resp_verify resp_verify;
        struct ipc_msg_resp_identify resp_identify;
    };
} IPCMessageBuf;

IPCMessageBuf *ipc_msg_buf_new();
void ipc_msg_buf_free(gpointer msg);
int ipc_msg_buf_steal_fd(IPCMessageBuf *msg);

GSubprocess *start_host_process(int sock, GError **error);
bool check_host_proc_dead(FpiDeviceTudor *tdev, GError **error);

void recv_ipc_msg(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data);
bool send_ipc_msg(FpiDeviceTudor *tdev, IPCMessageBuf *msg, GError **error);
void send_acked_ipc_msg(FpiDeviceTudor *tdev, IPCMessageBuf *msg, GAsyncReadyCallback callback, gpointer user_data);

#endif