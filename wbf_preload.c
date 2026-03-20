/*
 * LD_PRELOAD: WBFUsbInitialize via timer thread that sends SIGUSR1 to main.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

typedef unsigned int HRESULT;
typedef HRESULT __attribute__((ms_abi)) (*driver_fn_t)(void *self);

static void **p_usb_obj;
static void **p_driver_dll;
static void (*set_cur)(void*);
static pid_t main_tid;

static void wbf_sigusr(int sig) {
    if(!p_usb_obj || !*p_usb_obj || !p_driver_dll || !*p_driver_dll) return;

    void *base = *(void**)((uint8_t*)*p_driver_dll + 64);
    if(!base) return;
    uint8_t *img = (uint8_t*)base;
    if(img[0x929b] != 0xEB || img[0x161bb] != 0x41) return;

    if(set_cur) set_cur(*p_driver_dll);

    /* Fix uninitialized COM object at [self+0x18].
       The init code at RVA 0x162E8 reads this and calls vtable[15].
       It should be a device interface file handle. Use com_usb_device_obj
       as a proxy — not perfect but might prevent the NULL vtable crash. */
    void **self18 = (void**)((uint8_t*)*p_usb_obj + 0x18);
    if(!*self18) {
        /* Try to find g_wdf_device — it's static, not exported.
           Instead, use the usb_target_factory or device QI result. */
        write(2, "[WBF] [self+0x18] is NULL — skipping\n", 37);
        return;
    }
    char dbg[80];
    int dlen = snprintf(dbg, sizeof(dbg), "[WBF] [self+0x18] = %p\n", *self18);
    write(2, dbg, dlen);

    driver_fn_t wbf = (driver_fn_t)(img + 0x16160);
    write(2, "[WBF] >>> WBFUsbInitialize <<<\n", 30);
    HRESULT hr = wbf(*p_usb_obj);
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "[WBF] returned 0x%x\n", hr);
    write(2, buf, n);
}

static void *timer_thread(void *arg) {
    for(int i = 0; i < 30; i++) { /* 15 seconds max */
        usleep(500000);
        if(p_usb_obj && *p_usb_obj && p_driver_dll && *p_driver_dll) {
            void *base = *(void**)((uint8_t*)*p_driver_dll + 64);
            if(base) {
                uint8_t *img = (uint8_t*)base;
                if(img[0x929b] == 0xEB && img[0x161bb] == 0x41) {
                    /* Send signal to main thread */
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
