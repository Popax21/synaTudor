#include <sys/socket.h>
#include "open.h"
#include "data.h"
#include "ipc.h"

static void dispose_dev(FpiDeviceTudor *tdev) {
    //Kill the host process (even though the process might have died already, we still need to tell the launcher to free the associated resources)
    GError *error = NULL;
    if(tdev->host_has_id && !kill_host_process(tdev, &error)) {
        g_warning("Error cleaning up Tudor host process: %s (%s code %d)", error->message, g_quark_to_string(error->domain), error->code);
        g_clear_error(&error);
    }

    //Clear DB mirror array
    g_ptr_array_set_size(tdev->db_records, 0);

    //Free object references
    g_clear_object(&tdev->ipc_cancel);
    g_clear_object(&tdev->ipc_socket);
    g_clear_object(&tdev->dbus_con);
    g_clear_pointer(&tdev->sensor_name, g_free);
    g_clear_object(&tdev->close_task);

    g_debug("Disposed tudor device resources");
}

static void host_exit_cb(FpiDeviceTudor *tdev, guint host_id, gint status) {
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

    //If we have a close task, we have to dispose the device and complete the action here
    if(tdev->close_task) {
        g_task_return_int(tdev->close_task, 0);
        dispose_dev(tdev);
    }
}

static void open_recv_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *mtask = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    GTask *task = G_TASK(user_data);

    //Get the message buffer
    GError *error = NULL;
    IPCMessageBuf *msg = (IPCMessageBuf*) g_task_propagate_pointer(mtask, &error);
    if(!msg) goto error;

    //Handle message
    switch(msg->type) {
        case IPC_MSG_LOAD_PDATA: {
            //Check sensor name
            if(!tdev->sensor_name) tdev->sensor_name = g_strndup(msg->load_pdata.sensor_name, sizeof(msg->load_pdata.sensor_name));
            else if(strcmp(tdev->sensor_name, msg->load_pdata.sensor_name) != 0) {
                error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Disallowed load of sensor pairing data '%.*s'", (int) sizeof(msg->load_pdata.sensor_name), msg->load_pdata.sensor_name);
                goto error;
            }

            //Load pairing data
            GByteArray *pdata = NULL;
            if(!load_pdata(tdev, &pdata, &error)) goto error;

            if(pdata && pdata->len > IPC_MAX_PDATA_SIZE) {
                g_error("Stored Tudor pairing data length exceeds maximum! [%d > %d]", pdata->len, IPC_MAX_PDATA_SIZE);
                g_byte_array_unref(pdata);
                pdata = NULL;
            }

            //Send response
            tdev->send_msg->size = sizeof(struct ipc_msg_resp_load_pdata) + (pdata ? pdata->len : 0);
            tdev->send_msg->resp_load_pdata = (struct ipc_msg_resp_load_pdata) {
                .type = IPC_MSG_RESP_LOAD_PDATA,
                .has_pdata = (pdata != NULL)
            };
            if(pdata) {
                memcpy(tdev->send_msg->resp_load_pdata.pdata, pdata->data, pdata->len);
                g_byte_array_unref(pdata);
            }
            if(!send_ipc_msg(tdev, tdev->send_msg, &error)) goto error;

            //Receive further messages
            recv_ipc_msg(tdev, open_recv_cb, task);
        } break;
        case IPC_MSG_STORE_PDATA: {
            //Check sensor name
            if(!tdev->sensor_name) tdev->sensor_name = g_strndup(msg->load_pdata.sensor_name, sizeof(msg->load_pdata.sensor_name));
            else if(strcmp(tdev->sensor_name, msg->load_pdata.sensor_name) != 0) {
                error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Disallowed store of sensor pairing data '%.*s'", (int) sizeof(msg->load_pdata.sensor_name), msg->load_pdata.sensor_name);
                goto error;
            }

            //Create pairing data array
            size_t pdata_len = msg->size - sizeof(struct ipc_msg_store_pdata);
            if(pdata_len > IPC_MAX_PDATA_SIZE) {
                error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Pairing data exceeds maximum size");
                goto error;
            }
            GByteArray *pdata = g_byte_array_new_take(g_memdup2(msg->store_pdata.pdata, pdata_len), pdata_len);

            //Store pairing data
            if(!store_pdata(tdev, pdata, &error)) {
                g_byte_array_unref(pdata);
                goto error;
            }
            g_byte_array_unref(pdata);

            //Send ACK
            tdev->send_msg->size = sizeof(enum ipc_msg_type);
            tdev->send_msg->type = IPC_MSG_ACK;
            if(!send_ipc_msg(tdev, tdev->send_msg, &error)) goto error;

            //Receive further messages
            recv_ipc_msg(tdev, open_recv_cb, task);
        } break;
        case IPC_MSG_READY: {
            //Complete the open procedure
            g_info("Tudor host process ID reported ready state");
            g_task_return_int(task, 0);
            g_object_unref(task);
        } break;
        default: error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Unexpected message in init sequence: 0x%x", msg->type); goto error;
    }

    ipc_msg_buf_free(msg);
    return;

    error:;
    if(msg) ipc_msg_buf_free(msg);
    dispose_dev(tdev);
    g_task_return_error(task, error);
    g_object_unref(task);
}

void open_device(FpiDeviceTudor *tdev, int usb_fd, uint8_t usb_bus, uint8_t usb_addr, GAsyncReadyCallback callback, gpointer user_data) {
    //Create task
    GTask *task = g_task_new(tdev, NULL, callback, user_data);

    //Open a DBus connection
    GError *error = NULL;
    tdev->dbus_con = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    if(!tdev->dbus_con) {
        dispose_dev(tdev);
        g_task_return_error(task, error);
        g_object_unref(task);
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
        dispose_dev(tdev);
        g_task_return_error(task, error);
        g_object_unref(task);
        return;
    }
    g_info("Started tudor host process ID %u", tdev->host_id);

    //Create the IPC socket
    tdev->ipc_socket = g_socket_new_from_fd(sock_fd, &error);
    if(!tdev->ipc_socket) {
        dispose_dev(tdev);
        g_task_return_error(task, error);
        g_object_unref(task);
        return;
    }
    g_socket_set_timeout(tdev->ipc_socket, IPC_TIMEOUT_SECS);
    tdev->ipc_cancel = g_cancellable_new();

    //Send the init message
    enum log_level loglvl;
    const char *loglvl_env = g_getenv("TUDOR_LOGLVL");
    char *loglvl_env_end = NULL;
    if(loglvl_env) loglvl = (enum log_level) strtol(loglvl_env, &loglvl_env_end, 10);
    if(loglvl_env == loglvl_env_end) loglvl = LOG_INFO;
    if(loglvl < LOG_VERBOSE) loglvl = LOG_VERBOSE;
    if(loglvl > LOG_ERROR) loglvl = LOG_ERROR;

    tdev->send_msg->transfer_fd = usb_fd;
    tdev->send_msg->size = sizeof(struct ipc_msg_init); 
    tdev->send_msg->init = (struct ipc_msg_init) {
        .type = IPC_MSG_INIT,
        .log_level = loglvl,
        .usb_bus = usb_bus,
        .usb_addr = usb_addr
    };
    if(!send_ipc_msg(tdev, tdev->send_msg, &error)) {
        g_assert_no_errno(close(usb_fd));
        dispose_dev(tdev);
        g_task_return_error(task, error);
        g_object_unref(task);
        return;
    }
    g_assert_no_errno(close(usb_fd));
    g_debug("Initialized tudor host process ID %d with USB bus 0x%02x addr 0x%02x", tdev->host_id, tdev->send_msg->init.usb_bus, tdev->send_msg->init.usb_addr);

    //Receive IPC messages
    recv_ipc_msg(tdev, open_recv_cb, task);
}

static void close_timeout_cb(FpDevice *dev, gpointer user_data) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if there's a close task
    if(tdev->close_task) {
        g_warning("Tudor host process hit shut down timeout!");
        g_task_return_int(tdev->close_task, 0);
        dispose_dev(tdev);
    }
}

void close_device(FpiDeviceTudor *tdev, GAsyncReadyCallback callback, gpointer user_data) {
    //Create task
    GTask *task = g_task_new(tdev, NULL, callback, user_data);

    if(tdev->host_dead) {
        //Dispose the device directly
        dispose_dev(tdev);
        g_task_return_int(task, 0);
        g_object_unref(task);
        return;
    }

    //Send shutdown message
    tdev->close_task = task;
    tdev->send_msg->size = sizeof(enum ipc_msg_type);
    tdev->send_msg->type = IPC_MSG_SHUTDOWN;

    GError *error = NULL;
    if(!send_ipc_msg(tdev, tdev->send_msg, &error)) {
        g_task_return_error(task, error);
        dispose_dev(tdev);
        return;
    }

    //Add timeout
    fpi_device_add_timeout(FP_DEVICE(tdev), IPC_TIMEOUT_SECS * 1000, close_timeout_cb, NULL, NULL);
}

static void probe_open_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    GTask *task = G_TASK(res);


    //Check for errors
    GError *error = NULL;
    g_task_propagate_int(task, &error);
    if(error) {
        fpi_device_probe_complete(FP_DEVICE(tdev), NULL, NULL, error);
        return;
    }

    //Complete the action
    fpi_device_probe_complete(FP_DEVICE(tdev), tdev->sensor_name, NULL, NULL);
}

void fpi_device_tudor_probe(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Open the USB device
    GError *error = NULL;
    GUsbDevice *usb_dev = fpi_device_get_usb_device(FP_DEVICE(tdev));
    if(!g_usb_device_open(usb_dev, &error)) {
        fpi_device_probe_complete(FP_DEVICE(tdev), NULL, NULL, error);
        return;
    }

    //Get a USB device FD, and close the device, as it conflicts with the host's device usage
    int usb_fd;
    g_assert_no_errno(usb_fd = dup(((int**) usb_dev->priv)[3][10 + 2 + 4 + 2 + 1 + 1])); //Cursed offset magic
    g_usb_device_close(fpi_device_get_usb_device(FP_DEVICE(tdev)), NULL);

    //Open the device
    open_device(tdev, usb_fd, g_usb_device_get_bus(usb_dev), g_usb_device_get_address(usb_dev), probe_open_cb, NULL);
}

void fpi_device_tudor_open(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if the host is still alive
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_open_complete(dev, error);
    }

    //Complete the action
    fpi_device_open_complete(dev, NULL);
}

void fpi_device_tudor_close(FpDevice *dev) {
    //Complete the action
    fpi_device_close_complete(dev, NULL);
}