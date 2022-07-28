#include "device.h"
#include "data.h"
#include "ipc.h"
#include "open.h"
#include "enroll.h"
#include "verify.h"
#include "identify.h"

G_DEFINE_TYPE(FpiDeviceTudor, fpi_device_tudor, FP_TYPE_DEVICE)

GType fpi_tod_shared_driver_get_type() {
    return FPI_TYPE_DEVICE_TUDOR;
}

static FpIdEntry tudor_ids[] = {
    { .vid = 0x06cb, .pid = 0x00be },
//  { .vid = 0x06cb, .pid = 0x00bd }, //TODO Check if it's also supported
    { 0 }
};

static void close_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    //Check for errors
    GError *error = NULL;
    g_task_propagate_int(G_TASK(res), &error);
    if(error) {
        g_warning("Error closing tudor device: %s [%s code %d]", error->message, g_quark_to_string(error->domain), error->code);
        g_clear_error(&error);
    }

    //Set callback called flag
    *((bool*) user_data) = true;
}

static void ensure_closed(FpiDeviceTudor *tdev) {
    if(tdev->host_has_id) {
        //Close the device
        g_info("Closing tudor device host ID %u...", tdev->host_id);

        bool cb_called = false;
        close_device(g_object_ref(tdev), close_cb, &cb_called);
        while(!cb_called) g_main_context_iteration(NULL, TRUE);
    }
}

static GList *dev_list = 0;

static void fpi_device_tudor_init(FpiDeviceTudor *tdev) {
    tdev->usb_fd = -1;
    tdev->host_has_id = false;
    tdev->host_sleep_inhib = -1;
    tdev->ipc_socket = NULL;
    tdev->ipc_cancel = NULL;
    tdev->close_task = NULL;
    tdev->cancel_handler_id = 0;
    tdev->sensor_name = NULL;

    //Allocate data
    tdev->send_msg = ipc_msg_buf_new();
    tdev->db_records = g_ptr_array_new_with_free_func(g_object_unref);

    //Add to device list
    tdev->dev_list_link = dev_list = g_list_prepend(dev_list, tdev);
}

static void fpi_device_tudor_dispose(GObject *obj) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(obj);

    //Remove from device list
    dev_list = g_list_delete_link(dev_list, tdev->dev_list_link);

    //Ensure the device's closed
    ensure_closed(tdev);

    //Chain to parent
    G_OBJECT_CLASS(fpi_device_tudor_parent_class)->dispose(obj);
}

static void fpi_device_tudor_finalize(GObject *obj) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(obj);

    //Close DBus connection
    g_clear_object(&tdev->dbus_con);

    //Free data
    ipc_msg_buf_free(tdev->send_msg);
    g_ptr_array_unref(tdev->db_records);

    //Free USB FD
    if(tdev->usb_fd >= 0) g_assert_no_errno(close(tdev->usb_fd));

    //Chain to parent
    G_OBJECT_CLASS(fpi_device_tudor_parent_class)->finalize(obj);
}

static void cancel_cb(GCancellable *cancel, gpointer user_data) {
    invoke_cancel_handler(FPI_DEVICE_TUDOR(user_data));
}

void register_cancel_handler(FpiDeviceTudor *tdev) {
    tdev->has_canceled = false;
    tdev->cancel_handler_id = g_cancellable_connect(fpi_device_get_cancellable(FP_DEVICE(tdev)), G_CALLBACK(cancel_cb), tdev, NULL);
}

void unregister_cancel_handler(FpiDeviceTudor *tdev) {
    if(tdev->cancel_handler_id != 0) {
        g_cancellable_disconnect(fpi_device_get_cancellable(FP_DEVICE(tdev)), tdev->cancel_handler_id);
        tdev->cancel_handler_id = 0;
    }
}

void invoke_cancel_handler(FpiDeviceTudor *tdev) {
    if(tdev->cancel_handler_id == 0 || tdev->has_canceled) return;

    //Force timeout back to normal
    g_socket_set_timeout(tdev->ipc_socket, IPC_TIMEOUT_SECS);

    //Send cancel message
    GError *error = NULL;
    tdev->send_msg->size = sizeof(enum ipc_msg_type);
    tdev->send_msg->type = IPC_MSG_CANCEL;
    if(!send_ipc_msg(tdev, tdev->send_msg, &error)) {
        //Not much we can do
        g_warning("Failed to send cancel IPC message: %s [%s code %d]", error->message, g_quark_to_string(error->domain), error->code);
        g_clear_error(&error);
        return;
    }
    tdev->has_canceled = true;

    g_debug("Sent tudor host cancel IPC message");
}

GError *handle_cancel_ack(FpiDeviceTudor *tdev) {
    //Check if the action is actually cancelled
    if(tdev->has_canceled) {
        g_debug("Received tudor cancel IPC ACK");
        return g_error_new_literal(G_IO_ERROR, G_IO_ERROR_CANCELLED, "Cancellation requested");
    } else {
        g_warning("Received unexpected tudor IPC ACK for CANCEL which was never sent!");
        return fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Received unexpected ACK for CANCEL which was never sent");
    }
}

static void atexit_hook() {
    g_debug("Tudor close atexit hook called");
    for(GList *l = dev_list; l; l = l->next) {
        g_debug("Cleaning up tudor host %p...", l->data);
        ensure_closed(FPI_DEVICE_TUDOR(l->data));
    }
}

static void fpi_device_tudor_class_init(FpiDeviceTudorClass *class) {
    GObjectClass *obj_class = G_OBJECT_CLASS(class);
    obj_class->dispose = fpi_device_tudor_dispose;
    obj_class->finalize = fpi_device_tudor_finalize;

    FpDeviceClass *dev_class = FP_DEVICE_CLASS(class);
    dev_class->id = "syna_tudor_relink";
    dev_class->full_name = "Synaptics Tudor";
    dev_class->type = FP_DEVICE_TYPE_USB;
    dev_class->id_table = tudor_ids;
    dev_class->nr_enroll_stages = TUDOR_NUM_ENROLL_STAGES;
    dev_class->scan_type = FP_SCAN_TYPE_PRESS;

    dev_class->probe = fpi_device_tudor_probe;
    dev_class->open = fpi_device_tudor_open;
    dev_class->close = fpi_device_tudor_close;

    dev_class->enroll = fpi_device_tudor_enroll;
    dev_class->verify = fpi_device_tudor_verify;
    dev_class->identify = fpi_device_tudor_identify;
    dev_class->delete = fpi_device_tudor_delete;
    dev_class->clear_storage = fpi_device_clear_storage;

    fpi_device_class_auto_initialize_features(dev_class);

    //fprintd does some stupid stuff and just calls exit() when its timeout is hit
    //Use an atexit hook to still properly shut down anyway
    atexit(atexit_hook);
}