#include "data.h"
#include "ipc.h"
#include "enroll.h"

struct enroll_params {
    FpiDeviceTudor *tdev;

    RECGUID guid;
    enum tudor_finger finger;

    int enroll_stage;
};

static void free_enroll_params(struct enroll_params *params) {
    unregister_cancel_handler(params->tdev);
    g_slice_free(struct enroll_params, params);
}

static void enroll_recv_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct enroll_params *params = (struct enroll_params*) user_data;

    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_enroll_complete(FP_DEVICE(tdev), NULL, error);
        free_enroll_params(params);
        return;
    }

    //Handle the message
    switch(msg->type) {
        case IPC_MSG_ACK: {
            //Assume it's an ACK to a CANCEL message
            fpi_device_enroll_complete(FP_DEVICE(tdev), NULL, handle_cancel_ack(tdev));
            free_enroll_params(params);
        } break;
        case IPC_MSG_RESP_ENROLL: {
            if(!check_ipc_msg_size(msg, sizeof(msg->resp_enroll), &error)) {
                fpi_device_enroll_complete(FP_DEVICE(tdev), NULL, error);
                free_enroll_params(params);
                break;
            }

            //Check if we should retry
            if(msg->resp_enroll.retry) {
                g_info("Tudor host requested enroll capture retry for GUID %08x... finger %d", params->guid.PartA, params->finger);
                fpi_device_enroll_progress(FP_DEVICE(tdev), params->enroll_stage, NULL, fpi_device_retry_new(FP_DEVICE_RETRY_GENERAL));
                recv_ipc_msg_no_timeout(tdev, enroll_recv_cb, user_data);
                break;
            }

            //Check if we're done
            if(!msg->resp_enroll.done) {
                g_info("Tudor host requested more enroll captures for GUID %08x... finger %d", params->guid.PartA, params->finger);

                //Move onto next stage
                if(params->enroll_stage < TUDOR_NUM_ENROLL_STAGES-1) {
                    params->enroll_stage++;
                    fpi_device_enroll_progress(FP_DEVICE(tdev), params->enroll_stage, NULL, NULL);
                } else {
                    //Fall back to retrying
                    fpi_device_enroll_progress(FP_DEVICE(tdev), params->enroll_stage, NULL, fpi_device_retry_new(FP_DEVICE_RETRY_GENERAL));
                }

                //Await further messages
                recv_ipc_msg_no_timeout(tdev, enroll_recv_cb, user_data);
                break;
            }

            //Complete the enrollment
            //Get the print to enroll
            FpPrint *print;
            fpi_device_get_enroll_data(FP_DEVICE(tdev), &print);

            //Create a record and insert it into the database mirror
            size_t rec_size = msg->size - sizeof(msg->resp_enroll);
            if(rec_size > IPC_MAX_RECORD_SIZE) {
                fpi_device_enroll_complete(FP_DEVICE(tdev), NULL, fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Enrolmment record exceeding maximum size: %lu > %d", rec_size, IPC_MAX_RECORD_SIZE));
                free_enroll_params(params);
                break;
            }

            TudorRecord *rec = new_record(params->guid, params->finger, g_bytes_new(msg->resp_enroll.record_data, rec_size), print);
            insert_db_record(tdev, rec);

            g_info("Tudor host reported enroll completion for GUID %08x... finger %d - record size %lu", params->guid.PartA, params->finger, rec_size);

            //Complete the enrollment
            set_print_record(tdev, print, rec);
            fpi_device_enroll_complete(FP_DEVICE(tdev), g_object_ref(print), NULL);

            g_object_unref(rec);
            free_enroll_params(params);
        } break;
        default: {
            fpi_device_enroll_complete(FP_DEVICE(tdev), NULL, fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Unexpected message in enroll sequence: 0x%x", msg->type));
            free_enroll_params(params);
        } break;
    }

    ipc_msg_buf_free(msg);
}

static void enroll_start_acked_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(src_obj);
    struct enroll_params *params = (struct enroll_params*) user_data;

    //Check for errors
    GError *error = NULL;
    IPCMessageBuf *msg = g_task_propagate_pointer(task, &error);
    if(!msg) {
        fpi_device_enroll_complete(FP_DEVICE(tdev), NULL, error);
        free_enroll_params(params);
        return;
    }
    ipc_msg_buf_free(msg);

    g_debug("Tudor host ACKed enroll IPC message");

    //Set ready flag
    fpi_device_report_finger_status_changes(FP_DEVICE(tdev), FP_FINGER_STATUS_NEEDED, FP_FINGER_STATUS_NONE);

    //Register cancellation handler
    register_cancel_handler(tdev);

    //Start response listener
    recv_ipc_msg_no_timeout(tdev, enroll_recv_cb, user_data);
}

void fpi_device_tudor_enroll(FpDevice *dev) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    //Check if host process is dead
    GError *error = NULL;
    if(check_host_proc_dead(tdev, &error)) {
        fpi_device_enroll_complete(dev, NULL, error);
        return;
    }

    //Get the print to enroll
    FpPrint *print;
    fpi_device_get_enroll_data(dev, &print);

    //Check the driver ID
    gchar *print_driver;
    g_object_get(print, "driver", &print_driver, NULL);
    if(g_strcmp0(print_driver, fp_device_get_driver(dev)) != 0) {
        fpi_device_enroll_complete(dev, NULL, fpi_device_error_new_msg(FP_DEVICE_ERROR_DATA_INVALID, "Invalid driver ID '%s'", print_driver));
        g_free(print_driver);
        return;
    }
    g_free(print_driver);

    //Generate a new GUID
    RECGUID guid = {0};
    guid.PartA = g_random_int();
    *((uint64_t*) &guid.PartB) = g_get_real_time();
    g_debug("Generated new tudor host record GUID for enrollment: %08x...", guid.PartA);

    //Get the finger
    FpFinger fp_finger = fp_print_get_finger(print);
    enum tudor_finger finger;
    switch(fp_finger) {
        case FP_FINGER_LEFT_THUMB: finger = TUDOR_FINGER_LH_THUMB; break; 
        case FP_FINGER_LEFT_INDEX: finger = TUDOR_FINGER_LH_INDEX_FINGER; break; 
        case FP_FINGER_LEFT_MIDDLE: finger = TUDOR_FINGER_LH_MIDDLE_FINGER; break; 
        case FP_FINGER_LEFT_RING: finger = TUDOR_FINGER_LH_RING_FINGER; break; 
        case FP_FINGER_LEFT_LITTLE: finger = TUDOR_FINGER_LH_LITTLE_FINGER; break;
        case FP_FINGER_RIGHT_THUMB: finger = TUDOR_FINGER_RH_THUMB; break; 
        case FP_FINGER_RIGHT_INDEX: finger = TUDOR_FINGER_RH_INDEX_FINGER; break; 
        case FP_FINGER_RIGHT_MIDDLE: finger = TUDOR_FINGER_RH_MIDDLE_FINGER; break; 
        case FP_FINGER_RIGHT_RING: finger = TUDOR_FINGER_RH_RING_FINGER; break; 
        case FP_FINGER_RIGHT_LITTLE: finger = TUDOR_FINGER_RH_LITTLE_FINGER; break;
        default: {
            fpi_device_enroll_complete(dev, NULL, fpi_device_error_new_msg(FP_DEVICE_ERROR_DATA_INVALID, "Unsupported finger %d", fp_finger));
            return;
        }
    }

    //Create enroll params
    struct enroll_params *params = g_slice_new(struct enroll_params);
    *params = (struct enroll_params) {
        .tdev = tdev,
        .guid = guid,
        .finger = finger,
        .enroll_stage = 0
    };

    //Send enroll IPC message
    g_debug("Sending tudor enroll IPC message...");
    tdev->send_msg->size = sizeof(struct ipc_msg_enroll);
    tdev->send_msg->enroll = (struct ipc_msg_enroll) {
        .type = IPC_MSG_ENROLL,
        .guid = guid,
        .finger = finger
    };
    send_acked_ipc_msg(tdev, tdev->send_msg, enroll_start_acked_cb, params);
}