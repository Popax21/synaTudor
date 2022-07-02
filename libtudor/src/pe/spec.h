#ifndef LIBTUDOR_PE_SPEC_H
#define LIBTUDOR_PE_SPEC_H

#include <stdint.h>
#include <stdbool.h>
#include "attr.h"

#define PE_DOS_HEADER_SIZE 64
#define PE_DOS_HEADER_MAGIC 0x5a4d

#define PE_COFF_HEADER_MAGIC 0x00004550 /* "PE\x00\x00" */
#define PE_OPT_HEADER_MAGIC_PE32 0x010b
#define PE_OPT_HEADER_MAGIC_PE32PLUS 0x020b

#define PE_DATA_DIR_EXPORT 0
#define PE_DATA_DIR_IMPORT 1
#define PE_DATA_DIR_RESOURCE 2
#define PE_DATA_DIR_EXCEPTION 3
#define PE_DATA_DIR_RELOC 5
#define PE_DATA_DIR_TLS 9

enum PEs_machine_type {
    PE_MACHINE_UNKNOWN = 0,
    PE_MACHINE_x86 = 0x14c,
    PE_MACHINE_x86_64 = 0x8664,
};

enum PEs_flags {
    PE_FLAG_NO_RELOC = 0x0001,
    PE_FLAG_EXECUTABLE = 0x0002,
    PE_FLAG_LARGE_ADDR = 0x0020,
    PE_FLAG_REVERSED_LE = 0x0080,
    PE_FLAG_DLL = 0x2000,
    PE_FLAG_BE = 0x8000
};

struct PEs_coff_header {
    uint32_t Magic;
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t TimeDateStamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} __packed;

enum PEs_dll_flags {
    PE_DLL_FLAG_DYNAMIC = 0x0040,
    PE_DLL_FLAG_NX = 0x0100,
};

struct PEs_opt_header {
    union {
        uint16_t Magic;

        struct PEs_opt_header_pe32 {
            uint16_t Magic;
            uint8_t MajorLinkerVersion, MinorLinkerVersion;
            uint32_t SizeOfCode;
            uint32_t SizeOfInitializedData, SizeOfUninitializedData;
            uint32_t AddressOfEntryPoint;
            uint32_t BaseOfCode;
            uint32_t BaseOfData;
            uint32_t ImageBase;
            uint32_t SectionAlignment, FileAlignment;
            uint16_t MajorOperatingSystemVersion, MinorOperatingSystemVersion;
            uint16_t MajorImageVersion, MinorImageVersion;
            uint16_t MajorSubsystemVersion, MinorSubsystemVersion;
            uint32_t Win32VersionValue;
            uint32_t SizeOfImage, SizeOfHeaders;
            uint32_t CheckSum;
            uint16_t Subsystem;
            uint16_t DllCharacteristics;
            uint32_t SizeOfStackReserve, SizeOfStackCommit;
            uint32_t SizeOfHeapReserve, SizeOfHeapCommit;
            uint32_t LoaderFlags;
            uint32_t NumberOfRvaAndSizes;
        } __packed PE32;

        struct PEs_opt_header_pe32plus {
            uint16_t Magic;
            uint8_t MajorLinkerVersion, MinorLinkerVersion;
            uint32_t SizeOfCode;
            uint32_t SizeOfInitializedData, SizeOfUninitializedData;
            uint32_t AddressOfEntryPoint;
            uint32_t BaseOfCode;
            uint64_t ImageBase;
            uint32_t SectionAlignment, FileAlignment;
            uint16_t MajorOperatingSystemVersion, MinorOperatingSystemVersion;
            uint16_t MajorImageVersion, MinorImageVersion;
            uint16_t MajorSubsystemVersion, MinorSubsystemVersion;
            uint32_t Win32VersionValue;
            uint32_t SizeOfImage, SizeOfHeaders;
            uint32_t CheckSum;
            uint16_t Subsystem;
            uint16_t DllCharacteristics;
            uint64_t SizeOfStackReserve, SizeOfStackCommit;
            uint64_t SizeOfHeapReserve, SizeOfHeapCommit;
            uint32_t LoaderFlags;
            uint32_t NumberOfRvaAndSizes;
        } __packed PE32Plus;
    } __packed;
} __packed;

struct PEs_data_dir {
    uint32_t VirtualAddress, Size;
} __packed;

enum PEs_section_flags {
    PE_SECTION_CONTENT_CODE = 0x00000020,
    PE_SECTION_CONTENT_INIT_DATA = 0x00000040,
    PE_SECTION_CONTENT_UNINIT_DATA = 0x00000080,
    PE_SECTION_CONTENT_INFO = 0x00000200,
    PE_SECTION_CAN_EXECUTE = 0x20000000,
    PE_SECTION_CAN_READ = 0x40000000,
    PE_SECTION_CAN_WRITE = 0x80000000
};

struct PEs_section_header {
    char Name[8];
    uint32_t VirtualSize;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLinenumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLinenumbers;
    uint32_t Characteristics;
} __packed;

struct PEs_import_dir_entry {
    uint32_t ImportLookupTableRVA;
    uint32_t TimeDateStamp;
    uint32_t ForwarderChain;
    uint32_t NameRVA;
    uint32_t ImportAddressTableRVA;
} __packed;

struct PEs_export_dir {
    uint32_t ExportFlags;
    uint32_t TimeDateStamp;
    uint16_t MajorVersion, MinorVersion;
    uint32_t NameRVA;
    uint32_t OrdinalBase;
    uint32_t NumberOfAddressTableEntries, NumberOfNamePointers;
    uint32_t ExportAddressTableRVA, NamePointerRVA, OrdinalTableRVA;
} __packed;

enum PEs_reloc_type {
    PE_RELOC_NOP = 0,
    PE_RELOC_HIGHLOW = 3,
    PE_RELOC_DIR64 = 10
};

struct PEs_reloc_block {
    uint32_t PageRVA;
    uint32_t BlockSize;
    uint16_t TypeOffsets[];
} __packed;

#endif