#include "internal.h"

HANDLE winhandle_create(void *data, winhandle_destr_fnc *destr) {
    HANDLE handle = (HANDLE) malloc(sizeof(struct _HANDLE));
    if(!handle) {
        perror("Couldn't allocate memory for handle");
        abort();
    }

    handle->data = data;
    handle->destr = destr;
    return handle;
}

void winhandle_destroy(HANDLE handle) {
    if(handle->destr) handle->destr(handle->data);
    free(handle);
}

#define STD_INPUT_HANDLE ((DWORD) -10)
#define STD_OUTPUT_HANDLE ((DWORD) -11)
#define STD_ERROR_HANDLE ((DWORD) -12)

__winfnc HANDLE GetStdHandle(DWORD type) {
    switch(type) {
        case STD_INPUT_HANDLE: return NULL;
        case STD_OUTPUT_HANDLE: return NULL;
        case STD_ERROR_HANDLE: return NULL;
        default: return INVALID_HANDLE_VALUE;
    }
}
WINAPI(GetStdHandle)

__winfnc BOOL CloseHandle(HANDLE handle) {
    winhandle_destroy(handle);
    return TRUE;
}
WINAPI(CloseHandle)