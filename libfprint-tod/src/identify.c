#include "data.h"
#include "ipc.h"
#include "identify.h"

struct identify_params {
    FpiDeviceTudor *tdev;

    GArray *prints;
    int next_print_idx;
};

static void free_identify_params(struct identify_params *params) {
    unregister_cancel_handler(params->tdev);
    g_array_unref(params->prints);
    g_slice_free(struct identify_params, params);
}

struct identify_print {
    FpPrint *print;
    TudorRecord *record;
};

static void clear_identify_print(struct identify_print *print) {
    g_clear_object(&print->print);
    g_clear_object(&print->record);
}

static void identify_recv_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct identify_params *params = (struct identify_params*) user_data;

    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_identify_complete(FP_DEVICE(tdev), error);
        free_identify_params(params);
        return;
    }

    //Handle the message
    switch(msg->type) {
        case IPC_MSG_ACK: {
            //Assume it's an ACK to a CANCEL message
            fpi_device_identify_complete(FP_DEVICE(tdev), handle_cancel_ack(tdev));
            free_identify_params(params);
        } break;
        case IPC_MSG_RESP_IDENTIFY: {
            if(!check_ipc_msg_size(msg, sizeof(msg->resp_identify), &error)) {
                fpi_device_identify_complete(FP_DEVICE(tdev), error);
                free_identify_params(params);
                break;
            }

            //Check if we should retry
            if(msg->resp_identify.retry) {
                g_info("Tudor host requested identify capture retry");
                fpi_device_identify_report(FP_DEVICE(tdev), NULL, NULL, fpi_device_retry_new(FP_DEVICE_RETRY_GENERAL));
                recv_ipc_msg_no_timeout(tdev, identify_recv_cb, user_data);
                break;
            }

            //Check if there was a match
            if(!msg->resp_identify.did_match) {
                g_info("Tudor host reported no identify match");
                fpi_device_identify_report(FP_DEVICE(tdev), NULL, NULL, NULL);
                fpi_device_identify_complete(FP_DEVICE(tdev), NULL);
                free_identify_params(params);
                break;
            }

            //Search database mirror for record which matched
            TudorRecord *match_rec = NULL;
            for(int i = 0; i < tdev->db_records->len; i++) {
                TudorRecord *rec = (TudorRecord*) tdev->db_records->pdata[i];
                if(memcmp(&rec->guid, &msg->resp_identify.guid, sizeof(RECGUID)) != 0 || rec->finger != msg->resp_identify.finger) continue;
                match_rec = rec;
                break;
            }
            if(!match_rec) {
                g_warning("Tudor host reported identify match but couldn't find matching record!");
                fpi_device_identify_report(FP_DEVICE(tdev), NULL, NULL, NULL);
                fpi_device_identify_complete(FP_DEVICE(tdev), NULL);
                free_identify_params(params);
                break;
            }
            g_info("Tudor host reported identify match: GUID %08x... finger %d", match_rec->guid.PartA, match_rec->finger);

            //Check if the print shows up in the list of prints to match against
            FpPrint *list_print = NULL;
            for(int i = 0; i < params->prints->len; i++) {
                struct identify_print *print = &g_array_index(params->prints, struct identify_print, i);
                if(print->record != match_rec) continue;
                list_print = print->print;
                break;
            }

            //Report the result
            fpi_device_identify_report(FP_DEVICE(tdev), list_print, match_rec->print, NULL);
            fpi_device_identify_complete(FP_DEVICE(tdev), NULL);
            free_identify_params(params);
        } break;
        default: {
            fpi_device_identify_complete(FP_DEVICE(tdev), fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Unexpected message in identify sequence: 0x%x", msg->type));
            free_identify_params(params);
        } break;
    }

    ipc_msg_buf_free(msg);
}

static void identify_acked_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct identify_params *params = (struct identify_params*) user_data;

    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_identify_complete(FP_DEVICE(tdev), error);
        free_identify_params(params);
        return;
    }
    ipc_msg_buf_free(msg);

    g_debug("Tudor host ACKed identify IPC message");

    //Set ready flag
    fpi_device_report_finger_status_changes(FP_DEVICE(tdev), FP_FINGER_STATUS_NEEDED, FP_FINGER_STATUS_NONE);

    //Register cancellation handler
    register_cancel_handler(tdev);

    //Start response listener
    recv_ipc_msg_no_timeout(tdev, identify_recv_cb, user_data);
}

static void load_next_print(struct identify_params *params);

static void load_record_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct identify_params *params = (struct identify_params*) user_data;

    //Check for errors
    GError *error = NULL;
    TudorRecord *rec = g_task_propagate_pointer(task, &error);
    if(!rec) {
        fpi_device_identify_complete(FP_DEVICE(tdev), error);
        free_identify_params(params);
        return;
    }

    //Set the print's record
    struct identify_print *print = &g_array_index(params->prints, struct identify_print, params->next_print_idx);
    print->record = g_object_ref(rec);
    g_debug("Loaded tudor identify print record %d/%d: GUID %08x... finger %d", params->next_print_idx+1, params->prints->len, rec->guid.PartA, rec->finger);

    //Load the next print
    params->next_print_idx++;
    load_next_print(params);
}

static void load_next_print(struct identify_params *params) {
    FpiDeviceTudor *tdev = params->tdev;

    //Check if we're done loading prints
    if(params->next_print_idx >= params->prints->len) {
        g_info("Loaded %d tudor identify print records", params->prints->len);

        //Send identify IPC message
        g_debug("Sending tudor identify IPC message...");
        tdev->send_msg->size = sizeof(enum ipc_msg_type);
        tdev->send_msg->type = IPC_MSG_IDENTIFY;
        send_acked_ipc_msg(tdev, tdev->send_msg, identify_acked_cb, params);
        return;
    }
    struct identify_print *print = &g_array_index(params->prints, struct identify_print, params->next_print_idx);

    //Create a record for the print
    RECGUID guid;
    enum tudor_finger finger;
    GBytes *data;
    if(!parse_print_data(tdev, print->print, &guid, &finger, &data)) {
        fpi_device_identify_complete(FP_DEVICE(tdev), fpi_device_error_new_msg(FP_DEVICE_ERROR_DATA_INVALID, "Invalid print data"));
        free_identify_params(params);
        return;
    }

    TudorRecord *rec = new_record(guid, finger, data, print->print);

    //Load the record
    load_record(params->tdev, rec, load_record_cb, params);

    g_object_unref(rec);
}

void fpi_device_tudor_identify(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_identify_complete(dev, error);
        return;
    }

    //Get prints to identify
    GPtrArray *fp_prints;
    fpi_device_get_identify_data(dev, &fp_prints);

    //Create identify prints array
    GArray *prints = g_array_sized_new(false, false, sizeof(struct identify_print), fp_prints->len);
    g_array_set_clear_func(prints, (GDestroyNotify) clear_identify_print);
    for(int i = 0; i < fp_prints->len; i++) {
        struct identify_print print = {
            .print = g_object_ref(FP_PRINT(fp_prints->pdata[i])),
            .record = NULL
        };
        g_array_append_val(prints, print);
    }

    //Create identify params
    struct identify_params *params = g_slice_new(struct identify_params);
    *params = (struct identify_params) {
        .tdev = tdev,
        .prints = prints,
        .next_print_idx = 0
    };

    //Start to load the first print
    load_next_print(params);
}