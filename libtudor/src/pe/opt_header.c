#include <endian.h>
#include <tudor/log.h>
#include "internal.h"

bool pe_parse_opt_header(struct pe_file *pe) {
    if(pe->opt_header_size <= 0) {
        log_error("PE file doesn't have optional header!");
        return false;
    } else if(pe->opt_header_size < sizeof(struct PEs_opt_header)) {
        log_error("Invalid PE optional header size! [%x]", pe->opt_header_size);
        return false;
    }

    if(pe->size < pe->opt_header_off + pe->opt_header_size) {
        log_error("Invalid PE optional header offset! [%08x]", pe->opt_header_off);
        return false;
    }

    struct PEs_opt_header *header = (struct PEs_opt_header*) (pe->data + pe->opt_header_off);

    //Check header magic
    uint16_t magic = le16toh(header->Magic);
    switch(magic) {
        case PE_OPT_HEADER_MAGIC_PE32: pe->is_pe32_plus = false; break;
        case PE_OPT_HEADER_MAGIC_PE32PLUS: pe->is_pe32_plus = true; break;
        default: {
            log_error("Invalid PE optional header magic! [%04x]", magic);
            return false;
        }
    }

    //Extract header data
    if(!pe->is_pe32_plus) {
        struct PEs_opt_header_pe32 *pheader = (struct PEs_opt_header_pe32*) header;
        pe->sect_align = le32toh(pheader->SectionAlignment);
        pe->file_align = le32toh(pheader->FileAlignment);
        pe->image_size = le32toh(pheader->SizeOfImage);

        pe->dll_flags = le16toh(pheader->DllCharacteristics);
        pe->desired_base = le32toh(pheader->ImageBase);
        pe->entry_point_off = le32toh(pheader->AddressOfEntryPoint);

        pe->subsystem = le16toh(pheader->Subsystem);
        pe->ver_major = le16toh(pheader->MajorImageVersion);
        pe->ver_minor = le16toh(pheader->MinorImageVersion);
        pe->linker_ver_major = pheader->MajorLinkerVersion;
        pe->linker_ver_minor = pheader->MinorLinkerVersion;
        pe->os_ver_major = le16toh(pheader->MajorOperatingSystemVersion);
        pe->os_ver_minor = le16toh(pheader->MinorOperatingSystemVersion);
        pe->subsys_ver_major = le16toh(pheader->MajorSubsystemVersion);
        pe->subsys_ver_minor = le16toh(pheader->MinorSubsystemVersion);

        pe->data_dirs_off = pe->opt_header_off + sizeof(struct PEs_opt_header);
        pe->num_data_dirs = le32toh(pheader->NumberOfRvaAndSizes);
    } else {
        struct PEs_opt_header_pe32plus *pheader = (struct PEs_opt_header_pe32plus*) header;
        pe->sect_align = le32toh(pheader->SectionAlignment);
        pe->file_align = le32toh(pheader->FileAlignment);
        pe->image_size = le32toh(pheader->SizeOfImage);

        pe->dll_flags = le16toh(pheader->DllCharacteristics);
        pe->desired_base = le64toh(pheader->ImageBase);
        pe->entry_point_off = le32toh(pheader->AddressOfEntryPoint);

        pe->subsystem = le16toh(pheader->Subsystem);
        pe->ver_major = le16toh(pheader->MajorImageVersion);
        pe->ver_minor = le16toh(pheader->MinorImageVersion);
        pe->linker_ver_major = pheader->MajorLinkerVersion;
        pe->linker_ver_minor = pheader->MinorLinkerVersion;
        pe->os_ver_major = le16toh(pheader->MajorOperatingSystemVersion);
        pe->os_ver_minor = le16toh(pheader->MinorOperatingSystemVersion);
        pe->subsys_ver_major = le16toh(pheader->MajorSubsystemVersion);
        pe->subsys_ver_minor = le16toh(pheader->MinorSubsystemVersion);

        pe->data_dirs_off = pe->opt_header_off + sizeof(struct PEs_opt_header);
        pe->num_data_dirs = le32toh(pheader->NumberOfRvaAndSizes);
    }

    //Check DLL flags
    if(!(pe->dll_flags & PE_DLL_FLAG_DYNAMIC)) {
        log_error("PE file isn't dynamically loadable!");
        return false;
    }

    return true;
}