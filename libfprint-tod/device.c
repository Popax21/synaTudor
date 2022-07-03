#include "drivers_api.h"
#include "record.h"

#define NUM_ENROLL_STAGES 8

struct _FpiDeviceTudor {
    FpDevice parent;

    GPtrArray *records;
};

#define FPI_TYPE_DEVICE_TUDOR (fpi_device_tudor_get_type())
G_DECLARE_FINAL_TYPE(FpiDeviceTudor, fpi_device_tudor, FPI, DEVICE_TUDOR, FpDevice)
G_DEFINE_TYPE(FpiDeviceTudor, fpi_device_tudor, FP_TYPE_DEVICE)

GType fpi_tod_shared_driver_get_type() {
    return FPI_TYPE_DEVICE_TUDOR;
}

static FpIdEntry tudor_ids[] = {
    { .pid = 0x06cb, .vid = 0x00be },
//  { .pid = 0x06cb, .vid = 0x00bd }, //TODO Check if it's also supported
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
}

static void fpi_device_tudor_init(FpiDeviceTudor *tdev) {
    tdev->records = g_ptr_array_new_with_free_func((GDestroyNotify) record_free);
}

static void fpi_device_tudor_dispose(GObject *obj) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(obj);

    //Free records array
    g_ptr_array_unref(tdev->records);
}

static void fpi_device_tudor_open(FpDevice *dev) {

}

static void fpi_device_tudor_close(FpDevice *dev) {
    
}

static void fpi_device_tudor_enroll(FpDevice *dev) {
    FpPrint *print;
    fpi_device_get_enroll_data(dev, &print);
}

static void fpi_device_tudor_verify(FpDevice *dev) {
    FpPrint *print;
    fpi_device_get_verify_data(dev, &print);
}

static void fpi_device_tudor_identify(FpDevice *dev) {
    GPtrArray *prints;
    fpi_device_get_identify_data(dev, &prints);
}

static void fpi_device_tudor_list(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Create prints array
    GPtrArray *prints = g_ptr_array_new_with_free_func(g_object_unref);
    g_ptr_array_set_size(prints, tdev->records->len);

    //Fill with print references
    for(int i = 0; i < tdev->records->len; i++) {
        struct record *rec = (struct record*) tdev->records->pdata[i];
        prints->pdata[i] = g_object_ref(rec->print);
    }

    //Pass to libfprint
    fpi_device_list_complete(dev, prints, NULL);
}

static void fpi_device_tudor_delete(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

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

    //Remove the record
    g_ptr_array_remove_index_fast(tdev->records, rec_idx);

    //FIXME Tell the driver host over IPC

    fpi_device_delete_complete(dev, NULL);
}

static void fpi_device_tudor_class_init(FpiDeviceTudorClass *class) {
    GObjectClass *obj_class = G_OBJECT_CLASS(class);
    obj_class->dispose = fpi_device_tudor_dispose;

    FpDeviceClass *dev_class = FP_DEVICE_CLASS(class);
    dev_class->id = "tudor_relink_dev";
    dev_class->full_name = "Synaptics Tudor libfprint driver";
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