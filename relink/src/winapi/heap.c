#include <stdlib.h>
#include <string.h>
#include "internal.h"

#define PROC_HEAP_HANDLE ((HANDLE) (uintptr_t) 0x50524f4348454150) /* PROCHEAP */

#define HEAP_GENERATE_EXCEPTIONS 0x00000004
#define HEAP_NO_SERIALIZE 0x00000001
#define HEAP_ZERO_MEMORY 0x00000008

__winfnc HANDLE GetProcessHeap() { return PROC_HEAP_HANDLE; }
WINAPI(GetProcessHeap)

__winfnc void *HeapAlloc(HANDLE heap, DWORD flags, SIZE_T size) {
    if(heap != PROC_HEAP_HANDLE) {
        log_warn("HeapAlloc called with invalid heap handle");
        winerr_set();
        return NULL;
    }

    //Allocate the memory
    void *mem = malloc(size);
    if(mem) {
        if(flags & HEAP_ZERO_MEMORY) memset(mem, 0, size);
        return mem;
    }

    //There was an error allocating the memory
    if(flags & HEAP_GENERATE_EXCEPTIONS) {
        perror("Error allocating memory for HeapAlloc");
        log_error("HeapAlloc: HEAP_GENERATE_EXCEPTIONS flag set and memory allocation failed!");
        abort();
    }

    winerr_set_errno();
    return NULL;
}
WINAPI(HeapAlloc)

__winfnc BOOL HeapFree(HANDLE heap, DWORD flags, void *mem) {
    if(heap != PROC_HEAP_HANDLE) {
        log_warn("HeapAlloc called with invalid heap handle");
        winerr_set();
        return FALSE;
    }

    free(mem);
    return TRUE;
}
WINAPI(HeapFree)

__winfnc void *LocalFree(void *mem) {
    free(mem);
    return NULL;
}
WINAPI(LocalFree)