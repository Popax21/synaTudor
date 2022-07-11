#ifndef LIBTUDOR_WINAPI_WINDOWS_H
#define LIBTUDOR_WINAPI_WINDOWS_H

#include <stdint.h>
#include <stdbool.h>
#include <uchar.h>
#include "attr.h"

typedef char CHAR;
typedef unsigned char UCHAR;
typedef char16_t WCHAR;
typedef short SHORT;
typedef unsigned short USHORT;
typedef int INT;
typedef unsigned int UINT;
typedef int LONG;
typedef unsigned int ULONG;
typedef long long LONGLONG;
typedef unsigned long long ULONGLONG;
typedef intptr_t LONG_PTR;
typedef uintptr_t ULONG_PTR;

typedef uintptr_t SIZE_T;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef uint64_t DWORD64;

typedef DWORD BOOL;
typedef BYTE BOOLEAN;
#define TRUE 1
#define FALSE 0

typedef DWORD HRESULT;
typedef DWORD NTSTATUS;

typedef struct _HANDLE *HANDLE;

typedef struct {
    uint32_t PartA;
    uint16_t PartB, PartC;
    uint64_t PartD:16, PartE:48;
} __packed GUID;
#define DEFINE_GUID(a, b, c, d, e) ((GUID) {\
    .PartA = 0x##a,\
    .PartB = 0x##b, .PartC = 0x##c,\
    .PartD = (((0x##d >>  8) & 0xff) <<  0) | (((0x##d >>  0) & 0xff) <<  8),\
    .PartE = (((0x##e >> 40) & 0xff) <<  0) | (((0x##e >> 32) & 0xff) <<  8) | (((0x##e >> 24) & 0xff) << 16) | (((0x##e >> 16) & 0xff) << 24) | (((0x##e >>  8) & 0xff) << 32) | (((0x##e >>  0) & 0xff) << 40)\
})

enum {
    ERROR_SUCCESS = 0x0,
    ERROR_INSUFFICIENT_BUFFER = 0x7a,
    ERROR_IO_INCOMPLETE = 0x3e4,
    ERROR_IO_PENDING = 0x3e5,
    E_INVALIDARG = 0x80070057,
    E_NOTIMPL = 0x80004001
};

enum {
    STATUS_SUCCESS = 0x0,
    STATUS_PENDING = 0x00000103,
    STATUS_CANCELLED = 0xc0000120,
    STATUS_BUFFER_TOO_SMALL = 0xc0000023,
    STATUS_INVALID_SIGNATURE = 0xc000a000
};

typedef struct {
    USHORT Length;
    USHORT MaximumLength;
    char16_t *Buffer;
} UNICODE_STRING;

typedef struct {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
        struct {
            DWORD Offset;
            DWORD OffsetHigh;
        };
        void *Pointer;
    };
    HANDLE hEvent;
} OVERLAPPED;

typedef struct {
    LONG x;
    LONG y;
} POINT;

typedef struct {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT;

enum {
    FILE_FLAG_OVERLAPPED = 0x40000000
};

#endif