#ifndef TUDOR_HOST_IPC_H
#define TUDOR_HOST_IPC_H

#include <tudor/libfprint-proto.h>

enum ipc_msg_type ipc_peek_msg(int sock);
size_t ipc_recv_msg(int sock, void *buf, enum ipc_msg_type type, size_t min_sz, size_t max_sz, int *fd);
void ipc_send_msg(int sock, void *buf, size_t size);

#endif