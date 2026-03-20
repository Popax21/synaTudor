/*
 * LD_PRELOAD shim: calls WBFUsbInitialize after binary patches are applied.
 * Usage: LD_PRELOAD=./wbf_preload.so tudor_cli ...
 *
 * This runs OUTSIDE libtudor.so so it doesn't affect .data layout.
 * It polls for the patches to be applied, then calls WBFUsbInitialize
 * via the function address computed from the DLL image base.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

typedef unsigned int HRESULT;
typedef HRESULT __attribute__((ms_abi)) (*driver_fn_t)(void *self);

static void *wbf_poll(void *arg) {
    usleep(2000000); /* Wait 2s for tudor_init to complete */

    /* Find symbols in libtudor.so */
    void **p_usb_obj = (void**)dlsym(RTLD_DEFAULT, "com_usb_device_obj");
    void **p_driver_dll = (void**)dlsym(RTLD_DEFAULT, "tudor_driver_dll");
    void (*init_tib)(void) = dlsym(RTLD_DEFAULT, "win_init_tib");
    void (*set_cur)(void*) = dlsym(RTLD_DEFAULT, "winmodule_set_cur");

    if(!p_usb_obj || !p_driver_dll || !*p_usb_obj || !*p_driver_dll) {
        fprintf(stderr, "[WBF-PRELOAD] Driver not initialized\n");
        return NULL;
    }

    /* Get image base from windrv_dll struct (offset 64) */
    void *base = *(void**)((uint8_t*)*p_driver_dll + 64);
    if(!base) return NULL;

    uint8_t *img = (uint8_t*)base;
    if(img[0x929b] != 0xEB || img[0x161bf] != 0xEB) {
        fprintf(stderr, "[WBF-PRELOAD] Patches not active\n");
        return NULL;
    }

    /* Initialize Windows thread state */
    if(init_tib) init_tib();
    if(set_cur) set_cur(*p_driver_dll);

    /* Call WBFUsbInitialize */
    driver_fn_t wbf = (driver_fn_t)(img + 0x16160);
    fprintf(stderr, "[WBF-PRELOAD] Calling WBFUsbInitialize...\n");
    HRESULT hr = wbf(*p_usb_obj);
    fprintf(stderr, "[WBF-PRELOAD] WBFUsbInitialize returned 0x%x\n", hr);

    return NULL;
}

__attribute__((constructor))
static void start_poll(void) {
    pthread_t t;
    pthread_create(&t, NULL, wbf_poll, NULL);
    pthread_detach(t);
}
