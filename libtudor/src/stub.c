#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include "stub.h"
#include <tudor/log.h>

#define STUB_PAGE_SIZE 8192

static inline void encode_mov(uint8_t *targ, int reg, uint64_t val) {
    targ[0] = 0x48;                 //REX - 64 bit operands
    targ[1] = 0xb8 | reg;           //MOV r64, imm64
    targ[2] = (val >>  0) & 0xff;   //imm64 - byte 0
    targ[3] = (val >>  8) & 0xff;   //imm64 - byte 1
    targ[4] = (val >> 16) & 0xff;   //imm64 - byte 2
    targ[5] = (val >> 24) & 0xff;   //imm64 - byte 3
    targ[6] = (val >> 32) & 0xff;   //imm64 - byte 4
    targ[7] = (val >> 40) & 0xff;   //imm64 - byte 5
    targ[8] = (val >> 48) & 0xff;   //imm64 - byte 6
    targ[9] = (val >> 56) & 0xff;   //imm64 - byte 7
}

#ifdef DBGIMPORT
#define IMPORT_STUB_SIZE 32

static uint8_t *import_cur_page;
static int import_cur_slot;

static void import_stub(char *lib, char *name) {
    log_error("Unresolved import %s@%s called!", name, lib);
    abort();
}

void *create_import_stub(const char *lib, const char *name) {
    lib = strdup(lib);
    name = strdup(name);

    //Obtain a stub slot
    if(!import_cur_page || import_cur_slot*IMPORT_STUB_SIZE + IMPORT_STUB_SIZE > STUB_PAGE_SIZE) {
        import_cur_page = (uint8_t*) mmap(NULL, STUB_PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(!import_cur_page) {
            perror("Couldn't allocate import stub page");
            abort();
        }
        import_cur_slot = 0;
        log_debug("Allocated import stub page at %p", import_cur_page);
    }

    uint8_t *slot = import_cur_page + (import_cur_slot++)*IMPORT_STUB_SIZE;

    //Encode slot instructions
    encode_mov(slot +  0, 7, (uint64_t) lib);           //mov rdi, <lib>
    encode_mov(slot + 10, 6, (uint64_t) name);          //mov rsi, <name>
    encode_mov(slot + 20, 0, (uint64_t) &import_stub);  //mov rax, <stub function>
    slot[30] = 0xff;                                    //jmp rax
    slot[31] = 0xc0 | (4 << 3) | (0 << 0);              //  - ModR/M          

    return slot;
}
#endif

#ifdef DBGWDF
#define WDF_STUB_SIZE 22

static uint8_t *wdf_cur_page;
static int wdf_cur_slot;

static void wdf_stub(int num) {
    log_error("Unresolved WDF function %d called!", num);
    abort();
}

void *create_wdf_stub(int num) {
    //Obtain a stub slot
    if(!wdf_cur_page || wdf_cur_slot*WDF_STUB_SIZE + WDF_STUB_SIZE > STUB_PAGE_SIZE) {
        wdf_cur_page = (uint8_t*) mmap(NULL, STUB_PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if(!wdf_cur_page) {
            perror("Couldn't allocate WDF stub page");
            abort();
        }
        wdf_cur_slot = 0;
        log_debug("Allocated WDF stub page at %p", wdf_cur_page);
    }

    uint8_t *slot = wdf_cur_page + (wdf_cur_slot++)*WDF_STUB_SIZE;

    //Encode slot instructions
    encode_mov(slot + 00, 7, num);                      //mov rdi, <name>
    encode_mov(slot + 10, 0, (uint64_t) &wdf_stub);    //mov rax, <stub function>
    slot[20] = 0xff;                                    //jmp rax
    slot[21] = 0xc0 | (4 << 3) | (0 << 0);              //  - ModR/M          

    return slot;
}
#endif
