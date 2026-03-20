/*
 * LD_PRELOAD: WBFUsbInitialize trigger with full device pointer setup.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

typedef unsigned int HRESULT;
typedef HRESULT __attribute__((ms_abi)) (*driver_fn_t)(void *self);

static void **p_usb_obj;
static void **p_driver_dll;
static void (*set_cur)(void*);

/* Fake COM object for [self+0x18] — vtable[15] stores USB target at output ptr */
static void *cached_usb_target = NULL;

static HRESULT __attribute__((ms_abi)) fake_qi(void *self, void *riid, void **ppv) {
    *ppv = self;
    return 0;
}
static unsigned long __attribute__((ms_abi)) fake_addref(void *self) { return 1; }
static unsigned long __attribute__((ms_abi)) fake_release(void *self) { return 1; }

/* vtable[15] = CreateUsbTargetDevice(self, unused, &output) */
static HRESULT __attribute__((ms_abi)) fake_create_usb_target(void *self, int unused, void **output) {
    if(output && cached_usb_target) {
        *output = cached_usb_target;
        write(2, "[WBF] fake_create_usb_target: stored target\n", 44);
        return 0; /* S_OK */
    }
    return 0x80004005; /* E_FAIL */
}

/* Stub for all other slots */
static HRESULT __attribute__((ms_abi)) fake_stub() { return 0; }

static void *fake_vtbl[20];
static struct { void **vtbl; } fake_com_obj;

static void init_fake_obj(void) {
    for(int i = 0; i < 20; i++) fake_vtbl[i] = (void*)fake_stub;
    fake_vtbl[0] = (void*)fake_qi;
    fake_vtbl[1] = (void*)fake_addref;
    fake_vtbl[2] = (void*)fake_release;
    fake_vtbl[15] = (void*)fake_create_usb_target; /* The critical slot */
    fake_com_obj.vtbl = fake_vtbl;
}

static void wbf_sigusr(int sig) {
    if(!p_usb_obj || !*p_usb_obj || !p_driver_dll || !*p_driver_dll) return;

    void *base = *(void**)((uint8_t*)*p_driver_dll + 64);
    if(!base) return;
    uint8_t *img = (uint8_t*)base;
    if(img[0x929b] != 0xEB || img[0x161bb] != 0x41) return;

    /* Fix device pointer at self-0x498 (for AssignS0IdleSettings) */
    void **dev_at_58 = (void**)((uint8_t*)*p_usb_obj + 0x58);
    void **dev_at_neg498 = (void**)((uint8_t*)*p_usb_obj - 0x498);
    if(*dev_at_58) *dev_at_neg498 = *dev_at_58;

    /* Set USB target for fake_create_usb_target to return */
    void **self508 = (void**)((uint8_t*)*p_usb_obj + 0x508);
    cached_usb_target = *self508;

    /* Set [self+0x18] to our fake COM object with vtable[15] = create_usb_target */
    init_fake_obj();
    void **self18 = (void**)((uint8_t*)*p_usb_obj + 0x18);
    *self18 = &fake_com_obj;

    /* Zero field_0x80 for WBFUsbInitialize check */
    void **self80 = (void**)((uint8_t*)*p_usb_obj + 0x80);
    *self80 = NULL;

    if(set_cur) set_cur(*p_driver_dll);

    char buf[120];
    int n = snprintf(buf, sizeof(buf), "[WBF] Setup: dev=%p usb_target=%p fake_obj=%p\n",
        *dev_at_58, cached_usb_target, &fake_com_obj);
    write(2, buf, n);

    driver_fn_t wbf = (driver_fn_t)(img + 0x16160);
    write(2, "[WBF] >>> WBFUsbInitialize <<<\n", 30);
    HRESULT hr = wbf(*p_usb_obj);
    n = snprintf(buf, sizeof(buf), "[WBF] WBFUsbInit returned 0x%x\n", hr);
    write(2, buf, n);

    /* Set sensor name at base+0x90 (used by InitializeNiseCore → palUsbDriverOpen) */
    void *base_ptr = (uint8_t*)*p_usb_obj - 0x08;
    char **sensor_name = (char**)((uint8_t*)base_ptr + 0x90);
    if(!*sensor_name) {
        static char name[] = "ff82a8343717"; /* Sensor serial from lsusb */
        *sensor_name = name;
        write(2, "[WBF] Set sensor name at base+0x90\n", 34);
    }

    /* Call PrepareHardware → InitializeNiseCore */
    driver_fn_t prep_hw = (driver_fn_t)(img + 0x90f4);
    write(2, "[WBF] >>> PrepareHardware <<<\n", 29);
    hr = prep_hw(base_ptr);
    n = snprintf(buf, sizeof(buf), "[WBF] PrepareHardware returned 0x%x\n", hr);
    write(2, buf, n);
}

static void *timer_thread(void *arg) {
    for(int i = 0; i < 30; i++) {
        usleep(500000);
        if(p_usb_obj && *p_usb_obj && p_driver_dll && *p_driver_dll) {
            void *base = *(void**)((uint8_t*)*p_driver_dll + 64);
            if(base) {
                uint8_t *img = (uint8_t*)base;
                if(img[0x929b] == 0xEB && img[0x161bb] == 0x41) {
                    kill(getpid(), SIGUSR1);
                    return NULL;
                }
            }
        }
    }
    return NULL;
}

__attribute__((constructor(65535)))
static void setup(void) {
    p_usb_obj = (void**)dlsym(RTLD_DEFAULT, "com_usb_device_obj");
    p_driver_dll = (void**)dlsym(RTLD_DEFAULT, "tudor_driver_dll");
    set_cur = dlsym(RTLD_DEFAULT, "winmodule_set_cur");

    struct sigaction sa = {0};
    sa.sa_handler = wbf_sigusr;
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    pthread_t t;
    pthread_create(&t, NULL, timer_thread, NULL);
    pthread_detach(t);
}
