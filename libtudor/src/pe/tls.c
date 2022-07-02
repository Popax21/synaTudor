#include <stdlib.h>
#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse_tls_dir(struct pe_file *pe, struct pe_data_dir *dir) {
    if(dir->size <= 0) return true;
    log_warn("PE file contains unsupported TLS data directory!");
    return true;
}