#include <asm/prctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdarg.h>
#include <unistd.h>
#include "internal.h"

typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        };
    };
    DWORD dwPageSize;
    void *lpMinimumApplicationAddress;
    void *lpMaximumApplicationAddress;
    ULONG_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO;

typedef struct {
    ULONG dwOSVersionInfoSize;
    ULONG dwMajorVersion;
    ULONG dwMinorVersion;
    ULONG dwBuildNumber;
    ULONG dwPlatformId;
    WCHAR szCSDVersion[128];
} OSVERSIONINFOW;

typedef struct {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR szCSDVersion[128];
    WORD wServicePackMajor;
    WORD wServicePackMinor;
    WORD wSuiteMask;
    BYTE wProductType;
    BYTE wReserved;
} OSVERSIONINFOEXA;

__winfnc NTSTATUS RtlGetVersion(OSVERSIONINFOW *ver) {
    ver->dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    ver->dwMajorVersion = 10;
    ver->dwMinorVersion = 0;
    ver->dwBuildNumber = 17134;
    ver->dwPlatformId = 2; //VER_PLATFORM_WIN32_NT
    return 0;
}
WINAPI(RtlGetVersion)

__winfnc ULONGLONG VerSetConditionMask(ULONGLONG cond_mask, DWORD type_mask, BYTE condition) {
    return cond_mask;
}
WINAPI(VerSetConditionMask)

__winfnc void GetSystemInfo(SYSTEM_INFO *info) {
    info->wProcessorArchitecture = 9; //PROCESSOR_ARCHITECTURE_AMD64
    info->dwPageSize = 4096;
    info->lpMinimumApplicationAddress = (void*) 0;
    info->lpMaximumApplicationAddress = (void*) UINTPTR_MAX;
    info->dwActiveProcessorMask = 0b1;
    info->dwNumberOfProcessors = 1;
    info->dwProcessorType = 8664; //PROCESSOR_AMD_X8664
    info->dwAllocationGranularity = 8;
    info->wProcessorLevel = 0;
    info->wProcessorRevision = 0;
}
WINAPI(GetSystemInfo)

__winfnc BOOL VerifyVersionInfoW(ULONGLONG condition_mask, DWORD type_mask, ULONGLONG cond_mask) {
    //TODO
    return TRUE;
}
WINAPI(VerifyVersionInfoW)

__winfnc BOOL ConvertStringSecurityDescriptorToSecurityDescriptorW(const char16_t *str, DWORD rev, void *descrpt, ULONG *descrpt_size) {
    //TODO
    if(descrpt_size) *descrpt_size = 0;
    return TRUE;
}
WINAPI(ConvertStringSecurityDescriptorToSecurityDescriptorW)