#include "internal.h"

const struct tudor_pair_data *(*tudor_get_pdata_fnc)(const char *name);
void (*tudor_set_pdata_fnc)(const char *name, const struct tudor_pair_data *pdata);

bool tudor_reg_handler(void *ctx, void *ctx_obj, const char *key_name, const char *val_name, bool is_write, void *buf, size_t *buf_size, enum winreg_val_type *val_type) {
    if(!buf_size) return false;

    //Handle the driver configuration key
    log_info("REG: %s '%s\\%s' %s (buf_size=%zu)", is_write ? "WRITE" : "READ", key_name, val_name, is_write ? "" : "→", *buf_size);
    if(!is_write && strcmp(key_name, "HKEY_LOCAL_MACHINE\\SOFTWARE\\Syna") == 0) {
        if(strcmp(val_name, "wbfMode") == 0) {
            if(buf && *buf_size >= 4) {
                *((uint32_t*) buf) = TRUE;
            } else if(buf) return false;
            *buf_size = 4;
            *val_type = WINREG_DWORD;
            return true;
        }
        if(strcmp(val_name, "useWbf") == 0) {
            if(buf && *buf_size >= 4) {
                *((uint32_t*) buf) = FALSE;  /* Force WinUSB mode, not WBF */
            } else if(buf) return false;
            *buf_size = 4;
            *val_type = WINREG_DWORD;
            return true;
        }
        return false;
    }

    //Handle device state key
    if(strcmp(key_name, "HKEY_LOCAL_MACHINE\\Tudor\\Device") == 0 && ctx_obj) {
        struct tudor_device *dev = (struct tudor_device*) ctx_obj;

        /* Helper: handle a DWORD registry value with proper size-query support.
           For reads: if buffer too small, just return the required size (success).
           For writes: if buf_size=0, treat as "set to 0"; if buf too small, fail. */
        #define REG_DWORD_VAR(name, var) \
            if(strcasecmp(val_name, name) == 0) { \
                if(is_write) { \
                    if(buf && *buf_size >= 4) var = *((int*) buf); \
                    else var = 0; /* write with no data = clear */ \
                } else { \
                    if(buf && *buf_size >= 4) *((int*) buf) = var; \
                    /* else: size query — just report the required size */ \
                } \
                *buf_size = 4; \
                if(!is_write) *val_type = WINREG_DWORD; \
                return true; \
            }

        REG_DWORD_VAR("PairingInProcess", dev->state.pairing_in_process)
        REG_DWORD_VAR("UnairingInProcess", dev->state.unpairing_in_process)
        REG_DWORD_VAR("DeviceUpdateInProcess", dev->state.update_in_process)
        REG_DWORD_VAR("deviceInitializeFailures", dev->state.init_fails)
        REG_DWORD_VAR("updateFirmwareFailureCount", dev->state.update_fails)
        if(strcasecmp(val_name, "LastUpdateSystemTimeStamp") == 0) {
            if(is_write) {
                if(buf && *buf_size >= 4) dev->state.last_update_timestamp = *((uint32_t*) buf);
                else dev->state.last_update_timestamp = 0;
            } else {
                if(buf && *buf_size >= 4) *((uint32_t*) buf) = dev->state.last_update_timestamp;
            }
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        }
        /* DLL uses both cases for some keys */
        REG_DWORD_VAR("SetOwnershipFailureCount", dev->state.init_fails)
        REG_DWORD_VAR("SensorLockFailureCount", dev->state.init_fails)

        #undef REG_DWORD_VAR
        return false;
    }

    //Handle pair data store key
    if(strcmp(key_name, "HKEY_CURRENT_USER\\Software\\Synaptics\\PairingData") == 0) {
        //Call callback
        if(!is_write) {
            if(tudor_get_pdata_fnc) {
                const struct tudor_pair_data *pdata = tudor_get_pdata_fnc(val_name);
                if(!pdata) return false;

                if(buf && *buf_size >= pdata->data_size) {
                    memcpy(buf, pdata->data, pdata->data_size);
                } else if(buf) return false;
                *buf_size = pdata->data_size;
                *val_type = WINREG_BINARY;
            }
        } else {
            if(tudor_set_pdata_fnc) {
                tudor_set_pdata_fnc(val_name, &(struct tudor_pair_data) {
                    .data = buf,
                    .data_size = *buf_size
                });
                return true;
            }
        }

        return false;
    }

    return false;
}
