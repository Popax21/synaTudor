#include "internal.h"

#define PLUGPLAY_REGKEY_DEVICE 1
#define PLUGPLAY_REGKEY_DRIVER 2
#define WDF_REGKEY_DEVICE_SUBKEY 256
#define WDF_REGKEY_DRIVER_SUBKEY 256

struct wdf_reg_key {
    struct wdf_object object;
    HANDLE hkey;
};

static void key_destr(struct wdf_reg_key *key) {
    wdf_cleanup_obj(&key->object);
    free(key);
}

__winfnc NTSTATUS WdfRegistryOpenKey(WDF_DRIVER_GLOBALS *globals, WDFOBJECT parent, UNICODE_STRING *key_name, DWORD access, WDF_OBJECT_ATTRIBUTES *obj_attrs, WDFOBJECT *out) {
    //Get key name
    const char *par_name = parent ? winreg_get_key_name(((struct wdf_reg_key*) parent)->hkey) : "";
    char *subkey_name = winstr_to_str(key_name->Buffer);

    int par_len = strlen(par_name), subkey_len = strlen(subkey_name);
    char *name = (char*) malloc(par_len + 1 + subkey_len + 1);
    strcpy(name, par_name);
    name[par_len] = '\\';
    strcpy(name + par_len + 1, subkey_name);

    free(subkey_name);

    //Create the key object
    struct wdf_reg_key *key = (struct wdf_reg_key*) malloc(sizeof(struct wdf_reg_key));
    if(!key) return winerr_from_errno();

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &key->object, (wdf_obj_destr_fnc*) key_destr, obj_attrs);
    key->hkey = winreg_open_key(winwdf_get_driver(globals), name);
    free(name);

    *out = &key->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfRegistryOpenKey, 131)

__winfnc NTSTATUS WdfDeviceOpenRegistryKey(WDF_DRIVER_GLOBALS *globals, WDFOBJECT device, ULONG key_type, DWORD access, WDF_OBJECT_ATTRIBUTES *obj_attrs, WDFOBJECT *out) {
    //Get the key handle
    HANDLE hkey;
    switch(key_type) {
        case (PLUGPLAY_REGKEY_DRIVER | WDF_REGKEY_DRIVER_SUBKEY):
        case PLUGPLAY_REGKEY_DRIVER: hkey = wdf_get_driver_reg_key(winwdf_get_driver(globals)); break;
        case (PLUGPLAY_REGKEY_DEVICE | WDF_REGKEY_DEVICE_SUBKEY):
        case PLUGPLAY_REGKEY_DEVICE: hkey = wdf_get_device_reg_key((struct winwdf_device*) device); break;
        default: return WINERR_SET_CODE;
    }

    //Create the key object
    struct wdf_reg_key *key = (struct wdf_reg_key*) malloc(sizeof(struct wdf_reg_key));
    if(!key) return winerr_from_errno();

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &key->object, (wdf_obj_destr_fnc*) key_destr, obj_attrs);
    key->hkey = hkey;

    *out = &key->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfDeviceOpenRegistryKey, 18)

__winfnc NTSTATUS WdfRegistryClose(WDF_DRIVER_GLOBALS *globals, WDFOBJECT key) {
    winwdf_destroy_object(key);
    return STATUS_SUCCESS;
}
WDFFUNC(WdfRegistryClose, 133)

__winfnc NTSTATUS WdfRegistryQueryValue(WDF_DRIVER_GLOBALS *globals, WDFOBJECT key_obj, const UNICODE_STRING *val_name, ULONG val_len, void *value, ULONG *val_len_out, ULONG *val_type) {
    struct wdf_reg_key *key = (struct wdf_reg_key*) key_obj;

    char *val_cname = winstr_to_str(val_name->Buffer);

    size_t buf_size = val_len;
    if(!winreg_query_val(key->hkey, val_cname, value, &buf_size, (enum winreg_val_type*) val_type)) {
        free(val_cname);
        return WINERR_SET_CODE;
    }
    *val_len_out = (ULONG) buf_size;

    free(val_cname);
    return STATUS_SUCCESS;
}
WDFFUNC(WdfRegistryQueryValue, 136)

__winfnc NTSTATUS WdfRegistryAssignValue(WDF_DRIVER_GLOBALS *globals, WDFOBJECT key_obj, UNICODE_STRING *val_name, ULONG val_type, ULONG val_len, void *val) {
    struct wdf_reg_key *key = (struct wdf_reg_key*) key_obj;

    //Write value
    char *val_cname = winstr_to_str(val_name->Buffer);
    if(!winreg_write_val(key->hkey, val_cname, val, val_len, (enum winreg_val_type) val_type)) {
        free(val_cname);
        return WINERR_SET_CODE;
    }
    free(val_cname);

    return STATUS_SUCCESS;
}
WDFFUNC(WdfRegistryAssignValue, 142)

__winfnc NTSTATUS WdfRegistryAssignULong(WDF_DRIVER_GLOBALS *globals, WDFOBJECT key_obj, UNICODE_STRING *val_name, ULONG val) {
    struct wdf_reg_key *key = (struct wdf_reg_key*) key_obj;

    //Write value
    char *val_cname = winstr_to_str(val_name->Buffer);
    if(!winreg_write_val(key->hkey, val_cname, &val, sizeof(val), WINREG_DWORD)) {
        free(val_cname);
        return WINERR_SET_CODE;
    }
    free(val_cname);

    return STATUS_SUCCESS;
}
WDFFUNC(WdfRegistryAssignULong, 147)