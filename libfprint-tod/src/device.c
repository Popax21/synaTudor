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

static void fpi_device_tudor_init(FpiDeviceTudor *tdev) {
    //Allocate data
    tdev->send_msg = ipc_msg_buf_new();
    tdev->db_records = g_ptr_array_new_with_free_func(g_object_unref);
}

static void fpi_device_tudor_dispose(GObject *obj) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(obj);

    //Free data
    ipc_msg_buf_free(tdev->send_msg);
    g_ptr_array_unref(tdev->db_records);
}

static void cancel_cb(GCancellable *cancel, gpointer user_data) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(user_data);

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

    g_debug("Sent tudor cancel IPC message");
}

guint register_cancel_handler(FpiDeviceTudor *tdev) {
    tdev->has_canceled = false;
    return g_cancellable_connect(fpi_device_get_cancellable(FP_DEVICE(tdev)), G_CALLBACK(cancel_cb), tdev, NULL);
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

static void fpi_device_tudor_class_init(FpiDeviceTudorClass *class) {
    GObjectClass *obj_class = G_OBJECT_CLASS(class);
    obj_class->dispose = fpi_device_tudor_dispose;

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
}