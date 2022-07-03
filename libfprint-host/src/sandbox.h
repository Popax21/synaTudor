#ifndef LIBFPRINT_HOST_SANDBOX_H
#define LIBFPRINT_HOST_SANDBOX_H

#define SANDBOX_UID 3333
#define SANDBOX_GID 3333

void activate_sandbox(int *seccomp_notif_fd);

#endif