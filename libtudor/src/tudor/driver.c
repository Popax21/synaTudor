#include <sys/mman.h>
#include "internal.h"
#include "winapi/com/com.h"

bool tudor_using_com_path = false;
libusb_device_handle *tudor_com_usb_dev = NULL;

void tudor_set_com_usb_device(libusb_device_handle *dev) {
    tudor_com_usb_dev = dev;
}

extern uint8_t _binary_libtudor_synaFpAdapter132_dll_start, _binary_libtudor_synaFpAdapter132_dll_end;
extern uint8_t _binary_libtudor_synaWudfBioUsb132_dll_start, _binary_libtudor_synaWudfBioUsb132_dll_end;

#define NUM_WINDRV_DLLS 2
struct windrv_dll tudor_windrv_dlls[] = {
    {
        .module = {
            .name = "synaFpAdapter132.dll",
            .cmdline = "synaFpAdapter132.dll",
            .environ = (const char*[]) { NULL }
        },
        .pe_image = &_binary_libtudor_synaFpAdapter132_dll_start, .pe_image_end = &_binary_libtudor_synaFpAdapter132_dll_end,
        .is_adapter = true, .is_driver = false
    },
    {
        .module = {
            .name = "synaWudfBioUsb132.dll",
            .cmdline = "synaWudfBioUsb132.dll",
            .environ = (const char*[]) { NULL }
        },
        .pe_image = &_binary_libtudor_synaWudfBioUsb132_dll_start, .pe_image_end = &_binary_libtudor_synaWudfBioUsb132_dll_end,
        .is_adapter = false, .is_driver = true
    }
};

bool tudor_log_traces;

static struct winmodule ntdll_module = {
    .name = "ntdll.dll",
    .cmdline = "ntdll.dll",
    .environ = (const char*[]) { NULL }
};

#define DLL_PROCESS_ATTACH 1
#define DLL_PROCESS_DETACH 0
#define DLL_THREAD_ATTACH 2
#define DLL_THREAD_DETACH 3
typedef BOOL __winfnc (*api_DllMain)(HANDLE hinstDLL, int fdwReason, void *lpReserved);

struct windrv_dll *tudor_adapter_dll, *tudor_driver_dll;
void *_tudor_biodev_ctx = NULL;  /* CBiometricDevice context for runtime probing */
WINBIO_SENSOR_INTERFACE *tudor_sensor_adapter;
WINBIO_ENGINE_INTERFACE *tudor_engine_adapter;
WINBIO_STORAGE_INTERFACE *tudor_native_storage_adapter;

static void patch_v132_adapter_capture_buffer_size(void) {
    uint8_t *img = tudor_adapter_dll->image.base_addr;
    const int patch_rva = 0x6a54;

    if(!img || tudor_adapter_dll->image.image_size <= patch_rva + 3) return;

    if(img[patch_rva - 1] != 0xb9 || img[patch_rva] != 0x70 || img[patch_rva + 1] != 0x00 ||
            img[patch_rva + 2] != 0x00 || img[patch_rva + 3] != 0x00) {
        log_warn("Skipping v132 adapter capture buffer patch; unexpected bytes");
        return;
    }

    img[patch_rva] = 0x18;
    img[patch_rva + 1] = 0xd4;
    img[patch_rva + 2] = 0x01;
    img[patch_rva + 3] = 0x00;
    log_info("Patched v132 adapter capture buffer size");
}

static DRIVER_OBJECT umdf_driver;
struct winwdf_driver *tudor_wdf_driver;

bool tudor_init() {
    //Register dummy modules
    winmodule_register(&ntdll_module);

    if(tudor_log_traces) {
        //Register trace messages
        winlog_register_trace_msg(DEFINE_GUID(58f95b1a, 8efd, 39f0, 5626, 3e620b587295), 0x0c, "%s<X> checkpoint hit <X>");
        winlog_register_trace_msg(DEFINE_GUID(58f95b1a, 8efd, 39f0, 5626, 3e620b587295), 0x0d, "%s<X> checkpoint hit <X>");

        winlog_register_trace_msg(DEFINE_GUID(824d7f8b, e993, 3db5, 6a1a, 91a0d317b75a), 0x0a, "%s-> %s");
        winlog_register_trace_msg(DEFINE_GUID(824d7f8b, e993, 3db5, 6a1a, 91a0d317b75a), 0x0b, "%s<- %s");
        winlog_register_trace_msg(DEFINE_GUID(824d7f8b, e993, 3db5, 6a1a, 91a0d317b75a), 0x0c, "%s<- %s [0x%x]");
        winlog_register_trace_msg(DEFINE_GUID(824d7f8b, e993, 3db5, 6a1a, 91a0d317b75a), 0x0d, "%s-> %s");
        winlog_register_trace_msg(DEFINE_GUID(824d7f8b, e993, 3db5, 6a1a, 91a0d317b75a), 0x0f, "%s<- %s [0x%x]");

        winlog_register_trace_msg(DEFINE_GUID(2c18840b, 2ee0, 377e, f168, 1552bbd307c4), 0x0a, "VFM LOG | %s\033[1A");

        winlog_register_trace_msg(DEFINE_GUID(f4a2a1b6, b0f6, 3bc7, 38ef, 14ee547d9ad1), 0x45, "%s capture data pack");
        winlog_register_trace_msg(DEFINE_GUID(f4a2a1b6, b0f6, 3bc7, 38ef, 14ee547d9ad1), 0x47, "%s skipped completion: capture completion disabled");
        winlog_register_trace_msg(DEFINE_GUID(f4a2a1b6, b0f6, 3bc7, 38ef, 14ee547d9ad1), 0x48, "%s skipped completion: missing output buffer");
        winlog_register_trace_msg(DEFINE_GUID(f4a2a1b6, b0f6, 3bc7, 38ef, 14ee547d9ad1), 0x49, "%s skipped completion: output buffer too small");
    }

    //Set registry handler
    winreg_set_handler(tudor_reg_handler, NULL);

    //Load driver DLLs
    tudor_adapter_dll = tudor_driver_dll = NULL;
    for(int i = 0; i < NUM_WINDRV_DLLS; i++) {
        struct windrv_dll *dll = &tudor_windrv_dlls[i];
        if(!load_dll(&dll->image, dll->module.name, dll->pe_image, dll->pe_image_end - dll->pe_image)) {
            log_error("Error loading driver DLL!");
            return false;
        }
        winmodule_register(&dll->module);
        log_info("Loaded driver DLL '%s' [%ld bytes]", dll->module.name, dll->pe_image_end - dll->pe_image);

        if(dll->is_adapter) tudor_adapter_dll = dll;
        if(dll->is_driver) tudor_driver_dll = dll;
    }
    if(!tudor_adapter_dll) abort();
    if(!tudor_driver_dll) abort();

    //Initialize driver DLLs
    for(int i = 0; i < NUM_WINDRV_DLLS; i++) {
        struct windrv_dll *dll = &tudor_windrv_dlls[i];

        if(dll->image.entry_point) {
            log_info("Initializing driver DLL '%s'...", dll->module.name);
            winmodule_set_cur(&dll->module);
            if(!((api_DllMain) dll->image.entry_point)(dll->module.handle, DLL_PROCESS_ATTACH, NULL)) {
                log_error("Error initializing driver DLL '%s'!", dll->module.name);
                return false;
            }
        }
    }

    //Initialize driver — try UMDF v2 (FxDriverEntryUm) first, fall back to UMDF v1 (COM)
    winmodule_set_cur(&tudor_driver_dll->module);

    bool has_fx_entry = false;
    for(int i = 0; i < tudor_driver_dll->image.num_exports; i++) {
        if(strcmp(tudor_driver_dll->image.exports[i].name, "FxDriverEntryUm") == 0) {
            has_fx_entry = true;
            break;
        }
    }

    if(has_fx_entry) {
        //UMDF v2 path (e.g. v104 DLLs)
        log_info("Using UMDF v2 entry (FxDriverEntryUm)");
        init_winwdf();

        //Pre-load WINUSB.DLL BEFORE FxDriverEntryUm — the driver's EvtDriverDeviceAdd
        //calls PrepareHardware → palWinUsbInitialize → LoadLibrary("WINUSB.DLL").
        //Without this, first init fails with 0x80070259.
        {
            extern __winfnc HANDLE LoadLibraryA(const char *name);
            HANDLE h = LoadLibraryA("WINUSB.DLL");
            log_info("Pre-loaded WINUSB.DLL before FxDriverEntryUm: handle=%p", (void*)h);
        }

        char16_t *reg_path_wstr = winstr_from_str("HKEY_LOCAL_MACHINE\\Tudor\\Driver");
        UNICODE_STRING reg_path = {
            .Length = winstr_len(reg_path_wstr)+1,
            .MaximumLength = winstr_len(reg_path_wstr)+1,
            .Buffer = reg_path_wstr
        };

        NTSTATUS status;
        if((status = ((api_FxDriverEntryUm) find_dll_export(&tudor_driver_dll->image, "FxDriverEntryUm"))(&wdf_loader, NULL, &umdf_driver, &reg_path)) != 0) {
            log_error("Error in UMDF driver entry function: 0x%x!", status);
            return false;
        }

        free(reg_path_wstr);

        if(!(tudor_wdf_driver = winwdf_get_driver(&wdf_globals))) {
            log_error("UMDF entry function didn't create a WDF driver!");
            return false;
        }
    } else {
        //UMDF v1 COM path (e.g. v132 DLLs with DllGetClassObject)
        log_info("Using UMDF v1 entry (COM/DllGetClassObject)");
        tudor_using_com_path = true;
        patch_v132_adapter_capture_buffer_size();
        if(!tudor_com_usb_dev) {
            log_warn("No USB device set for COM path");
        } else {
            com_set_usb_device(tudor_com_usb_dev);
        }
        if(!com_init_driver(&tudor_driver_dll->image)) {
            log_error("COM driver initialization failed!");
            return false;
        }
        /* Runtime binary patches */
        {
            uint8_t *img = tudor_driver_dll->image.base_addr;
            if(img && img[0x929b] == 0x74) { img[0x929b] = 0xEB; log_info("Patched 0x929b"); }
            if(img && img[0x161bb] == 0x49) { img[0x161bb] = 0x41; log_info("Patched 0x161bb"); }
            if(img && img[0xd037] == 0x4c && img[0xd038] == 0x8d && img[0xd039] == 0x49 && img[0xd03a] == 0x08) {
                img[0xd037] = 0x90;
                img[0xd038] = 0x90;
                img[0xd039] = 0x90;
                img[0xd03a] = 0x90;
                log_info("Patched v132 capture packer selector");
            }
            extern int tudor_wbf_done;
            tudor_wbf_done = 1;
        }
        if(!com_finish_init(&tudor_driver_dll->image)) {
            log_error("COM driver hardware initialization failed!");
            return false;
        }
    }

    //Query WINBIO interfaces
    winmodule_set_cur(&tudor_adapter_dll->module);

    HRESULT hres;
    if((hres = ((api_WbioQuerySensorInterface) find_dll_export(&tudor_adapter_dll->image, "WbioQuerySensorInterface"))(&tudor_sensor_adapter)) != 0) {
        log_error("Error querying sensor interface: 0x%x!", hres);
        return false;
    }
    if((hres = ((api_WbioQueryEngineInterface) find_dll_export(&tudor_adapter_dll->image, "WbioQueryEngineInterface"))(&tudor_engine_adapter)) != 0) {
        log_error("Error querying engine interface: 0x%x!", hres);
        return false;
    }
    api_WbioQueryStorageInterface query_storage = (api_WbioQueryStorageInterface) try_find_dll_export(&tudor_adapter_dll->image, "WbioQueryStorageInterface");
    tudor_native_storage_adapter = NULL;
    if(query_storage) {
        if((hres = query_storage(&tudor_native_storage_adapter)) != 0) {
            log_warn("Error querying native storage interface: 0x%x; falling back to host storage", hres);
            tudor_native_storage_adapter = NULL;
        } else {
            log_info("Queried native storage interface version=%u.%u size=0x%lx",
                tudor_native_storage_adapter->Version.MajorVersion,
                tudor_native_storage_adapter->Version.MinorVersion,
                tudor_native_storage_adapter->Size);
        }
    }

    return true;
}

bool tudor_shutdown() {
    //Unload the driver
    winmodule_set_cur(&tudor_driver_dll->module);

    if(tudor_using_com_path) {
        log_debug("Unloading COM driver...");
        com_shutdown_driver();
    } else if(tudor_wdf_driver) {
        log_debug("Unloading WDF driver...");
        winwdf_unload_driver(tudor_wdf_driver);
    }
    
    if(umdf_driver.DriverUnload) {
        log_debug("Unloading UMDF driver...");
        umdf_driver.DriverUnload(&umdf_driver);
    }
    umdf_driver = (DRIVER_OBJECT) {0};

    //Uninitialize driver DLLs
    for(int i = 0; i < NUM_WINDRV_DLLS; i++) {
        struct windrv_dll *dll = &tudor_windrv_dlls[i];

        if(dll->image.entry_point) {
            log_info("Uninitializing driver DLL '%s'...", dll->module.name);
            winmodule_set_cur(&dll->module);
            if(!((api_DllMain) dll->image.entry_point)(dll->module.handle, DLL_PROCESS_DETACH, NULL)) {
                log_error("Error uninitializing driver DLL '%s'!", dll->module.name);
                return false;
            }
        }
        winmodule_unregister(&dll->module);
    }

    //Destroy driver DLLs
    for(int i = 0; i < NUM_WINDRV_DLLS; i++) destroy_dll(&tudor_windrv_dlls[i].image);

    //Unregister dummy modules
    winmodule_unregister(&ntdll_module);

    return true;
}
