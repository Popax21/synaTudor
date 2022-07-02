#ifndef LIBTUDOR_WDF_H
#define LIBTUDOR_WDF_H

#include "winapi/api.h"
#include "winapi/wdf/wdf.h"

extern WUDF_LOADER_FX_INTERFACE wdf_loader;
extern WDF_DRIVER_GLOBALS wdf_globals;

void init_winwdf();

#endif