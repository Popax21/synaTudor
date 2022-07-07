#include "sandbox.h"

#define USB_PATH_PREFIX "/sys/bus/usb/devices/"

static int handle_usb_open(FpiDeviceTudor *tdev, const char *filename, int flags, GError **error) {
    //Get the device name
    gchar *dev_name = g_path_get_dirname(filename + strlen(USB_PATH_PREFIX));

    //Check if it's the allowed device
    GUsbDevice *usb_dev = fpi_device_get_usb_device(FP_DEVICE(tdev));
    gchar *allowed_dev = g_strdup_printf("%d-%d", (int) g_usb_device_get_bus(usb_dev), (int) g_usb_device_get_address(usb_dev));
    if(!g_str_has_prefix(dev_name, allowed_dev)) {
        *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Attempted open of prohibited USB device '%s'", dev_name);
        g_free(dev_name);
        return -1;
    }

    g_free(dev_name);
    return 0;
}

int sandbox_open_file(FpiDeviceTudor *tdev, const char *filename, int flags, GError **error) {
    int fd;

    //Check for the USB prefix
    if(g_str_has_prefix(filename, USB_PATH_PREFIX)) {
        fd = handle_usb_open(tdev, filename, flags, error);
    } else {
        *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Attempted open of prohibited file '%s'", filename);
        fd = -1;
    }

    if(fd < 0) g_warning("Tudor host process ID %d couldn't to open file '%s': %s (%s code %d)", tdev->host_id, filename, (*error)->message, g_quark_to_string((*error)->domain), (*error)->code);
    return fd;
}