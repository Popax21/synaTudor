#include "record.h"

#define RECORD_VARIANT_TYPE "(@ayy@ay)"

bool peek_record_guid_finger(GVariant *var, RECGUID *guid, enum tudor_finger *finger) {
    if(!g_variant_check_format_string(var, RECORD_VARIANT_TYPE, FALSE)) return false;

    //Parse the top level variant
    *finger = 0;
    GVariant *guid_var;
    g_variant_get(var, RECORD_VARIANT_TYPE, NULL, finger, &guid_var);

    //Parse the GUID variant
    size_t var_guid_sz;
    const RECGUID *var_guid = (const RECGUID*) g_variant_get_fixed_array(guid_var, &var_guid_sz, 1);
    if(var_guid_sz == sizeof(RECGUID)) *guid = *var_guid;

    g_variant_unref(guid_var);
    return var_guid_sz == sizeof(RECGUID);
}