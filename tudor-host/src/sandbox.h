#ifndef TUDOR_HOST_SANDBOX_H
#define TUDOR_HOST_SANDBOX_H

#include <sys/utsname.h>

#define SANDBOX_UID 3333
#define SANDBOX_GID 3333

#define SANDBOX_DATA_LIMIT (32 * 1024 * 1024)
#define SANDBOX_STACK_LIMIT (64 * 1024)
#define SANDBOX_MAX_FDS 16

extern int sbox_ipc_sock;
extern struct utsname sbox_utsname;

void activate_sandbox(int sock);

#endif