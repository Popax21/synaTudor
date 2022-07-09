#ifndef TUDOR_HOST_SANDBOX_H
#define TUDOR_HOST_SANDBOX_H

#define SANDBOX_UID 3333
#define SANDBOX_GID 3333

#define SANDBOX_DATA_LIMIT (32 * 1024 * 1024)
#define SANDBOX_STACK_LIMIT (64 * 1024)
#define SANDBOX_MAX_FDS 16
#define SANDBOX_MAX_THREADS 4

void activate_sandbox(int sock);

#endif