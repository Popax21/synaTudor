#include <tudor/tudor.h>
#include "handler.h"

static void consume_simple_msg(int ipc_sock, enum ipc_msg_type type) {
    ipc_recv_msg(ipc_sock, &type, type, sizeof(type), sizeof(type), NULL);
}

static void send_ack(int ipc_sock) {
    enum ipc_msg_type type = IPC_MSG_ACK;
    ipc_send_msg(ipc_sock, &type, sizeof(type));
}

bool handle_ipc_msg(struct tudor_device *dev, int ipc_sock, enum ipc_msg_type type) {
    switch(type) {
        case IPC_MSG_SHUTDOWN: {
            consume_simple_msg(ipc_sock, type);
        } return true;

        case IPC_MSG_ADD_RECORD: {
            struct {
                struct ipc_msg_add_record msg;
                char buf[IPC_MAX_RECORD_SIZE];
            } msg;
            size_t rec_size = ipc_recv_msg(ipc_sock, &msg, IPC_MSG_ADD_RECORD, sizeof(msg.msg), sizeof(msg), NULL) - sizeof(msg.msg);

            if(!tudor_add_record(dev, msg.msg.guid, msg.msg.finger, msg.msg.record_data, rec_size)) {
                //Delete the old one first
                tudor_wipe_records(dev, &msg.msg.guid, msg.msg.finger);
                if(!tudor_add_record(dev, msg.msg.guid, msg.msg.finger, msg.msg.record_data, rec_size)) {
                    log_error("Couldn't add record!");
                    abort();
                }
            }

            log_debug("Added record GUID %08x... finger %d size %lu", msg.msg.guid.PartA, msg.msg.finger, rec_size);
            send_ack(ipc_sock);
        } break;

        case IPC_MSG_DEL_RECORD: {
            struct ipc_msg_del_record msg;
            ipc_recv_msg(ipc_sock, &msg, IPC_MSG_DEL_RECORD, sizeof(msg), sizeof(msg), NULL);

            int num_recs = tudor_wipe_records(dev, &msg.guid, msg.finger);
            log_debug("Deleted %d records with GUID %08x... finger %d", num_recs, msg.guid.PartA, msg.finger);

            send_ack(ipc_sock);
        } break;

        case IPC_MSG_CLEAR_RECORDS: {
            consume_simple_msg(ipc_sock, type);

            int num_recs = tudor_wipe_records(dev, NULL, TUDOR_FINGER_ANY);
            log_debug("Cleared %d records", num_recs);

            send_ack(ipc_sock);
        } break;

        default: {
            log_error("Unknown or unexpected IPC message type 0x%x!", type);
            abort();
        }
    }

    return false;
}