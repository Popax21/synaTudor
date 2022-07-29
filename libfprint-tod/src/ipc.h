#ifndef LIBFPRINT_TOD_IPC_H
#define LIBFPRINT_TOD_IPC_H

#include <tudor/libfprint-proto.h>
#include "device.h"

#define IPC_TIMEOUT_SECS 25

typedef struct _IPCMessageBuf {
    int transfer_fd;
    size_t size;
    union {
        guint8 data[IPC_MAX_MESSAGE_SIZE];

        enum ipc_msg_type type;
        struct ipc_msg_init init;

        struct ipc_msg_load_pdata load_pdata;
        struct ipc_msg_resp_load_pdata resp_load_pdata;
        struct ipc_msg_store_pdata store_pdata;

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

void recv_ipc_msg(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data);
void recv_ipc_msg_no_timeout(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data);
bool send_ipc_msg(FpiDeviceTudor *tdev, IPCMessageBuf *msg, GError **error);
void send_acked_ipc_msg(FpiDeviceTudor *tdev, IPCMessageBuf *msg, GAsyncReadyCallback callback, gpointer user_data);

static inline bool check_ipc_msg_size(IPCMessageBuf *msg, size_t min_size, GError **error) {
    if(msg->size < min_size) {
        *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Message 0x%x smaller than minimum size", msg->type);
        return false;
    }
    return true;
}

bool open_dbus_con(FpiDeviceTudor *tdev, GError **error);

bool start_host_process(FpiDeviceTudor *tdev, int *sock_fd, GError **error);
bool kill_host_process(FpiDeviceTudor *tdev, GError **error);

static inline bool check_host_proc_dead(FpiDeviceTudor *tdev, GError **error) {
    if(tdev->host_has_id && !tdev->host_dead) return false;
    g_clear_error(error);
    *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Tudor host process died");
    return true;
}

#endif