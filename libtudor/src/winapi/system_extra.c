#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "internal.h"

//Power, PnP configuration manager, firmware table and COM allocator APIs used
//by the Dell (v103) driver. None of the underlying OS facilities exist in the
//sandbox, so each reports "nothing there" and logs, so that a driver path
//which really depends on one shows up in the log.

#define CR_SUCCESS 0x00
#define CR_NO_SUCH_DEVNODE 0x0D
#define CR_NO_SUCH_VALUE 0x25

__winfnc DWORD CM_Locate_DevNodeW(DWORD *devinst, const char16_t *dev_id, ULONG flags) {
    char *id = dev_id ? winstr_to_str(dev_id) : NULL;
    log_debug("CM_Locate_DevNodeW | id '%s' flags 0x%x -> CR_NO_SUCH_DEVNODE", id ? id : "(root)", flags);
    free(id);
    return CR_NO_SUCH_DEVNODE;
}
WINAPI(CM_Locate_DevNodeW)

__winfnc DWORD CM_Get_Device_Interface_List_SizeA(ULONG *len, const GUID *iface_guid, const char *dev_id, ULONG flags) {
    log_debug("CM_Get_Device_Interface_List_SizeA | flags 0x%x -> empty list", flags);
    *len = 1;
    return CR_SUCCESS;
}
WINAPI(CM_Get_Device_Interface_List_SizeA)

__winfnc DWORD CM_Get_Device_Interface_ListA(const GUID *iface_guid, const char *dev_id, char *buf, ULONG buf_len, ULONG flags) {
    log_debug("CM_Get_Device_Interface_ListA | flags 0x%x -> empty list", flags);
    if(buf_len < 1) return 0x1A; /* CR_BUFFER_SMALL */
    buf[0] = 0;
    return CR_SUCCESS;
}
WINAPI(CM_Get_Device_Interface_ListA)

__winfnc DWORD CM_Get_Device_Interface_PropertyW(const char16_t *iface, const void *prop_key, ULONG *prop_type, BYTE *buf, ULONG *buf_size, ULONG flags) {
    log_debug("CM_Get_Device_Interface_PropertyW -> CR_NO_SUCH_VALUE");
    return CR_NO_SUCH_VALUE;
}
WINAPI(CM_Get_Device_Interface_PropertyW)

__winfnc DWORD CM_Query_And_Remove_SubTreeA(DWORD devinst, int *veto_type, char *veto_name, ULONG veto_name_len, ULONG flags) {
    log_warn("CM_Query_And_Remove_SubTreeA called (devinst 0x%x flags 0x%x) - ignoring", devinst, flags);
    return CR_SUCCESS;
}
WINAPI(CM_Query_And_Remove_SubTreeA)

__winfnc DWORD CM_Setup_DevNode(DWORD devinst, ULONG flags) {
    log_warn("CM_Setup_DevNode called (devinst 0x%x flags 0x%x) - ignoring", devinst, flags);
    return CR_SUCCESS;
}
WINAPI(CM_Setup_DevNode)

__winfnc DWORD PowerSettingRegisterNotification(const GUID *setting, DWORD flags, void *recipient, HANDLE *out) {
    log_debug("PowerSettingRegisterNotification | flags 0x%x", flags);
    *out = (HANDLE) (uintptr_t) 0x504f574e; /* POWN - never dereferenced by the driver */
    return ERROR_SUCCESS;
}
WINAPI(PowerSettingRegisterNotification)

__winfnc DWORD PowerSettingUnregisterNotification(HANDLE handle) {
    return ERROR_SUCCESS;
}
WINAPI(PowerSettingUnregisterNotification)

__winfnc NTSTATUS CallNtPowerInformation(int level, void *in, ULONG in_len, void *out, ULONG out_len) {
    log_debug("CallNtPowerInformation | level %d out_len %u -> zeroed", level, out_len);
    if(out) memset(out, 0, out_len);
    return STATUS_SUCCESS;
}
WINAPI(CallNtPowerInformation)

//The driver derives a host identity from the SMBIOS system information
//(product name + serial number) and won't finish initializing without it.
//The real table is root-only, so build a small one from the world-readable
//fields in /sys/class/dmi/id. The serial number is root-only too; a stable
//stand-in is derived from /etc/machine-id - it only has to stay the same
//between runs.

#define SMBIOS_PROVIDER_RSMB 0x52534D42

static size_t read_dmi_field(const char *field, char *out, size_t out_size) {
    char path[128];
    snprintf(path, sizeof(path), "/sys/class/dmi/id/%s", field);
    FILE *file = fopen(path, "r");
    if(!file) { out[0] = 0; return 0; }
    if(!fgets(out, out_size, file)) out[0] = 0;
    fclose(file);
    out[strcspn(out, "\n")] = 0;
    return strlen(out);
}

static void read_stand_in_serial(char *out, size_t out_size) {
    if(read_dmi_field("product_serial", out, out_size)) return;

    char id[33] = {0};
    FILE *file = fopen("/etc/machine-id", "r");
    if(file) {
        if(fread(id, 1, 32, file) != 32) id[0] = 0;
        fclose(file);
    }
    snprintf(out, out_size, "LNX%.7s", id[0] ? id : "0000000");
    for(char *c = out; *c; c++) if(*c >= 'a' && *c <= 'z') *c -= 'a' - 'A';
}

struct smbios_builder {
    uint8_t buf[16384];
    size_t len;
};

//Appends one structure: its formatted area, then its string set. String
//fields in the formatted area hold 1-based indices; empty strings get index 0.
static void smbios_add(struct smbios_builder *b, const uint8_t *fmt, size_t fmt_len, const int *str_fields, const char **strs, int num_strs) {
    uint8_t *start = b->buf + b->len;
    memcpy(start, fmt, fmt_len);
    b->len += fmt_len;

    int idx = 0;
    for(int i = 0; i < num_strs; i++) {
        if(!strs[i] || !strs[i][0]) { start[str_fields[i]] = 0; continue; }
        start[str_fields[i]] = ++idx;
        size_t slen = strlen(strs[i]) + 1;
        memcpy(b->buf + b->len, strs[i], slen);
        b->len += slen;
    }
    if(!idx) b->buf[b->len++] = 0;
    b->buf[b->len++] = 0;
}

static size_t build_smbios(struct smbios_builder *b) {
    char vendor[64], product[64], version[64], serial[64], sku[64], family[64];
    char board_vendor[64], board_name[64], board_version[64];
    char bios_vendor[64], bios_version[64], bios_date[64];
    read_dmi_field("sys_vendor", vendor, sizeof(vendor));
    read_dmi_field("product_name", product, sizeof(product));
    read_dmi_field("product_version", version, sizeof(version));
    read_dmi_field("product_sku", sku, sizeof(sku));
    read_dmi_field("product_family", family, sizeof(family));
    read_dmi_field("board_vendor", board_vendor, sizeof(board_vendor));
    read_dmi_field("board_name", board_name, sizeof(board_name));
    read_dmi_field("board_version", board_version, sizeof(board_version));
    read_dmi_field("bios_vendor", bios_vendor, sizeof(bios_vendor));
    read_dmi_field("bios_version", bios_version, sizeof(bios_version));
    read_dmi_field("bios_date", bios_date, sizeof(bios_date));
    read_stand_in_serial(serial, sizeof(serial));

    b->len = 0;

    //Type 0: BIOS information
    const uint8_t bios[0x18] = { 0, 0x18, 0x00, 0x00, 0, 0, 0x00, 0xF0, 0, 0xFF, 0x08, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF };
    smbios_add(b, bios, sizeof(bios), (const int[]) { 4, 5, 8 }, (const char*[]) { bios_vendor, bios_version, bios_date }, 3);

    //Type 1: system information (UUID left zero, wake-up type "power switch")
    uint8_t sys[0x1B] = { 1, 0x1B, 0x01, 0x00 };
    sys[0x18] = 6;
    smbios_add(b, sys, sizeof(sys), (const int[]) { 4, 5, 6, 7, 0x19, 0x1A }, (const char*[]) { vendor, product, version, serial, sku, family }, 6);

    //Type 2: baseboard information
    const uint8_t board[0x08] = { 2, 0x08, 0x02, 0x00 };
    smbios_add(b, board, sizeof(board), (const int[]) { 4, 5, 6 }, (const char*[]) { board_vendor, board_name, board_version }, 3);

    //Type 127: end of table
    const uint8_t end[0x04] = { 127, 0x04, 0x00, 0x7F };
    smbios_add(b, end, sizeof(end), NULL, NULL, 0);

    log_debug("Built SMBIOS table [%zu bytes] for '%s %s' serial '%s'", b->len, vendor, product, serial);
    return b->len;
}

static struct smbios_builder table;
static size_t table_len;

static bool read_smbios_file(const char *path, bool warn) {
    FILE *file = fopen(path, "rb");
    if(!file) {
        if(warn) log_warn("Couldn't open SMBIOS table '%s'", path);
        return false;
    }
    table_len = fread(table.buf, 1, sizeof(table.buf), file);
    if(!feof(file)) {
        log_warn("SMBIOS table '%s' is larger than %zu bytes", path, sizeof(table.buf));
        table_len = 0;
    }
    fclose(file);
    if(table_len) log_debug("Using SMBIOS table '%s' [%zu bytes]", path, table_len);
    return table_len > 0;
}

//The host identity derived from SMBIOS must stay the same between runs, or
//the sensor has to be re-paired. Prefer the real table: a dump named by
//TUDOR_SMBIOS_TABLE, else /sys/firmware/dmi/tables/DMI (root-only), else a
//table synthesized from /sys/class/dmi/id. Sandboxed hosts call this (via
//tudor_preload_host_info) before they lose file access.
void winapi_load_smbios_table() {
    if(table_len) return;

    const char *path = getenv("TUDOR_SMBIOS_TABLE");
    if(path && read_smbios_file(path, true)) return;
    if(read_smbios_file("/sys/firmware/dmi/tables/DMI", false)) return;

    log_warn("Real SMBIOS table not available - synthesizing one (host identity will differ from runs that had it)");
    table_len = build_smbios(&table);
}

__winfnc UINT GetSystemFirmwareTable(DWORD provider, DWORD table_id, void *buf, DWORD buf_size) {
    if(provider != SMBIOS_PROVIDER_RSMB) {
        log_warn("GetSystemFirmwareTable called (provider 0x%08x table 0x%x) - not available", provider, table_id);
        winerr_set_code(50); /* ERROR_NOT_SUPPORTED */
        return 0;
    }

    winapi_load_smbios_table();
    UINT total = 8 + table_len;
    if(!buf || buf_size < total) return total;

    uint8_t *out = (uint8_t*) buf;
    out[0] = 0;     //Used20CallingMethod
    out[1] = 3;     //SMBIOSMajorVersion
    out[2] = 2;     //SMBIOSMinorVersion
    out[3] = 0;     //DmiRevision
    uint32_t len32 = table_len;
    memcpy(out + 4, &len32, 4);
    memcpy(out + 8, table.buf, table_len);
    return total;
}
WINAPI(GetSystemFirmwareTable)

__winfnc void *CoTaskMemAlloc(SIZE_T size) { return malloc(size); }
WINAPI(CoTaskMemAlloc)

__winfnc void CoTaskMemFree(void *mem) { free(mem); }
WINAPI(CoTaskMemFree)
