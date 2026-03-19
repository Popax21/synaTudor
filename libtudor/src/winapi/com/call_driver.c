/*
 * Isolated ms_abi trampoline for calling driver functions.
 */
#include <stdint.h>
#include <tudor/log.h>

typedef unsigned int HRESULT;
typedef HRESULT __attribute__((ms_abi)) (*driver_fn_t)(void *self);

extern void *com_usb_device_obj;
struct windrv_dll;
extern struct windrv_dll *tudor_driver_dll;
struct winmodule;
extern void winmodule_set_cur(struct winmodule *);
extern void win_init_tib(void);

void tudor_call_wbf_usb_init(void) {
    if(!com_usb_device_obj || !tudor_driver_dll) return;

    /* Get image base: winmodule(48) + pe_image(8) + pe_image_end(8) = offset 64 */
    void *base_addr = *(void**)((uint8_t*)tudor_driver_dll + 64);
    if(!base_addr) return;

    uint8_t *img = (uint8_t*)base_addr;
    if(img[0x929b] != 0xEB || img[0x161bf] != 0xEB) {
        log_debug("[WBF] Patches not active — skipping");
        return;
    }

    driver_fn_t wbf_init = (driver_fn_t)(img + 0x16160);
    winmodule_set_cur((struct winmodule*)tudor_driver_dll);

    log_info("[WBF] Calling WBFUsbInitialize (main thread, patches active)...");
    HRESULT hr = wbf_init(com_usb_device_obj);
    log_info("[WBF] WBFUsbInitialize returned 0x%x", hr);
}
