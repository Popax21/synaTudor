#include "internal.h"

typedef struct _PROPVARIANT PROPVARIANT;
typedef struct _PROPVARIANT {
    USHORT vt;
    BYTE wReserved1;
    BYTE wReserved2;
    ULONG wReserved3;
    union {
        CHAR cVal;
        UCHAR bVal;
        SHORT iVal;
        USHORT uiVal;
        LONG lVal;
        ULONG ulVal;
        INT intVal;
        UINT uintVal;
        CHAR *pcVal;
        UCHAR *pbVal;
        SHORT *piVal;
        USHORT *puiVal;
        LONG *plVal;
        ULONG *pulVal;
        INT *pintVal;
        UINT *puintVal;
        PROPVARIANT *pvarVal;
    };
} PROPVARIANT;

__winfnc HRESULT PropVariantClear(PROPVARIANT *pvar) {
    *pvar = (PROPVARIANT) {0};
    return ERROR_SUCCESS;
}
WINAPI(PropVariantClear)