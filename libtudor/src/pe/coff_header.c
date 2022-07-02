#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse_coff_header(struct pe_file *pe) {
    if(pe->coff_header_off < PE_DOS_HEADER_SIZE || pe->size < pe->coff_header_off + sizeof(struct PEs_coff_header)) {
        log_error("Invalid PE COFF header offset! [%08x]", pe->coff_header_off);
        return false;
    }

    struct PEs_coff_header *header = (struct PEs_coff_header*) (pe->data + pe->coff_header_off);

    //Check header magic
    uint32_t magic = le32toh(header->Magic);
    if(magic != PE_COFF_HEADER_MAGIC) {
        log_error("Invalid PE COFF header magic! [%08x != %08x]", magic, PE_COFF_HEADER_MAGIC);
        return false;
    }

    //Extract header data
    pe->machine = le16toh(header->Machine);
    pe->flags = le16toh(header->Characteristics);

    pe->opt_header_off = pe->coff_header_off + sizeof(struct PEs_coff_header);
    pe->opt_header_size = le16toh(header->SizeOfOptionalHeader);

    pe->sects_off = pe->opt_header_off + pe->opt_header_size;
    pe->num_sects = le16toh(header->NumberOfSections);

    //Check flags
    if(!(pe->flags & PE_FLAG_EXECUTABLE)) {
        log_error("PE file doesn't have the executable flag set!");
        return false;
    }

    if(pe->flags & PE_FLAG_NO_RELOC) {
        log_error("PE file doesn't have relocations!");
        return false;
    }

    if((pe->flags & PE_FLAG_REVERSED_LE) || (pe->flags & PE_FLAG_BE)) {
        log_error("Unsupported endianness");
        return false;
    }

    return true;
}