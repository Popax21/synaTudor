#include <gio/gunixfdmessage.h>
#include "drivers_api.h"
#include "ipc.h"

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

GSubprocess *start_host_process(int sock, GError **error) {
    //Create the launcher
    GSubprocessLauncher *launcher = g_subprocess_launcher_new(G_SUBPROCESS_FLAGS_NONE);
    g_subprocess_launcher_take_stdin_fd(launcher, sock);

    //Launch the IPC process
    return g_subprocess_launcher_spawn(launcher, error, "/sbin/tudor/libfprint_host", NULL);
}

bool check_host_proc_dead(FpiDeviceTudor *tdev, GError **error) {
    if(!tdev->host_proc_dead) return false;
    g_clear_error(error);
    *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Tudor host process died");
    return true;
}

static gboolean sock_ready(GSocket *sock, GIOCondition cond, gpointer user_data) {
    GTask *task = G_TASK(user_data);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(g_task_get_source_object(task));

    //Allocate the message buffer
    IPCMessageBuf *msg = ipc_msg_buf_new();

    //Receive the message
    GInputVector iv = { .buffer = msg->data, .size = IPC_MAX_MESSAGE_SIZE };

    GSocketControlMessage **cmsgs;
    int num_cmsgs = 0;

    gint flags = 0;
    GError *error = NULL;
    ssize_t msg_size = g_socket_receive_message(sock, NULL, &iv, 1, &cmsgs, &num_cmsgs, &flags, tdev->ipc_cancel, &error);
    if(msg_size < 0) {
        check_host_proc_dead(tdev, &error);
        ipc_msg_buf_free(msg);
        g_task_return_error(task, error);
        g_object_unref(task);
        return G_SOURCE_REMOVE;
    }

    //Get the transfer FD
    if(num_cmsgs > 0) {
        for(int i = 0; i < num_cmsgs; i++) {
            GSocketControlMessage *cmsg = cmsgs[i];
            if(G_IS_UNIX_FD_MESSAGE(cmsg)) {
                GUnixFDMessage *fdmsg = G_UNIX_FD_MESSAGE(cmsg);

                //Steal FDs
                int num_fds = 0;
                gint *fds = g_unix_fd_message_steal_fds(fdmsg, &num_fds);

                //Get the FD
                for(int j = 0; j < num_fds; j++) {
                    if(msg->transfer_fd >= 0) close(msg->transfer_fd);
                    msg->transfer_fd = fds[j];
                }
                g_free(fds);
            }
            g_object_unref(cmsg);
        }
    }
    g_free(cmsgs);

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

static void ipc_cancelled(GCancellable *cancel, gpointer user_data) {
    g_cancellable_cancel(G_CANCELLABLE(user_data));
    g_object_disconnect(cancel, "cancelled", ipc_cancelled, user_data, NULL);
}

void recv_ipc_msg(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data) {
    //Create a task
    GTask *task = g_task_new(tdev, tdev->ipc_cancel, callback, user_data);

    //Create socket source and wait for it
    GSource *sock_src = g_socket_create_source(tdev->socket, G_IO_IN, tdev->ipc_cancel);
    g_task_attach_source(task, sock_src, (GSourceFunc) (GSocketSourceFunc) sock_ready);
    g_source_unref(sock_src);
}

bool send_ipc_msg(FpiDeviceTudor *tdev, IPCMessageBuf *msg, GError **error) {
    //Check if host process is dead
    if(check_host_proc_dead(tdev, error)) return false;

    //Send message
    GOutputVector iv = {
        .buffer = msg->data,
        .size = msg->size
    };

    ssize_t num_written = g_socket_send_message(tdev->socket, NULL, &iv, 1, NULL, 0, 0, tdev->ipc_cancel, error);
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