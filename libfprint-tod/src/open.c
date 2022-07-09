#include <sys/socket.h>
#include "open.h"
#include "ipc.h"

static void dispose_dev(FpiDeviceTudor *tdev) {
    //Kill the host process (even though the process might have died already, we still need to tell the launcher to free the associated resources)
    GError *error = NULL;
    if(!kill_host_process(tdev, &error)) {
        g_warning("Error cleaning up Tudor host process: %s (%s code %d)", error->message, g_quark_to_string(error->domain), error->code);
        g_clear_error(&error);
    }

    //Free object references
    g_clear_object(&tdev->ipc_cancel);
    g_clear_object(&tdev->ipc_socket);
    g_clear_object(&tdev->dbus_con);

    g_debug("Disposed tudor device resources");
}

static void host_exit_cb(FpiDeviceTudor *tdev, guint host_id, gint status) {
    FpDevice *dev = FP_DEVICE(tdev);

    //Check host ID
    if(tdev->host_dead || tdev->host_id != host_id) return;

    //Mark host as dead
    tdev->host_dead = true;

    //Log status
    if(status != EXIT_SUCCESS) g_warning("Tudor host process died! Exit Code %d", status);

    //Cancel IPC
    if(tdev->ipc_cancel) {
        g_cancellable_cancel(tdev->ipc_cancel);
        g_debug("Cancelled tudor host process ID %u IPC", tdev->host_id);
    }

    //If we're in a close action, we have to dispose the device and complete the action here
    if(fpi_device_get_current_action(dev) == FPI_DEVICE_ACTION_CLOSE) {
        dispose_dev(tdev);
        fpi_device_close_complete(dev, NULL);
    }
}

static void open_recv_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    FpDevice *dev = FP_DEVICE(tdev);

    //Get the message buffer
    GError *error = NULL;
    IPCMessageBuf *msg = (IPCMessageBuf*) g_task_propagate_pointer(task, &error);
    if(!msg) goto error;

    //Handle message
    switch(msg->type) {
        case IPC_MSG_READY: {
            //Complete the open procedure
            g_info("Tudor host process ID %d sent READY message", tdev->host_id);
            fpi_device_open_complete(dev, NULL);
        } break;
        default: error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Unexpected message in init sequence: 0x%x", msg->type); goto error;
    }

    ipc_msg_buf_free(msg);
    return;

    error:;
    if(msg) ipc_msg_buf_free(msg);
    dispose_dev(tdev);
    fpi_device_open_complete(dev, error);
    return;
}

void fpi_device_tudor_open(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Open a DBus connection
    GError *error = NULL;
    tdev->dbus_con = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if(!tdev->dbus_con) {
        fpi_device_open_complete(dev, error);
        return;
    }

    //Initialize fields
    tdev->host_has_id = false;

    //Register host process monitor
    register_host_process_monitor(tdev, host_exit_cb);

    //Start the host process
    int sock_fd;
    if(!start_host_process(tdev, &sock_fd, &error)) {
        g_warning("Failed to start Tudor host process - is tudor-host-launcher.service running? Error: '%s' (%s code %d)", error->message, g_quark_to_string(error->domain), error->code);

        g_clear_object(&tdev->dbus_con);
        fpi_device_open_complete(dev, error);
        return;
    }
    g_info("Started tudor host process ID %u", tdev->host_id);

    //Create the IPC socket
    tdev->ipc_socket = g_socket_new_from_fd(sock_fd, &error);
    if(!tdev->ipc_socket) {
        g_assert_no_errno(close(sock_fd));
        g_clear_object(&tdev->dbus_con);
        fpi_device_open_complete(dev, error);
        return;
    }
    g_socket_set_timeout(tdev->ipc_socket, IPC_TIMEOUT_SECS);
    tdev->ipc_cancel = g_cancellable_new();

    //Send the init message
    GUsbDevice *usb_dev = fpi_device_get_usb_device(dev);
    tdev->send_msg->size = sizeof(struct ipc_msg_init); 
    tdev->send_msg->init = (struct ipc_msg_init) {
        .type = IPC_MSG_INIT,
        .log_level = LOG_DEBUG,
        .usb_bus = g_usb_device_get_bus(usb_dev),
        .usb_addr = g_usb_device_get_address(usb_dev)
    };
    if(!send_ipc_msg(tdev, tdev->send_msg, &error)) {
        GError *kerror = NULL;
        kill_host_process(tdev, &kerror);

        g_cancellable_cancel(tdev->ipc_cancel);
        g_clear_object(&tdev->ipc_cancel);
        g_clear_object(&tdev->ipc_socket);

        g_clear_object(&tdev->dbus_con);

        fpi_device_open_complete(dev, error);
        return;
    }
    g_debug("Initialized tudor host process ID %d with USB bus 0x%02x addr 0x%02x", tdev->host_id, tdev->send_msg->init.usb_bus, tdev->send_msg->init.usb_addr);

    //Receive IPC messages
    recv_ipc_msg(tdev, open_recv_cb, NULL);
}

void fpi_device_tudor_close(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    if(tdev->host_dead) {
        //Dispose the device directly
        dispose_dev(tdev);
        fpi_device_close_complete(dev, NULL);
        return;
    }

    //Send shutdown message
    tdev->send_msg->size = sizeof(enum ipc_msg_type);
    tdev->send_msg->type = IPC_MSG_SHUTDOWN;

    GError *error = NULL;
    if(!send_ipc_msg(tdev, tdev->send_msg, &error)) {
        fpi_device_close_complete(dev, error);
        return;
    }
}