#include "drivers_api.h"
#include "ipc.h"

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
    IPCMessageBuf *msg_buf = (IPCMessageBuf*) g_malloc(sizeof(IPCMessageBuf));

    //Receive the message
    GInputVector iv = { .buffer = msg_buf->data, .size = IPC_MAX_MESSAGE_SIZE };
    gint flags = 0;
    GError *error = NULL;
    ssize_t msg_size = g_socket_receive_message(sock, NULL, &iv, 1, NULL, 0, &flags, tdev->ipc_cancel, &error);
    if(msg_size < 0) {
        check_host_proc_dead(tdev, &error);
        g_free(msg_buf);
        g_task_return_error(task, error);
        return FALSE;
    }

    //Check message size
    if(msg_size < sizeof(enum ipc_msg_type)) {
        g_free(msg_buf);
        g_task_return_error(task, fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Received message smaller than minimum size"));
        return FALSE;
    }

    msg_buf->size = msg_size;
    g_task_return_pointer(task, msg_buf, g_free);

    return FALSE;
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