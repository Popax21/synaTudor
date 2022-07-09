#include <tudor/log.h>
#include "sandbox.h"
#include "ipc.h"

#include "noredirect.h"
#include <fcntl.h>
#undef open

int open(const char *path, int flags, ...) {
    //We don't support O_PATH
    if(flags & O_PATH) {
        errno = EINVAL;
        return -1;
    }

    log_debug("Hooked open call for file '%s' flags %d", path, flags);

    //Send the host module a message
    struct ipc_msg_sbox_open open_msg = {
        .type = IPC_MSG_SBOX_OPEN,
        .flags = flags
    };
    strncpy(open_msg.file_path, path, IPC_SBOX_PATH_SIZE);
    ipc_send_msg(sbox_ipc_sock, &open_msg, sizeof(open_msg));

    //Receive the response
    struct ipc_msg_resp_sbox_open resp_msg;
    int open_fd;
    ipc_recv_msg(sbox_ipc_sock, &resp_msg, IPC_MSG_RESP_SBOX_OPEN, sizeof(resp_msg), sizeof(resp_msg), &open_fd);

    errno = resp_msg.error;
    return open_fd;
}

int open64(const char *path, int flags, ...) { return open(path, flags); }