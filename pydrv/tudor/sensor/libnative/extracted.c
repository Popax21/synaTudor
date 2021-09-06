#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "extracted.h"

void __extracted *EXTR_malloc(unsigned int size) {
    void *mem = malloc(size);
    memset(mem, 0, size);
    return mem;
}

void __extracted EXTR_free(void *ptr) {
    free(ptr);
}

void __extracted EXTR_memcpy(uint8_t *dst, uint8_t *src, unsigned int size) {
    memcpy(dst, src, size);
}

void __extracted EXTR_memset(uint8_t *ptr, uint8_t val, unsigned int size) {
    memset(ptr, val, size);
}