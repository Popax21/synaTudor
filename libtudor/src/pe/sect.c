#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse_sections(struct pe_file *pe) {
    if(pe->size < pe->sects_off + pe->num_sects * sizeof(struct PEs_section_header)) {
        log_error("Invalid PE section table offset! [%08x]", pe->sects_off);
        return false;
    }

    pe->sections = (struct pe_section*) malloc(pe->num_sects * sizeof(struct pe_section));

    //Iterate over sections
    for(int i = 0; i < pe->num_sects; i++) {
        struct PEs_section_header *sec_header = (struct PEs_section_header*) (pe->data + pe->sects_off + i*sizeof(struct PEs_section_header));

        //Initialize section entry
        struct pe_section *sec = &pe->sections[i];
        strncpy(sec->name, sec_header->Name, 8);
        sec->file_off = le32toh(sec_header->PointerToRawData);
        sec->file_size = le32toh(sec_header->SizeOfRawData);
        sec->mem_off = le32toh(sec_header->VirtualAddress);
        sec->mem_size = le32toh(sec_header->VirtualSize);
        sec->flags = le32toh(sec_header->Characteristics);

        //Check bounds
        if(sec->file_off + sec->file_size > pe->size) {
            log_error("Section %d has invalid file bounds! [end 0x%x > file end 0x%x]", i, sec->file_off + sec->file_size, pe->size);
            return false;
        }

        if(sec->mem_off + sec->mem_size > pe->image_size) {
            log_error("Section %d has invalid memory bounds! [end 0x%x > image end 0x%x]", i, sec->mem_off + sec->mem_size, pe->image_size);
            return false;
        }

        //Check alignment
        if(sec->file_off % pe->file_align != 0) {
            log_error("Section %d isn't properly file aligned! [off 0x%x alignment 0x%x]", i, sec->file_off, pe->file_align);
            return false;
        }

        if(sec->mem_off % pe->sect_align != 0) {
            log_error("Section %d isn't properly memory aligned! [off 0x%x alignment 0x%x]", i, sec->mem_off, pe->sect_align);
            return false;
        }
    }

    return true;
}

uint8_t pe_peek_mem_i8(struct pe_file *pe, uint32_t off) {
    //Iterate over sections
    for(int i = 0; i < pe->num_sects; i++) {
        struct pe_section *sec = &pe->sections[i];

        //Does the offset lie within the section?
        if(off < sec->mem_off || sec->mem_off + sec->mem_size <= off) continue;

        //Return the byte
        uint32_t sec_off = off - sec->mem_off;
        if(sec->file_size <= sec_off) return 0;
        return pe->data[sec->file_off + sec_off];
    }

    return 0;
}

uint16_t pe_peek_mem_i16(struct pe_file *pe, uint32_t off) {
    uint16_t v;
    pe_copy_mem(pe, off, sizeof(v), &v);
    return le16toh(v);
}

uint32_t pe_peek_mem_i32(struct pe_file *pe, uint32_t off) {
    uint32_t v;
    pe_copy_mem(pe, off, sizeof(v), &v);
    return le32toh(v);
}

uint64_t pe_peek_mem_i64(struct pe_file *pe, uint32_t off) {
    uint64_t v;
    pe_copy_mem(pe, off, sizeof(v), &v);
    return le64toh(v);
}

void pe_copy_mem(struct pe_file *pe, uint32_t off, uint32_t size, void *buf) {
    memset(buf, 0, size);

    //Iterate over sections
    for(int i = 0; i < pe->num_sects; i++) {
        struct pe_section *sec = &pe->sections[i];

        //Get the sub range of the requested range which lies within the section
        uint32_t start_off = off, end_off = off + size;
        if(start_off < sec->mem_off) start_off = sec->mem_off;
        if(sec->mem_off + sec->mem_size < end_off) end_off = sec->mem_off + sec->mem_size;

        if(start_off >= end_off) continue;
        uint32_t buf_off = start_off - off;

        //Copy data
        uint32_t copy_end_off = sec->mem_off + sec->file_size;
        if(copy_end_off < start_off) copy_end_off = start_off;
        if(end_off < copy_end_off) copy_end_off = end_off;

        uint32_t copy_off = start_off - sec->mem_off, copy_size = copy_end_off - start_off;
        memcpy((uint8_t*) buf + buf_off, pe->data + sec->file_off + copy_off, copy_size);
    }
}

char *pe_copy_mem_string(struct pe_file *pe, uint32_t off) {
    //Get the length of the string
    int str_len = 0;
    for(uint32_t o = off; pe_peek_mem_i8(pe, o) != 0; o++) str_len++;

    //Allocate the string
    char *str = (char*) malloc(str_len+1);

    //Copy the string's data
    pe_copy_mem(pe, off, str_len+1, (uint8_t*) str);

    return str;
}