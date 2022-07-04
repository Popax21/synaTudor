#ifndef LIBFPRINT_TOD_DATA_H
#define LIBFPRINT_TOD_DATA_H

#include <tudor/tudor.h>
#include "device.h"

struct record {
    RECGUID guid;
    enum tudor_finger finger;
    GBytes *data;
    FpPrint *print;
};

bool get_record_guid_finger(GVariant *var, RECGUID *guid, enum tudor_finger *finger);
bool get_print_guid_finger(FpiDeviceTudor *dev, FpPrint *print, RECGUID *guid, enum tudor_finger *finger);

void fpi_device_tudor_delete(FpDevice *dev);
void fpi_device_clear_storage(FpDevice *dev);

#endif