/*
 * Additional WinAPI shims for v132 Synaptics driver DLLs.
 * These functions are imported by the Kensington VeriMark Desktop driver
 * but were not needed for the v104 driver.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include "internal.h"

#define WM_QUIT 0x0012

typedef struct {
    HANDLE hwnd;
    UINT message;
    ULONG_PTR wParam;
    LONG_PTR lParam;
    DWORD time;
    struct {
        LONG x;
        LONG y;
    } pt;
} win_msg;

struct queued_msg {
    win_msg msg;
    struct queued_msg *next;
};

struct msg_queue {
    DWORD thread_id;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    struct queued_msg *head;
    struct queued_msg *tail;
    struct msg_queue *next;
};

static pthread_mutex_t msg_queues_lock = PTHREAD_MUTEX_INITIALIZER;
static struct msg_queue *msg_queues;

static struct msg_queue *get_msg_queue(DWORD thread_id, bool create) {
    cant_fail_ret(pthread_mutex_lock(&msg_queues_lock));

    struct msg_queue *queue = msg_queues;
    while(queue && queue->thread_id != thread_id) queue = queue->next;

    if(!queue && create) {
        queue = calloc(1, sizeof(*queue));
        if(!queue) {
            pthread_mutex_unlock(&msg_queues_lock);
            winerr_set_errno();
            return NULL;
        }

        queue->thread_id = thread_id;
        cant_fail_ret(pthread_mutex_init(&queue->lock, NULL));
        cant_fail_ret(pthread_cond_init(&queue->cond, NULL));
        queue->next = msg_queues;
        msg_queues = queue;
    }

    cant_fail_ret(pthread_mutex_unlock(&msg_queues_lock));
    return queue;
}

/* OutputDebugStringA - debug output, just forward to log */
__winfnc void OutputDebugStringA(const char *str) {
    if(str) log_debug("OutputDebugStringA: %s", str);
}
WINAPI(OutputDebugStringA)

__winfnc HANDLE GetTraceLoggerHandle(void *buffer) { return (HANDLE) 1; }
WINAPI(GetTraceLoggerHandle)

__winfnc UCHAR GetTraceEnableLevel(HANDLE handle) { return 0xff; }
WINAPI(GetTraceEnableLevel)

__winfnc ULONG GetTraceEnableFlags(HANDLE handle) { return 0xffff; }
WINAPI(GetTraceEnableFlags)

__winfnc BOOL WTSRegisterSessionNotification(HANDLE hwnd, DWORD flags) {
    log_debug("WTSRegisterSessionNotification (stub)");
    return TRUE;
}
WINAPI(WTSRegisterSessionNotification)

__winfnc BOOL WTSUnRegisterSessionNotification(HANDLE hwnd) {
    log_debug("WTSUnRegisterSessionNotification (stub)");
    return TRUE;
}
WINAPI(WTSUnRegisterSessionNotification)

__winfnc HANDLE CreateWindowExW(DWORD exStyle, const char16_t *className, const char16_t *windowName,
    DWORD style, int x, int y, int width, int height, HANDLE parent, HANDLE menu, HANDLE instance, void *param) {
    log_debug("CreateWindowExW (stub)");
    return winhandle_create(NULL, NULL);
}
WINAPI(CreateWindowExW)

__winfnc BOOL DestroyWindow(HANDLE hwnd) {
    log_debug("DestroyWindow (stub)");
    winhandle_destroy(hwnd);
    return TRUE;
}
WINAPI(DestroyWindow)

__winfnc BOOL GetMessageW(void *msg, HANDLE hwnd, UINT min, UINT max) {
    (void)hwnd;
    (void)min;
    (void)max;

    DWORD thread_id = win_get_thread_id();
    struct msg_queue *queue = get_msg_queue(thread_id, true);
    if(!queue) return FALSE;

    cant_fail_ret(pthread_mutex_lock(&queue->lock));
    while(!queue->head) cant_fail_ret(pthread_cond_wait(&queue->cond, &queue->lock));

    struct queued_msg *node = queue->head;
    queue->head = node->next;
    if(!queue->head) queue->tail = NULL;
    cant_fail_ret(pthread_mutex_unlock(&queue->lock));

    if(msg) memcpy(msg, &node->msg, sizeof(node->msg));
    UINT message = node->msg.message;
    free(node);

    log_debug("GetMessageW(thread=%u) -> msg=0x%x", thread_id, message);
    return message == WM_QUIT ? FALSE : TRUE;
}
WINAPI(GetMessageW)

__winfnc BOOL TranslateMessage(const void *msg) { return TRUE; }
WINAPI(TranslateMessage)

__winfnc LONG_PTR DispatchMessageW(const void *msg) { return 0; }
WINAPI(DispatchMessageW)

__winfnc BOOL PostThreadMessageW(DWORD threadId, UINT msg, ULONG_PTR wparam, LONG_PTR lparam) {
    struct msg_queue *queue = get_msg_queue(threadId, true);
    if(!queue) return FALSE;

    struct queued_msg *node = calloc(1, sizeof(*node));
    if(!node) {
        winerr_set_errno();
        return FALSE;
    }

    node->msg.message = msg;
    node->msg.wParam = wparam;
    node->msg.lParam = lparam;

    cant_fail_ret(pthread_mutex_lock(&queue->lock));
    if(queue->tail) queue->tail->next = node;
    else queue->head = node;
    queue->tail = node;
    cant_fail_ret(pthread_cond_signal(&queue->cond));
    cant_fail_ret(pthread_mutex_unlock(&queue->lock));

    log_debug("PostThreadMessageW(thread=%u, msg=0x%x)", threadId, msg);
    return TRUE;
}
WINAPI(PostThreadMessageW)

__winfnc HANDLE RegisterPowerSettingNotification(HANDLE recipient, const GUID *guid, DWORD flags) {
    log_debug("RegisterPowerSettingNotification (stub)");
    return winhandle_create(NULL, NULL);
}
WINAPI(RegisterPowerSettingNotification)

__winfnc BOOL UnregisterPowerSettingNotification(HANDLE handle) {
    log_debug("UnregisterPowerSettingNotification (stub)");
    winhandle_destroy(handle);
    return TRUE;
}
WINAPI(UnregisterPowerSettingNotification)

__winfnc LONG_PTR DefWindowProcW(HANDLE hwnd, UINT msg, ULONG_PTR wparam, LONG_PTR lparam) { return 0; }
WINAPI(DefWindowProcW)

__winfnc LONG_PTR CallWindowProcW(void *prev, HANDLE hwnd, UINT msg, ULONG_PTR wparam, LONG_PTR lparam) { return 0; }
WINAPI(CallWindowProcW)

__winfnc USHORT RegisterClassExW(const void *classInfo) {
    log_debug("RegisterClassExW (stub)");
    return 1;
}
WINAPI(RegisterClassExW)

__winfnc BOOL UnregisterClassW(const char16_t *className, HANDLE instance) {
    log_debug("UnregisterClassW (stub)");
    return TRUE;
}
WINAPI(UnregisterClassW)

__winfnc LONG_PTR SetWindowLongPtrW(HANDLE hwnd, int index, LONG_PTR value) { return 0; }
WINAPI(SetWindowLongPtrW)

__winfnc LONG_PTR GetWindowLongPtrW(HANDLE hwnd, int index) { return 0; }
WINAPI(GetWindowLongPtrW)

__winfnc HANDLE BeginPaint(HANDLE hwnd, void *paint) { return NULL; }
WINAPI(BeginPaint)

__winfnc BOOL EndPaint(HANDLE hwnd, const void *paint) { return TRUE; }
WINAPI(EndPaint)

__winfnc void RtlUnwind(void *target_frame, void *target_ip, void *exception_record, void *return_value) {
    log_debug("RtlUnwind (stub)");
}
WINAPI(RtlUnwind)

/* lstrcpyA */
__winfnc char *lstrcpyA(char *dst, const char *src) {
    if(!dst || !src) return NULL;
    strcpy(dst, src);
    return dst;
}
WINAPI(lstrcpyA)

/* EncodePointer / DecodePointer - XOR obfuscation, no-op on Linux */
__winfnc void *EncodePointer(void *ptr) {
    return ptr;
}
WINAPI(EncodePointer)

/* RtlVirtualUnwind - SEH unwinding, stub */
__winfnc void *RtlVirtualUnwind(DWORD type, DWORD64 base, DWORD64 pc,
    void *entry, void *ctx, void **handler_data, DWORD64 *establisher_frame,
    void *context_ptrs)
{
    return NULL;
}
WINAPI(RtlVirtualUnwind)

/* RtlUnwindEx - SEH, stub */
__winfnc void RtlUnwindEx(void *target_frame, void *target_ip,
    void *exception_record, void *return_value, void *context, void *history)
{
    log_warn("RtlUnwindEx called - SEH unwinding not supported");
}
WINAPI(RtlUnwindEx)

/* RtlPcToFileHeader */
__winfnc void *RtlPcToFileHeader(void *pc_value, void **base_of_image) {
    if(base_of_image) *base_of_image = NULL;
    return NULL;
}
WINAPI(RtlPcToFileHeader)

/* RaiseException */
__winfnc void RaiseException(DWORD code, DWORD flags, DWORD nargs, const ULONG_PTR *args) {
    /* 0x406D1388 is the "set thread name" exception - harmless */
    if(code == 0x406D1388) return;
    log_error("RaiseException called! code=0x%x flags=0x%x", code, flags);
    abort();
}
WINAPI(RaiseException)

/* ExitProcess */
__winfnc void ExitProcess(UINT code) {
    log_error("ExitProcess called with code %u!", code);
    exit(code);
}
WINAPI(ExitProcess)

/* TerminateThread */
__winfnc BOOL TerminateThread(HANDLE thread, DWORD exit_code) {
    log_warn("TerminateThread called (exit_code=%u) - stub", exit_code);
    return TRUE;
}
WINAPI(TerminateThread)

/* FreeLibraryAndExitThread */
__winfnc void FreeLibraryAndExitThread(HANDLE module, DWORD exit_code) {
    log_debug("FreeLibraryAndExitThread called");
    pthread_exit(NULL);
}
WINAPI(FreeLibraryAndExitThread)

/* Heap extensions */
__winfnc size_t HeapSize(HANDLE heap, DWORD flags, const void *mem) {
    return (size_t)-1;
}
WINAPI(HeapSize)

__winfnc void *HeapReAlloc(HANDLE heap, DWORD flags, void *mem, size_t size) {
    void *ptr = realloc(mem, size);
    if(!ptr && (flags & 0x00000008)) /* HEAP_ZERO_MEMORY */
        ptr = calloc(1, size);
    return ptr;
}
WINAPI(HeapReAlloc)

/* Console stubs - driver runs headless */
__winfnc BOOL GetConsoleMode(HANDLE console, DWORD *mode) { return FALSE; }
WINAPI(GetConsoleMode)

__winfnc BOOL SetConsoleMode(HANDLE console, DWORD mode) { return FALSE; }
WINAPI(SetConsoleMode)

__winfnc UINT GetConsoleCP() { return 65001; }
WINAPI(GetConsoleCP)

__winfnc BOOL WriteConsoleW(HANDLE con, const void *buf, DWORD chars, DWORD *written, void *res) {
    if(written) *written = chars;
    return TRUE;
}
WINAPI(WriteConsoleW)

__winfnc BOOL ReadConsoleW(HANDLE con, void *buf, DWORD chars, DWORD *read, void *ctrl) { return FALSE; }
WINAPI(ReadConsoleW)

__winfnc BOOL PeekConsoleInputA(HANDLE con, void *buf, DWORD len, DWORD *ev) {
    if(ev) *ev = 0;
    return FALSE;
}
WINAPI(PeekConsoleInputA)

__winfnc BOOL ReadConsoleInputW(HANDLE con, void *buf, DWORD len, DWORD *ev) {
    if(ev) *ev = 0;
    return FALSE;
}
WINAPI(ReadConsoleInputW)

__winfnc BOOL GetNumberOfConsoleInputEvents(HANDLE con, DWORD *ev) {
    if(ev) *ev = 0;
    return FALSE;
}
WINAPI(GetNumberOfConsoleInputEvents)

/* File system stubs */
__winfnc BOOL CreateDirectoryA(const char *path, void *attrs) {
    if(!path) return FALSE;
    if(mkdir(path, 0755) == 0 || errno == EEXIST) return TRUE;
    winerr_set_errno();
    return FALSE;
}
WINAPI(CreateDirectoryA)

__winfnc DWORD GetCurrentDirectoryA(DWORD size, char *buf) {
    if(!getcwd(buf, size)) return 0;
    return strlen(buf);
}
WINAPI(GetCurrentDirectoryA)

__winfnc BOOL SetEndOfFile(HANDLE file) { return TRUE; }
WINAPI(SetEndOfFile)

__winfnc BOOL FlushFileBuffers(HANDLE file) { return TRUE; }
WINAPI(FlushFileBuffers)

__winfnc DWORD GetFileType(HANDLE file) { return 0x0001; }
WINAPI(GetFileType)

__winfnc BOOL GetFileSizeEx(HANDLE file, LARGE_INTEGER *size) {
    if(size) size->QuadPart = 0;
    return FALSE;
}
WINAPI(GetFileSizeEx)

__winfnc BOOL SetFilePointerEx(HANDLE file, LARGE_INTEGER dist, LARGE_INTEGER *new_pos, DWORD method) {
    return FALSE;
}
WINAPI(SetFilePointerEx)

__winfnc HANDLE SetStdHandle(DWORD std_handle, HANDLE handle) { return (HANDLE)-1; }
WINAPI(SetStdHandle)

/* Locale / string stubs */
__winfnc UINT GetOEMCP() { return 437; }
WINAPI(GetOEMCP)

__winfnc int CompareStringW(DWORD locale, DWORD flags, const char16_t *s1, int len1, const char16_t *s2, int len2) {
    if(!s1 && !s2) return 2;
    if(!s1) return 1;
    if(!s2) return 3;
    int i = 0;
    while(1) {
        if(len1 == 0 && len2 == 0) return 2;
        if(i == len1 || (len1 < 0 && !s1[i])) return (i == len2 || (len2 < 0 && !s2[i])) ? 2 : 1;
        if(i == len2 || (len2 < 0 && !s2[i])) return 3;
        if(s1[i] < s2[i]) return 1;
        if(s1[i] > s2[i]) return 3;
        i++;
    }
}
WINAPI(CompareStringW)

__winfnc DWORD GetTimeZoneInformation(void *tzi) {
    if(tzi) memset(tzi, 0, 172);
    return 0;
}
WINAPI(GetTimeZoneInformation)

__winfnc BOOL SetEnvironmentVariableW(const char16_t *name, const char16_t *value) { return TRUE; }
WINAPI(SetEnvironmentVariableW)

/* Find file stubs (CRT internal use) */
__winfnc HANDLE FindFirstFileExW(const char16_t *name, int level, void *data, int type, void *filter, DWORD flags) {
    return INVALID_HANDLE_VALUE;
}
WINAPI(FindFirstFileExW)

__winfnc BOOL FindNextFileW(HANDLE find, void *data) { return FALSE; }
WINAPI(FindNextFileW)

__winfnc HANDLE FindFirstFileA(const char *name, void *data) { return INVALID_HANDLE_VALUE; }
WINAPI(FindFirstFileA)

__winfnc BOOL FindNextFileA(HANDLE find, void *data) { return FALSE; }
WINAPI(FindNextFileA)

__winfnc BOOL FindClose(HANDLE find) { return TRUE; }
WINAPI(FindClose)

/* File operations */
__winfnc BOOL DeleteFileW(const char16_t *path) { return FALSE; }
WINAPI(DeleteFileW)

__winfnc BOOL MoveFileExW(const char16_t *src, const char16_t *dst, DWORD flags) { return FALSE; }
WINAPI(MoveFileExW)

__winfnc HANDLE CreateFileA(const char *name, DWORD access, DWORD share, void *sec, DWORD disp, DWORD flags, HANDLE tmpl) {
    log_info("CreateFileA('%s', access=0x%x, flags=0x%x)", name ? name : "(null)", access, flags);
    /* Return a dummy handle — WinUsb_Initialize uses it */
    return winhandle_create(NULL, NULL);
}
WINAPI(CreateFileA)

__winfnc HANDLE CreateFileW(const char16_t *name, DWORD access, DWORD share, void *sec, DWORD disp, DWORD flags, HANDLE tmpl) {
    log_info("CreateFileW(access=0x%x, flags=0x%x)", access, flags);
    return winhandle_create(NULL, NULL);
}
WINAPI(CreateFileW)

__winfnc BOOL CancelIo(HANDLE file) { return TRUE; }
WINAPI(CancelIo)

/* Security API stubs — driver sets up ACLs during deep init */
typedef struct {
    BYTE Revision;
    BYTE SubAuthorityCount;
    BYTE IdentifierAuthority[6];
    DWORD SubAuthority[];
} SID;

/* Note: this is __winfnc (ms_abi) with 11 params. Params 5+ (a4-a7, pSid) are on the stack.
   The stack layout for ms_abi with >4 params: rcx, rdx, r8, r9, [rsp+0x20], [rsp+0x28], ... */
__winfnc BOOL AllocateAndInitializeSid(void *auth, BYTE count,
    DWORD a0, DWORD a1, DWORD a2, DWORD a3,
    DWORD a4, DWORD a5, DWORD a6, DWORD a7, void **pSid) {
    log_info("AllocateAndInitializeSid CALLED (auth=%p, count=%d, pSid=%p)", auth, count, pSid);
    if(!pSid) return FALSE;
    if(count > 8) count = 8;
    SID *sid = (SID*)calloc(1, sizeof(SID) + 8 * sizeof(DWORD));
    if(!sid) { *pSid = NULL; return FALSE; }
    sid->Revision = 1;
    sid->SubAuthorityCount = count;
    if(auth) memcpy(sid->IdentifierAuthority, auth, 6);
    DWORD subs[] = {a0,a1,a2,a3,a4,a5,a6,a7};
    for(int i = 0; i < count; i++) sid->SubAuthority[i] = subs[i];
    *pSid = sid;
    return TRUE;
}
WINAPI(AllocateAndInitializeSid)

__winfnc void *FreeSid(SID *sid) {
    free(sid);
    return NULL;
}
WINAPI(FreeSid)

__winfnc BOOL InitializeSecurityDescriptor(void *sd, DWORD rev) {
    log_debug("InitializeSecurityDescriptor (stub)");
    if(sd) memset(sd, 0, 40); /* SECURITY_DESCRIPTOR is ~40 bytes */
    return TRUE;
}
WINAPI(InitializeSecurityDescriptor)

__winfnc BOOL SetSecurityDescriptorDacl(void *sd, BOOL present, void *acl, BOOL defaulted) {
    log_debug("SetSecurityDescriptorDacl (stub)");
    return TRUE;
}
WINAPI(SetSecurityDescriptorDacl)

__winfnc DWORD SetEntriesInAclA(ULONG count, void *entries, void *oldAcl, void **newAcl) {
    log_debug("SetEntriesInAclA (stub)");
    if(newAcl) *newAcl = NULL;
    return 0; /* ERROR_SUCCESS */
}
WINAPI(SetEntriesInAclA)

/* CfgMgr32 stubs — driver uses these to find USB device interface paths.
   The DLL's PAL layer calls these to enumerate USB device interfaces, then
   passes the path to CreateFile → WinUsb_Initialize. We return a fake
   Windows-style device path so the DLL proceeds to open the device. */
static const char fake_dev_path[] = "\\\\?\\USB#VID_047D&PID_00F2&MI_01#TUDOR#{a5dcbf10-6530-11d2-901f-00c04fb951ed}";

__winfnc DWORD CM_Get_Device_Interface_List_SizeA(DWORD *size, void *guid, const char *devid, DWORD flags) {
    log_info("CM_Get_Device_Interface_List_SizeA(devid='%s')", devid ? devid : "(null)");
    /* Return size for our fake device path + double null terminator */
    if(size) *size = (DWORD)(strlen(fake_dev_path) + 2);
    return 0; /* CR_SUCCESS */
}
WINAPI(CM_Get_Device_Interface_List_SizeA)

__winfnc DWORD CM_Get_Device_Interface_ListA(void *guid, const char *devid, char *buf, DWORD buflen, DWORD flags) {
    log_info("CM_Get_Device_Interface_ListA(devid='%s', buflen=%u)", devid ? devid : "(null)", buflen);
    DWORD needed = (DWORD)(strlen(fake_dev_path) + 2);
    if(buf && buflen >= needed) {
        memcpy(buf, fake_dev_path, strlen(fake_dev_path));
        buf[strlen(fake_dev_path)] = '\0';     /* null-terminate path */
        buf[strlen(fake_dev_path) + 1] = '\0'; /* double-null = end of list */
    }
    return 0; /* CR_SUCCESS */
}
WINAPI(CM_Get_Device_Interface_ListA)

__winfnc DWORD CM_Locate_DevNodeW(DWORD *devInst, const char16_t *devid, DWORD flags) {
    log_debug("CM_Locate_DevNodeW (stub)");
    if(devInst) *devInst = 1; /* dummy device instance */
    return 0; /* CR_SUCCESS */
}
WINAPI(CM_Locate_DevNodeW)

__winfnc DWORD CM_Get_Device_Interface_PropertyW(const char16_t *iface, void *key, DWORD *type, void *buf, DWORD *size, DWORD flags) {
    log_debug("CM_Get_Device_Interface_PropertyW (stub)");
    return 13; /* CR_NO_SUCH_VALUE */
}
WINAPI(CM_Get_Device_Interface_PropertyW)

__winfnc DWORD CM_Setup_DevNode(DWORD devInst, DWORD flags) {
    log_debug("CM_Setup_DevNode (stub)");
    return 0;
}
WINAPI(CM_Setup_DevNode)

__winfnc DWORD CM_Query_And_Remove_SubTreeA(DWORD devInst, void *veto, char *name, DWORD len, DWORD flags) {
    log_debug("CM_Query_And_Remove_SubTreeA (stub)");
    return 0;
}
WINAPI(CM_Query_And_Remove_SubTreeA)

/* Registry extension */
__winfnc LONG RegDeleteValueA(HANDLE key, const char *name) {
    log_debug("RegDeleteValueA: %s (stub)", name ? name : "(null)");
    return 0;
}
WINAPI(RegDeleteValueA)

/* WBFUsbInitialize caller — invoked via resolve_windows_api after patches active */
#include "tudor/internal.h"
extern void *com_usb_device_obj;
typedef DWORD __winfnc (*wbf_driver_fn)(void *self);
extern int tudor_wbf_done;
__winfnc void tudor_call_wbf_usb_init(void) {
    if(!com_usb_device_obj || !tudor_driver_dll) return;
    uint8_t *img = tudor_driver_dll->image.base_addr;
    if(!img) return;
    /* Only call when patches are active */
    if(img[0x929b] != 0xEB || img[0x161bb] != 0x41) return;
    tudor_wbf_done = 1; /* Prevent further calls */
    winmodule_set_cur(&tudor_driver_dll->module);
    log_info("Calling WBFUsbInitialize (patches active)...");
    DWORD hr = ((wbf_driver_fn)(img + 0x16160))(com_usb_device_obj);
    log_info("WBFUsbInitialize returned 0x%x", hr);
}
WINAPI(tudor_call_wbf_usb_init)
