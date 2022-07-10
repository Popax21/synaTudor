#include "internal.h"

const struct tudor_pair_data *(*tudor_get_pdata_fnc)(const char *name);
void (*tudor_set_pdata_fnc)(const char *name, const struct tudor_pair_data *pdata);

bool tudor_reg_handler(void *ctx, void *ctx_obj, const char *key_name, const char *val_name, bool is_write, void *buf, size_t *buf_size, enum winreg_val_type *val_type) {
    if(!buf_size) return false;

    //Handle the driver configuration key
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
                *((uint32_t*) buf) = TRUE;
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

        if(strcmp(val_name, "PairingInProcess") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.pairing_in_process;
                else dev->state.pairing_in_process = *((int*) buf) != 0;
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcmp(val_name, "UnairingInProcess") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.unpairing_in_process;
                else dev->state.unpairing_in_process = *((int*) buf) != 0;
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcmp(val_name, "DeviceUpdateInProcess") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.update_in_process;
                else dev->state.update_in_process = *((int*) buf) != 0;
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcmp(val_name, "deviceInitializeFailures") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.init_fails;
                else dev->state.init_fails = *((int*) buf);
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcmp(val_name, "updateFirmwareFailureCount") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.update_fails;
                else dev->state.update_fails = *((int*) buf);
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcmp(val_name, "LastUpdateSystemTimeStamp") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((uint32_t*) buf) = dev->state.last_update_timestamp;
                else dev->state.last_update_timestamp = *((uint32_t*) buf);
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        }

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
