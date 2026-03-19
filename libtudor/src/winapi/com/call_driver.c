/*
 * Isolated trampoline for calling driver functions with ms_abi convention.
 */

#include <stdint.h>
#include <tudor/log.h>
#include "winapi/api.h"
#include "tudor/internal.h"
#include "winapi/com/com.h"

typedef unsigned int HRESULT;
typedef HRESULT __attribute__((ms_abi)) (*driver_fn_t)(void *self);

volatile int tudor_wbf_trigger = 0;

void tudor_check_wbf_trigger(void) {
    if(!tudor_wbf_trigger) return;
    tudor_wbf_trigger = 0;

    extern struct windrv_dll *tudor_driver_dll;
    if(!com_usb_device_obj || !tudor_driver_dll) return;

    uint8_t *img = tudor_driver_dll->image.base_addr;
    if(!img) return;

    driver_fn_t wbf_init = (driver_fn_t)(img + 0x16160);
    winmodule_set_cur(&tudor_driver_dll->module);
    log_info("WBFUsbInitialize via trampoline...");
    HRESULT hr = wbf_init(com_usb_device_obj);
    log_info("WBFUsbInitialize returned 0x%x", hr);
}
