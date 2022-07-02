#include "internal.h"

#define HKEY_CLASSES_ROOT 0x80000000
#define HKEY_CURRENT_USER 0x80000001
#define HKEY_LOCAL_MACHINE 0x80000002
#define HKEY_USERS 0x80000003
#define HKEY_PERFORMANCE_DATA 0x80000004
#define HKEY_PERFORMANCE_TEXT 0x80000050
#define HKEY_PERFORMANCE_NLSTEXT 0x80000060
#define HKEY_CURRENT_CONFIG 0x80000005
#define HKEY_DYN_DATA 0x80000006
#define HKEY_CURRENT_USER_LOCAL_SETTINGS 0x80000007

static winreg_handler_fnc *reg_handler;
static void *reg_context;

void winreg_set_handler(winreg_handler_fnc *handler, void *ctx) {
    reg_handler = handler;
    reg_context = ctx;
}

struct reg_key {
    void *ctx_obj;
    char *name;
};

static void key_destr(struct reg_key *key) {
    free(key->name);
    free(key);
}

static const char *key_name(HANDLE hkey) {
    switch((uint32_t) (uintptr_t) hkey) {
        case HKEY_CLASSES_ROOT: return "HKEY_CLASSES_ROOT";
        case HKEY_CURRENT_USER: return "HKEY_CURRENT_USER";
        case HKEY_LOCAL_MACHINE: return "HKEY_LOCAL_MACHINE";
        case HKEY_USERS: return "HKEY_USERS";
        case HKEY_PERFORMANCE_DATA: return "HKEY_PERFORMANCE_DATA";
        case HKEY_PERFORMANCE_TEXT: return "HKEY_PERFORMANCE_TEXT";
        case HKEY_PERFORMANCE_NLSTEXT: return "HKEY_PERFORMANCE_NLSTEXT";
        case HKEY_CURRENT_CONFIG: return "HKEY_CURRENT_CONFIG";
        case HKEY_DYN_DATA: return "HKEY_DYN_DATA";
        case HKEY_CURRENT_USER_LOCAL_SETTINGS: return "HKEY_CURRENT_USER_LOCAL_SETTINGS";
        default: return ((struct reg_key*) hkey->data)->name;
    }
}

HANDLE winreg_open_key(void *ctx_obj, const char *key_name) {
    //Allocate key
    struct reg_key *key = (struct reg_key*) malloc(sizeof(struct reg_key));
    if(!key) { perror("Couldn't allocate registry key memory"); abort(); }

    key->ctx_obj = ctx_obj;
    key->name = strdup(key_name);

    return winhandle_create(key, (winhandle_destr_fnc*) key_destr);
}

const char *winreg_get_key_name(HANDLE hkey) {
    struct reg_key *key = (struct reg_key*) hkey->data;
    return key->name;
}

bool winreg_query_val(HANDLE hkey, const char *val_name, void *buf, size_t *buf_size, enum winreg_val_type *val_type) {
    struct reg_key *key = (struct reg_key*) hkey->data;
    if(!reg_handler) return false;

    //Forward to registry handler
    bool suc = reg_handler(reg_context, key->ctx_obj, key->name, val_name, false, buf, buf_size, val_type);
    log_verbose("REGREAD  | ctx %18p key '%s' value '%s' val type %d buf size %lu suc %d", key->ctx_obj, key->name, val_name, *val_type, *buf_size, suc);
    return suc;
}

bool winreg_write_val(HANDLE hkey, const char *val_name, const void *buf, size_t buf_size, enum winreg_val_type val_type) {
    struct reg_key *key = (struct reg_key*) hkey->data;
    if(!reg_handler) return false;

    //Forward to registry handler
    bool suc = reg_handler(reg_context, key->ctx_obj, key->name, val_name, true, (void*) buf, &buf_size, &val_type);
    log_verbose("REGWRITE | ctx %18p key '%s' value '%s' val type %d buf size %lu suc %d", key->ctx_obj, key->name, val_name, val_type, buf_size, suc);
    return suc;
}

__winfnc LONG RegOpenKeyExA(HANDLE hkey, const char *subkey, DWORD opts, DWORD sam, HANDLE *out) {
    if(!subkey) {
        *out = hkey;
        return ERROR_SUCCESS;
    }

    //Determine key name
    const char *par_name = key_name(hkey);

    int par_len = strlen(par_name), subkey_len = strlen(subkey);
    char *name = (char*) malloc(par_len + 1 + subkey_len + 1);
    strcpy(name, par_name);
    name[par_len] = '\\';
    strcpy(name + par_len + 1, subkey);

    *out = winreg_open_key(NULL, name);
    free(name);
    return ERROR_SUCCESS;
}
WINAPI(RegOpenKeyExA)

__winfnc LONG RegOpenKeyExW(HANDLE hkey, const char16_t *subkey, DWORD opts, DWORD sam, HANDLE *out) {
    if(!subkey) {
        *out = hkey;
        return ERROR_SUCCESS;
    }

    //Determine key name
    const char *par_name = key_name(hkey);
    char *subkey_name = winstr_to_str(subkey);

    int par_len = strlen(par_name), subkey_len = strlen(subkey_name);
    char *name = (char*) malloc(par_len + 1 + subkey_len + 1);
    strcpy(name, par_name);
    name[par_len] = '\\';
    strcpy(name + par_len + 1, subkey_name);

    free(subkey_name);

    *out = winreg_open_key(NULL, name);
    free(name);
    return ERROR_SUCCESS;
}
WINAPI(RegOpenKeyExW)

__winfnc LONG RegCreateKeyExA(HANDLE hkey, const char *subkey, DWORD reserved, const char *class, DWORD opts, DWORD sam, void *sec_attrs, HANDLE *out, DWORD *dispos) {
    if(dispos) *dispos = 0x2; //REG_OPENED_EXISTING_KEY
    return RegOpenKeyExA(hkey, subkey, opts, sam, out);
}
WINAPI(RegCreateKeyExA)

__winfnc LONG RegCreateKeyExW(HANDLE hkey, const char16_t *subkey, DWORD reserved, const char16_t *class, DWORD opts, DWORD sam, void *sec_attrs, HANDLE *out, DWORD *dispos) {
    if(dispos) *dispos = 0x2; //REG_OPENED_EXISTING_KEY
    return RegOpenKeyExW(hkey, subkey, opts, sam, out);
}
WINAPI(RegCreateKeyExW)

__winfnc LONG RegCloseKey(HANDLE hkey) {
    winhandle_destroy(hkey);
    return ERROR_SUCCESS;
}
WINAPI(RegCloseKey)

__winfnc LONG RegQueryValueExA(HANDLE hkey, const char *val_name, DWORD *resv, DWORD *type, BYTE *data, DWORD *data_size) {
    //Forward to registry handler
    size_t data_sz = (size_t) *data_size;

    enum winreg_val_type out_type;
    bool suc;
    if(data) suc = winreg_query_val(hkey, val_name, data, &data_sz, &out_type);
    else suc = winreg_query_val(hkey, val_name, NULL, &data_sz, &out_type);

    *data_size = (DWORD) data_sz;

    return suc ? ERROR_SUCCESS : WINERR_SET_CODE;
}
WINAPI(RegQueryValueExA)

__winfnc LONG RegQueryValueExW(HANDLE hkey, const char16_t *val_name, DWORD *resv, DWORD *type, BYTE *data, DWORD *data_size) {
    //Forward to registry handler
    char *val_cname = winstr_to_str(val_name);
    size_t data_sz = (size_t) *data_size;

    enum winreg_val_type out_type;
    bool suc;
    if(data) suc = winreg_query_val(hkey, val_cname, data, &data_sz, &out_type);
    else suc = winreg_query_val(hkey, val_cname, NULL, &data_sz, &out_type);

    *data_size = (DWORD) data_sz;
    free(val_cname);

    return suc ? ERROR_SUCCESS : WINERR_SET_CODE;
}
WINAPI(RegQueryValueExW)

__winfnc LONG RegSetValueExA(HANDLE hkey, const char *val_name, DWORD resv, DWORD type, const BYTE *data, DWORD data_size) {
    //Forward to registry handler
    bool suc = winreg_write_val(hkey, val_name, data, data_size, (enum winreg_val_type) type);
    return suc ? ERROR_SUCCESS : WINERR_SET_CODE;
}
WINAPI(RegSetValueExA)

__winfnc LONG RegSetValueExW(HANDLE hkey, const char16_t *val_name, DWORD resv, DWORD type, const BYTE *data, DWORD data_size) {
    //Forward to registry handler
    char *val_cname = winstr_to_str(val_name);
    bool suc = winreg_write_val(hkey, val_cname, data, data_size, (enum winreg_val_type) type);
    free(val_cname);
    return suc ? ERROR_SUCCESS : WINERR_SET_CODE;
}
WINAPI(RegSetValueExW)