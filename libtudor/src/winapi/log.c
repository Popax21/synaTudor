#include <stdlib.h>
#include <pthread.h>
#include "internal.h"

void winlog_printf(const char *format, bool ptr_mode, win_va_list va) {
    for(const char *p = format; *p; p++) {
        char c = *p;
        switch(c) {
            case '%': {
                //Determine format specifier attributes
                int frmt_len = 1;
                for(; *(p + frmt_len); frmt_len++) {
                    char nc = *(p + frmt_len);
                    switch(nc) {
                        case 'd':
                        case 'u':
                        case 'x':
                        case 'p':
                        case 's':
                        case '%': break;

                        default: printf("[Unknown format specifier %c]", nc); break;
                    }
                    break;
                }

                //Print format specifier
                switch(*(p + frmt_len)) {
                    case 'd': {
                        if(!ptr_mode) printf("%d", win_va_arg(va, int)); 
                        else {
                            printf("%d", *win_va_arg(va, int*));
                            win_va_arg(va, size_t);
                        }
                    } break;
                    case 'u': {
                        if(!ptr_mode) printf("%u", win_va_arg(va, unsigned int)); 
                        else {
                            printf("%u", *win_va_arg(va, unsigned int*));
                            win_va_arg(va, size_t);
                        }
                    } break;
                    case 'x': {
                        if(!ptr_mode) printf("%x", win_va_arg(va, unsigned int)); 
                        else {
                            printf("%x", *win_va_arg(va, unsigned int*));
                            win_va_arg(va, size_t);
                        }
                    } break;
                    case 'p': {
                        printf("%p", win_va_arg(va, void*));
                        if(ptr_mode) win_va_arg(va, size_t);  
                    } break;
                    case 's': {
                        printf("%s", win_va_arg(va, char*));
                        if(ptr_mode) win_va_arg(va, size_t);  
                    } break;
                    case '%': putc('%', stdout); break;
                }

                p += frmt_len;
            } break;
            default: putc(c, stdout);
        }
    }
}

static pthread_rwlock_t trace_msgs_lock = PTHREAD_RWLOCK_INITIALIZER;
static struct trace_msg {
    struct trace_msg *next;

    GUID guid;
    int msg_num;
    const char *format;
} *trace_msgs_head = NULL;

void winlog_register_trace_msg(GUID guid, int num, const char *format) {
    //Create message struct
    struct trace_msg *msg = (struct trace_msg*) malloc(sizeof(struct trace_msg));
    if(!msg) { perror("Couldn't allocate memory for trace message"); abort(); }

    msg->guid = guid;
    msg->msg_num = num;
    msg->format = format;

    //Add to message list
    cant_fail_ret(pthread_rwlock_wrlock(&trace_msgs_lock));
    msg->next = trace_msgs_head;
    trace_msgs_head = msg;
    cant_fail_ret(pthread_rwlock_unlock(&trace_msgs_lock));
}

void winlog_trace(GUID guid, int num, win_va_list vas) {
    //Find the trace message
    cant_fail_ret(pthread_rwlock_rdlock(&trace_msgs_lock));

    struct trace_msg *msg = NULL;
    for(struct trace_msg *m = trace_msgs_head; m != NULL; m = m->next) {
        if(memcmp(&m->guid, &guid, sizeof(GUID)) == 0 && m->msg_num == num) {
            msg = m;
            break;
        }
    }

    cant_fail_ret(pthread_rwlock_unlock(&trace_msgs_lock));

    //Log the message
    if(!msg) return;

    cant_fail_ret(pthread_mutex_lock(&LOG_LOCK));
    printf("[TRACE][%u][%x...][%02x] ", win_get_thread_id(), guid.PartA, num);
    winlog_printf(msg->format, true, vas);
    printf("\n");
    cant_fail_ret(pthread_mutex_unlock(&LOG_LOCK));
}

__winfnc ULONG DbgPrintEx(ULONG comp_id, ULONG level, const char *format, ...) {
    win_va_list vas;
    win_va_start(vas, format);
    cant_fail_ret(pthread_mutex_lock(&LOG_LOCK));
    printf("[DbgPrintEx] ");
    winlog_printf(format, false, vas);
    cant_fail_ret(pthread_mutex_unlock(&LOG_LOCK));
    win_va_end(vas);
    return 0;
}
WINAPI(DbgPrintEx)

#define WMI_ENABLE_EVENTS 4
#define WMI_DISABLE_EVENTS 5

typedef struct {
    GUID Guid;
    HANDLE RegHandle;
} TRACE_GUID_REGISTRATION;

typedef ULONG Wmidprequest(DWORD RequestCode, void *RequestContext, ULONG *BufferSize, void *Buffer);

struct trace_guid {
    struct trace_provider *prov;
    GUID guid;
    HANDLE handle;
};

struct trace_provider {
    GUID control_guid;
    Wmidprequest *request_fnc;
    int num_guids;
    struct trace_guid *guids;
};

static void trace_prov_destr(struct trace_provider *prov) {
    for(int i = 0; i < prov->num_guids; i++) winhandle_destroy(prov->guids[i].handle);
    free(prov->guids);
    free(prov);
}

__winfnc DWORD RegisterTraceGuidsA(Wmidprequest *request_fnc, void *request_ctx, GUID *control_guid, ULONG num_guids, TRACE_GUID_REGISTRATION *trace_guids, const char *mof_image, const char *mof_resource, HANDLE *handle) {
    //Allocate trace provider
    struct trace_provider *prov = (struct trace_provider*) malloc(sizeof(struct trace_provider));
    if(!prov) { return winerr_from_errno(); }

    prov->control_guid = *control_guid;
    prov->request_fnc = request_fnc;

    //Initialize GUIDs
    prov->num_guids = num_guids;
    prov->guids = (struct trace_guid*) malloc(num_guids * sizeof(struct trace_guid));
    if(!prov->guids) return winerr_from_errno();

    for(int i = 0; i < num_guids; i++) {
        prov->guids[i].prov = prov;
        prov->guids[i].guid = trace_guids[i].Guid;
        prov->guids[i].handle = winhandle_create(&prov->guids[i], NULL);
        trace_guids[i].RegHandle = prov->guids[i].handle;
    }

    *handle = winhandle_create(prov, (winhandle_destr_fnc*) trace_prov_destr);

    return ERROR_SUCCESS;
}
WINAPI(RegisterTraceGuidsA)

__winfnc DWORD RegisterTraceGuidsW(Wmidprequest *request_fnc, void *request_ctx, GUID *control_guid, ULONG num_guids, TRACE_GUID_REGISTRATION *trace_guids, const char16_t *mof_image, const char16_t *mof_resource, HANDLE *handle) {
    return RegisterTraceGuidsA(request_fnc, request_ctx, control_guid, num_guids, trace_guids, NULL, NULL, handle);
}
WINAPI(RegisterTraceGuidsW)

__winfnc DWORD UnregisterTraceGuids(HANDLE handle) {
    winhandle_destroy(handle);
    return ERROR_SUCCESS;
}
WINAPI(UnregisterTraceGuids)

__winfnc ULONG TraceMessage(HANDLE handle, ULONG flags, GUID *guid, USHORT num, ...) {
    win_va_list vas;
    win_va_start(vas, num);
    winlog_trace(*guid, num, vas);
    win_va_end(vas);
    return ERROR_SUCCESS;
}
WINAPI(TraceMessage)