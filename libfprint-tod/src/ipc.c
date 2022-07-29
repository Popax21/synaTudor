#include <gio/gunixfdmessage.h>
#include <tudor/dbus-launcher.h>
#include "drivers_api.h"
#include "ipc.h"
#include "open.h"
#include "suspend.h"

IPCMessageBuf *ipc_msg_buf_new() {
    IPCMessageBuf *msg = g_new(IPCMessageBuf, 1);
    msg->transfer_fd = -1;
    return msg;
}

void ipc_msg_buf_free(gpointer ptr) {
    IPCMessageBuf *msg = (IPCMessageBuf*) ptr;
    if(msg->transfer_fd >= 0) close(msg->transfer_fd);
    g_free(msg);
}

int ipc_msg_buf_steal_fd(IPCMessageBuf *msg) {
    int fd = msg->transfer_fd;
    msg->transfer_fd = -1;
    return fd;
}

static gboolean sock_ready(GSocket *sock, GIOCondition cond, gpointer user_data) {
    GTask *task = G_TASK(user_data);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(g_task_get_source_object(task));

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        g_task_return_error(task, error);
        g_object_unref(task);
        return G_SOURCE_REMOVE;
    }

    //Allocate the message buffer
    IPCMessageBuf *msg = ipc_msg_buf_new();

    //Receive the message
    GInputVector iv = { .buffer = msg->data, .size = IPC_MAX_MESSAGE_SIZE };

    GSocketControlMessage **cmsgs;
    int num_cmsgs = 0;

    gint flags = 0;
    ssize_t msg_size = g_socket_receive_message(sock, NULL, &iv, 1, &cmsgs, &num_cmsgs, &flags, g_task_get_cancellable(task), &error);
    if(msg_size < 0) {
        check_host_proc_dead(tdev, &error);
        ipc_msg_buf_free(msg);
        g_task_return_error(task, error);
        g_object_unref(task);
        return G_SOURCE_REMOVE;
    }

    //Get the transfer FD
    GError *fd_error = NULL;
    if(num_cmsgs > 0) {
        for(int i = 0; i < num_cmsgs; i++) {
            GSocketControlMessage *cmsg = cmsgs[i];
            if(!fd_error && G_IS_UNIX_FD_MESSAGE(cmsg)) {
                GUnixFDMessage *fdmsg = G_UNIX_FD_MESSAGE(cmsg);

                //Get the FD
                GUnixFDList *fds = g_unix_fd_message_get_fd_list(fdmsg);
                if(g_unix_fd_list_get_length(fds) >= 1) {
                    msg->transfer_fd = g_unix_fd_list_get(fds, 0, &error);
                    if(msg->transfer_fd < 0) fd_error = error;
                }
            }
            g_object_unref(cmsg);
        }
    }
    g_free(cmsgs);

    if(fd_error) {
        ipc_msg_buf_free(msg);
        g_task_return_error(task, fd_error);
        g_object_unref(task);
        return G_SOURCE_REMOVE;
    }

    //Check message size
    if(msg_size < sizeof(enum ipc_msg_type)) {
        ipc_msg_buf_free(msg);
        g_task_return_error(task, fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Received message smaller than minimum size"));
        g_object_unref(task);
        return G_SOURCE_REMOVE;
    }

    msg->size = msg_size;
    g_task_return_pointer(task, msg, ipc_msg_buf_free);
    g_object_unref(task);
    return G_SOURCE_REMOVE;
}

void recv_ipc_msg(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data) {
    //Create a task
    GTask *task = g_task_new(tdev, tdev->ipc_cancel, callback, user_data);

    //Create socket source and wait for it
    GSource *sock_src = g_socket_create_source(tdev->ipc_socket, G_IO_IN, tdev->ipc_cancel);
    g_task_attach_source(task, sock_src, (GSourceFunc) (GSocketSourceFunc) sock_ready);
    g_source_unref(sock_src);
}

static void recv_ipc_msg_no_timeout_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *otask = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    GTask *task = G_TASK(user_data);

    //Reset timeout
    g_socket_set_timeout(tdev->ipc_socket, 0);

    //Forward result
    GError *error = NULL;
    gpointer ptr = g_task_propagate_pointer(otask, &error);
    if(ptr) g_task_return_pointer(task, ptr, ipc_msg_buf_free);
    else g_task_return_error(task, error);
    g_object_unref(task);
}

void recv_ipc_msg_no_timeout(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data) {
    //Create a task
    GTask *task = g_task_new(tdev, NULL, callback, user_data);

    //Wrap recv_ipc_msg
    g_socket_set_timeout(tdev->ipc_socket, 0);
    recv_ipc_msg(tdev, recv_ipc_msg_no_timeout_cb, task);
}

bool send_ipc_msg(FpiDeviceTudor *tdev, IPCMessageBuf *msg, GError **error) {
    //Check if host process is dead
    if(check_host_proc_dead(tdev, error)) return false;

    //Send message
    GOutputVector iv = {
        .buffer = msg->data,
        .size = msg->size
    };

    GSocketControlMessage *cmsgs[1];
    int num_cmsgs = 0;
    if(msg->transfer_fd >= 0) {
        GUnixFDMessage *fdmsg = G_UNIX_FD_MESSAGE(g_unix_fd_message_new());
        int fd = msg->transfer_fd;
        msg->transfer_fd = -1;
        if(!g_unix_fd_message_append_fd(fdmsg, fd, error)) {
            g_object_unref(fdmsg);
            return false;
        }

        cmsgs[0] = G_SOCKET_CONTROL_MESSAGE(fdmsg);
        num_cmsgs = 1;
    }

    ssize_t num_written = g_socket_send_message(tdev->ipc_socket, NULL, &iv, 1, cmsgs, num_cmsgs, 0, tdev->ipc_cancel, error);

    for(int i = 0; i < num_cmsgs; i++) g_object_unref(cmsgs[i]);

    if(num_written < 0) {
        check_host_proc_dead(tdev, error);
        return false;
    }

    //Check number of written bytes
    if(num_written < msg->size) {
        *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_GENERAL, "Couldn't send full IPC message: only wrote %ld out of %lu bytes", num_written, msg->size);
        return false;
    }

    return true;
}

static void ack_recv_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(user_data);

    //Get the message buffer
    GError *error = NULL;
    IPCMessageBuf *msg = (IPCMessageBuf*) g_task_propagate_pointer(G_TASK(res), &error);
    if(!msg) {
        g_task_return_error(task, error);
        g_object_unref(task);
        return;
    }

    //Check if it's an ACK
    if(msg->type != IPC_MSG_ACK) {
        g_task_return_error(task, fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Expected ACK IPC message, but received message type 0x%x", msg->type));
        g_object_unref(task);
        ipc_msg_buf_free(msg);
        return;
    }

    g_task_return_pointer(task, msg, ipc_msg_buf_free);
    g_object_unref(task);
}

void send_acked_ipc_msg(FpiDeviceTudor *tdev, IPCMessageBuf *msg, GAsyncReadyCallback callback, gpointer user_data) {
    //Create a task
    GTask *task = g_task_new(tdev, tdev->ipc_cancel, callback, user_data);

    //Send the message
    GError *error = NULL;
    if(!send_ipc_msg(tdev, msg, &error)) {
        g_task_return_error(task, error);
        g_object_unref(task);
        return;
    }

    //Receive the ACK
    recv_ipc_msg(tdev, ack_recv_cb, task);
}

bool open_dbus_con(FpiDeviceTudor *tdev, GError **error) {
    if(tdev->dbus_con) return true;

    //Open a DBus connection
    tdev->dbus_con = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, error);
    if(!tdev->dbus_con) return false;

    //Register host process monitor
    register_host_process_monitor(tdev);

    //Register suspend monitor
    register_suspend_monitor(tdev);

    return true;
}

bool start_host_process(FpiDeviceTudor *tdev, int *sock_fd, GError **error) {
    g_assert_false(tdev->host_has_id);

    //Request the host launcher service to launch a host process
    GUnixFDList *fds;
    GVariant *rets = g_dbus_connection_call_with_unix_fd_list_sync(tdev->dbus_con,
        TUDOR_HOST_LAUNCHER_SERVICE, TUDOR_HOST_LAUNCHER_OBJ, TUDOR_HOST_LAUNCHER_INTERF,
        TUDOR_HOST_LAUNCHER_LAUNCH_METHOD, NULL, G_VARIANT_TYPE("(uh)"), G_DBUS_CALL_FLAGS_NONE,
        G_MAXINT,
        NULL, &fds,
        NULL, error
    );
    if(!rets) return false;

    //Get the result host ID and socket FD index
    int fd_idx;
    g_variant_get(rets, "(uh)", &tdev->host_id, &fd_idx);
    g_variant_unref(rets);

    //Get the socket FD from the list
    *sock_fd = g_unix_fd_list_get(fds, fd_idx, error);
    g_object_unref(fds);
    if(*sock_fd < 0) return false;

    tdev->host_has_id = true;
    tdev->host_dead = false;
    return true;
}

bool kill_host_process(FpiDeviceTudor *tdev, GError **error) {
    g_assert_true(tdev->host_has_id);
    tdev->host_has_id = false;

    //Request the host launcher service to kill the host process
    GVariant *rets = g_dbus_connection_call_sync(tdev->dbus_con,
        TUDOR_HOST_LAUNCHER_SERVICE, TUDOR_HOST_LAUNCHER_OBJ, TUDOR_HOST_LAUNCHER_INTERF,
        TUDOR_HOST_LAUNCHER_KILL_METHOD, g_variant_new("(u)", tdev->host_id), NULL, G_DBUS_CALL_FLAGS_NONE,
        G_MAXINT, NULL, error
    );
    if(!rets) return false;
    g_variant_unref(rets);

    return true;
}