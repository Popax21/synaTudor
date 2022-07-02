#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse(struct pe_file *pe, uint8_t *data, int size) {
    memset(pe, 0, sizeof(struct pe_file));
    pe->data = data;
    pe->size = size;

    if(!pe_parse_dos_header(pe)) return false;
    if(!pe_parse_coff_header(pe)) return false;
    if(!pe_parse_opt_header(pe)) return false;

    if(!pe_parse_sections(pe)) return false;
    if(!pe_parse_data_dirs(pe)) return false;

    return true;
}

void pe_destroy(struct pe_file *pe) {
    //Free allocated memory
    pe->data = NULL;

    for(int i = 0; i < pe->num_data_dirs; i++) free(pe->data_dirs[i].data);
    free(pe->data_dirs);
    pe->data_dirs = NULL;

    free(pe->sections);
    pe->sections = NULL;

    for(int i = 0; i < pe->num_import_libs; i++) {
        struct pe_import_lib *lib = &pe->import_libs[i];
        free(lib->name);

        for(int i = 0; i < lib->num_imports; i++) free(lib->imports[i].name);
        free(lib->imports);
    }
    free(pe->import_libs);
    pe->import_libs = NULL;

    free(pe->export_lib_name);
    pe->export_lib_name = NULL;

    for(int i = 0; i < pe->num_exports; i++) {
        struct pe_export *export = &pe->exports[i];
        free(export->name);
        free(export->forwarder);
    }
    free(pe->exports);
    pe->exports = NULL;

    free(pe->relocations);
    pe->relocations = NULL;
}

bool pe_parse_dos_header(struct pe_file *pe) {
    if(pe->size < PE_DOS_HEADER_SIZE) {
        log_error("File too small for DOS header!");
        return false;
    }

    //Check header magic
    uint16_t magic = le16toh(*(uint16_t*) pe->data);
    if(magic != PE_DOS_HEADER_MAGIC) {
        log_error("Invalid DOS header magic! [%04x != %04x]", le16toh(*(uint16_t*) pe->data), PE_DOS_HEADER_MAGIC);
        return false;
    }

    //Get the PE header offset
    pe->coff_header_off = le32toh(*(uint32_t*) (pe->data + 0x3c)); //e_lfanew

    return true;
}