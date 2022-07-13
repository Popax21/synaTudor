#ifndef TUDOR_HOST_SANDBOX_H
#define TUDOR_HOST_SANDBOX_H

#include <stdint.h>

#define SANDBOX_UID 3333
#define SANDBOX_GID 3333

#define SANDBOX_DATA_LIMIT (128 * 1024 * 1024)
#define SANDBOX_STACK_LIMIT (64 * 1024)
#define SANDBOX_MAX_FDS 16
#define SANDBOX_MAX_THREADS 12

void activate_sandbox();
void setup_usb_sbox(int usb_fd, uint8_t usb_bus, uint8_t usb_addr);

#endif