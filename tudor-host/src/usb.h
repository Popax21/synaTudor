#ifndef TUDOR_HOST_USB_H
#define TUDOR_HOST_USB_H

#include <stdint.h>
#include <libusb.h>

void init_libusb();
void exit_libusb();
void notify_usb_shutdown();

struct usb_dev_params {
    uint8_t bus, addr;
};

libusb_device_handle *open_usb_dev(struct usb_dev_params *params);

#endif