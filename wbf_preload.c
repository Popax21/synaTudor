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
static volatile int vfm_init_done = 0;
static void patch_sensor_adapter(void);

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

/* ─── USB control transfer implementation ─── */

/*
 * Replace the stubbed FormatRequestForControlTransfer (vtable[16]) with a
 * real implementation that performs the USB control transfer via libusb.
 *
 * In WDF, FormatRequestForControlTransfer just stores the setup packet in the
 * request. The actual transfer happens on Send(). But since the driver likely
 * does synchronous Send() right after, we perform the transfer immediately
 * and store the result in the memory buffer.
 *
 * WINUSB_SETUP_PACKET layout (8 bytes):
 *   uint8_t  RequestType
 *   uint8_t  Request
 *   uint16_t Value
 *   uint16_t Index
 *   uint16_t Length
 *
 * com_memory layout: { com_object obj; void *buffer; size_t size; }
 * com_object layout: { void *vtbl; uint32_t ref_count; uint32_t _pad; void *impl_data; }
 */

/* libusb handle from libtudor.so */
static void **p_libusb_dev = NULL;

typedef void *libusb_dev_handle_t;
/* libusb_control_transfer from the system libusb */
static int (*real_libusb_control_transfer)(libusb_dev_handle_t dev,
    uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue,
    uint16_t wIndex, unsigned char *data, uint16_t wLength,
    unsigned int timeout);

static HRESULT __attribute__((ms_abi)) real_format_ctrl_transfer(
    void *self, void *request, void *setup_packet, void *memory, void *offset)
{
    char buf[200];
    int n;

    write(2, "[USB-CTRL] >>> real_format_ctrl_transfer CALLED <<<\n", 51);

    if(!setup_packet) {
        write(2, "[USB-CTRL] No setup packet!\n", 27);
        return 0; /* S_OK — let it proceed */
    }

    uint8_t *sp = (uint8_t*)setup_packet;
    uint8_t  bmRequestType = sp[0];
    uint8_t  bRequest      = sp[1];
    uint16_t wValue        = *(uint16_t*)(sp + 2);
    uint16_t wIndex        = *(uint16_t*)(sp + 4);
    uint16_t wLength       = *(uint16_t*)(sp + 6);

    n = snprintf(buf, sizeof(buf),
        "[USB-CTRL] bmReqType=0x%02x bReq=0x%02x wVal=0x%04x wIdx=0x%04x wLen=%u\n",
        bmRequestType, bRequest, wValue, wIndex, wLength);
    write(2, buf, n);

    /* Get the data buffer from the memory object */
    unsigned char *data_buf = NULL;
    size_t data_size = 0;
    if(memory) {
        /* com_memory: { com_object(24 bytes), void *buffer, size_t size } */
        uint8_t *mem = (uint8_t*)memory;
        data_buf = *(unsigned char**)(mem + 24);   /* buffer pointer */
        data_size = *(size_t*)(mem + 32);          /* size */
    }

    /* Perform actual USB control transfer via libusb */
    if(p_libusb_dev && *p_libusb_dev && real_libusb_control_transfer) {
        unsigned char xfer_buf[4096];
        uint16_t xfer_len = (wLength < sizeof(xfer_buf)) ? wLength : sizeof(xfer_buf);

        /* For OUT transfers, copy data from memory to xfer_buf */
        if(!(bmRequestType & 0x80) && data_buf && xfer_len > 0) {
            size_t copy_len = (xfer_len < data_size) ? xfer_len : data_size;
            memcpy(xfer_buf, data_buf, copy_len);
        }

        int ret = real_libusb_control_transfer(*p_libusb_dev,
            bmRequestType, bRequest, wValue, wIndex,
            xfer_buf, xfer_len, 5000);

        n = snprintf(buf, sizeof(buf), "[USB-CTRL] libusb_control_transfer: ret=%d\n", ret);
        write(2, buf, n);

        /* For IN transfers, copy response to memory buffer */
        if(ret >= 0 && (bmRequestType & 0x80) && data_buf) {
            size_t copy_len = (ret < (int)data_size) ? ret : data_size;
            memcpy(data_buf, xfer_buf, copy_len);
        }
    } else {
        write(2, "[USB-CTRL] No libusb device or function available\n", 50);
    }

    return 0; /* S_OK */
}

static void patch_usb_target_vtable(void) {
    char buf[200];
    int n;

    if(!p_usb_obj) return;

    /*
     * g_usb_target_vtbl is at a fixed offset from com_usb_device_obj in libtudor.so.
     * From nm: g_usb_target_vtbl @ 0x1e37c0, com_usb_device_obj @ 0x1e5288
     * Offset: 0x1e5288 - 0x1e37c0 = 0x1ac8
     */
    /*
     * g_usb_target_vtbl is at com_usb_device_obj_addr - 0x1ac8
     * g_usb_target_device is at com_usb_device_obj_addr - 0x1ba8
     */
    void **usb_target_vtbl = (void**)((uint8_t*)p_usb_obj - 0x1ac8);
    void *usb_target_device = (void*)((uint8_t*)p_usb_obj - 0x1ba8);

    /* Verify: g_usb_target_device.vtbl should point to g_usb_target_vtbl */
    void **dev_vtbl_ptr = *(void***)usb_target_device;

    n = snprintf(buf, sizeof(buf),
        "[USB-CTRL] p_usb_obj=%p calculated_vtbl=%p dev_at=%p dev->vtbl=%p match=%s\n",
        (void*)p_usb_obj, (void*)usb_target_vtbl, usb_target_device,
        (void*)dev_vtbl_ptr, (dev_vtbl_ptr == usb_target_vtbl) ? "YES" : "NO");
    write(2, buf, n);

    if(dev_vtbl_ptr != usb_target_vtbl) {
        /* Try to find the actual vtable by reading through the device object */
        write(2, "[USB-CTRL] Vtable mismatch! Patching via device->vtbl instead\n", 62);
        usb_target_vtbl = dev_vtbl_ptr;
    }

    n = snprintf(buf, sizeof(buf),
        "[USB-CTRL] Patching vtbl at %p, slot[16] was %p\n",
        (void*)usb_target_vtbl, usb_target_vtbl[16]);
    write(2, buf, n);

    usb_target_vtbl[16] = (void*)real_format_ctrl_transfer;

    /* Verify the patch by reading the value back */
    n = snprintf(buf, sizeof(buf),
        "[USB-CTRL] Patched slot[16] → %p (verify: slot[16]=%p)\n",
        (void*)real_format_ctrl_transfer, usb_target_vtbl[16]);
    write(2, buf, n);

    /* Also check nearby slots to understand the vtable layout */
    for(int i = 14; i <= 20; i++) {
        n = snprintf(buf, sizeof(buf), "[USB-CTRL] vtbl[%d] = %p\n", i, usb_target_vtbl[i]);
        write(2, buf, n);
    }
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

    /* Step 2.5: Patch USB target vtable BEFORE any VFM calls that use it */
    patch_usb_target_vtable();

    /* Dump COM interface pointers to find CBiometricDevice base offset */
    {
        void **p_ioctl_cb = (void**)dlsym(RTLD_DEFAULT, "com_ioctl_callback");
        void **p_pnp_hw = (void**)dlsym(RTLD_DEFAULT, "com_pnp_hw_callback");
        void **p_drv_cb = (void**)dlsym(RTLD_DEFAULT, "com_driver_callback");
        void *usb_obj = p_usb_obj ? *p_usb_obj : NULL;
        void *ioctl_obj = p_ioctl_cb ? *p_ioctl_cb : NULL;
        void *pnp_obj = p_pnp_hw ? *p_pnp_hw : NULL;
        void *drv_obj = p_drv_cb ? *p_drv_cb : NULL;

        n = snprintf(buf, sizeof(buf),
            "[VFM-DBG] COM ptrs: usb=%p ioctl=%p pnp_hw=%p drv=%p\n",
            usb_obj, ioctl_obj, pnp_obj, drv_obj);
        write(2, buf, n);

        if(usb_obj && ioctl_obj) {
            ptrdiff_t diff = (uint8_t*)ioctl_obj - (uint8_t*)usb_obj;
            n = snprintf(buf, sizeof(buf),
                "[VFM-DBG] ioctl_cb - usb_obj = %td (0x%tx)\n", diff, diff);
            write(2, buf, n);
        }
        if(usb_obj && pnp_obj) {
            ptrdiff_t diff = (uint8_t*)pnp_obj - (uint8_t*)usb_obj;
            n = snprintf(buf, sizeof(buf),
                "[VFM-DBG] pnp_hw - usb_obj = %td (0x%tx)\n", diff, diff);
            write(2, buf, n);
        }
    }

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

    /*
     * Step 6: Store VFM session + device handle in the CBiometricDevice object
     * so the driver's IOCTL handler can find them.
     *
     * InitializeNiseCore (FUN_180004de4) stores:
     *   base + 0x48 = initialized flag (byte, set to 1)
     *   base + 0x70 = device handle (from vfmUtilSessionGetDeviceHandle)
     *   base + 0x78 = VFM session pointer
     *
     * com_usb_device_obj is the {1493cd1b} interface = base + 0x08
     * So: base = com_usb_device_obj - 0x08
     */
    if(p_usb_obj && *p_usb_obj && session) {
        /*
         * CBiometricDevice C++ object layout (from runtime pointer analysis):
         *   com_usb_device_obj  = cpp_obj + 0x00  ({1493cd1b} interface)
         *   com_pnp_hw_callback = cpp_obj + 0x08  (IPnpCallbackHardware)
         *   com_ioctl_callback  = cpp_obj + 0x28  (IQueueCallbackDeviceIoControl)
         *
         * PrepareHardware is called on IPnpCallbackHardware = cpp_obj + 0x08
         * InitializeNiseCore stores fields relative to PrepareHardware's self:
         *   self + 0x48 = init flag    → cpp_obj + 0x50
         *   self + 0x70 = dev handle   → cpp_obj + 0x78
         *   self + 0x78 = VFM session  → cpp_obj + 0x80
         *
         * The IOCTL handler (self = com_ioctl_callback = cpp_obj + 0x28)
         * adjusts back to the base and accesses these same fields.
         */
        /*
         * Corrected offsets — confirmed by IOCTL handler decompilation:
         *   OnDeviceIoControl thunk adjusts this by +0x08 before the handler
         *   Handler does: plVar3 = param_1 - 0x30
         *   So plVar3 = (com_ioctl_callback + 0x08) - 0x30 = com_usb_device_obj
         *   All field offsets are relative to com_usb_device_obj directly.
         */
        uint8_t *cpp_obj = (uint8_t*)*p_usb_obj;  /* com_usb_device_obj */

        /* Set initialized flag: cpp_obj + 0x48 */
        *(uint8_t*)(cpp_obj + 0x48) = 1;

        /* Store device handle: cpp_obj + 0x70 */
        if(dev_handle) {
            *(void**)(cpp_obj + 0x70) = dev_handle;
        }

        /* Store VFM session: cpp_obj + 0x78 */
        *(void**)(cpp_obj + 0x78) = session;

        /*
         * CRITICAL: Clear the "degraded mode" flag at +0xBC.
         * OnGetSensorStatus checks this flag — if set to 1, it forces
         * sensor status to 6 (NOT_CALIBRATED). PrepareHardware sets it
         * to 1 when vtable[0x40] fails. Since we skip PrepareHardware,
         * we must ensure it's 0.
         */
        *(uint8_t*)(cpp_obj + 0xBC) = 0;

        /* Set sensor status to READY (3) at +0x84.
         * OnGetSensorStatus reads this field for the actual status value. */
        *(uint32_t*)(cpp_obj + 0x84) = 3;  /* WINBIO_SENSOR_READY */

        /* Set session-active flag at +0x4F1.
         * SessionStart checks this — if 0, tries to re-open session.
         * Since we already opened the session, set to 1. */
        *(uint8_t*)(cpp_obj + 0x4F1) = 1;

        n = snprintf(buf, sizeof(buf),
            "[VFM] Stored: init@+0x48=1 dev@+0x70=%p session@+0x78=%p degraded@+0xBC=0 status@+0x84=READY\n",
            dev_handle, session);
        write(2, buf, n);
    }

    /* Step 6.5: Create sensor data manager at +0x428
     * FUN_180002ce0(param_1):
     *   puVar1 = operator_new(0x138);
     *   puVar2 = FUN_18000e2e4(puVar1, *(param_1 + 0x78));  // session
     *   *(param_1 + 0x428) = puVar2;
     *
     * We need to call FUN_18000e2e4 with the allocated 0x138 buffer and session,
     * then store the result at cpp_obj + 0x428.
     */
    if(p_usb_obj && *p_usb_obj && session) {
        uint8_t *cpp_obj = (uint8_t*)*p_usb_obj;

        /* Allocate 0x138 bytes (operator new) */
        typedef void* __attribute__((ms_abi)) (*fn_alloc_t)(size_t size);
        /* Use the DLL's allocator at FUN_18003dc80 */
        fn_alloc_t palloc = (fn_alloc_t)(img + 0x3dc80);
        void *mgr_buf = palloc(0x138);

        if(mgr_buf) {
            /* Zero it */
            memset(mgr_buf, 0, 0x138);

            /* Call FUN_18000e2e4(buffer, session) to initialize */
            typedef void* __attribute__((ms_abi)) (*fn_mgr_init_t)(void *buf, void *session);
            fn_mgr_init_t mgr_init = (fn_mgr_init_t)(img + 0xe2e4);
            void *mgr = mgr_init(mgr_buf, session);

            /* Store at cpp_obj + 0x428 */
            *(void**)(cpp_obj + 0x428) = mgr;

            /* Verify manager vtable */
            void **mgr_vtbl = mgr ? *(void***)mgr : NULL;
            n = snprintf(buf, sizeof(buf),
                "[VFM] Created sensor data mgr: %p vtbl=%p vtbl[0]=%p vtbl[1]=%p\n",
                mgr, (void*)mgr_vtbl,
                mgr_vtbl ? mgr_vtbl[0] : NULL,
                mgr_vtbl ? mgr_vtbl[1] : NULL);
            write(2, buf, n);
        }
    }

    /* Step 7: Try to start a VFM device session (triggers actual USB communication) */
    if(dev_handle) {
        typedef int __attribute__((ms_abi)) (*fn_session_start_vfm_t)(void *dev_handle, uint32_t type, uint32_t *out_flag);
        fn_session_start_vfm_t vfm_sess_start = (fn_session_start_vfm_t)(img + 0x1bbd0);

        uint32_t out_flag = 0;
        write(2, "[VFM] Calling vfmDeviceSessionStart...\n", 38);
        rc = vfm_sess_start(dev_handle, 1, &out_flag);
        n = snprintf(buf, sizeof(buf), "[VFM] vfmDeviceSessionStart: rc=%d out_flag=%u\n", rc, out_flag);
        write(2, buf, n);
    }

    /* Step 7.5: Send FPS_INIT through proto IoControl TLS path */
    if(dev_handle) {
        void **_dh = (void**)dev_handle;
        void *ts = _dh[1] ? *(void**)_dh[1] : NULL;
        if(ts) {
            void *proto_ctx = *(void**)ts;
            typedef int __attribute__((ms_abi)) (*fn_proto_t)(
                void *ctx, int cmd, void *data, uint32_t dlen,
                void *resp, uint32_t rlen, int *err);
            fn_proto_t proto_fn = *(fn_proto_t*)((uint8_t*)ts + 0xD8);
            if(proto_ctx && proto_fn) {
                uint8_t fps[] = { 0xFE, 0x01, 0x11, 0x00, 0x00 };
                uint32_t rdesc[4] = {0}; rdesc[0] = 0x10;
                int err = 0;
                write(2, "[VFM] Sending FPS_INIT [FE 01 11 00 00]...\n", 43);
                rc = proto_fn(proto_ctx, 0x67, fps, 5, rdesc, 0x10, &err);
                n = snprintf(buf, sizeof(buf), "[VFM] FPS_INIT: rc=%d err=%d\n", rc, err);
                write(2, buf, n);
            }
        }
    }

    /* Step 8: Initialize device and test capture */
    if(dev_handle) {
        /*
         * vfmDeviceInitialize(dev_handle, output_buffer)
         * Calls module_vtable+0x48 (ssi initDevice).
         * param2 is an output buffer, not NULL.
         */
        typedef int __attribute__((ms_abi)) (*fn_dev_init_t)(void *dev_handle, void *param2);
        fn_dev_init_t dev_init = (fn_dev_init_t)(img + 0x196d0);

        /* Binary-patch tudorCaptureStart: change the NAV state check
         * (MOV [RSP+0x34], 0x68) at RVA 0x58bea to MOV [RSP+0x34], 0.
         * This forces the NAV check to "succeed" instead of returning 0x68.
         * The instruction bytes at 0x58bea: C7 44 24 34 68 00 00 00
         * We change byte at 0x58bee (the 0x68 immediate) to 0x00. */
        /* Patch ALL 0x68 error returns in the tudor capture chain.
         * Found 4 locations via Ghidra instruction search. */
        int patches_68[] = { 0x58bee, 0x5a39a, 0x6537d, 0x68fc4 };
        for(int i = 0; i < 4; i++) {
            if(img[patches_68[i]] == 0x68) {
                img[patches_68[i]] = 0x00;
                n = snprintf(buf, sizeof(buf), "[VFM] Patched 0x%x (0x68→0x00)\n", patches_68[i]);
                write(2, buf, n);
            }
        }

        /*
         * vfmCaptureStart(dev_handle, purpose, options, timeout)
         * Calls module_vtable+0x80 → tudorCaptureStart
         * purpose: 1=verify, 2=enroll, 8=low-power
         */
        typedef int __attribute__((ms_abi)) (*fn_capture_start_t)(void *dev_handle, uint32_t p2, uint32_t p3, uint32_t p4);
        fn_capture_start_t capture_start = (fn_capture_start_t)(img + 0x2d0a0);

        /* Check device handle internal state */
        void **dh = (void**)dev_handle;
        n = snprintf(buf, sizeof(buf),
            "[VFM] dev_handle[0..4]: %p %p %p %p %p\n",
            dh[0], dh[1], dh[2], dh[3], dh[4]);
        write(2, buf, n);

        /* dh[0] is the USB device container. Dump its structure. */
        if(dh[0]) {
            void **usb_container = (void**)dh[0];
            n = snprintf(buf, sizeof(buf),
                "[VFM] dh[0] (USB container): [0]=%p [1]=%p\n",
                usb_container[0], usb_container[1]);
            write(2, buf, n);
            /* Check if dh[0][0] is the PAL USB 0xC0 struct */
            if(usb_container[0]) {
                uint8_t *pal_usb = (uint8_t*)usb_container[0];
                n = snprintf(buf, sizeof(buf),
                    "[VFM] PAL USB struct? +0xb0=%p +0xb8=%p\n",
                    *(void**)(pal_usb + 0xb0), *(void**)(pal_usb + 0xb8));
                write(2, buf, n);
            }
        }

        /* vfmDeviceInitialize passes **(dev_handle+8) = *dh[1] to tudorInitDevice.
         * The tudor function accesses the sensor struct at large offsets (0xD8, 0xC4, etc.)
         * and calls function pointers within it. Let me dump it. */
        if(dh[1]) {
            void **inner = (void**)dh[1];
            void *sensor = inner[0];
            n = snprintf(buf, sizeof(buf),
                "[VFM] dh[1]=%p → *dh[1]=%p (sensor struct for tudorInitDevice)\n",
                (void*)inner, sensor);
            write(2, buf, n);

            if(sensor) {
                uint8_t *s = (uint8_t*)sensor;
                /* _tudorInitDevice calls: func(*param_1, 0x12, 0, 0)
                 * where func = param_1[0x1b] and *param_1 = sensor[0]
                 * If sensor[0] is a struct and func accesses it at +0x14C,
                 * a NULL sub-pointer causes SIGSEGV at 0x14C.
                 * Dump sensor[0] and the struct it points to. */
                void *sensor0 = *(void**)s;
                n = snprintf(buf, sizeof(buf),
                    "[VFM] sensor[0]=%p (passed to tudorProtoIoControl)\n", sensor0);
                write(2, buf, n);

                if(sensor0) {
                    uint8_t *s0 = (uint8_t*)sensor0;
                    /* Dump around 0x14C */
                    n = snprintf(buf, sizeof(buf),
                        "[VFM] sensor[0]+0x140=%p +0x148=%p +0x150=%p\n",
                        *(void**)(s0 + 0x140), *(void**)(s0 + 0x148), *(void**)(s0 + 0x150));
                    write(2, buf, n);
                    /* Dump ALL non-NULL fields to understand what IS initialized */
                    for(int off = 0; off <= 0x160; off += 8) {
                        void *val = *(void**)(s0 + off);
                        if(val != NULL) {
                            n = snprintf(buf, sizeof(buf), "[VFM] sensor[0]+0x%x = %p\n", off, val);
                            write(2, buf, n);
                        }
                    }
                }

                /* Key fields used by _tudorInitDevice */
                n = snprintf(buf, sizeof(buf),
                    "[VFM] sensor+0xBC=%08x +0xC4=%08x +0xD8=%p\n",
                    *(uint32_t*)(s + 0xBC), *(uint32_t*)(s + 0xC4), *(void**)(s + 0xD8));
                write(2, buf, n);
            }
        }

        /*
         * The tudor sensor struct (at *dh[1]) has a proto context (at sensor[0]).
         * The proto context needs a USB connection handle at offset +0x8.
         * Without it, tudorUsbProtoIoControl crashes trying to send commands.
         *
         * Let's populate it with the PAL inner device pointer (dh[0]).
         */
        if(dh[1]) {
            void **inner = (void**)dh[1];
            void *tudor_sensor = inner[0];
            if(tudor_sensor) {
                void **proto_ctx = *(void***)tudor_sensor; /* proto = sensor[0][0] */
                n = snprintf(buf, sizeof(buf),
                    "[VFM] tudor_sensor=%p proto_ctx=%p proto[0]=%p proto[1]=%p\n",
                    tudor_sensor, proto_ctx,
                    proto_ctx ? proto_ctx[0] : NULL,
                    proto_ctx ? proto_ctx[1] : NULL);
                write(2, buf, n);

                /* Try setting proto_ctx+0x8 (the USB context) to the PAL device */
                uint8_t *ts = (uint8_t*)tudor_sensor;
                void *usb_ctx_field = *(void**)(ts + 0x8);
                if(!usb_ctx_field && dh[0]) {
                    n = snprintf(buf, sizeof(buf),
                        "[VFM] sensor+0x8 is NULL — populating with PAL device %p\n", dh[0]);
                    write(2, buf, n);
                    *(void**)(ts + 0x8) = dh[0];
                }
            }
        }

        /* PAL session start confirmed working (via vfmDeviceSessionStart).
         * The PAL struct has only +0xb0 populated; other commands crash.
         * Skip PAL direct tests — focus on fixing the tudor sensor struct. */
        if(0) {
            typedef int __attribute__((ms_abi)) (*fn_pal_ioctl_t)(
                void *driver_ctx, int cmd, void *data, int data_len,
                void *resp, int resp_len, void *error);
            fn_pal_ioctl_t pal_ioctl = (fn_pal_ioctl_t)(img + 0x437c0);

            /* The PAL driver context is **tudor_sensor (proto_ctx[0]).
             * Tudor layer: sensor → proto_ctx → driver_handle → PAL functions
             * NOT dh[0] — that's the VFM-level USB device, different from PAL context. */
            void *tudor_sensor = dh[1] ? *(void**)dh[1] : NULL;
            void *proto_ctx = tudor_sensor ? *(void**)tudor_sensor : NULL;
            void *pal_ctx = proto_ctx ? *(void**)proto_ctx : NULL;
            n = snprintf(buf, sizeof(buf),
                "[PAL] Chain: tudor_sensor=%p proto_ctx=%p pal_ctx=%p\n",
                tudor_sensor, proto_ctx, pal_ctx);
            write(2, buf, n);

            /* Dump pal_ctx contents around offset 0x5F to find the NULL */
            if(pal_ctx) {
                uint8_t *pc = (uint8_t*)pal_ctx;
                n = snprintf(buf, sizeof(buf),
                    "[PAL] pal_ctx: +0x00=%p +0x08=%p +0x10=%p +0x50=%p +0x58=%p +0x60=%p\n",
                    *(void**)(pc), *(void**)(pc+8), *(void**)(pc+0x10),
                    *(void**)(pc+0x50), *(void**)(pc+0x58), *(void**)(pc+0x60));
                write(2, buf, n);
            }

            n = snprintf(buf, sizeof(buf), "[PAL] Testing palDriverIoControl(ctx=%p, cmd=2/ReadInfo)\n", pal_ctx);
            write(2, buf, n);

            /* But wait — maybe the PAL context for palDriverIoControl is NOT pal_ctx
             * (= proto_ctx[0]) but rather the PAL USB struct from dh[0].
             * The PAL USB struct has the WinUSB interface at +0xb0.
             * palDriverIoControl may expect the PAL USB struct directly. */
            void *pal_usb_struct = dh[0] ? *(void**)dh[0] : NULL;
            n = snprintf(buf, sizeof(buf),
                "[PAL] pal_usb_struct=%p (from *dh[0]), pal_ctx=%p SAME=%s\n",
                pal_usb_struct, pal_ctx,
                (pal_usb_struct == pal_ctx) ? "YES" : "NO");
            write(2, buf, n);

            /* Try session start first (uses +0xb0 only) */
            uint32_t sess_type = 1;
            uint8_t sess_resp = 0;
            rc = pal_ioctl(pal_usb_struct, 0x15, &sess_type, 4, &sess_resp, 1, NULL);
            n = snprintf(buf, sizeof(buf),
                "[PAL] SessionStart(cmd=0x15): rc=%d resp=%d\n", rc, sess_resp);
            write(2, buf, n);

            /* Now try Device Reset (cmd=1) — needs +0x00 populated.
             * The WinUSB interface struct at +0xb0 has the real handles.
             * Let's set +0x00 to point to the WinUSB interface struct,
             * since that's where the USB functions live. */
            if(pal_usb_struct) {
                uint8_t *pus = (uint8_t*)pal_usb_struct;
                void *winusb_iface = *(void**)(pus + 0xb0);
                if(winusb_iface && *(void**)pus == NULL) {
                    n = snprintf(buf, sizeof(buf),
                        "[PAL] Populating pal_ctx+0x00 with WinUSB interface %p\n", winusb_iface);
                    write(2, buf, n);
                    *(void**)pus = winusb_iface;
                }

                /* Try device reset (cmd=1) — simplest command */
                rc = pal_ioctl(pal_usb_struct, 1, NULL, 0, NULL, 0, NULL);
                n = snprintf(buf, sizeof(buf), "[PAL] DeviceReset(cmd=1): rc=%d\n", rc);
                write(2, buf, n);

                /* Try bulk read from EP0 (cmd=3) — reads sensor data via control transfer */
                if(rc == 0) {
                    uint8_t bulk_resp[64];
                    memset(bulk_resp, 0, sizeof(bulk_resp));
                    rc = pal_ioctl(pal_usb_struct, 3, NULL, 0, bulk_resp, 32, NULL);
                    n = snprintf(buf, sizeof(buf),
                        "[PAL] BulkReadEP0(cmd=3): rc=%d data=[%02x %02x %02x %02x]\n",
                        rc, bulk_resp[0], bulk_resp[1], bulk_resp[2], bulk_resp[3]);
                    write(2, buf, n);
                }
            }

            if(rc == 0) {
                write(2, "[PAL] *** USB COMMUNICATION CONFIRMED! ***\n", 43);
            }
        }

        /* Call tudorCaptureStart directly via module vtable to bypass VFM wrapper */
        {
            /* Get the module vtable */
            void **p_vfm_ctx = (void**)dlsym(RTLD_DEFAULT, "DAT_18014c008");
            /* The global is at a fixed offset. Let me use the DLL image to find it. */
            void **vfm_ctx_ptr = (void**)(img + (0x14c008 - 0));  /* RVA in .data section */

            /* Actually, the global DAT_18014c008 is at the DLL's data section.
             * img = base_addr of the DLL image. RVA 0x14c008 → img + 0x14c008 */
            void *vfm_ctx = *(void**)(img + 0x14c008);
            void *module_vtable = vfm_ctx ? *(void**)vfm_ctx : NULL;

            n = snprintf(buf, sizeof(buf),
                "[VFM] DAT_18014c008=%p *=%p module_vtable=%p\n",
                (void*)(img + 0x14c008), vfm_ctx, module_vtable);
            write(2, buf, n);

            if(module_vtable && dh[1]) {
                void *tudor_sensor = *(void**)dh[1];
                typedef int __attribute__((ms_abi)) (*fn_tudor_cap_t)(
                    void *sensor, void *action_info, uint32_t purpose, uint32_t options, int timeout);
                fn_tudor_cap_t tudor_capture = *(fn_tudor_cap_t*)((uint8_t*)module_vtable + 0x80);

                /* Dump the sensor state that tudorCaptureStart will check */
                uint32_t state_c4 = *(uint32_t*)((uint8_t*)tudor_sensor + 0xC4);
                uint32_t retry_c0 = *(uint32_t*)((uint8_t*)tudor_sensor + 0xC0);
                void *proto = *(void**)tudor_sensor;
                void *driver_handle = proto ? *(void**)proto : NULL;

                /* Check what function is at sensor+0xD8 (proto IoControl) */
                void *proto_fn = *(void**)((uint8_t*)tudor_sensor + 0xD8);
                ptrdiff_t proto_fn_rva = proto_fn ? (uint8_t*)proto_fn - img : -1;

                n = snprintf(buf, sizeof(buf),
                    "[VFM] tudor_sensor=%p state+0xC4=%u proto_fn=%p (RVA 0x%tx)\n",
                    tudor_sensor, state_c4, proto_fn, proto_fn_rva);
                write(2, buf, n);

                /* Replace module_vtable+0x80 with a hook that wraps the real function */
                fn_tudor_cap_t real_tudor_cap = tudor_capture;
                static fn_tudor_cap_t saved_real_cap = NULL;
                saved_real_cap = real_tudor_cap;

                /* For now, call directly and check sensor setup return */
                uint32_t action_info[8] = {0};
                write(2, "[VFM] Calling tudorCaptureStart...\n", 34);
                rc = tudor_capture(tudor_sensor, action_info, 2, 0, 5000);

                /* Check sensor state after the call */
                uint32_t state_after = *(uint32_t*)((uint8_t*)tudor_sensor + 0xC4);
                uint32_t retry_count = *(uint32_t*)((uint8_t*)tudor_sensor + 0xC0);

                n = snprintf(buf, sizeof(buf),
                    "[VFM] tudorCaptureStart: rc=%d (0x%x) state=%u retries=%u action=[%u %u]\n",
                    rc, rc, state_after, retry_count, action_info[0], action_info[1]);
                write(2, buf, n);

                /* If rc is from sensor setup, try calling with different parameters */
                if(rc != 0 && rc != 0x68) {
                    n = snprintf(buf, sizeof(buf),
                        "[VFM] Non-0x68 error! rc=%d (0x%x) — this is the REAL error!\n", rc, rc);
                    write(2, buf, n);
                }
            }
        }

        write(2, "[VFM] Also trying vfmCaptureStart wrapper...\n", 45);
        rc = capture_start(dev_handle, 2, 0, 5000);
        n = snprintf(buf, sizeof(buf), "[VFM] vfmCaptureStart: rc=%d (0x%x)\n", rc, rc);
        write(2, buf, n);

        if(rc == 0) {
            write(2, "[VFM] *** SENSOR CAPTURE STARTED! Place your finger! ***\n", 57);
        }
    }

    /* Mark VFM as initialized for IOCTL interception */
    vfm_init_done = 1;

    write(2, "[VFM] === VFM/PAL initialization complete ===\n", 47);
}

/* ─── IOCTL interception via LD_PRELOAD symbol interposition ─── */

/*
 * WINBIO IOCTL codes:
 *   0x440004 = IOCTL_BIOMETRIC_GET_ATTRIBUTES    (function 1)
 *   0x440008 = IOCTL_BIOMETRIC_RESET             (function 2)
 *   0x44000C = IOCTL_BIOMETRIC_CALIBRATE          (function 3)
 *   0x440010 = IOCTL_BIOMETRIC_GET_SENSOR_STATUS  (function 4)
 *   0x440014 = IOCTL_BIOMETRIC_CAPTURE_DATA       (function 5)
 *
 * WINBIO_SENSOR_STATUS values: READY=3, BUSY=4, NOT_CALIBRATED=5, FAILURE=6
 *
 * WINBIO_DIAGNOSTICS (output of GET_SENSOR_STATUS):
 *   uint32_t PayloadSize;      +0x00
 *   HRESULT  WinBioHresult;    +0x04
 *   uint32_t SensorStatus;     +0x08
 */
#define IOCTL_BIOMETRIC_GET_SENSOR_STATUS 0x440010
#define IOCTL_BIOMETRIC_RESET             0x440008
#define IOCTL_BIOMETRIC_GET_ATTRIBUTES    0x440004

typedef int NTSTATUS;
#define STATUS_SUCCESS 0

/* Original com_send_ioctl — resolved at first call */
static NTSTATUS (*orig_com_send_ioctl)(unsigned long code, const void *in_buf,
    size_t in_size, void *out_buf, size_t out_size, size_t *bytes_returned) = NULL;

/* Interposed com_send_ioctl — intercepts specific IOCTLs */
NTSTATUS com_send_ioctl(unsigned long code, const void *in_buf, size_t in_size,
                        void *out_buf, size_t out_size, size_t *bytes_returned) {
    char buf[120];
    int n;

    if(!orig_com_send_ioctl) {
        orig_com_send_ioctl = dlsym(RTLD_NEXT, "com_send_ioctl");
        if(!orig_com_send_ioctl) {
            write(2, "[IOCTL] FATAL: Cannot find original com_send_ioctl!\n", 51);
            return -1;
        }
    }

    /* ALWAYS intercept GET_SENSOR_STATUS — return READY even before VFM init.
       The adapter latches sensor state during Activate — if it sees FAILURE,
       it refuses to StartCapture even after VFM init completes later. */
    if(code == IOCTL_BIOMETRIC_GET_SENSOR_STATUS && out_buf && out_size >= 12) {
        /* Return WINBIO_DIAGNOSTICS with SensorStatus = READY (3) */
        uint32_t *diag = (uint32_t*)out_buf;
        diag[0] = 12;  /* PayloadSize */
        diag[1] = 0;   /* WinBioHresult = S_OK */
        diag[2] = 3;   /* SensorStatus = WINBIO_SENSOR_READY */
        if(bytes_returned) *bytes_returned = 12;

        n = snprintf(buf, sizeof(buf), "[IOCTL] GET_SENSOR_STATUS → READY (intercepted)\n");
        write(2, buf, n);
        return STATUS_SUCCESS;
    }

    /*
     * Lazy VFM init on first IOCTL from the main thread.
     * The VFM session MUST be created on the same thread that processes IOCTLs,
     * because the driver's IOCTL handler may check thread ownership or use
     * thread-local state initialized during the VFM session creation.
     */
    /* Patch adapter QueryStatus on first IOCTL (adapter pointers now valid) */
    static int adapter_patched = 0;
    if(!adapter_patched) {
        adapter_patched = 1;
        patch_sensor_adapter();
    }

    static int main_thread_vfm_init = 0;
    if(!main_thread_vfm_init && p_driver_dll && *p_driver_dll) {
        void *base = *(void**)((uint8_t*)*p_driver_dll + 64);
        if(base) {
            uint8_t *img = (uint8_t*)base;
            if(img[0x929b] == 0xEB && img[0x161bb] == 0x41) {
                main_thread_vfm_init = 1;
                write(2, "[IOCTL] Triggering VFM init on main thread (first IOCTL)...\n", 60);
                if(set_cur) set_cur(*p_driver_dll);
                do_vfm_init(img);
            }
        }
    }

    /* Log ALL IOCTLs for diagnostic */
    n = snprintf(buf, sizeof(buf), "[IOCTL] code=0x%lx in=%zu out=%zu fn=%lu\n",
        code, in_size, out_size, (code >> 2) & 0xFFF);
    write(2, buf, n);

    /* Intercept CAPTURE_DATA (0x440014) and related standard IOCTLs that
       would crash the driver's handler */
    if(code == 0x440014 || code == 0x440018 || code == 0x44001c ||
       code == 0x440020 || code == 0x440024) {
        n = snprintf(buf, sizeof(buf), "[IOCTL] Standard BIOMETRIC code=0x%lx → empty success\n", code);
        write(2, buf, n);
        if(out_buf && out_size >= 4) {
            memset(out_buf, 0, out_size);
            *(uint32_t*)out_buf = (out_size > 4) ? (uint32_t)out_size : 4;
        }
        if(bytes_returned) *bytes_returned = (out_size >= 4) ? 4 : 0;
        return STATUS_SUCCESS;
    }

    /* Vendor IOCTLs (0x442xxx) — sensor commands */
    if((code & 0xFFFF0000) == 0x00440000 && (code & 0x2000)) {
        if(out_buf && out_size >= 4) memset(out_buf, 0, out_size);
        if(bytes_returned) *bytes_returned = (out_size >= 4) ? 4 : 0;
        return STATUS_SUCCESS;
    }

    /* BIOMETRIC_RESET (0x440008) */
    if(code == IOCTL_BIOMETRIC_RESET && out_buf && out_size >= 8) {
        /* WINBIO_BLANK_PAYLOAD: { PayloadSize=8, WinBioHresult=0 } */
        uint32_t *p = (uint32_t*)out_buf;
        p[0] = 8; p[1] = 0;
        if(bytes_returned) *bytes_returned = 8;
        n = snprintf(buf, sizeof(buf), "[IOCTL] RESET → success\n");
        write(2, buf, n);
        return STATUS_SUCCESS;
    }

    /* BIOMETRIC_GET_ATTRIBUTES (0x440004) */
    if(code == IOCTL_BIOMETRIC_GET_ATTRIBUTES && out_buf && out_size >= 0x62C) {
        /* Return WINBIO_SENSOR_ATTRIBUTES with Synaptics info */
        memset(out_buf, 0, out_size);
        uint32_t *p = (uint32_t*)out_buf;
        p[0] = 0x62C;      /* PayloadSize */
        p[1] = 0;           /* WinBioHresult = S_OK */
        p[2] = 8;           /* SensorType = WINBIO_TYPE_FINGERPRINT */
        p[3] = 2;           /* SensorSubType */
        p[4] = 0x4041F;     /* Capabilities */
        /* Manufacturer string at offset 0x14 (wide char) */
        uint16_t *mfr = (uint16_t*)((uint8_t*)out_buf + 0x14);
        const char *s = "Synaptics";
        for(int i = 0; s[i]; i++) mfr[i] = s[i];
        /* Model string at offset 0x214 */
        uint16_t *mdl = (uint16_t*)((uint8_t*)out_buf + 0x214);
        const char *m = "Synaptics WBF Tudor";
        for(int i = 0; m[i]; i++) mdl[i] = m[i];
        if(bytes_returned) *bytes_returned = 0x62C;
        write(2, "[IOCTL] GET_ATTRIBUTES → Synaptics WBF Tudor\n", 46);
        return STATUS_SUCCESS;
    }

    n = snprintf(buf, sizeof(buf), "[IOCTL] code=0x%lx in=%zu out=%zu → forwarding to driver\n",
        code, in_size, out_size);
    write(2, buf, n);

    /* Forward remaining IOCTLs to the real driver */
    NTSTATUS status = orig_com_send_ioctl(code, in_buf, in_size, out_buf, out_size, bytes_returned);

    n = snprintf(buf, sizeof(buf), "[IOCTL] code=0x%lx → status=%d info=%zu\n",
        code, status, bytes_returned ? *bytes_returned : 0);
    write(2, buf, n);

    return status;
}

/* ─── WINBIO adapter patching ─── */

/* Replace sensor adapter's QueryStatus to always return READY.
 * The adapter latches sensor state during Activate — if it ever sees
 * FAILURE, it refuses to StartCapture. By always returning READY,
 * the adapter initializes properly. */
static HRESULT __attribute__((ms_abi)) patched_query_status(void *pipeline, unsigned long *status) {
    if(status) *status = 3;  /* WINBIO_SENSOR_READY */
    return 0; /* S_OK */
}

static void patch_sensor_adapter(void) {
    char buf[120];
    int n;

    void **p_adapter = (void**)dlsym(RTLD_DEFAULT, "tudor_sensor_adapter");
    if(!p_adapter || !*p_adapter) return;

    /*
     * WINBIO_SENSOR_INTERFACE layout:
     *   +0x00: Version (4B), Type (4B), Size (8B), AdapterId (16B) = 32 bytes header
     *   +0x20: Attach, +0x28: Detach, +0x30: ClearContext
     *   +0x38: QueryStatus, +0x40: Reset
     *   +0x48: SetMode, +0x50: SetIndicatorStatus, +0x58: GetIndicatorStatus
     *   +0x60: StartCapture, +0x68: FinishCapture
     */
    void **adapter = (void**)*p_adapter;
    void *old_qs = *(void**)((uint8_t*)adapter + 0x38);

    *(void**)((uint8_t*)adapter + 0x38) = (void*)patched_query_status;

    n = snprintf(buf, sizeof(buf),
        "[ADAPTER] Patched QueryStatus: %p → %p (always READY)\n",
        old_qs, (void*)patched_query_status);
    write(2, buf, n);
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

    /* VFM init will happen on the main thread (via IOCTL interceptor)
       to ensure thread ownership is correct for the VFM session.
       The timer thread just does WBFUsbInitialize. */
    write(2, "[WBF] WBF init done. VFM init will trigger on first IOCTL.\n", 59);
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
    p_libusb_dev = (void**)dlsym(RTLD_DEFAULT, "tudor_com_usb_dev");
    real_libusb_control_transfer = dlsym(RTLD_DEFAULT, "libusb_control_transfer");

    /*
     * Patch the USB target vtable IMMEDIATELY — the driver caches function
     * pointers from the vtable during OnDeviceAdd/WBFUsbInitialize, so we
     * must patch BEFORE tudor_init() runs.
     */
    if(p_usb_obj) {
        patch_usb_target_vtable();
    }

    /* Patch the sensor adapter's QueryStatus AFTER tudor_init() queries the
       interfaces but BEFORE tudor_open() initializes the pipeline.
       We use a timer with shorter delay to patch between init and open. */

    pthread_t t;
    pthread_create(&t, NULL, timer_thread, NULL);
    pthread_detach(t);
}
