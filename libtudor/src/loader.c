#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <tudor/log.h>
#include "pe/pe.h"
#include "winapi/api.h"
#include "loader.h"
#include "stub.h"

void register_windows_api(char *name, void *api) {
    log_verbose("Registered Windows API function %s", name);
}

static void ord_stub() {
    log_error("Ordinal import called!");
    abort();
}

#ifndef DBGIMPORT
static void unresolved_stub() {
    log_error("Unresolved import called!");
    abort();
}
#endif

static void *resolve_import(const char *lib, const char *name) {
    //Try to resolve the import
    void *winapi = resolve_windows_api(name);
    if(winapi) return winapi;

    //Return a stub
    log_verbose("Couldn't resolve import %s@%s", name, lib);
#ifdef DBGIMPORT
    return create_import_stub(lib, name);
#else
    return &unresolved_stub;
#endif
}

bool load_dll(struct dll_image *dll, const char *name, uint8_t *data, uint32_t size) {
    //Parse the PE file
    struct pe_file pe;
    pe_parse(&pe, data, size);
    log_debug("DLL %s: %s image", name, pe.is_pe32_plus ? "PE+" : "PE");
    log_debug("-> machine: %x", pe.machine);
    log_debug("-> image size: %08x", pe.image_size);
    log_debug("-> entry point: %08x", pe.entry_point_off);
    log_debug("-> num data dirs: %d", pe.num_data_dirs);
    log_debug("-> num sections: %d", pe.num_sects);
    log_debug("-> num relocations: %d", pe.num_relocs);

    if(
        !(sizeof(void*) == 4 && pe.machine == PE_MACHINE_x86 && !pe.is_pe32_plus) &&
        !(sizeof(void*) == 8 && pe.machine == PE_MACHINE_x86_64 && pe.is_pe32_plus)
    ) {
        log_error("DLL target architecture incompatbile with host program!");
        return false;
    }

    //Map the image memory
    uint8_t *image_mem = mmap(NULL, pe.image_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(!image_mem) {
        perror("Couldn't create DLL image memory mapping");
        return false;
    }

    //Copy data
    pe_copy_mem(&pe, 0, pe.image_size, image_mem);
    log_debug("Copied image memory to mapping at %p - %p", image_mem, image_mem + pe.image_size);

    //Resolve imports
    for(int i = 0; i < pe.num_import_libs; i++) {
        struct pe_import_lib *lib = &pe.import_libs[i];
        for(int j = 0; j < lib->num_imports; j++) {
            struct pe_import *imp = &lib->imports[j];

            //Resolve the import
            void *resolv_addr;
            if(imp->ord >= 0) {
                log_warn("DLL %s has ordinal import %s#%d!", name, lib->name, imp->ord);
                resolv_addr = &ord_stub;
            } else {
                resolv_addr = resolve_import(lib->name, imp->name);
            }

            //Write the address into the image
            if(!pe.is_pe32_plus) {
                *((uint32_t*) (image_mem + imp->addr_off)) = (uint32_t) (uint64_t) resolv_addr;
            } else {
                *((uint64_t*) (image_mem + imp->addr_off)) = (uint64_t) (uint64_t) resolv_addr;
            }
        }
    }

    //Apply relocations
    for(int i = 0; i < pe.num_relocs; i++) {
        struct pe_reloc *reloc = &pe.relocations[i];
        if(reloc->addr_bits == 32) {
            *((uint32_t*) (image_mem + reloc->offset)) = (uint32_t) ((uint64_t) image_mem + reloc->delta);
        } else if(reloc->addr_bits == 64) {
            *((uint64_t*) (image_mem + reloc->offset)) = (uint64_t) ((uint64_t) image_mem + reloc->delta);
        } else {
            log_error("Unsupported number of relocation bits! [%d]", reloc->addr_bits);
            return false;
        }
    }
    log_debug("Applied %d relocations", pe.num_relocs);

    //Apply section protections
    log_debug("Applying memory protections to image");

    if(mprotect(image_mem, pe.image_size, PROT_NONE)) {
        perror("Could't apply default image protection");
        return false;
    }

    for(int i = 0; i < pe.num_sects; i++) {
        struct pe_section *sec = &pe.sections[i];

        int prot = 0;
        if(sec->flags & PE_SECTION_CAN_READ) prot |= PROT_READ;
        if(sec->flags & PE_SECTION_CAN_WRITE) prot |= PROT_WRITE;
        if(sec->flags & PE_SECTION_CAN_EXECUTE) prot |= PROT_EXEC;
        if(mprotect(image_mem + sec->mem_off, sec->mem_size, prot)) {
            perror("Could't apply image section protection");
            return false;
        }

        log_debug("-> section %10s | %p - %p | %c%c%c", sec->name, image_mem + sec->mem_off, image_mem + sec->mem_off + sec->mem_size, (prot & PROT_READ) ? 'r' : '-', (prot & PROT_WRITE) ? 'w' : '-', (prot & PROT_EXEC) ? 'x' : '-');
    }

    //Initialize DLL structure
    dll->base_addr = image_mem;
    dll->image_size = pe.image_size;

    dll->entry_point = pe.entry_point_off ? image_mem + pe.entry_point_off : NULL;

    dll->num_exports = pe.num_exports;
    dll->exports = (struct dll_export*) malloc(pe.num_exports * sizeof(struct dll_export));
    for(int i = 0; i < pe.num_exports; i++) {
        struct pe_export *pexp = &pe.exports[i];
        struct dll_export *dexp = &dll->exports[i];

        dexp->name = strdup(pexp->name);
        dexp->addr = image_mem + pexp->offset;
    }

    //Cleanup
    pe_destroy(&pe);

    return true;
}

void destroy_dll(struct dll_image *dll) {
    //Free allocated memory
    for(int i = 0; i < dll->num_exports; i++) free(dll->exports[i].name);
    free(dll->exports);
    dll->exports = NULL;

    munmap(dll->base_addr, dll->image_size);
    dll->base_addr = NULL;
}

void *find_dll_export(struct dll_image *dll, const char *name) {
    for(int i = 0; i < dll->num_exports; i++) {
        if(strcmp(dll->exports[i].name, name) == 0) return dll->exports[i].addr;
    }

    log_error("Couldn't find DLL export '%s'!", name);
    abort();
}