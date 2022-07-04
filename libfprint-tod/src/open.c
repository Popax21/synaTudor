#include <sys/socket.h>
#include "open.h"
#include "ipc.h"

static void host_proc_exit_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(user_data);
    tdev->host_proc_dead = true;

    //Check status
    int status = g_subprocess_get_exit_status(G_SUBPROCESS(src_obj));
    if(status != EXIT_SUCCESS) {
        g_warning("Tudor host process died! Exit Code %d", status);
    }

    //Cancel IPC cancellable
    if(tdev->ipc_cancel) g_cancellable_cancel(tdev->ipc_cancel);
}

static void open_recv_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    FpDevice *dev = FP_DEVICE(tdev);

    //Get the message buffer
    GError *error = NULL;
    IPCMessageBuf *msg = (IPCMessageBuf*) g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_open_complete(dev, error);
        return;
    }

    //Handle message
    switch(msg->type) {
        case IPC_MSG_READY: {
            //Complete the open procedure
            fpi_device_open_complete(dev, NULL);
        } break;
        default: fpi_device_open_complete(dev, fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Unexpected message in init sequence: 0x%x", msg->type));
    }

    ipc_msg_buf_free(msg);
}

void fpi_device_tudor_open(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_open_complete(dev, error);
        return;
    }

    //Create the socket pair
    int fds[2];
    if(socketpair(AF_UNIX, SOCK_DGRAM, 0, fds) < 0) {
        int err = errno;
        fpi_device_open_complete(dev, fpi_device_error_new_msg(FP_DEVICE_ERROR_GENERAL, "Failed creating IPC socket pair: %d [%s]", err, g_strerror(err)));
        return;
    }
    int mod_sock = fds[0], host_sock = fds[1];

    //Create the socket
    tdev->socket = g_socket_new_from_fd(mod_sock, &error);
    if(!tdev->socket) {
        close(mod_sock);
        close(host_sock);
        fpi_device_open_complete(dev, error);
        return;
    }

    //Start the host process
    tdev->host_proc = start_host_process(host_sock, &error);
    close(host_sock);
    if(!tdev->host_proc) {
        g_object_unref(tdev->socket);
        fpi_device_open_complete(dev, error);
        return;
    }

    //Start the host process monitor
    tdev->ipc_cancel = g_cancellable_new();
    g_subprocess_wait_async(tdev->host_proc, tdev->ipc_cancel, host_proc_exit_cb, tdev);

    //Send the init message
    GUsbDevice *usb_dev = fpi_device_get_usb_device(dev);
    tdev->send_msg->size = sizeof(struct ipc_msg_init); 
    tdev->send_msg->init = (struct ipc_msg_init) {
        .type = IPC_MSG_INIT,
        .log_level = LOG_INFO,
        .usb_bus = g_usb_device_get_bus(usb_dev),
        .usb_addr = g_usb_device_get_address(usb_dev)
    };
    if(!send_ipc_msg(tdev, tdev->send_msg, &error)) {
        g_cancellable_cancel(tdev->ipc_cancel);
        g_object_unref(tdev->ipc_cancel);

        g_subprocess_force_exit(tdev->host_proc);
        g_object_unref(tdev->host_proc);

        g_object_unref(tdev->socket);

        fpi_device_open_complete(dev, error);
        return;
    }

    //Receive the ready message
    recv_ipc_msg(tdev, open_recv_cb, NULL);
}

static void host_proc_wait_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(user_data);

    //Free memory
    g_cancellable_cancel(tdev->ipc_cancel);
    g_clear_object(&tdev->ipc_cancel);
    g_clear_object(&tdev->host_proc);
    g_clear_object(&tdev->socket);

    //Notify libfprint of completion
    fpi_device_close_complete(FP_DEVICE(tdev), NULL);
}

void fpi_device_tudor_close(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    if(!tdev->host_proc_dead) {
        //Send shutdown message
        tdev->send_msg->size = sizeof(enum ipc_msg_type);
        tdev->send_msg->type = IPC_MSG_SHUTDOWN;

        GError *error = NULL;
        if(!send_ipc_msg(tdev, tdev->send_msg, &error)) {
            fpi_device_close_complete(dev, error);
            return;
        }
    }

    //Wait for host process
    g_subprocess_wait_async(tdev->host_proc, NULL, host_proc_wait_cb, tdev);
}