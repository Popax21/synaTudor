#include <stdlib.h>
#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse_reloc_dir(struct pe_file *pe, struct pe_data_dir *dir) {
    if(dir->size <= 0) return true;

    //Determine number of relocations
    int num_relocs = 0;
    uint32_t block_off = 0;
    for(int i = 0; block_off < dir->size; i++) {
        uint32_t rem_size = dir->size - block_off;
        if(rem_size < sizeof(struct PEs_reloc_block)) {
            log_error("Relocation block %d doesn't fit! [%d < %d]", i, rem_size, (int) sizeof(struct PEs_reloc_block));
            return false;
        }

        struct PEs_reloc_block *block = (struct PEs_reloc_block*) (dir->data + block_off);
        uint32_t block_size = le32toh(block->BlockSize);
        int num_block_relocs = (block_size - sizeof(struct PEs_reloc_block)) / sizeof(int16_t);
        if(block_size < sizeof(struct PEs_reloc_block) || block_size - sizeof(struct PEs_reloc_block) != num_block_relocs * sizeof(int16_t)) {
            log_error("Relocation block %d has invalid size! [%d]", i, block_size);
            return false;
        }

        block_off += block_size;
        num_relocs += num_block_relocs;
    }

    pe->relocations = (struct pe_reloc*) malloc(num_relocs * sizeof(struct pe_reloc));

    //Parse relocations
    block_off = 0;
    pe->num_relocs = 0;
    for(int i = 0; block_off < dir->size; i++) {
        struct PEs_reloc_block *block = (struct PEs_reloc_block*) (dir->data + block_off);
        uint32_t block_size = le32toh(block->BlockSize), page_rva = le32toh(block->PageRVA);
        int num_block_relocs = (block_size - sizeof(struct PEs_reloc_block)) / sizeof(int16_t);

        for(int j = 0; j < num_block_relocs; j++) {
            uint16_t bits = le16toh(block->TypeOffsets[j]);
            uint32_t off = page_rva + (bits & 0xfff);

            //Check type
            int type = (bits >> 12) & 0xf;
            switch(type) {
                case PE_RELOC_NOP: continue;
                case PE_RELOC_HIGHLOW: {
                    pe->relocations[pe->num_relocs].addr_bits = 64;
                    pe->relocations[pe->num_relocs].offset = off;
                    pe->relocations[pe->num_relocs].delta = pe_peek_mem_i32(pe, off) - pe->desired_base;
                    pe->num_relocs++;
                } break;
                case PE_RELOC_DIR64: {
                    pe->relocations[pe->num_relocs].addr_bits = 64;
                    pe->relocations[pe->num_relocs].offset = off;
                    pe->relocations[pe->num_relocs].delta = pe_peek_mem_i64(pe, off) - pe->desired_base;
                    pe->num_relocs++;
                } break;
                default: {
                    log_error("Unsupported relocation type %d!", type);
                    return false;
                }
            }
        }

        block_off += block->BlockSize;
    }

    return true;
}