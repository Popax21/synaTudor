#include "internal.h"

__winfnc void WppAutoLogStart(DRIVER_OBJECT *driver, UNICODE_STRING *reg_path) {}
WINAPI(WppAutoLogStart)

__winfnc void WppAutoLogStop(DRIVER_OBJECT *driver) {}
WINAPI(WppAutoLogStop)

__winfnc NTSTATUS WppAutoLogTrace(void *context, UCHAR level, ULONG flags, GUID *guid, USHORT num, ...) {
    win_va_list vas;
    win_va_start(vas, num);
    winlog_trace(*guid, num, vas);
    win_va_end(vas);
    return ERROR_SUCCESS;
}
WINAPI(WppAutoLogTrace)