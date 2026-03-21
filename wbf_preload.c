/*
 * LD_PRELOAD: WBFUsbInitialize + direct VFM/PAL initialization.
 *
 * After the COM bootstrap and binary patches complete, this preload:
 * 1. Calls WBFUsbInitialize (sets up WinUSB handles)
 * 2. Calls the VFM/PAL init functions DIRECTLY, bypassing PrepareHardware
 *    (which crashes due to incomplete COM vtable setup)
 * 3. Uses transport type 3 (direct WinUSB) instead of type 2 (WDF external)
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stddef.h>

typedef unsigned int HRESULT;
typedef unsigned int uint32_t_w;
typedef HRESULT __attribute__((ms_abi)) (*driver_fn_t)(void *self);

/* ms_abi function pointer types matching actual DLL function signatures */
typedef int __attribute__((ms_abi)) (*fn_set_callback_t)(void *callback);
typedef int __attribute__((ms_abi)) (*fn_session_init_t)(void **out_session);
typedef uint32_t_w __attribute__((ms_abi)) (*fn_device_open_t)(void *session, void *dev_id, void *config);
typedef int __attribute__((ms_abi)) (*fn_get_dev_handle_t)(void *session, void **out_handle);
typedef int __attribute__((ms_abi)) (*fn_stg_init_t)(void *dev_handle);

static void **p_usb_obj;
static void **p_driver_dll;
static void (*set_cur)(void*);

/* ─── Fake COM object for WBFUsbInitialize's vtable[15] ─── */

static void *cached_usb_target = NULL;

static HRESULT __attribute__((ms_abi)) fake_qi(void *self, void *riid, void **ppv) {
    *ppv = self;
    return 0;
}
static unsigned long __attribute__((ms_abi)) fake_addref(void *self) { return 1; }
static unsigned long __attribute__((ms_abi)) fake_release(void *self) { return 1; }

static HRESULT __attribute__((ms_abi)) fake_create_usb_target(void *self, int unused, void **output) {
    if(output && cached_usb_target) {
        *output = cached_usb_target;
        write(2, "[WBF] fake_create_usb_target: stored target\n", 44);
        return 0;
    }
    return 0x80004005;
}

static HRESULT __attribute__((ms_abi)) fake_stub() { return 0; }

static void *fake_vtbl[20];
static struct { void **vtbl; } fake_com_obj;

static void init_fake_obj(void) {
    for(int i = 0; i < 20; i++) fake_vtbl[i] = (void*)fake_stub;
    fake_vtbl[0] = (void*)fake_qi;
    fake_vtbl[1] = (void*)fake_addref;
    fake_vtbl[2] = (void*)fake_release;
    fake_vtbl[15] = (void*)fake_create_usb_target;
    fake_com_obj.vtbl = fake_vtbl;
}

/* ─── VFM/PAL direct initialization ─── */

static void do_vfm_init(uint8_t *img) {
    char buf[200];
    int n;

    /*
     * RVA map (from Ghidra decompilation):
     *   0x3d740 — FUN_18003d740: set VFM debug callback (stores param to global)
     *   0x13ee0 — FUN_180013ee0: debug log callback function
     *   0x2f780 — FUN_18002f780: vfmUtilSessionInit (alloc 0x440 session, LED thread)
     *   0x300f0 — FUN_1800300f0: vfmUtilDeviceOpenWithParam(session, devId, config)
     *   0x303d0 — FUN_1800303d0: vfmUtilSessionGetDeviceHandle(session, &handle)
     *   0x1f0a0 — FUN_18001f0a0: vfmStgModuleInit(handle)
     */

    /* Step 1: Set VFM debug callback */
    fn_set_callback_t set_dbg_cb = (fn_set_callback_t)(img + 0x3d740);
    void *dbg_cb = (void*)(img + 0x13ee0);
    int rc = set_dbg_cb(dbg_cb);
    n = snprintf(buf, sizeof(buf), "[VFM] set_debug_callback: %d\n", rc);
    write(2, buf, n);

    /* Step 2: Create VFM session */
    fn_session_init_t session_init = (fn_session_init_t)(img + 0x2f780);
    void *session = NULL;
    rc = session_init(&session);
    n = snprintf(buf, sizeof(buf), "[VFM] vfmUtilSessionInit: rc=%d session=%p\n", rc, session);
    write(2, buf, n);
    if(rc != 0 || !session) {
        write(2, "[VFM] Session init failed, aborting VFM init\n", 45);
        return;
    }

    /*
     * Step 3: Open device via _vfmUtilDeviceOpen (RVA 0x332e0)
     *
     * This is the INNER function that handles NULL device ID (auto-enumerate)
     * and calls vfmInitialize + vfmDeviceOpen internally.
     *
     * Config struct layout (24 bytes):
     *   +0x00: char transport_type (1=WBF, 2=external, 3=direct WinUSB)
     *   +0x08: void* usb_handle (not needed for type 3)
     *   +0x10: void* existing_device (0 = create new)
     *
     * Signature: uint _vfmUtilDeviceOpen(longlong session, uint* devId, char* config)
     *   devId = NULL → auto-enumerate USB devices
     */
    uint8_t config[24];
    memset(config, 0, sizeof(config));
    config[0] = 0x01;  /* WBF/standard — calls palWinUsbInitialize to load our shims */

    fn_device_open_t dev_open = (fn_device_open_t)(img + 0x332e0);

    /*
     * Device ID struct: { uint32_t size, uint32_t pad, char *name }
     * The name is passed to CreateFileA → WinUsb_Initialize.
     * Our shims don't care about the path content — they use the cached libusb handle.
     * But it must be non-empty for the flow to work.
     */
    static char sensor_name[] = "\\\\?\\USB#VID_047D&PID_00F2#TUDOR0";
    struct __attribute__((packed)) {
        uint32_t size;
        uint32_t _pad;
        char *name;
    } dev_id;
    dev_id.size = (uint32_t)sizeof(sensor_name);
    dev_id._pad = 0;
    dev_id.name = sensor_name;

    n = snprintf(buf, sizeof(buf), "[VFM] Calling _vfmUtilDeviceOpen(type=3, name='%s', size=%u)\n",
        sensor_name, dev_id.size);
    write(2, buf, n);

    uint32_t_w drc = dev_open(session, &dev_id, config);
    n = snprintf(buf, sizeof(buf), "[VFM] _vfmUtilDeviceOpen: rc=%u (0x%x)\n", drc, drc);
    write(2, buf, n);

    if(drc != 0) {
        n = snprintf(buf, sizeof(buf), "[VFM] Device open failed (rc=%u)\n", drc);
        write(2, buf, n);
        return;
    }

    /* Step 4: Get device handle from session */
    fn_get_dev_handle_t get_dev_handle = (fn_get_dev_handle_t)(img + 0x303d0);
    void *dev_handle = NULL;
    rc = get_dev_handle(session, &dev_handle);
    n = snprintf(buf, sizeof(buf), "[VFM] vfmUtilSessionGetDeviceHandle: rc=%d handle=%p\n", rc, dev_handle);
    write(2, buf, n);

    /* Step 5: Initialize storage module */
    if(dev_handle) {
        fn_stg_init_t stg_init = (fn_stg_init_t)(img + 0x1f0a0);
        rc = stg_init(dev_handle);
        n = snprintf(buf, sizeof(buf), "[VFM] vfmStgModuleInit: rc=%d\n", rc);
        write(2, buf, n);
    }

    write(2, "[VFM] === VFM/PAL initialization complete ===\n", 47);
}

/* ─── Full init sequence (runs on timer thread with full stack) ─── */

static void do_full_init(uint8_t *img) {
    char buf[200];
    int n;

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

    n = snprintf(buf, sizeof(buf), "[WBF] Setup: dev=%p usb_target=%p fake_obj=%p\n",
        *dev_at_58, cached_usb_target, &fake_com_obj);
    write(2, buf, n);

    /* Call WBFUsbInitialize (RVA 0x16160) */
    driver_fn_t wbf = (driver_fn_t)(img + 0x16160);
    write(2, "[WBF] >>> WBFUsbInitialize <<<\n", 30);
    HRESULT hr = wbf(*p_usb_obj);
    n = snprintf(buf, sizeof(buf), "[WBF] WBFUsbInit returned 0x%x\n", hr);
    write(2, buf, n);

    /* Now call VFM/PAL directly (we have a full thread stack here) */
    write(2, "[WBF] Calling VFM/PAL init (on timer thread w/ full stack)...\n", 62);
    do_vfm_init(img);
}

/* ─── Timer thread + constructor ─── */

static void *timer_thread(void *arg) {
    for(int i = 0; i < 30; i++) {
        usleep(500000);
        if(!p_usb_obj || !*p_usb_obj || !p_driver_dll || !*p_driver_dll) continue;
        void *base = *(void**)((uint8_t*)*p_driver_dll + 64);
        if(!base) continue;
        uint8_t *img = (uint8_t*)base;
        if(i == 2) {
            char buf[80];
            int n = snprintf(buf, sizeof(buf), "[WBF-TIMER] check: 0x929b=%02x 0x161bb=%02x\n",
                img[0x929b], img[0x161bb]);
            write(2, buf, n);
        }
        if(img[0x929b] == 0xEB && img[0x161bb] == 0x41) {
            write(2, "[WBF-TIMER] Patches active! Running init on timer thread...\n", 60);
            do_full_init(img);
            return NULL;
        }
    }
    write(2, "[WBF-TIMER] Timed out waiting for patches\n", 42);
    return NULL;
}

__attribute__((constructor(65535)))
static void setup(void) {
    p_usb_obj = (void**)dlsym(RTLD_DEFAULT, "com_usb_device_obj");
    p_driver_dll = (void**)dlsym(RTLD_DEFAULT, "tudor_driver_dll");
    set_cur = dlsym(RTLD_DEFAULT, "winmodule_set_cur");

    pthread_t t;
    pthread_create(&t, NULL, timer_thread, NULL);
    pthread_detach(t);
}
