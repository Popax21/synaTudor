#ifndef LIBFPRINT_HOST_SANDBOX_H
#define LIBFPRINT_HOST_SANDBOX_H

void activate_sandbox(int *sockfd, int *seccomp_notif_fd);

#endif