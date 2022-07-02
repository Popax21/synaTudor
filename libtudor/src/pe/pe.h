#ifndef LIBTUDOR_PE_PE_H
#define LIBTUDOR_PE_PE_H

#include <stdint.h>
#include <stdbool.h>
#include "spec.h"

struct pe_data_dir {
    uint32_t offset, size;
    uint8_t *data;
};

struct pe_section {
    char name[9];
    uint32_t file_off, mem_off;
    uint32_t file_size, mem_size;
    enum PEs_section_flags flags;
};

struct pe_import {
    int ord; //Negative if by name
    char *name;
    int addr_off;
};

struct pe_import_lib {
    char *name;
    int num_imports;
    struct pe_import *imports;
};

struct pe_export {
    int ord;
    uint32_t offset;
    char *name, *forwarder;
};

struct pe_reloc {
    int addr_bits;
    uint32_t offset;
    uint64_t delta;
};

struct pe_file {
    uint8_t *data;
    int size;

    //Headers
    int coff_header_off;
    int opt_header_off, opt_header_size;

    bool is_pe32_plus;
    int sect_align, file_align;
    uint32_t image_size;

    enum PEs_machine_type machine;
    enum PEs_flags flags;
    enum PEs_dll_flags dll_flags;
    uint64_t desired_base;
    uint32_t entry_point_off;

    int subsystem;
    int ver_major, ver_minor;
    int linker_ver_major, linker_ver_minor;
    int os_ver_major, os_ver_minor;
    int subsys_ver_major, subsys_ver_minor;

    //Data directories
    int data_dirs_off;
    int num_data_dirs;
    struct pe_data_dir *data_dirs;

    //Sections
    int sects_off;
    int num_sects;
    struct pe_section *sections;

    //Imports
    int num_import_libs;
    struct pe_import_lib *import_libs;

    //Exports
    char *export_lib_name;
    int num_exports;
    struct pe_export *exports;

    //Relocations
    int num_relocs;
    struct pe_reloc *relocations;
};

bool pe_parse(struct pe_file *pe, uint8_t *data, int size);
void pe_destroy(struct pe_file *pe);

uint8_t pe_peek_mem_i8(struct pe_file *pe, uint32_t off);
uint16_t pe_peek_mem_i16(struct pe_file *pe, uint32_t off);
uint32_t pe_peek_mem_i32(struct pe_file *pe, uint32_t off);
uint64_t pe_peek_mem_i64(struct pe_file *pe, uint32_t off);

void pe_copy_mem(struct pe_file *pe, uint32_t off, uint32_t size, void *buf);
char *pe_copy_mem_string(struct pe_file *pe, uint32_t off);

#endif