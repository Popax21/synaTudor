#include "internal.h"

extern uint8_t _binary_libtudor_synaFpAdapter104_dll_start, _binary_libtudor_synaFpAdapter104_dll_end;
extern uint8_t _binary_libtudor_synaWudfBioUsb104_dll_start, _binary_libtudor_synaWudfBioUsb104_dll_end;
extern uint8_t _binary_libtudor_synaFpAdapter103_dll_start, _binary_libtudor_synaFpAdapter103_dll_end;
extern uint8_t _binary_libtudor_synaWudfBioUsb103_dll_start, _binary_libtudor_synaWudfBioUsb103_dll_end;

#define WINDRV_DLL(dll_name, adapter) { \
    .module = { \
        .name = #dll_name ".dll", \
        .cmdline = #dll_name ".dll", \
        .environ = (const char*[]) { NULL } \
    }, \
    .pe_image = &_binary_libtudor_##dll_name##_dll_start, .pe_image_end = &_binary_libtudor_##dll_name##_dll_end, \
    .is_adapter = adapter, .is_driver = !(adapter) \
}

//The Windows driver packages, and the sensors each one is used for
static struct windrv windrvs[] = {
    {
        .name = "Lenovo synaWudfBioUsb104",
        .vid = 0x06cb, .pids = (const uint16_t[]) { 0x00be, 0 },
        .dlls = { WINDRV_DLL(synaFpAdapter104, true), WINDRV_DLL(synaWudfBioUsb104, false) }
    },
    {
        .name = "Dell synaWudfBioUsb103 (Latitude 7200/7210 2-in-1)",
        .vid = 0x06cb, .pids = (const uint16_t[]) { 0x00bc, 0 },
        .dlls = { WINDRV_DLL(synaFpAdapter103, true), WINDRV_DLL(synaWudfBioUsb103, false) },
        .machine_guid = true,
        .native_storage = true, .database_id = DEFINE_GUID(5B783138, D12A, 4BA5, AD3D, 465EACA1D030),
        .usb_serial_id = true
    }
};
#define NUM_WINDRVS (sizeof(windrvs) / sizeof(windrvs[0]))

struct windrv *tudor_windrv;

static struct windrv *find_windrv(uint16_t vid, uint16_t pid) {
    for(int i = 0; i < NUM_WINDRVS; i++) {
        if(windrvs[i].vid != vid) continue;
        for(const uint16_t *p = windrvs[i].pids; *p; p++) {
            if(*p == pid) return &windrvs[i];
        }
    }
    return NULL;
}

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
WINBIO_SENSOR_INTERFACE *tudor_sensor_adapter;
WINBIO_ENGINE_INTERFACE *tudor_engine_adapter;
WINBIO_STORAGE_INTERFACE *tudor_native_storage_adapter;

static DRIVER_OBJECT umdf_driver;
struct winwdf_driver *tudor_wdf_driver;

void tudor_preload_host_info() {
    char16_t guid[37];
    tudor_machine_guid(guid);
    winapi_load_smbios_table();
}

bool tudor_identify_by_usb_serial() {
    return tudor_windrv->usb_serial_id;
}

bool tudor_init(uint16_t vid, uint16_t pid) {
    //Pick the Windows driver for the sensor
    if(!(tudor_windrv = find_windrv(vid, pid))) {
        log_error("No Windows driver for sensor %04x:%04x!", vid, pid);
        return false;
    }
    log_info("Using Windows driver '%s' for sensor %04x:%04x", tudor_windrv->name, vid, pid);

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
    }

    //Set registry handler
    winreg_set_handler(tudor_reg_handler, NULL);

    //Load driver DLLs
    tudor_adapter_dll = tudor_driver_dll = NULL;
    for(int i = 0; i < NUM_WINDRV_DLLS; i++) {
        struct windrv_dll *dll = &tudor_windrv->dlls[i];
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
        struct windrv_dll *dll = &tudor_windrv->dlls[i];

        if(dll->image.entry_point) {
            log_info("Initializing driver DLL '%s'...", dll->module.name);
            winmodule_set_cur(&dll->module);
            if(!((api_DllMain) dll->image.entry_point)(dll->module.handle, DLL_PROCESS_ATTACH, NULL)) {
                log_error("Error initializing driver DLL '%s'!", dll->module.name);
                return false;
            }
        }
    }

    //Call UMDF driver entry function
    init_winwdf();
    winmodule_set_cur(&tudor_driver_dll->module);

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

    //Drivers for match-in-sensor parts keep their templates on the sensor
    tudor_native_storage_adapter = NULL;
    if(tudor_windrv->native_storage) {
        if((hres = ((api_WbioQueryStorageInterface) find_dll_export(&tudor_adapter_dll->image, "WbioQueryStorageInterface"))(&tudor_native_storage_adapter)) != 0) {
            log_error("Error querying vendor storage interface: 0x%x!", hres);
            return false;
        }
        log_info("Using the vendor storage adapter");
    }

    return true;
}

bool tudor_shutdown() {
    //Unload the driver
    winmodule_set_cur(&tudor_driver_dll->module);

    log_debug("Unloading WDF driver...");
    winwdf_unload_driver(tudor_wdf_driver);
    
    if(umdf_driver.DriverUnload) {
        log_debug("Unloading UMDF driver...");
        umdf_driver.DriverUnload(&umdf_driver);
    }
    umdf_driver = (DRIVER_OBJECT) {0};

    //Uninitialize driver DLLs
    for(int i = 0; i < NUM_WINDRV_DLLS; i++) {
        struct windrv_dll *dll = &tudor_windrv->dlls[i];

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
    for(int i = 0; i < NUM_WINDRV_DLLS; i++) destroy_dll(&tudor_windrv->dlls[i].image);

    //Unregister dummy modules
    winmodule_unregister(&ntdll_module);

    return true;
}