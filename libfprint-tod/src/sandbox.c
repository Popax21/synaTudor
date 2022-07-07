#include <fcntl.h>
#include <errno.h>
#include "sandbox.h"

#define USB_PATH_PREFIX "/sys/bus/usb/devices/"

static int handle_usb_open(FpiDeviceTudor *tdev, const char *filename, int flags, GError **error) {
    //Check for path traversal bypasses
    if(strstr(filename, "..")) {
        *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Potential open bypass restriction");
        return -1;
    }

    //Get the device name
    gchar *dev_name = g_path_get_dirname(filename + strlen(USB_PATH_PREFIX));

    //Check if it's the allowed device
    //FIXME The device path doesn't contain the bus/address numbers
    GUsbDevice *usb_dev = fpi_device_get_usb_device(FP_DEVICE(tdev));
    gchar *allowed_dev = g_strdup_printf("%d-%d", (int) g_usb_device_get_bus(usb_dev), (int) g_usb_device_get_address(usb_dev));
    if(!g_str_has_prefix(dev_name, allowed_dev)) {
        g_debug("Tudor host process ID %d attempted to open prohibited USB device '%s' path '%s'", tdev->host_id, dev_name, filename);
        g_free(dev_name);
        return -EPERM;
    }
    g_free(dev_name);

    //Proxy the allowed open
    int fd = open(filename, flags);
    int err = errno;
    g_debug("Proxied Tudor host process ID %d USB file open '%s' -> FD %d errno %d [%s]", tdev->host_id, filename, fd, err, strerror(err));
    if(fd < 0) return -errno;
    return fd;
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

    if(fd < 0 && *error) g_warning("Tudor host process ID %d couldn't to open file '%s': %s (%s code %d)", tdev->host_id, filename, (*error)->message, g_quark_to_string((*error)->domain), (*error)->code);
    return fd;
}