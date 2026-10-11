#include "internal.h"

static void mem_destr(struct wdf_memory *mem) {
    //Free memory
    wdf_cleanup_obj(&mem->object);
    if(mem->owns_data) free(mem->data);
    free(mem);
}
struct wdf_memory *wdf_create_memory(WDF_DRIVER_GLOBALS *globals, WDF_OBJECT_ATTRIBUTES *obj_attrs, void *data, size_t data_size, bool owns_data) {
    struct wdf_memory *mem = (struct wdf_memory*) malloc(sizeof(struct wdf_memory));
    if(!mem) return NULL;

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &mem->object, (wdf_obj_destr_fnc*) mem_destr, obj_attrs);

    mem->data = data;
    mem->data_size = data_size;
    mem->owns_data = owns_data;
    return mem;
}

__winfnc NTSTATUS WdfMemoryCreatePreallocated(WDF_DRIVER_GLOBALS *globals, WDF_OBJECT_ATTRIBUTES *obj_attrs, void *buffer, size_t buffer_size, WDFOBJECT *out) {
    //Create the memory object
    struct wdf_memory *mem = wdf_create_memory(globals, obj_attrs, buffer, buffer_size, false);
    if(!mem) return winerr_from_errno();

    *out = &mem->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfMemoryCreatePreallocated, 118)