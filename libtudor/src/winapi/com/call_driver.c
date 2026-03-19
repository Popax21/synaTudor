/*
 * Isolated trampoline for calling driver functions with ms_abi convention.
 */
#include <stdint.h>

typedef unsigned int HRESULT;
typedef HRESULT __attribute__((ms_abi)) (*driver_fn_t)(void *self);

__attribute__((noinline))
unsigned int call_driver_function(void *fn_addr, void *this_ptr) {
    if(!fn_addr || !this_ptr) return 0x80004005; /* E_FAIL */
    return ((driver_fn_t)fn_addr)(this_ptr);
}
