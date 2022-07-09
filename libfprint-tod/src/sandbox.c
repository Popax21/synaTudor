#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include "sandbox.h"

#define USB_BUS_ROOT_DIR "/dev/bus/usb"

static const struct ipc_sbox_dir_ent root_usb_dir_ents[] = {
    (struct ipc_sbox_dir_ent) { .name = "001", .type = DT_DIR }
};

static bool check_traversal_bypass(const char *path, GError **error) {
    //Check for path traversal bypasses
    if(strstr(path, "..")) {
        *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Potential open bypass restriction");
        return true;
    }
    return false;
}

int sandbox_open_file(FpiDeviceTudor *tdev, const char *path, int flags, GError **error) {
    if(check_traversal_bypass(path, error)) return -1;
    int fd;

    //Handle the file open request
    *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Attempted open of prohibited file '%s'", path);
    fd = -1;

    if(*error) g_warning("Tudor host process ID %d couldn't open file '%s': %s (%s code %d)", tdev->host_id, path, (*error)->message, g_quark_to_string((*error)->domain), (*error)->code);
    return fd;
}


int sandbox_ls_dir(FpiDeviceTudor *tdev, const char *raw_path, struct ipc_sbox_dir_ent **entries, GError **error) {
    if(check_traversal_bypass(raw_path, error)) return -1;
    int num_entries = -1;

    //Remove trailing slashes
    char *path = g_strdup(raw_path);
    for(char *p = path + strlen(path); p > path && *p == '/'; p--) *p = 0;

    //Handle the directory list request
    if(strcmp(path, USB_BUS_ROOT_DIR) == 0) {
        *entries = g_memdup2(&root_usb_dir_ents, sizeof(root_usb_dir_ents));
        num_entries = sizeof(root_usb_dir_ents) / sizeof(struct ipc_sbox_dir_ent);
    } else {
        *error = fpi_device_error_new_msg(FP_DEVICE_ERROR_PROTO, "Attempted listing of prohibited directory '%s'", path);
        num_entries = -1;
    }

    g_free(path);

    if(*error) g_warning("Tudor host process ID %d couldn't list directory '%s': %s (%s code %d)", tdev->host_id, path, (*error)->message, g_quark_to_string((*error)->domain), (*error)->code);
    return num_entries;
}