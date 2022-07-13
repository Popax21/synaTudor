#include <tudor/tudor.h>
#include "handler.h"

struct handler_state {
    struct tudor_device *dev;
    pthread_mutex_t lock;
    int ipc_sock;

    tudor_async_res_t async_res;
    bool async_cancelled;

    union {
        struct {
            RECGUID guid;
            enum tudor_finger finger;
            bool done;
        } enroll;
        struct {
            RECGUID guid;
            enum tudor_finger finger;
            bool retry, matches;
        } verify;
        struct {
            bool retry, has_match;
            RECGUID guid;
            enum tudor_finger finger;
        } identify;
    } action;
};

static inline void consume_simple_msg(int ipc_sock, enum ipc_msg_type type) {
    ipc_recv_msg(ipc_sock, &type, type, sizeof(type), sizeof(type), NULL);
}

static inline void send_ack(int ipc_sock) {
    enum ipc_msg_type type = IPC_MSG_ACK;
    ipc_send_msg(ipc_sock, &type, sizeof(type));
}

static inline void check_in_action(struct handler_state *state, enum ipc_msg_type type) {
    if(state->async_res) {
        log_error("Invalid message 0x%x while already in an action!", type);
        abort();
    }
}

static void init_action(struct handler_state *state) {
    state->async_res = NULL;
    state->async_cancelled = false;
}

static bool cleanup_action(struct handler_state *state) {
    //Clean up result
    tudor_cleanup_async(state->async_res);
    state->async_res = NULL;

    //If we were cancelled, send an ACK   
    if(state->async_cancelled) {
        send_ack(state->ipc_sock);
        return false;   
    }

    return true;
}

static void enroll_cb(tudor_async_res_t *res, bool success, struct handler_state *state) {
    cant_fail_ret(pthread_mutex_lock(&state->lock));

    if(!cleanup_action(state)) {
        if(!tudor_enroll_discard(state->dev)) {
            log_error("Couldn't discard enrollment!");
            abort();
        }

        cant_fail_ret(pthread_mutex_unlock(&state->lock));
        return;
    }

    //Check success
    if(!success && state->action.enroll.done) {
        log_error("Enroll action failed!");
        abort();
    }

    //Send response
    if(state->action.enroll.done) {
        //Commit the enrollment
        bool is_dupl;
        if(!tudor_enroll_commit(state->dev, &is_dupl)) {
            log_error("Couldn't commit enrollment!");
            abort();
        }
        if(is_dupl) log_warn("Overwritting duplicate enrollment...");

        //Find the resulting record
        cant_fail_ret(pthread_mutex_lock(&state->dev->records_lock));

        struct tudor_record *enroll_rec = NULL;
        for(struct tudor_record *rec = state->dev->records_head; rec; rec = rec->next) {
            if(memcmp(&rec->guid, &state->action.enroll.guid, sizeof(RECGUID)) == 0 && rec->finger == state->action.enroll.finger) {
                enroll_rec = rec;
                break;
            }
        }
        if(!enroll_rec) {
            log_error("Couldn't find enrollment record!");
            abort();
        }

        //Allocate response message
        size_t rec_size = enroll_rec->data_size, resp_size = sizeof(struct ipc_msg_resp_enroll) + rec_size;

        if(rec_size > IPC_MAX_RECORD_SIZE) {
            log_error("Enrollment record size exceeding maximum size: %lu > %d!", rec_size, IPC_MAX_RECORD_SIZE);
            abort();
        }

        struct ipc_msg_resp_enroll *resp = (struct ipc_msg_resp_enroll*) malloc(resp_size);
        if(!resp) {
            perror("Couldn't allocate enrollment response buffer");
            abort();
        }
        *resp = (struct ipc_msg_resp_enroll) {
            .type = IPC_MSG_RESP_ENROLL,
            .retry = false,
            .done = true
        };
        memcpy(resp->record_data, enroll_rec->data, enroll_rec->data_size);

        cant_fail_ret(pthread_mutex_unlock(&state->dev->records_lock));

        //Send response
        ipc_send_msg(state->ipc_sock, resp, resp_size);
        free(resp);

        log_info("Enroll capture successfull -> enrollment commited for GUID %08x... finger %d - record size %lu", state->action.enroll.guid.PartA, state->action.enroll.finger, rec_size);
    } else {
        //Start another capture
        init_action(state);
        if(!tudor_enroll_capture(state->dev, &state->action.enroll.done, &state->async_res)) {
            log_error("Couldn't start enrollment capture!");
            abort();
        }

        //Send response
        struct ipc_msg_resp_enroll msg = {
            .type = IPC_MSG_RESP_ENROLL,
            .retry = !success,
            .done = false
        };
        ipc_send_msg(state->ipc_sock, &msg, sizeof(msg));

        if(success) {
            log_info("Enroll capture successfull -> need more samples");
        } else {
            log_info("Enroll capture error -> retrying...");
        }

        //Only set the callback now to avoid reentrance issues
        tudor_set_async_callback(state->async_res, (tudor_async_cb_fnc*) enroll_cb, state);
    }

    cant_fail_ret(pthread_mutex_unlock(&state->lock));
}

static void verify_cb(tudor_async_res_t *res, bool success, struct handler_state *state) {
    cant_fail_ret(pthread_mutex_lock(&state->lock));

    if(!cleanup_action(state)) {
        cant_fail_ret(pthread_mutex_unlock(&state->lock));
        return;
    }

    //Check success
    if(!success && !state->action.verify.retry) {
        log_error("Verify action failed!");
        abort();
    }

    //If we're retrying, start again
    if(!success) {
        init_action(state);
        if(!tudor_verify(state->dev, state->action.verify.guid, state->action.verify.finger, &state->action.verify.retry, &state->action.verify.matches, &state->async_res)) {
            log_error("Couldn't start verify action retry!");
            abort();
        }
    }

    //Send response
    struct ipc_msg_resp_verify msg = {
        .type = IPC_MSG_RESP_VERIFY,
        .retry = !success,
        .did_match = state->action.verify.matches
    };
    ipc_send_msg(state->ipc_sock, &msg, sizeof(msg));

    if(success) {
        log_info("Verify GUID %08x... finger %d -> %s match", state->action.verify.guid.PartA, state->action.verify.finger, msg.did_match ? "does" : "doesn't");
    } else {
        log_info("Verify GUID %08x... finger %d capture error -> retrying...", state->action.verify.guid.PartA, state->action.verify.finger);
    }

    cant_fail_ret(pthread_mutex_unlock(&state->lock));

    //Only set the callback now to avoid reentrance issues
    if(!success) tudor_set_async_callback(state->async_res, (tudor_async_cb_fnc*) verify_cb, state);
}

static void identify_cb(tudor_async_res_t *res, bool success, struct handler_state *state) {
    cant_fail_ret(pthread_mutex_lock(&state->lock));

    if(!cleanup_action(state)) {
        cant_fail_ret(pthread_mutex_unlock(&state->lock));
        return;
    }

    //Check success
    if(!success) {
        log_error("Identify action failed!");
        abort();
    }

    //If we're retrying, start again
    if(!success && !state->action.verify.retry) {
        init_action(state);
        if(!tudor_identify(state->dev, &state->action.identify.retry, &state->action.identify.has_match, &state->action.identify.guid, &state->action.identify.finger, &state->async_res)) {
            log_error("Couldn't start identify action retry!");
            abort();
        }
    }

    //Send response
    struct ipc_msg_resp_identify msg = {
        .type = IPC_MSG_RESP_IDENTIFY,
        .retry = !success,
        .did_match = state->action.identify.has_match,
        .guid = state->action.identify.guid,
        .finger = state->action.identify.finger
    };
    ipc_send_msg(state->ipc_sock, &msg, sizeof(msg));

    if(success && msg.did_match) {
        log_info("Identify result -> matches GUID %08x... finger %d", msg.guid.PartA, msg.finger);
    } else if(success) {
        log_info("Identify result -> no match");
    } else {
        log_info("Identify capture error -> retrying...");
    }

    cant_fail_ret(pthread_mutex_unlock(&state->lock));

    //Only set the callback now to avoid reentrance issues
    if(!success) tudor_set_async_callback(state->async_res, (tudor_async_cb_fnc*) identify_cb, state);
}

static inline bool handle_msg(struct handler_state *state, enum ipc_msg_type type) {
    switch(type) {
        case IPC_MSG_SHUTDOWN: {
            check_in_action(state, type);
            consume_simple_msg(state->ipc_sock, type);
        } return true;

        case IPC_MSG_CANCEL: {
            consume_simple_msg(state->ipc_sock, type);

            //Check if there currently is an action
            if(state->async_res) {
                //Cancel the action
                state->async_cancelled = true;
                tudor_cancel_async(state->async_res);
            } else {
                //Immediatly ACK the cancel
                log_warn("Received cancel message while not in an action!");
                send_ack(state->ipc_sock);
            }
        } break;

        case IPC_MSG_ADD_RECORD: {
            check_in_action(state, type);

            //Receive the message
            struct ipc_msg_add_record *msg = (struct ipc_msg_add_record*) malloc(sizeof(struct ipc_msg_add_record) + IPC_MAX_RECORD_SIZE);
            if(!msg) { perror("Couldn't allocate ADD_RECORD IPC message buffer"); abort(); }
            size_t rec_size = ipc_recv_msg(state->ipc_sock, msg, type, sizeof(struct ipc_msg_add_record), sizeof(struct ipc_msg_add_record) + IPC_MAX_RECORD_SIZE, NULL) - sizeof(struct ipc_msg_add_record);

            //Add the record
            if(!tudor_add_record(state->dev, msg->guid, msg->finger, msg->record_data, rec_size)) {
                //Delete the old one first
                log_warn("Replacing old record GUID %08x... finger %d", msg->guid.PartA, msg->finger);
                tudor_wipe_records(state->dev, &msg->guid, msg->finger);
                if(!tudor_add_record(state->dev, msg->guid, msg->finger, msg->record_data, rec_size)) {
                    log_error("Couldn't add record!");
                    abort();
                }
            }

            log_info("Added record GUID %08x... finger %d size %lu", msg->guid.PartA, msg->finger, rec_size);
            free(msg);

            //Send ACK
            send_ack(state->ipc_sock);
        } break;

        case IPC_MSG_DEL_RECORD: {
            check_in_action(state, type);

            //Receive the message
            struct ipc_msg_del_record msg;
            ipc_recv_msg(state->ipc_sock, &msg, type, sizeof(msg), sizeof(msg), NULL);

            //Delete the record
            int num_recs = tudor_wipe_records(state->dev, &msg.guid, msg.finger);
            log_info("Deleted %d records with GUID %08x... finger %d", num_recs, msg.guid.PartA, msg.finger);

            //Send ACK
            send_ack(state->ipc_sock);
        } break;

        case IPC_MSG_CLEAR_RECORDS: {
            check_in_action(state, type);
            consume_simple_msg(state->ipc_sock, type);

            //Wipe records
            int num_recs = tudor_wipe_records(state->dev, NULL, TUDOR_FINGER_ANY);
            log_info("Cleared %d records", num_recs);

            //Send ACK
            send_ack(state->ipc_sock);
        } break;

        case IPC_MSG_ENROLL: {
            check_in_action(state, type);
            
            //Receive the message
            struct ipc_msg_enroll msg;
            ipc_recv_msg(state->ipc_sock, &msg, type, sizeof(msg), sizeof(msg), NULL);

            //Initialize state
            init_action(state);
            state->action.enroll.guid = msg.guid;
            state->action.enroll.finger = msg.finger;

            //Start enrollment
            if(!tudor_enroll_start(state->dev, msg.guid, msg.finger)) {
                log_error("Couldn't start enrollment!");
                abort();
            }

            //Start first capture
            if(!tudor_enroll_capture(state->dev, &state->action.enroll.done, &state->async_res)) {
                log_error("Couldn't start enrollment capture!");
                abort();
            }
            log_debug("Started enroll action");
            tudor_set_async_callback(state->async_res, (tudor_async_cb_fnc*) enroll_cb, state);

            //Send ACK
            send_ack(state->ipc_sock);
        } break;

        case IPC_MSG_VERIFY: {
            check_in_action(state, type);

            //Receive the message
            struct ipc_msg_verify msg;
            ipc_recv_msg(state->ipc_sock, &msg, type, sizeof(msg), sizeof(msg), NULL);

            //Initialize state
            init_action(state);
            state->action.verify.guid = msg.guid;
            state->action.verify.finger = msg.finger;

            //Start verify action
            if(!tudor_verify(state->dev, msg.guid, msg.finger, &state->action.verify.retry, &state->action.verify.matches, &state->async_res)) {
                log_error("Couldn't start verify action!");
                abort();
            }
            log_debug("Started verify action");
            tudor_set_async_callback(state->async_res, (tudor_async_cb_fnc*) verify_cb, state);

            //Send ACK
            send_ack(state->ipc_sock);
        } break;

        case IPC_MSG_IDENTIFY: {
            check_in_action(state, type);
            consume_simple_msg(state->ipc_sock, type);

            //Initialize state
            init_action(state);

            //Start identify action
            if(!tudor_identify(state->dev, &state->action.identify.retry, &state->action.identify.has_match, &state->action.identify.guid, &state->action.identify.finger, &state->async_res)) {
                log_error("Couldn't start identify action!");
                abort();
            }
            log_debug("Started identify action");
            tudor_set_async_callback(state->async_res, (tudor_async_cb_fnc*) identify_cb, state);

            //Send ACK
            send_ack(state->ipc_sock);
        } break;

        default: {
            log_error("Unknown or unexpected IPC message type 0x%x!", type);
            abort();
        }
    }

    return false;
}

void run_handler_loop(struct tudor_device *dev, int sock) {
    //Create handler state
    struct handler_state state = (struct handler_state) {
        .dev = dev,
        .ipc_sock = sock,
        .async_cancelled = false,
        .async_res = NULL
    };

    pthread_mutexattr_t lock_attr;
    cant_fail_ret(pthread_mutexattr_init(&lock_attr));
    cant_fail_ret(pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_RECURSIVE));
    cant_fail_ret(pthread_mutex_init(&state.lock, &lock_attr));
    cant_fail_ret(pthread_mutexattr_destroy(&lock_attr));

    for(bool shutdown = false; !shutdown;) {
        //Peek message
        enum ipc_msg_type msg_type = ipc_peek_msg(sock);

        //Handle the message
        cant_fail_ret(pthread_mutex_lock(&state.lock));
        shutdown = handle_msg(&state, msg_type);
        cant_fail_ret(pthread_mutex_unlock(&state.lock));
    }

    cant_fail_ret(pthread_mutex_destroy(&state.lock));
}