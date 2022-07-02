#include <stdlib.h>
#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse_import_dir(struct pe_file *pe, struct pe_data_dir *dir) {
    if(dir->size <= 0) return true;

    //Determine number of imported libraries
    pe->num_import_libs = 0;
    for(int i = 0;; i++) {
        if(dir->size < (i+1) * sizeof(struct PEs_import_dir_entry)) {
            log_error("Import directory entry %d has invalid bounds! [end 0x%x > directory end 0x%x]", i, (i+1) * (uint32_t) sizeof(struct PEs_import_dir_entry), dir->size);
            return false;
        }

        struct PEs_import_dir_entry *entry = (struct PEs_import_dir_entry*) (dir->data + i * sizeof(struct PEs_import_dir_entry));

        //Is the entry empty?
        if(entry->ImportLookupTableRVA == 0 && entry->TimeDateStamp == 0 && entry->ForwarderChain == 0 && entry->NameRVA == 0 && entry->ImportAddressTableRVA == 0) break;

        pe->num_import_libs++;
    }

    pe->import_libs = (struct pe_import_lib*) malloc(pe->num_import_libs * sizeof(struct pe_import_lib));

    //Iterate over imported libraries
    for(int i = 0; i < pe->num_import_libs; i++) {
        struct PEs_import_dir_entry *entry = (struct PEs_import_dir_entry*) (dir->data + i * sizeof(struct PEs_import_dir_entry));
        struct pe_import_lib *lib = &pe->import_libs[i];

        //Extract info
        uint32_t lookup_table_rva = le32toh(entry->ImportLookupTableRVA), name_rva = le32toh(entry->NameRVA), addr_table_rva = le32toh(entry->ImportAddressTableRVA);
        lib->name = pe_copy_mem_string(pe, name_rva);

        //Determine number of imports
        lib->num_imports = 0;
        if(!pe->is_pe32_plus) {
            for(int i = 0;; i++) {
                if(pe_peek_mem_i32(pe, lookup_table_rva + i*sizeof(uint32_t)) == 0) break;
                lib->num_imports++;
            }
        } else {
            for(int i = 0;; i++) {
                if(pe_peek_mem_i64(pe, lookup_table_rva + i*sizeof(uint64_t)) == 0) break;
                lib->num_imports++;
            }
        }

        lib->imports = (struct pe_import*) malloc(lib->num_imports * sizeof(struct pe_import));

        //Iterate over imports
        for(int i = 0; i < lib->num_imports; i++) {
            struct pe_import *import = &lib->imports[i];
            if(!pe->is_pe32_plus) {
                uint32_t bits = pe_peek_mem_i32(pe, lookup_table_rva + i*sizeof(uint32_t));
                if(bits & (1L << 31)) {
                    import->ord = bits & ((1 << 16) - 1);
                    import->name = NULL;
                } else {
                    import->ord = -1;
                    import->name = pe_copy_mem_string(pe, (bits & ((1u << 31) - 1)) + 2);
                }
                import->addr_off = addr_table_rva + i*sizeof(uint32_t);
            } else {
                uint64_t bits = pe_peek_mem_i32(pe, lookup_table_rva + i*sizeof(uint64_t));
                if(bits & (1L << 63)) {
                    import->ord = bits & ((1 << 16) - 1);
                    import->name = NULL;
                } else {
                    import->ord = -1;
                    import->name = pe_copy_mem_string(pe, (bits & ((1u << 31) - 1)) + 2);
                }
                import->addr_off = addr_table_rva + i*sizeof(uint64_t);
            }
        }
    }

    return true;
}