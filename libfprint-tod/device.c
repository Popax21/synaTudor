#include "device.h"
#include "data.h"
#include "ipc.h"
#include "open.h"

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

static void record_free(struct record *rec) {
    g_bytes_unref(rec->data);
    g_object_unref(rec->print);
    g_free(rec);
}

static void fpi_device_tudor_init(FpiDeviceTudor *tdev) {
    //Allocate data
    tdev->send_msg = (IPCMessageBuf*) g_malloc(sizeof(IPCMessageBuf));
    tdev->db_records = g_ptr_array_new_with_free_func((GDestroyNotify) record_free);
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
    g_ptr_array_unref(tdev->db_records);
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
    dev_class->delete = fpi_device_tudor_delete;
    dev_class->clear_storage = fpi_device_clear_storage;

    fpi_device_class_auto_initialize_features(dev_class);
}