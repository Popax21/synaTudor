#ifndef LIBFPRINT_TOD_RECORD_H
#define LIBFPRINT_TOD_RECORD_H

#include <tudor/tudor.h>
#include "drivers_api.h"

struct record {
    RECGUID guid;
    enum tudor_finger finger;
    GBytes *data;
    FpPrint *print;
};

bool peek_record_guid_finger(GVariant *var, RECGUID *guid, enum tudor_finger *finger);

#endif