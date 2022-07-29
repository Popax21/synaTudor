#include "data.h"
#include "ipc.h"
#include "verify.h"

struct verify_params {
    FpiDeviceTudor *tdev;

    RECGUID guid;
    enum tudor_finger finger;
};

static void free_verify_params(struct verify_params *params) {
    unregister_cancel_handler(params->tdev);
    g_slice_free(struct verify_params, params);
}

static void verify_recv_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct verify_params *params = (struct verify_params*) user_data;

    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_verify_complete(FP_DEVICE(tdev), error);
        free_verify_params(params);
        return;
    }

    //Handle the message
    switch(msg->type) {
        case IPC_MSG_ACK: {
            //Assume it's an ACK to a CANCEL message
            fpi_device_verify_complete(FP_DEVICE(tdev), handle_cancel_ack(tdev));
            free_verify_params(params);
        } break;
        case IPC_MSG_RESP_VERIFY: {
            if(!check_ipc_msg_size(msg, sizeof(msg->resp_verify), &error)) {
                fpi_device_verify_complete(FP_DEVICE(tdev), error);
                free_verify_params(params);
                break;
            }

            //Check if we should retry
            if(msg->resp_verify.retry) {
                g_info("Tudor host requested verify capture retry for GUID %08x... finger %d", params->guid.PartA, params->finger);
                fpi_device_verify_report(FP_DEVICE(tdev), FPI_MATCH_ERROR, NULL, fpi_device_retry_new(FP_DEVICE_RETRY_GENERAL));
                recv_ipc_msg_no_timeout(tdev, verify_recv_cb, user_data);
                break;
            }

            g_info("Tudor host reported verify result %s match", msg->resp_verify.did_match ? "did" : "did not");

            //Report the result
            fpi_device_verify_report(FP_DEVICE(tdev), msg->resp_verify.did_match ? FPI_MATCH_SUCCESS : FPI_MATCH_FAIL, NULL, NULL);
            fpi_device_verify_complete(FP_DEVICE(tdev), NULL);
            free_verify_params(params);
        } break;
        default: {
            fpi_device_verify_complete(FP_DEVICE(tdev), fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Unexpected message in verify sequence: 0x%x", msg->type));
            free_verify_params(params);
        } break;
    }

    ipc_msg_buf_free(msg);
}

static void verify_acked_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct verify_params *params = (struct verify_params*) user_data;

    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_verify_complete(FP_DEVICE(tdev), error);
        free_verify_params(params);
        return;
    }
    ipc_msg_buf_free(msg);

    g_debug("Tudor host ACKed verify IPC message");

    //Set ready flag
    fpi_device_report_finger_status_changes(FP_DEVICE(tdev), FP_FINGER_STATUS_NEEDED, FP_FINGER_STATUS_NONE);

    //Register cancellation handler
    register_cancel_handler(tdev);

    //Start response listener
    recv_ipc_msg_no_timeout(tdev, verify_recv_cb, user_data);
}

static void verify_load_record_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct verify_params *params = (struct verify_params*) user_data;

    //Check for errors
    GError *error = NULL;
    TudorRecord *rec = g_task_propagate_pointer(task, &error);
    if(!rec) {
        fpi_device_verify_complete(FP_DEVICE(tdev), error);
        free_verify_params(params);
        return;
    }

    g_info("Loaded tudor verify record: GUID %08x... finger %d", rec->guid.PartA, rec->finger);

    //Send verify IPC message
    g_debug("Sending tudor verify IPC message...");
    tdev->send_msg->size = sizeof(struct ipc_msg_verify);
    tdev->send_msg->verify = (struct ipc_msg_verify) {
        .type = IPC_MSG_VERIFY,
        .guid = params->guid,
        .finger = params->finger
    };
    send_acked_ipc_msg(tdev, tdev->send_msg, verify_acked_cb, params);
}

void fpi_device_tudor_verify(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_verify_complete(dev, error);
        return;
    }

    //Get print to verify
    FpPrint *print;
    fpi_device_get_verify_data(dev, &print);

    //Parse the print data
    RECGUID guid;
    enum tudor_finger finger;
    GBytes *data;
    if(!parse_print_data(tdev, print, &guid, &finger, &data)) {
        fpi_device_verify_complete(dev, fpi_device_error_new_msg(FP_DEVICE_ERROR_DATA_INVALID, "Invalid print data"));
        return;
    }

    //Create verify params
    struct verify_params *params = g_slice_new(struct verify_params);
    *params = (struct verify_params) {
        .tdev = tdev,
        .guid = guid,
        .finger = finger
    };

    //Load the record
    TudorRecord *rec = new_record(guid, finger, data, print);
    g_debug("Loading verify print tudor host record...");
    load_record(params->tdev, rec, verify_load_record_cb, params);
    g_object_unref(rec);
}