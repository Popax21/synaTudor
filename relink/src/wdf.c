#include "wdf.h"
#include "stub.h"

WUDF_LOADER_FX_INTERFACE wdf_loader;
void *wdf_functions[NUM_WDF_FUNCS];
WDF_DRIVER_GLOBALS wdf_globals;

#ifndef DBGWDF
static void wdf_func_stub() {
    log_error("Unresolved WDF function called!");
    abort();
}
#endif

__winfnc static NTSTATUS wdf_bind_version(void *ctx, WDF_BIND_INFO *bind_info, void **comp_globals) {
    char *ccomp = winstr_to_str(bind_info->Component);
    log_debug("Binding WDF function table '%s' version %u.%u#%u... [%u functions]", ccomp, bind_info->Version.Major, bind_info->Version.Minor, bind_info->Version.Build, bind_info->FuncCount);
    free(ccomp);

    //Initialize function table
    for(int i = 0; i < NUM_WDF_FUNCS; i++) {
        if(!wdf_functions[i]) {
#ifdef DBGWDF
            wdf_functions[i] = create_wdf_stub(i);
#else
            wdf_functions[i] = wdf_func_stub;
#endif
        }
    }

    //Bind functions
    *bind_info->FuncTable = wdf_functions;
    *comp_globals = &wdf_globals;

    return 0;
}

void init_winwdf() {
    //Initialize WDF loader
    wdf_loader.Size = sizeof(wdf_loader);
    wdf_loader.VersionBind = wdf_bind_version;

    //Initialize globals
    wdf_globals.Driver = NULL;
    wdf_globals.DriverFlags = 0;
    wdf_globals.DriverTag = 0;
    strcpy(wdf_globals.DriverName, "TUDOR-DRIVER");
    wdf_globals.DisplaceDriverUnload = TRUE;
}