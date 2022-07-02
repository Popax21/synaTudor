#include <asm/prctl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "internal.h"

static struct {
    ULONG alloc_size;
    ULONG size;
    ULONG flags;
    ULONG debug_flags;
    //...
} process_params;

static struct {
    BOOLEAN inherited_addr_space, read_img_file_exec_opts, being_debugged, __pad1;
    HANDLE mutant;
    void *image_base_addr;
    void *peb_ldr_data;
    void *proc_params;
    void *sub_sys_data;
    HANDLE proc_heap;
    //...
} process_peb;

static __thread struct {
    void *cur_seh_frame;
    void *stack_base, *stack_limit;
    void *sub_system_tib, *fiber_data;
    void *arbitrary_data;
    void *teb_address;

    void *env_ptr;
    uintptr_t proc_id, thread_id;
    void *active_rpc;
    void *thread_local_storage;
    void *peb_ptr;
    //...
} thread_tib;

__constr static void init_peb() {
    //TODO This is incredibly hacky and crude

    //Initialize the process parameters
    process_params.alloc_size = 0;
    process_params.size = 0;
    process_params.flags = 0;
    process_params.debug_flags = 0;

    //Initialize the Process Environment Block
    process_peb.inherited_addr_space = FALSE;
    process_peb.read_img_file_exec_opts = TRUE;
    process_peb.being_debugged = FALSE;
    process_peb.mutant = (HANDLE) 0xdeadc0de;
    process_peb.image_base_addr = (void*) 0xdeadc0de;
    process_peb.peb_ldr_data = (void*) 0xdeadc0de;
    process_peb.proc_params = &process_params;
    process_peb.sub_sys_data = (void*) 0xdeadc0de;
    process_peb.proc_heap = (HANDLE) -1;
}

__constr void win_init_tib() {
    //TODO This is incredibly hacky and crude

    //Initialize the Thread Information Block
    thread_tib.cur_seh_frame = (void*) 0xdeadc0de;
    thread_tib.stack_base = (void*) 0xdeadc0de;
    thread_tib.stack_limit = (void*) 0xdeadc0de;
    thread_tib.sub_system_tib = (void*) 0xdeadc0de;
    thread_tib.fiber_data = (void*) 0xdeadc0de;
    thread_tib.arbitrary_data = (void*) 0xdeadc0de;
    thread_tib.teb_address = (void*) 0xdeadc0de;

    thread_tib.env_ptr = (void*) 0xdeadc0de;
    thread_tib.proc_id = getpid();
    thread_tib.thread_id = win_get_thread_id();
    thread_tib.active_rpc = (void*) 0xdeadc0de;
    thread_tib.thread_local_storage = (void*) 0xdeadc0de;
    thread_tib.peb_ptr = &process_peb;
    syscall(SYS_arch_prctl, ARCH_SET_GS, &thread_tib);
}

DWORD win_get_thread_id() { return (DWORD) syscall(__NR_gettid); }

__winfnc HANDLE GetCurrentProcess() {
    return (HANDLE) -1;
}
WINAPI(GetCurrentProcess)

__winfnc DWORD GetCurrentProcessId() {
    return getpid();
}
WINAPI(GetCurrentProcessId)

__winfnc DWORD GetCurrentThreadId() {
    return win_get_thread_id();
}
WINAPI(GetCurrentThreadId)

__winfnc BOOL TerminateProcess(HANDLE proc, UINT exit_code) {
    if(proc != (HANDLE) -1) return FALSE;
    log_error("TerminateProcess called with exit code 0x%x!", exit_code);
    abort();
}
WINAPI(TerminateProcess)

typedef struct {
    DWORD cb;
    const char16_t *lpReserved;
    const char16_t *lpDesktop;
    const char16_t *lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    BYTE *lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOW;

__winfnc void GetStartupInfoW(STARTUPINFOW *info) {
    static char16_t *desktop = NULL;
    if(!desktop) desktop = winstr_from_str("TUDORHOST");

    info->cb = sizeof(STARTUPINFOW);
    info->lpReserved = NULL;
    info->lpDesktop = desktop;
    info->lpTitle = NULL;
    info->dwX = info->dwY = 0;
    info->dwXSize = info->dwYSize = 0;
    info->dwXCountChars = info->dwYCountChars = 0;
    info->dwFillAttribute = 0;
    info->dwFlags = 0;
    info->wShowWindow = 0;
    info->cbReserved2 = 0;
    info->lpReserved2 = NULL;
    info->hStdInput = info->hStdOutput = info->hStdError = NULL;
}
WINAPI(GetStartupInfoW);

__winfnc const char *GetCommandLineA() {
    return winmodule_get_cur()->cmdline;
}
WINAPI(GetCommandLineA)

__winfnc const char16_t *GetCommandLineW() {
    static char16_t *cmd_line;
    if(!cmd_line) cmd_line = winstr_from_str(GetCommandLineA());
    return cmd_line;
}
WINAPI(GetCommandLineW)

__winfnc const char *GetEnvironmentStringsA() {
    //Determine environment block size
    int len = 1;
    for(const char **p = winmodule_get_cur()->environ; *p; p++) len += strlen(*p) + 1;

    //Create environment block
    char *env = (char*) malloc(len);
    if(!env) { winerr_set_errno(); return NULL; }

    char *d = env;
    for(const char **p = winmodule_get_cur()->environ; *p; p++) {
        int l = strlen(*p);
        memcpy(d, *p, l+1);
        d += l + 1;
    }
    *d = 0;

    return env;
}
WINAPI(GetEnvironmentStringsA)

__winfnc BOOL FreeEnvironmentStringsA(const char *env) {
    free((void*) env);
    return TRUE;
}
WINAPI(FreeEnvironmentStringsA)

__winfnc const char16_t *GetEnvironmentStringsW() {
    //Determine environment block size
    int len = 1;
    for(const char **p = winmodule_get_cur()->environ; *p; p++) len += strlen(*p) + 1;

    //Create environment block
    char16_t *env = (char16_t*) malloc(len * sizeof(char16_t));
    if(!env) { winerr_set_errno(); return NULL; }

    char16_t *d = env;
    for(const char **p = winmodule_get_cur()->environ; *p; p++) {
        char16_t *wp = winstr_from_str(*p);
        int l = winstr_len(wp);
        memcpy(d, *p, (l+1) * sizeof(char16_t));
        d += l + 1;
        free(wp);
    }
    *d = 0;

    return env;
}
WINAPI(GetEnvironmentStringsW)

__winfnc BOOL FreeEnvironmentStringsW(const char16_t *env) {
    free((void*) env);
    return TRUE;
}
WINAPI(FreeEnvironmentStringsW)

__winfnc BOOL IsProcessorFeaturePresent(DWORD feature) {
    log_debug("IsProcessorFeaturePresent | feature: %d", feature);
    return FALSE;
}
WINAPI(IsProcessorFeaturePresent)

#define ARM64_MAX_BREAKPOINTS 8
#define ARM64_MAX_WATCHPOINTS 2

typedef union {
    struct {
        ULONGLONG Low;
        LONGLONG High;
    } DUMMYSTRUCTNAME;
    double D[2];
    float S[4];
    WORD H[8];
    BYTE B[16];
} ARM64_NT_NEON128;

typedef struct {
    DWORD ContextFlags;
    DWORD Cpsr;
    union {
        struct {
            DWORD64 X0;
            DWORD64 X1;
            DWORD64 X2;
            DWORD64 X3;
            DWORD64 X4;
            DWORD64 X5;
            DWORD64 X6;
            DWORD64 X7;
            DWORD64 X8;
            DWORD64 X9;
            DWORD64 X10;
            DWORD64 X11;
            DWORD64 X12;
            DWORD64 X13;
            DWORD64 X14;
            DWORD64 X15;
            DWORD64 X16;
            DWORD64 X17;
            DWORD64 X18;
            DWORD64 X19;
            DWORD64 X20;
            DWORD64 X21;
            DWORD64 X22;
            DWORD64 X23;
            DWORD64 X24;
            DWORD64 X25;
            DWORD64 X26;
            DWORD64 X27;
            DWORD64 X28;
            DWORD64 Fp;
            DWORD64 Lr;
        } DUMMYSTRUCTNAME;
        DWORD64 X[31];
    } DUMMYUNIONNAME;
    DWORD64 Sp;
    DWORD64 Pc;
    ARM64_NT_NEON128 V[32];
    DWORD Fpcr;
    DWORD Fpsr;
    DWORD Bcr[ARM64_MAX_BREAKPOINTS];
    DWORD64 Bvr[ARM64_MAX_BREAKPOINTS];
    DWORD Wcr[ARM64_MAX_WATCHPOINTS];
    DWORD64 Wvr[ARM64_MAX_WATCHPOINTS];
} ARM64_NT_CONTEXT;

__winfnc void RtlCaptureContext(ARM64_NT_CONTEXT *context) {
    log_warn("Unsupported function RtlCaptureContext called!");
    *context = (ARM64_NT_CONTEXT) {0};
}
WINAPI(RtlCaptureContext)

__winfnc void *RtlLookupFunctionEntry(DWORD64 pc, DWORD64 *image_base, void *history) {
    return NULL;
}
WINAPI(RtlLookupFunctionEntry)

__winfnc BOOL IsDebuggerPresent() {
    return FALSE;
}
WINAPI(IsDebuggerPresent)

typedef LONG __winfnc TOP_LEVEL_EXCEPTION_FILTER(void *ExceptionPointers);

static TOP_LEVEL_EXCEPTION_FILTER *excep_filter = NULL;

__winfnc TOP_LEVEL_EXCEPTION_FILTER *SetUnhandledExceptionFilter(TOP_LEVEL_EXCEPTION_FILTER *filter) {
    TOP_LEVEL_EXCEPTION_FILTER *old_filter = excep_filter;
    excep_filter = filter;
    return old_filter;
}
WINAPI(SetUnhandledExceptionFilter)

__winfnc LONG UnhandledExceptionFilter(void *excep_pointers) {
    if(excep_filter) {
        LONG ret = excep_filter(excep_pointers);
        if(ret == 0 || ret == 1) return ret;
    }
    return 1;
}
WINAPI(UnhandledExceptionFilter)

__winfnc void Sleep(DWORD num_ms) {
    cant_fail(usleep((useconds_t) num_ms * 1000));
}
WINAPI(Sleep)