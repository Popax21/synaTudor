#include <stdlib.h>
#include <string.h>
#include "internal.h"

#define PROC_HEAP_HANDLE ((HANDLE) (uintptr_t) 0x50524f4348454150) /* PROCHEAP */

#define HEAP_GENERATE_EXCEPTIONS 0x00000004
#define HEAP_NO_SERIALIZE 0x00000001
#define HEAP_ZERO_MEMORY 0x00000008

__winfnc HANDLE GetProcessHeap() { return PROC_HEAP_HANDLE; }
WINAPI(GetProcessHeap)

/* One-shot WBF trigger — calls tudor_call_wbf_usb_init when available */
int tudor_wbf_done = 0; /* Set by tudor_call_wbf_usb_init when it succeeds */
/* Check uses a counter to avoid hammering resolve_windows_api every HeapAlloc */
static int wbf_counter = 0;
static void wbf_check(void) {
    if(tudor_wbf_done) return;
    if(++wbf_counter % 100 != 0) return;
    void (*fn)(void) = resolve_windows_api("tudor_call_wbf_usb_init");
    if(!fn) return;
    fn(); /* Function checks patches/objects internally, returns early if not ready */
    /* Don't set wbf_done here — the function sets it via a separate mechanism */
}

__winfnc void *HeapAlloc(HANDLE heap, DWORD flags, SIZE_T size) {
    if(heap != PROC_HEAP_HANDLE) {
        log_warn("HeapAlloc called with invalid heap handle");
        winerr_set();
        return NULL;
    }

    if(!tudor_wbf_done) wbf_check();

    //Allocate the memory — always zero-init to prevent heap junk in driver objects
    void *mem = calloc(1, size);
    if(mem) return mem;

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