#include <sys/socket.h>
#include "record.h"
#include "device.h"
#include "ipc.h"

#define NUM_ENROLL_STAGES 8

G_DEFINE_TYPE(FpiDeviceTudor, fpi_device_tudor, FP_TYPE_DEVICE)

GType fpi_tod_shared_driver_get_type() {
    return FPI_TYPE_DEVICE_TUDOR;
}

static FpIdEntry tudor_ids[] = {
    { .vid = 0x06cb, .pid = 0x00be },
//  { .vid = 0x06cb, .pid = 0x00bd }, //TODO Check if it's also supported
    { 0 }
};

static bool get_print_guid_finger(FpiDeviceTudor *dev, FpPrint *print, RECGUID *guid, enum tudor_finger *finger) {
    //Check the driver ID
    gchar *print_driver;
    g_object_get(print, "driver", &print_driver, NULL);
    if(g_strcmp0(print_driver, fp_device_get_driver(FP_DEVICE(dev))) != 0) {
        g_free(print_driver);
        return false;
    }
    g_free(print_driver);

    //Check if the print is marked as "device stored"
    if(!fp_print_get_device_stored(print)) return false;

    //Get the print data
    GVariant *print_data;
    g_object_get(print, "fpi-data", &print_data, NULL);

    //Peek the GUID and finger
    bool data_valid = peek_record_guid_finger(print_data, guid, finger);

    g_variant_unref(print_data);
    return data_valid;
}

static void record_free(struct record *rec) {
    g_bytes_unref(rec->data);
    g_object_unref(rec->print);
    g_free(rec);
}

static void fpi_device_tudor_init(FpiDeviceTudor *tdev) {
    //Allocate data
    tdev->send_msg = (IPCMessageBuf*) g_malloc(sizeof(IPCMessageBuf));
    tdev->records = g_ptr_array_new_with_free_func((GDestroyNotify) record_free);
}

static void fpi_device_tudor_dispose(GObject *obj) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(obj);

    //Cleanup potential residue from close
    if(tdev->ipc_cancel) g_cancellable_cancel(tdev->ipc_cancel);
    g_clear_object(&tdev->ipc_cancel);
    if(tdev->host_proc) g_subprocess_force_exit(tdev->host_proc);
    g_clear_object(&tdev->host_proc);
    g_clear_object(&tdev->socket);

    //Free data
    g_free(tdev->send_msg);
    g_ptr_array_unref(tdev->records);
}

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

    //Check if it's a READY message
    if(msg->type != IPC_MSG_READY) {
        fpi_device_open_complete(dev, fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Expected READY message, got type 0x%x", msg->type));
        g_free(msg);
        return;
    }

    //Complete the open procedure
    fpi_device_open_complete(dev, NULL);
    g_free(msg);
}

static void fpi_device_tudor_open(FpDevice *dev) {
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

static void fpi_device_tudor_close(FpDevice *dev) {
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

static void fpi_device_tudor_enroll(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_enroll_complete(dev, NULL, error);
        return;
    }

    //Get print to enroll
    FpPrint *print;
    fpi_device_get_enroll_data(dev, &print);
}

static void fpi_device_tudor_verify(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_enroll_complete(dev, NULL, error);
        return;
    }

    //Get print to verify
    FpPrint *print;
    fpi_device_get_verify_data(dev, &print);
}

static void fpi_device_tudor_identify(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_enroll_complete(dev, NULL, error);
        return;
    }

    //Get prints to identify
    GPtrArray *prints;
    fpi_device_get_identify_data(dev, &prints);
}

static void fpi_device_tudor_list(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_list_complete(dev, NULL, error);
        return;
    }

    //Create prints array
    GPtrArray *prints = g_ptr_array_new_with_free_func(g_object_unref);
    g_ptr_array_set_size(prints, tdev->records->len);

    //Fill with print references
    for(int i = 0; i < tdev->records->len; i++) {
        struct record *rec = (struct record*) tdev->records->pdata[i];
        prints->pdata[i] = g_object_ref(rec->print);
    }

    //Notify libfprint of completion
    fpi_device_list_complete(dev, prints, NULL);
}

static void fpi_device_tudor_delete(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_delete_complete(dev, error);
        return;
    }

    //Get the print and its GUID / finger
    FpPrint *print;
    fpi_device_get_delete_data(dev, &print);

    RECGUID guid;
    enum tudor_finger finger;
    if(!get_print_guid_finger(tdev, print, &guid, &finger)) {
        fpi_device_delete_complete(dev, fpi_device_error_new(FP_DEVICE_ERROR_DATA_INVALID));
        return;
    }

    //Try to find the record's index
    int rec_idx = -1;
    for(int i = 0; i < tdev->records->len; i++) {
        struct record *rec = (struct record*) tdev->records->pdata[i];
        if(memcmp(&guid, &rec->guid, sizeof(RECGUID)) == 0 && finger == rec->finger) {
            rec_idx = i;
            break;
        }
    }
    if(rec_idx < 0) {
        fpi_device_delete_complete(dev, fpi_device_error_new(FP_DEVICE_ERROR_DATA_NOT_FOUND));
        return;
    }

    //Send the driver host an IPC message
    tdev->send_msg->size = sizeof(struct ipc_msg_del_record);
    tdev->send_msg->del_record = (struct ipc_msg_del_record) {
        .type = IPC_MSG_DEL_RECORD,
        .guid = guid,
        .finger = finger
    };
    if(!send_ipc_msg(tdev, tdev->send_msg, &error)) {
        fpi_device_delete_complete(dev, error);
        return;
    }

    //Remove the record
    g_ptr_array_remove_index_fast(tdev->records, rec_idx);

    //Notify libfprint of completion
    fpi_device_delete_complete(dev, NULL);
}

static void fpi_device_tudor_class_init(FpiDeviceTudorClass *class) {
    GObjectClass *obj_class = G_OBJECT_CLASS(class);
    obj_class->dispose = fpi_device_tudor_dispose;

    FpDeviceClass *dev_class = FP_DEVICE_CLASS(class);
    dev_class->id = "tudor_relink_dev";
    dev_class->full_name = "Synaptics Tudor";
    dev_class->type = FP_DEVICE_TYPE_USB;
    dev_class->id_table = tudor_ids;
    dev_class->nr_enroll_stages = NUM_ENROLL_STAGES;
    dev_class->scan_type = FP_SCAN_TYPE_PRESS;

    dev_class->open = fpi_device_tudor_open;
    dev_class->close = fpi_device_tudor_close;

    // dev_class->enroll = fpi_device_tudor_enroll;
    // dev_class->verify = fpi_device_tudor_verify;
    // dev_class->identify = fpi_device_tudor_identify;
    dev_class->list = fpi_device_tudor_list;
    dev_class->delete = fpi_device_tudor_delete;

    fpi_device_class_auto_initialize_features(dev_class);
}