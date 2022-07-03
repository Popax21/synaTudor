#ifndef LIBFPRINT_HOST_USB_H
#define LIBFPRINT_HOST_USB_H

#include <stdint.h>
#include <libusb.h>

void start_usb_thread();
void notify_usb_shutdown();

void init_libusb();
void exit_libusb();

struct usb_dev_params {
    uint8_t bus, addr;
};

libusb_device_handle *open_usb_dev(struct usb_dev_params *params);

#endif