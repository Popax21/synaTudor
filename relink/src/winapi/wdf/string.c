#include "internal.h"

struct wdf_string {
    struct wdf_object object;
    char *buffer;
};

static void str_destr(struct wdf_string *str) {
    wdf_cleanup_obj(&str->object);
    free(str->buffer);
    free(str);
}

void wdf_set_string(struct wdf_string *str, const char *val) {
    free(str->buffer);
    str->buffer = strdup(val);
    if(!str->buffer) { perror("Error allocating WDF string buffer"); abort(); }
}

__winfnc NTSTATUS WdfStringCreate(WDF_DRIVER_GLOBALS *globals, const UNICODE_STRING *ustr, WDF_OBJECT_ATTRIBUTES *obj_attrs, WDFOBJECT *out) {
    //Allocate the string object
    struct wdf_string *str = (struct wdf_string*) malloc(sizeof(struct wdf_string));
    if(!str) return winerr_from_errno();

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &str->object, (wdf_obj_destr_fnc*) str_destr, obj_attrs);

    if(ustr) str->buffer = winstr_to_str(ustr->Buffer);
    else str->buffer = NULL;

    *out = &str->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfStringCreate, 188)

__winfnc void WdfStringGetUnicodeString(WDF_DRIVER_GLOBALS *globals, WDFOBJECT str_obj, UNICODE_STRING *ustr) {
    struct wdf_string* str = (struct wdf_string*) str_obj;

    //This leaks memory, but there's no avoiding that
    char16_t *wstr = winstr_from_str(str->buffer ? str->buffer : "");
    ustr->Length = ustr->MaximumLength = winstr_len(wstr)+1;
    ustr->Buffer = wstr;
}
WDFFUNC(WdfStringGetUnicodeString, 189)