#ifndef LIBTUDOR_WINAPI_API_H
#define LIBTUDOR_WINAPI_API_H

#include "attr.h"
#include <tudor/log.h>
#include "windows.h"

#define WIN_CLOBBER_NONVOL_REGS __asm__("" ::: "%rbx","%rsi","%rdi");

//APIs
struct __winapi_descr {
    const char *name;
    void *func;
    struct __winapi_descr *next;
};

#define WINAPI(api_name) \
static struct __winapi_descr __winapi_##api_name##_descr = (struct __winapi_descr) {\
    .name = #api_name,\
    .func = &api_name\
};\
__constr static void __register_winapi_##api_name() { __register_windows_api(&__winapi_##api_name##_descr); }
void __register_windows_api(struct __winapi_descr *descr);

void *resolve_windows_api(const char *name);

#define WIN_CALL_CALLBACK(cb, ...) ((cb) ? (cb)(__VA_ARGS__) : 0)

//Strings
int winstr_len(const char16_t *str);
char16_t *winstr_from_str(const char *str);
char *winstr_to_str(const char16_t *wstr);

//Errors
#define WINERR_SET_CODE 0xdeadc0de
void winerr_clear();
void winerr_set();
void winerr_set_code(int code);
void winerr_set_errno();
int winerr_from_errno();

//Handles
typedef void winhandle_destr_fnc(void *data);
struct _HANDLE {
    void *data;
    winhandle_destr_fnc *destr;
};
#define INVALID_HANDLE_VALUE ((HANDLE) (uintptr_t) -1)

HANDLE winhandle_create(void *data, winhandle_destr_fnc *destr);
void winhandle_destroy(HANDLE handle);

//Modules
struct winmodule {
    struct winmodule *prev, *next;
    HANDLE handle;

    const char *name;
    const char *cmdline;
    const char **environ;
};

struct winmodule *winmodule_find(const char *name);
void winmodule_register(struct winmodule *module);
void winmodule_unregister(struct winmodule *module);

struct winmodule *winmodule_get_cur();
void winmodule_set_cur(struct winmodule *module);

//Log
typedef __builtin_ms_va_list win_va_list;
#define win_va_start(list, start) __builtin_ms_va_start(list, start)
#define win_va_end(list) __builtin_ms_va_end(list)
#define win_va_copy(a, b) __builtin_ms_va_copy(a, b)
#define win_va_arg(list, type) __builtin_va_arg(list, type)

void winlog_printf(const char *format, bool ptr_mode, win_va_list vas);

void winlog_register_trace_msg(GUID guid, int num, const char *format);
void winlog_trace(GUID guid, int num, win_va_list vas);

//IO
typedef void winio_overlapped_cb_fnc(OVERLAPPED *ovlp, NTSTATUS status, void *ctx);
void winio_set_overlapped_callback(OVERLAPPED *ovlp, winio_overlapped_cb_fnc *cb, void *ctx, bool new_thread);
void winio_cancel_overlapped(OVERLAPPED *ovlp);
void winio_complete_overlapped(OVERLAPPED *ovlp, NTSTATUS status, size_t num_transfered);
NTSTATUS winio_wait_overlapped(OVERLAPPED *ovlp, size_t *num_transfered);
void winio_cleanup_overlapped(OVERLAPPED *ovlp);

typedef NTSTATUS winio_read_fnc(void *ctx, OVERLAPPED *ovlp, off_t offset, void *buf, size_t buf_size, void **op_ctx);
typedef NTSTATUS winio_write_fnc(void *ctx, OVERLAPPED *ovlp, off_t offset, const void *buf, size_t buf_size, void **op_ctx);
typedef NTSTATUS winio_devctrl_fnc(void *ctx, OVERLAPPED *ovlp, ULONG code, const void *in_buf, size_t in_size, void *out_buf, size_t out_size, void **op_ctx);
typedef NTSTATUS winio_cancel_fnc(void *ctx, OVERLAPPED *ovlp, void *op_ctx);
typedef void winio_cleanup_fnc(void *ctx, OVERLAPPED *ovlp, void *op_ctx);
typedef void winio_destroy_fnc(void *ctx);

HANDLE winio_create_file(void *ctx, bool is_async, winio_read_fnc *read_fnc, winio_write_fnc *write_fnc, winio_devctrl_fnc *devctrl_fnc, winio_cancel_fnc *cancel_fnc, winio_cleanup_fnc *cleanup_fnc, winio_destroy_fnc *destroy_fnc);
void *winio_get_file_context(HANDLE file);

//Registry
enum winreg_val_type {
    WINREG_STR = 1,
    WINREG_BINARY = 3,
    WINREG_DWORD = 4,
    WINREG_QWORD = 11,
};

typedef bool winreg_handler_fnc(void *ctx, void *ctx_obj, const char *key_name, const char *val_name, bool is_write, void *buf, size_t *buf_size, enum winreg_val_type *val_type);
void winreg_set_handler(winreg_handler_fnc *handler, void *ctx);

HANDLE winreg_open_key(void *ctx_obj, const char *key_name);
const char *winreg_get_key_name(HANDLE key);
bool winreg_query_val(HANDLE key, const char *val_name, void *buf, size_t *buf_size, enum winreg_val_type *val_type);
bool winreg_write_val(HANDLE key, const char *val_name, const void *buf, size_t buf_size, enum winreg_val_type val_type);

#endif