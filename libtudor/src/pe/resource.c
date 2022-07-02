#include <stdlib.h>
#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse_resource_dir(struct pe_file *pe, struct pe_data_dir *dir) {
    if(dir->size <= 0) return true;
    log_warn("PE file contains unsupported resource data directory!");
    return true;
}