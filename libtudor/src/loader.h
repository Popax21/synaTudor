#ifndef LIBTUDOR_LOADER_H
#define LIBTUDOR_LOADER_H

#include <stdint.h>
#include <stdbool.h>

struct dll_export {
    char *name;
    void *addr;
};

struct dll_image {
    void *base_addr;
    int image_size;

    void *entry_point;

    int num_exports;
    struct dll_export *exports;
};

bool load_dll(struct dll_image *dll, const char *name, uint8_t *data, uint32_t size);
void destroy_dll(struct dll_image *dll);

void *find_dll_export(struct dll_image *dll, const char *name);

#endif