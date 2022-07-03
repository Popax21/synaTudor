#include <sys/socket.h>
#include <tudor/log.h>
#include "ipc.h"

static bool ipc_shutdown = false;

size_t ipc_recv_msg(int sock, void *buf, enum ipc_msg_type type, size_t min_sz, size_t max_sz) {
    struct iovec iov = {
        .iov_base = buf,
        .iov_len = max_sz
    };

    struct msghdr msg_hdr = {
        .msg_name = NULL,
        .msg_namelen = 0,
        .msg_iov = &iov,
        .msg_iovlen = 1,
        .msg_control = NULL,
        .msg_controllen = 0
    };

    cant_fail(recvmsg(sock, &msg_hdr, 0));

    if(iov.iov_len < min_sz || max_sz < iov.iov_len) {
        log_error("Invalid IPC message size: 0x%lx (min 0x%lx, max 0x%lx)", iov.iov_len, min_sz, max_sz);
        return EXIT_FAILURE;
    }

    if(*((enum ipc_msg_type*) buf) != type) {
        log_error("Unexpected IPC message type: 0x%x (expected 0x%x)", *((enum ipc_msg_type*) buf), type);
        return EXIT_FAILURE;
    }

    return iov.iov_len;
}

void ipc_send_msg(int sock, void *buf, size_t size) {
    struct iovec iov = {
        .iov_base = buf,
        .iov_len = size
    };

    struct msghdr msg_hdr = {
        .msg_name = NULL,
        .msg_namelen = 0,
        .msg_iov = &iov,
        .msg_iovlen = 1,
        .msg_control = NULL,
        .msg_controllen = 0
    };

    cant_fail(sendmsg(sock, &msg_hdr, 0));
}

void ipc_handle_msg(int sock, enum ipc_msg_type type) {
    switch(type) {
        case IPC_MSG_SHUTDOWN: {
            ipc_recv_msg(sock, &type, type, sizeof(type), sizeof(type));
            ipc_shutdown = true;
        } break;
        default: {
            log_error("Unknown or unexpected IPC message type 0x%x!", type);
            abort();
        }
    }
}

void ipc_run_handler_loop(int sock) {
    while(!ipc_shutdown) {
        //Peek message
        enum ipc_msg_type msg_type;

        struct iovec iov = {
            .iov_base = &msg_type,
            .iov_len = sizeof(msg_type)
        };

        struct msghdr msg_hdr = {
            .msg_name = NULL,
            .msg_namelen = 0,
            .msg_iov = &iov,
            .msg_iovlen = 1,
            .msg_control = NULL,
            .msg_controllen = 0
        };

        cant_fail(recvmsg(sock, &msg_hdr, MSG_PEEK));

        //Handle the message
        ipc_handle_msg(sock, msg_type);
    }
}