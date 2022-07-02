#include <errno.h>
#include <string.h>
#include "internal.h"

static __thread DWORD last_error;

void winerr_clear() {
    last_error = 0;
}

void winerr_set() {
    winerr_set_code(WINERR_SET_CODE);
}

void winerr_set_code(int code) {
    last_error = code;
}

void winerr_set_errno() {
    last_error = errno;
    log_verbose("Setting LastError from errno: %d [%s]", errno, strerror(errno));
}

int winerr_from_errno() {
    return errno;
    log_verbose("Converting errno to LastError: %d [%s]", errno, strerror(errno));
}

__winfnc DWORD GetLastError() {
    return last_error;
}
WINAPI(GetLastError)

__winfnc void SetLastError(DWORD err) {
    last_error = err;
}
WINAPI(SetLastError)