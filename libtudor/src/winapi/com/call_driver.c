/*
 * Isolated ms_abi trampoline for calling driver functions.
 * ALL ms_abi function pointer code lives here to avoid .data layout
 * shifts in other compilation units.
 */
#include <stdint.h>
#include <tudor/log.h>

typedef unsigned int HRESULT;
typedef HRESULT __attribute__((ms_abi)) (*driver_fn_t)(void *self);

/* These are set by other modules */
extern void *com_usb_device_obj;

/* Image base — set by tudor_set_driver_image_base() */
static void *driver_image_base = 0;

void tudor_set_driver_image_base(void *base) {
    driver_image_base = base;
}

void tudor_call_wbf_usb_init(void) {
    if(!com_usb_device_obj || !driver_image_base) {
        log_warn("WBF trampoline: missing obj=%p base=%p", com_usb_device_obj, driver_image_base);
        return;
    }

    uint8_t *base = (uint8_t*)driver_image_base;
    driver_fn_t wbf = (driver_fn_t)(base + 0x16160);

    log_info("WBFUsbInitialize via trampoline...");
    HRESULT hr = wbf(com_usb_device_obj);
    log_info("WBFUsbInitialize returned 0x%x", hr);
}
