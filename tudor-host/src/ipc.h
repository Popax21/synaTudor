#ifndef TUDOR_HOST_IPC_H
#define TUDOR_HOST_IPC_H

#include <tudor/libfprint-proto.h>

size_t ipc_recv_msg(int sock, void *buf, enum ipc_msg_type type, size_t min_sz, size_t max_sz);
void ipc_send_msg(int sock, void *buf, size_t size);
void ipc_handle_msg(int sock, enum ipc_msg_type type);

void ipc_run_handler_loop(int sock);

#endif