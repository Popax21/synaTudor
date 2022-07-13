#include <tudor/dbus-launcher.h>
#include "data.h"
#include "ipc.h"

bool load_pdata(FpiDeviceTudor *tdev, GByteArray **pdata, GError **error) {
    //Tell the host launcher service to load the pairing data
    GVariant *rets = g_dbus_connection_call_sync(tdev->dbus_con,
        TUDOR_HOST_LAUNCHER_SERVICE, TUDOR_HOST_LAUNCHER_OBJ, TUDOR_HOST_LAUNCHER_INTERF,
        TUDOR_HOST_LAUNCHER_LOAD_PAIRING_DATA_METHOD, g_variant_new("(s)", tdev->sensor_name), G_VARIANT_TYPE("(bay)"), G_DBUS_CALL_FLAGS_NONE,
        G_MAXINT, NULL, error
    );
    if(!rets) return false;

    //Parse return value
    gboolean has_pdata;
    GVariant *pdata_var;
    g_variant_get(rets, "(b@ay)", &has_pdata, &pdata_var);
    g_variant_unref(rets);

    if(has_pdata) {
        //Create pairing data array
        gsize pdata_len;
        const void *pdata_data = g_variant_get_fixed_array(pdata_var, &pdata_len, 1);
        *pdata = g_byte_array_new_take(g_memdup2(pdata_data, pdata_len), pdata_len);

        g_info("Loaded pairing data for tudor sensor '%s' - %lu bytes", tdev->sensor_name, pdata_len);
    } else {
        *pdata = NULL;

        g_info("Loaded pairing data for tudor sensor '%s' - no stored data", tdev->sensor_name);
    }

    g_variant_unref(pdata_var);
    return true;
}

bool store_pdata(FpiDeviceTudor *tdev, GByteArray *pdata, GError **error) {
    //Tell the host launcher service to store the pairing data
    GVariant *params = g_variant_new("(s@ay)", tdev->sensor_name, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, pdata->data, pdata->len, 1));
    GVariant *rets = g_dbus_connection_call_sync(tdev->dbus_con,
        TUDOR_HOST_LAUNCHER_SERVICE, TUDOR_HOST_LAUNCHER_OBJ, TUDOR_HOST_LAUNCHER_INTERF,
        TUDOR_HOST_LAUNCHER_STORE_PAIRING_DATA_METHOD, params, NULL, G_DBUS_CALL_FLAGS_NONE,
        G_MAXINT, NULL, error
    );
    if(!rets) return false;
    g_variant_unref(rets);

    g_info("Stored pairing data for tudor sensor '%s' - %u bytes", tdev->sensor_name, pdata->len);
    return true;
}

G_DEFINE_TYPE(TudorRecord, tudor_record, G_TYPE_OBJECT);

static void tudor_record_init(TudorRecord *rec) {
    rec->guid = (RECGUID) {0};
    rec->finger = TUDOR_FINGER_UNKNOWN;
    rec->data = NULL;
    rec->print = NULL;
}

static void tudor_record_dispose(GObject *obj) {
    TudorRecord *rec = TUDOR_RECORD(obj);
    g_clear_pointer(&rec->data, g_bytes_unref);
    g_clear_object(&rec->print);

    G_OBJECT_CLASS(tudor_record_parent_class)->dispose(obj);
}

static void tudor_record_class_init(TudorRecordClass *class) {
    GObjectClass *obj_class = G_OBJECT_CLASS(class);
    obj_class->dispose = tudor_record_dispose;
}

TudorRecord *new_record(RECGUID guid, enum tudor_finger finger, GBytes *data, FpPrint *print) {
    TudorRecord *rec = g_object_new(tudor_record_get_type(), NULL);
    rec->guid = guid;
    rec->finger = finger;
    rec->data = data;
    rec->print = g_object_ref(print);
    return rec;
}

static void load_rec_acked_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *mtask = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    GTask *task = G_TASK(user_data);
    
    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(mtask, &error);
    if(!msg) {
        g_task_return_error(task, error);
        g_object_unref(task);
        return;
    }
    ipc_msg_buf_free(msg);

    //Insert the record to the list of loaded ones
    TudorRecord *rec = (TudorRecord*) g_task_get_task_data(task);
    insert_db_record(tdev, rec);

    g_debug("Loaded tudor host record GUID %08x... finger %d - size %lu", rec->guid.PartA, rec->finger, g_bytes_get_size(rec->data));

    g_task_return_pointer(task, rec, NULL);
    g_object_unref(task);
}

void load_record(FpiDeviceTudor *tdev, TudorRecord *rec, GAsyncReadyCallback cb, gpointer user_data) {
    //Create task
    GTask *task = g_task_new(tdev, tdev->ipc_cancel, cb, user_data);
    g_task_set_task_data(task, g_object_ref(rec), g_object_unref);

    //Check if the record is already loaded in the host DB
    for(int i = 0; i < tdev->db_records->len; i++) {
        TudorRecord *r = (TudorRecord*) tdev->db_records->pdata[i];
        if(r == rec || rec->print == r->print) goto rec_match;

        //Compare records
        if(memcmp(&rec->guid, &r->guid, sizeof(RECGUID)) != 0 || rec->finger != r->finger) continue;
        if(g_bytes_compare(rec->data, r->data) != 0) continue;

        rec_match:;
        g_task_return_pointer(task, r, NULL);
        g_object_unref(task);
        return;
    }

    //Send a message to the host to add the record
    size_t rec_size;
    const void *rec_data = g_bytes_get_data(rec->data, &rec_size);
    if(rec_size > IPC_MAX_RECORD_SIZE) {
        g_task_return_error(task, fpi_device_error_new_msg(FP_DEVICE_ERROR_DATA_INVALID, "Record data over maximum size: %lu > %d", rec_size, IPC_MAX_RECORD_SIZE));
        g_object_unref(task);
        return;
    }

    tdev->send_msg->size = sizeof(struct ipc_msg_add_record) + rec_size;
    tdev->send_msg->add_record = (struct ipc_msg_add_record) {
        .type = IPC_MSG_ADD_RECORD,
        .guid = rec->guid,
        .finger = rec->finger
    };
    memcpy(tdev->send_msg->add_record.record_data, rec_data, rec_size);

    send_acked_ipc_msg(tdev, tdev->send_msg, load_rec_acked_cb, task);
}

void insert_db_record(FpiDeviceTudor *tdev, TudorRecord *rec) {
    //Remove existing records
    for(int i = 0; i < tdev->db_records->len; i++) {
        TudorRecord *r = (TudorRecord*) tdev->db_records->pdata[i];
        if(r == rec) return;

        if(memcmp(&rec->guid, &r->guid, sizeof(RECGUID)) != 0 || rec->finger != r->finger) continue;
        if(g_bytes_compare(rec->data, r->data) != 0) continue;

        g_info("Removing conflicting duplicate tudor host record GUID %08x... finger %d", r->guid.PartA, r->finger);
        g_ptr_array_remove_index_fast(tdev->db_records, i);
    }

    //Add record
    g_ptr_array_add(tdev->db_records, g_object_ref(rec));
}

bool parse_print_data(FpiDeviceTudor *tdev, FpPrint *print, RECGUID *guid, enum tudor_finger *finger, GBytes **data) {
    //Check the driver ID
    gchar *print_driver;
    g_object_get(print, "driver", &print_driver, NULL);
    if(g_strcmp0(print_driver, fp_device_get_driver(FP_DEVICE(tdev))) != 0) {
        g_free(print_driver);
        return false;
    }
    g_free(print_driver);

    //Check if the print is marked as "device stored"
    if(!fp_print_get_device_stored(print)) return false;

    //Get the print data
    GVariant *print_data;
    g_object_get(print, "fpi-data", &print_data, NULL);

    //Check the data format
    if(!g_variant_check_format_string(print_data, "(@ayy@ay)", FALSE)) return false;

    //Parse the top level variant
    if(finger) *finger = 0;
    GVariant *guid_var = NULL, *data_var = NULL;
    g_variant_get(print_data, "(@ayy@ay)", guid ? &guid_var : NULL, finger, data ? &data_var : NULL);

    //Parse the GUID variant
    if(guid) {
        size_t var_guid_sz;
        const RECGUID *var_guid = (const RECGUID*) g_variant_get_fixed_array(guid_var, &var_guid_sz, 1);
        if(var_guid_sz != sizeof(RECGUID)) {
            g_clear_pointer(&guid_var, g_variant_unref);
            g_clear_pointer(&data_var, g_variant_unref);
            g_clear_pointer(&print_data, g_variant_unref);
            return false;
        }
        *guid = *var_guid;
    }

    if(data) {
        //Copy the data variant
        size_t data_sz;
        const void *data_ptr = g_variant_get_fixed_array(data_var, &data_sz, 1);
        *data = g_bytes_new(data_ptr, data_sz);
    }

    g_clear_pointer(&guid_var, g_variant_unref);
    g_clear_pointer(&data_var, g_variant_unref);
    g_clear_pointer(&print_data, g_variant_unref);
    return true;
}

void set_print_record(FpiDeviceTudor *tdev, FpPrint *print, TudorRecord *record) {
    //Set metadata
    fpi_print_set_type(print, FPI_PRINT_RAW);
    fpi_print_set_device_stored(print, TRUE);

    //Set data
    size_t rec_size;
    const void *rec_data = g_bytes_get_data(record->data, &rec_size);
    GVariant *print_data = g_variant_new("(@ayy@ay)", g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, &record->guid, sizeof(RECGUID), 1), record->finger, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, rec_data, rec_size, 1));
    g_object_set(print, "fpi-data", print_data, NULL);
}

struct delete_params {
    RECGUID guid;
    enum tudor_finger finger;
};

static void delete_acked_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct delete_params *params = (struct delete_params*) user_data;

    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_clear_storage_complete(FP_DEVICE(tdev), error);
        g_slice_free(struct delete_params, params);
        return;
    }
    ipc_msg_buf_free(msg);

    g_debug("Tudor host ACKed record GUID %08x... finger %d deletion", params->guid.PartA, params->finger);

    //Remove the record if it is in the DB mirror
    for(int i = 0; i < tdev->db_records->len; i++) {
        TudorRecord *rec = (TudorRecord*) tdev->db_records->pdata[i];
        if(memcmp(&rec->guid, &params->guid, sizeof(RECGUID)) == 0 && rec->finger == params->finger) {
            g_ptr_array_remove_index_fast(tdev->db_records, i);
            break;
        }
    }

    //Notify libfprint of completion
    fpi_device_delete_complete(FP_DEVICE(tdev), NULL);

    g_slice_free(struct delete_params, params);
}

void fpi_device_tudor_delete(FpDevice *dev) {
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

    struct delete_params *params = g_slice_new(struct delete_params);
    if(!parse_print_data(tdev, print, &params->guid, &params->finger, NULL)) {
        fpi_device_delete_complete(dev, fpi_device_error_new(FP_DEVICE_ERROR_DATA_INVALID));
        g_slice_free(struct delete_params, params);
        return;
    }

    //Tell the driver host
    tdev->send_msg->size = sizeof(struct ipc_msg_del_record);
    tdev->send_msg->del_record = (struct ipc_msg_del_record) {
        .type = IPC_MSG_DEL_RECORD,
        .guid = params->guid,
        .finger = params->finger
    };
    send_acked_ipc_msg(tdev, tdev->send_msg, delete_acked_cb, params);
}

static void clear_storage_acked_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);

    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_clear_storage_complete(FP_DEVICE(tdev), error);
        return;
    }
    ipc_msg_buf_free(msg);

    g_debug("Tudor host ACKed record clearing");

    //Clear DB records array
    g_ptr_array_set_size(tdev->db_records, 0);

    //Notify libfprint of completion
    fpi_device_clear_storage_complete(FP_DEVICE(tdev), NULL);
}

void fpi_device_clear_storage(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_clear_storage_complete(dev, error);
        return;
    }

    //Tell the driver host
    tdev->send_msg->size = sizeof(enum ipc_msg_type);
    tdev->send_msg->type = IPC_MSG_CLEAR_RECORDS;
    send_acked_ipc_msg(tdev, tdev->send_msg, clear_storage_acked_cb, NULL);
}