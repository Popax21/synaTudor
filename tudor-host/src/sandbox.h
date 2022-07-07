#ifndef TUDOR_HOST_SANDBOX_H
#define TUDOR_HOST_SANDBOX_H

#define SANDBOX_UID 3333
#define SANDBOX_GID 3333

extern int sandbox_ipc_sock;

void activate_sandbox();

#endif