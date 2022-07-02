#include "internal.h"

static void mem_destr(struct wdf_memory *mem) {
    //Free memory
    wdf_cleanup_obj(&mem->object);
    if(mem->owns_data) free(mem->data);
    free(mem);
}
__winfnc NTSTATUS WdfMemoryCreatePreallocated(WDF_DRIVER_GLOBALS *globals, WDF_OBJECT_ATTRIBUTES *obj_attrs, void *buffer, size_t buffer_size, WDFOBJECT *out) {
    //Create the memory object
    struct wdf_memory *mem = (struct wdf_memory*) malloc(sizeof(struct wdf_memory));
    if(!mem) return winerr_from_errno();

    wdf_create_obj((struct wdf_object*) winwdf_get_driver(globals), &mem->object, (wdf_obj_destr_fnc*) mem_destr, obj_attrs);

    mem->data = buffer;
    mem->data_size = buffer_size;
    mem->owns_data = false;

    *out = &mem->object;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfMemoryCreatePreallocated, 118)