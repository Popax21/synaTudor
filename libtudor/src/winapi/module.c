#include <pthread.h>
#include "internal.h"

static pthread_rwlock_t modules_lock = PTHREAD_RWLOCK_INITIALIZER;
static struct winmodule *modules_head;

static void module_destr(struct winmodule *module) {
    if(module->cmdline) return;

    //Free the module
    module->handle = NULL;
    winmodule_unregister(module);
    free((void*) module->name);
    free(module);
}

struct winmodule *winmodule_find(const char *name) {
    if(!name) return NULL;
    cant_fail_ret(pthread_rwlock_rdlock(&modules_lock));

    struct winmodule *module = NULL;
    for(struct winmodule *m = modules_head; m != NULL; m = m->next) {
        if(strcmp(m->name, name) == 0) {
            module = m;
            break;
        }
    }

    cant_fail_ret(pthread_rwlock_unlock(&modules_lock));
    return module;
}

void winmodule_register(struct winmodule *module) {
    cant_fail_ret(pthread_rwlock_wrlock(&modules_lock));

    module->handle = winhandle_create(module, (winhandle_destr_fnc*) module_destr);

    module->prev = NULL;
    module->next = modules_head;
    if(modules_head) modules_head->prev = module;
    modules_head = module;

    cant_fail_ret(pthread_rwlock_unlock(&modules_lock));
}

void winmodule_unregister(struct winmodule *module) {
    cant_fail_ret(pthread_rwlock_wrlock(&modules_lock));

    if(module->handle) {
        winhandle_destroy(module->handle);
        module->handle = NULL;
    }

    if(module->prev) module->prev->next = module->next;
    else modules_head = module->next;
    if(module->next) module->next->prev = module->prev;

    cant_fail_ret(pthread_rwlock_unlock(&modules_lock));
}

static __thread struct winmodule *cur_module;

struct winmodule *winmodule_get_cur() {
    WIN_CLOBBER_NONVOL_REGS
    return cur_module;
}

void winmodule_set_cur(struct winmodule *module) {
    cur_module = module;
}

__winfnc HANDLE LoadLibraryExW(const char16_t *name, HANDLE file, DWORD flags) {
    //We don't support loading librarys dynamically, but some stdlib functions have to be loaded that way
    //As such return dummy modules
    struct winmodule *module = (struct winmodule*) malloc(sizeof(struct winmodule));
    if(!module) { winerr_set_errno(); return NULL; }
    *module = (struct winmodule) {0};
    module->name = winstr_to_str(name);
    winmodule_register(module);
    return module->handle;
}
WINAPI(LoadLibraryExW)

__winfnc BOOL FreeLibrary(HANDLE handle) {
    winhandle_destroy(handle);
    return TRUE;
}
WINAPI(FreeLibrary)

__winfnc HANDLE GetModuleHandleA(const char *name) {
    if(!name) return cur_module ? cur_module->handle : NULL;
    struct winmodule *module = (struct winmodule*) winmodule_find(name);
    return module ? module->handle : NULL;
}
WINAPI(GetModuleHandleA)

__winfnc HANDLE GetModuleHandleW(const char16_t *name) {
    if(!name) return cur_module ? cur_module->handle : NULL;
    char *cname = winstr_to_str(name);
    struct winmodule *module = (struct winmodule*) winmodule_find(cname);
    free(cname);
    return module ? module->handle : NULL;
}
WINAPI(GetModuleHandleW)

#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 0x00000004
__winfnc BOOL GetModuleHandleExW(DWORD flags, const char16_t *name, HANDLE *out) {
    if(flags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS) {
        log_warn("GetModuleHandleExW called with unsupported flag GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS! [addr=%p] (returning current module)", name);
        if(out) *out = cur_module ? cur_module->handle : NULL;
        return TRUE;
    }

    char *cname = winstr_to_str(name);
    struct winmodule *module = (struct winmodule*) winmodule_find(cname);
    free(cname);

    if(module) *out = module->handle;
    return module != NULL;
}
WINAPI(GetModuleHandleExW)

__winfnc DWORD GetModuleFileNameA(HANDLE handle, char *name, DWORD size) {
    struct winmodule *module = handle ? (struct winmodule*) handle->data : cur_module;
 
    int name_len = strlen(module->name);

    if(size == 0) {
        winerr_set_code(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    } else if(name_len+1 <= size) {
        memcpy(name, module->name, name_len+1);
        winerr_clear();
        return name_len;
    } else {
        name_len = size - 1;
        memcpy(name, module->name, name_len+1);
        winerr_set_code(ERROR_INSUFFICIENT_BUFFER);
        return name_len;
    }
}
WINAPI(GetModuleFileNameA)

__winfnc DWORD GetModuleFileNameW(HANDLE handle, char16_t *name, DWORD size) {
    struct winmodule *module = handle ? (struct winmodule*) handle->data : cur_module;
 
    char16_t *wname = winstr_from_str(module->name);
    int wname_len = winstr_len(wname);

    if(size == 0) {
        free(wname);
        winerr_set_code(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    } else if(wname_len+1 <= size / sizeof(char16_t)) {
        memcpy(name, wname, (wname_len+1) * sizeof(char16_t));
        winerr_clear();
        free(wname);
        return wname_len;
    } else {
        wname_len = size / sizeof(char16_t) - 1;
        memcpy(name, wname, (wname_len+1) * sizeof(char16_t));
        winerr_set_code(ERROR_INSUFFICIENT_BUFFER);
        free(wname);
        return wname_len;
    }
}
WINAPI(GetModuleFileNameW)

__winfnc BOOL DisableThreadLibraryCalls(HANDLE handle) {
    return TRUE;
}
WINAPI(DisableThreadLibraryCalls)

__winfnc void *GetProcAddress(HANDLE handle, const char *name) {
    struct winmodule *module = (struct winmodule*) handle->data;

    //Check if it's an ordinal import
    uintptr_t nbits = (uintptr_t) name;
    if(!(nbits & ~((128ull << sizeof(uintptr_t)) - 1))) {
        int ord = (int) (nbits & ((128ull << sizeof(uintptr_t)) - 1));
        log_warn("GetProcAddress: Attempted ordinal import! module '%s' [%p] ord %d", module->name, module, ord);
        return NULL;
    }

    //Try to resolve the Windows API function
    void *resolv = resolve_windows_api(name);
    if(!resolv) winerr_set();
    return resolv;
}
WINAPI(GetProcAddress)
/* --- minimal USER32 stubs (a UMDF biometric driver shouldn't need GUI calls) --- */
__winfnc unsigned short RegisterClassExW(const void *wnd_class) {
    (void) wnd_class;
    return 1; /* fake ATOM */
}
WINAPI(RegisterClassExW)

__winfnc unsigned short RegisterClassW(const void *wnd_class) {
    (void) wnd_class;
    return 1;
}
WINAPI(RegisterClassW)

__winfnc int UnregisterClassW(const char16_t *name, HANDLE hinst) {
    (void) name; (void) hinst;
    return 1;
}
WINAPI(UnregisterClassW)

/* --- USER32 window/message-loop stubs (driver creates a hidden window for device
 *     notifications; we fake it so the notification thread runs without a real GUI) --- */
#include <stdint.h>

__winfnc HANDLE CreateWindowExW(DWORD ex_style, const char16_t *cls, const char16_t *title,
        DWORD style, int x, int y, int w, int h, HANDLE parent, HANDLE menu, HANDLE inst, void *param) {
    (void)ex_style;(void)cls;(void)title;(void)style;(void)x;(void)y;(void)w;(void)h;
    (void)parent;(void)menu;(void)inst;(void)param;
    return (HANDLE)(uintptr_t)1;
}
WINAPI(CreateWindowExW)

__winfnc int DestroyWindow(HANDLE hwnd) { (void)hwnd; return 1; }
WINAPI(DestroyWindow)

__winfnc long long DefWindowProcW(HANDLE hwnd, unsigned int msg, unsigned long long wp, long long lp) {
    (void)hwnd;(void)msg;(void)wp;(void)lp; return 0;
}
WINAPI(DefWindowProcW)

/* Return 0 (WM_QUIT) so any message pump we start exits cleanly instead of spinning. */
__winfnc int GetMessageW(void *msg, HANDLE hwnd, unsigned int min, unsigned int max) {
    (void)msg;(void)hwnd;(void)min;(void)max; return 0;
}
WINAPI(GetMessageW)

__winfnc int PeekMessageW(void *msg, HANDLE hwnd, unsigned int min, unsigned int max, unsigned int remove) {
    (void)msg;(void)hwnd;(void)min;(void)max;(void)remove; return 0;
}
WINAPI(PeekMessageW)

__winfnc int TranslateMessage(const void *msg) { (void)msg; return 0; }
WINAPI(TranslateMessage)

__winfnc long long DispatchMessageW(const void *msg) { (void)msg; return 0; }
WINAPI(DispatchMessageW)

__winfnc void PostQuitMessage(int code) { (void)code; }
WINAPI(PostQuitMessage)

__winfnc HANDLE RegisterDeviceNotificationW(HANDLE recipient, void *filter, DWORD flags) {
    (void)recipient;(void)filter;(void)flags; return (HANDLE)(uintptr_t)1;
}
WINAPI(RegisterDeviceNotificationW)

__winfnc int UnregisterDeviceNotification(HANDLE h) { (void)h; return 1; }
WINAPI(UnregisterDeviceNotification)
