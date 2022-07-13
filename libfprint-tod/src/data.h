#ifndef LIBFPRINT_TOD_DATA_H
#define LIBFPRINT_TOD_DATA_H

#include <tudor/tudor.h>
#include "device.h"

struct _TudorRecord {
    GObject parent;

    RECGUID guid;
    enum tudor_finger finger;
    GBytes *data;
    FpPrint *print;
};
G_DECLARE_FINAL_TYPE(TudorRecord, tudor_record, TUDOR, RECORD, GObject);

bool load_pdata(FpiDeviceTudor *tdev, GByteArray **pdata, GError **error);
bool store_pdata(FpiDeviceTudor *tdev, GByteArray *pdata, GError **error);

TudorRecord *new_record(RECGUID guid, enum tudor_finger finger, GBytes *data, FpPrint *print);
void load_record(FpiDeviceTudor *tdev, TudorRecord *rec, GAsyncReadyCallback cb, gpointer user_data);
void insert_db_record(FpiDeviceTudor *tdev, TudorRecord *record);

bool parse_print_data(FpiDeviceTudor *tdev, FpPrint *print, RECGUID *guid, enum tudor_finger *finger, GBytes **data);
void set_print_record(FpiDeviceTudor *tdev, FpPrint *print, TudorRecord *record);

void fpi_device_tudor_delete(FpDevice *dev);
void fpi_device_clear_storage(FpDevice *dev);

#endif