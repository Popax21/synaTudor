#include <stdio.h>
#include <strings.h>
#include "internal.h"

const struct tudor_pair_data *(*tudor_get_pdata_fnc)(const char *name);
void (*tudor_set_pdata_fnc)(const char *name, const struct tudor_pair_data *pdata);

bool tudor_machine_guid(char16_t out[37]) {
    static char guid[37];
    if(!guid[0]) {
        FILE *file = fopen("/etc/machine-id", "r");
        char id[33] = {0};
        bool ok = file && fread(id, 1, 32, file) == 32;
        if(file) fclose(file);
        if(!ok) {
            log_warn("Couldn't read /etc/machine-id for MachineGuid");
            return false;
        }
        snprintf(guid, sizeof(guid), "%.8s-%.4s-%.4s-%.4s-%.12s", id, id + 8, id + 12, id + 16, id + 20);
    }

    for(int i = 0; i < 37; i++) out[i] = guid[i];
    return true;
}

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

    //Stable host identity - the driver uses it when wrapping pairing data, so it
    //must not change between runs. Derived from /etc/machine-id.
    if(!is_write && tudor_windrv->machine_guid && strcasecmp(key_name, "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography") == 0 && strcasecmp(val_name, "MachineGuid") == 0) {
        char16_t guid[37];
        if(!tudor_machine_guid(guid)) return false;

        if(buf && *buf_size >= sizeof(guid)) memcpy(buf, guid, sizeof(guid));
        else if(buf) return false;
        *buf_size = sizeof(guid);
        *val_type = WINREG_STR;
        return true;
    }

    //Handle device state key
    if(strcmp(key_name, "HKEY_LOCAL_MACHINE\\Tudor\\Device") == 0 && ctx_obj) {
        struct tudor_device *dev = (struct tudor_device*) ctx_obj;

        if(strcasecmp(val_name, "PairingInProcess") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.pairing_in_process;
                else dev->state.pairing_in_process = *((int*) buf) != 0;
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcasecmp(val_name, "UnairingInProcess") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.unpairing_in_process;
                else dev->state.unpairing_in_process = *((int*) buf) != 0;
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcasecmp(val_name, "DeviceUpdateInProcess") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.update_in_process;
                else dev->state.update_in_process = *((int*) buf) != 0;
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcasecmp(val_name, "deviceInitializeFailures") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.init_fails;
                else dev->state.init_fails = *((int*) buf);
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcasecmp(val_name, "updateFirmwareFailureCount") == 0) {
            if(buf && *buf_size >= 4) {
                if(!is_write) *((int*) buf) = dev->state.update_fails;
                else dev->state.update_fails = *((int*) buf);
            } else if(is_write || buf) return false;
            *buf_size = 4;
            if(!is_write) *val_type = WINREG_DWORD;
            return true;
        } else if(strcasecmp(val_name, "LastUpdateSystemTimeStamp") == 0) {
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
