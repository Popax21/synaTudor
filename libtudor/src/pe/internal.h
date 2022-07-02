#ifndef LIBTUDOR_PE_INTERNAL_H
#define LIBTUDOR_PE_INTERNAL_H

#include <stdbool.h>
#include "pe.h"
#include "spec.h"

bool pe_parse_coff_header(struct pe_file *pe);
bool pe_parse_dos_header(struct pe_file *pe);
bool pe_parse_opt_header(struct pe_file *pe);

bool pe_parse_data_dirs(struct pe_file *pe);
bool pe_parse_sections(struct pe_file *pe);

bool pe_parse_export_dir(struct pe_file *pe, struct pe_data_dir *dir);
bool pe_parse_import_dir(struct pe_file *pe, struct pe_data_dir *dir);
bool pe_parse_resource_dir(struct pe_file *pe, struct pe_data_dir *dir);
bool pe_parse_reloc_dir(struct pe_file *pe, struct pe_data_dir *dir);
bool pe_parse_exception_dir(struct pe_file *pe, struct pe_data_dir *dir);
bool pe_parse_tls_dir(struct pe_file *pe, struct pe_data_dir *dir);

#endif