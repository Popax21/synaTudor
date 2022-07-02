#include <stdlib.h>
#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse_data_dirs(struct pe_file *pe) {
    if(pe->num_data_dirs * sizeof(struct PEs_data_dir) > pe->opt_header_size - sizeof(struct PEs_opt_header)) {
        log_error("PE file has too many data directory entries!");
        return false;
    }

    pe->data_dirs = (struct pe_data_dir*) malloc(pe->num_data_dirs * sizeof(struct pe_data_dir));

    //Iterate over data directories
    for(int i = 0; i < pe->num_data_dirs; i++) {
        struct PEs_data_dir *sdir = (struct PEs_data_dir*) (pe->data + pe->data_dirs_off + i*sizeof(struct PEs_data_dir));

        //Check bounds
        uint32_t mem_off = le32toh(sdir->VirtualAddress), mem_size = le32toh(sdir->Size);
        if(mem_off + mem_size > pe->image_size) {
            log_warn("Data directory %d has invalid bounds! [end 0x%x > image end 0x%x]", i, mem_off + mem_size, pe->image_size);
            mem_size = 0;
        }

        //Read data
        struct pe_data_dir *dir = &pe->data_dirs[i];
        dir->offset = mem_off;
        dir->size = mem_size;
        dir->data = (uint8_t*) malloc(mem_size);
        pe_copy_mem(pe, mem_off, mem_size, dir->data);

        //Parse "special" data directories
        switch(i) {
            case PE_DATA_DIR_EXPORT:    { if(!pe_parse_export_dir(pe, dir))     return false; } break;
            case PE_DATA_DIR_IMPORT:    { if(!pe_parse_import_dir(pe, dir))     return false; } break;
            case PE_DATA_DIR_RESOURCE:  { if(!pe_parse_resource_dir(pe, dir))   return false; } break;
            case PE_DATA_DIR_EXCEPTION: { if(!pe_parse_exception_dir(pe, dir))  return false; } break;
            case PE_DATA_DIR_RELOC:     { if(!pe_parse_reloc_dir(pe, dir))      return false; } break;
            case PE_DATA_DIR_TLS:       { if(!pe_parse_tls_dir(pe, dir))        return false; } break;
        }
    }

    return true;
}