#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "internal.h"

//pthread timed waits take an absolute CLOCK_REALTIME deadline
static void win_wait_deadline(struct timespec *time, DWORD timeout_ms) {
    cant_fail_ret(clock_gettime(CLOCK_REALTIME, time));
    time->tv_sec += timeout_ms / 1000;
    time->tv_nsec += (timeout_ms % 1000) * 1000000L;
    if(time->tv_nsec >= 1000000000L) {
        time->tv_sec++;
        time->tv_nsec -= 1000000000L;
    }
}

DWORD win_wait_sync_obj(HANDLE handle, DWORD timeout) {
    struct win_sync_object *obj = (struct win_sync_object*) handle->data;
    return obj->wait_fnc(obj, timeout);
}

typedef struct {
    void *DebugInfo;
    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;
    HANDLE LockSemaphore;
    DWORD SpinCount;
} CRITICAL_SECTION;

__winfnc BOOL InitializeCriticalSectionEx(CRITICAL_SECTION *sect, DWORD spinCount, DWORD flags) {
    memset(sect, 0, sizeof(CRITICAL_SECTION));

    sect->LockCount = 0;
    sect->RecursionCount = 0;
    sect->OwningThread = NULL;
    sect->LockSemaphore = malloc(sizeof(pthread_mutex_t));
    if(!sect->LockSemaphore) { winerr_set_errno(); return FALSE; }
    sect->SpinCount = spinCount;

    //Initialize the mutex
    pthread_mutexattr_t attr;
    cant_fail_ret(pthread_mutexattr_init(&attr));
    cant_fail_ret(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
    if(pthread_mutex_init((pthread_mutex_t*) sect->LockSemaphore, &attr) != 0) {
        cant_fail_ret(pthread_mutexattr_destroy(&attr));
        winerr_set_errno();
        return FALSE;
    }
    cant_fail_ret(pthread_mutexattr_destroy(&attr));

    return TRUE;
}
WINAPI(InitializeCriticalSectionEx)

__winfnc void InitializeCriticalSection(CRITICAL_SECTION *sect) {
    if(!InitializeCriticalSectionEx(sect, 0, 0)) {
        log_error("InitializeCriticalSection failed");
        abort();
    }
}
WINAPI(InitializeCriticalSection)

__winfnc BOOL InitializeCriticalSectionAndSpinCount(CRITICAL_SECTION *sect, DWORD spin_count) {
    InitializeCriticalSection(sect);
    return TRUE;
}
WINAPI(InitializeCriticalSectionAndSpinCount)

__winfnc void DeleteCriticalSection(CRITICAL_SECTION* sect) {
    //Destroy the mutex
    cant_fail_ret(pthread_mutex_destroy((pthread_mutex_t*) sect->LockSemaphore));
    free(sect->LockSemaphore);
}
WINAPI(DeleteCriticalSection)

__winfnc void EnterCriticalSection(CRITICAL_SECTION *sect) {
    //Lock the mutex
    cant_fail_ret(pthread_mutex_lock((pthread_mutex_t*) sect->LockSemaphore));

    if(sect->RecursionCount++ == 0) {
        sect->LockCount = 1;
        sect->OwningThread = (HANDLE) pthread_self();
    }
}
WINAPI(EnterCriticalSection)

__winfnc void LeaveCriticalSection(CRITICAL_SECTION *sect) {
    if(--sect->RecursionCount == 0) {
        sect->LockCount = 0;
        sect->OwningThread = NULL;
    }

    //Unlock the mutex
    cant_fail_ret(pthread_mutex_unlock((pthread_mutex_t*) sect->LockSemaphore));
}
WINAPI(LeaveCriticalSection)

static pthread_rwlock_t events_lock = PTHREAD_RWLOCK_INITIALIZER;
static struct sync_event {
    struct win_sync_object sync_obj;
    struct sync_event *prev, *next;
    HANDLE handle;

    const char *name;
    bool manual_reset;

    pthread_mutex_t lock;
    pthread_cond_t cond;
    bool state;
    int waiters;
} *events_head;

static void evt_destr(struct sync_event *evt) {
    //Unlink the event
    cant_fail_ret(pthread_rwlock_wrlock(&events_lock));

    if(evt->prev) evt->prev->next = evt->next;
    else events_head = evt->next;
    if(evt->next) evt->next->prev = evt->prev;

    cant_fail_ret(pthread_rwlock_unlock(&events_lock));

    //Drivers may close an event while one of their threads still waits on
    //it (e.g. a worker woken by a shutdown event). Windows tolerates that;
    //here, leave the event allocated rather than pull it from under the waiter.
    if(pthread_mutex_trylock(&evt->lock) != 0) {
        log_debug("Leaking event '%s' closed while in use", evt->name ? evt->name : "");
        return;
    }
    bool in_use = evt->waiters > 0;
    cant_fail_ret(pthread_mutex_unlock(&evt->lock));
    if(in_use) {
        log_debug("Leaking event '%s' closed with waiters", evt->name ? evt->name : "");
        return;
    }

    //Free memory
    cant_fail_ret(pthread_cond_destroy(&evt->cond));
    cant_fail_ret(pthread_mutex_destroy(&evt->lock));
    free((void*) evt->name);
    free(evt);
}

static DWORD evt_wait(struct sync_event *evt, DWORD timeout) {
    DWORD res = 0;
    cant_fail_ret(pthread_mutex_lock(&evt->lock));
    evt->waiters++;

    //Wait for the event
    while(!evt->state) {
        if(timeout != INFINITE) {
            struct timespec time;
            win_wait_deadline(&time, timeout);
            int err = pthread_cond_timedwait(&evt->cond, &evt->lock, &time);
            if(err == ETIMEDOUT) {
                res = WAIT_TIMEOUT;
                break;
            }
            cant_fail_ret(err);
        } else cant_fail_ret(pthread_cond_wait(&evt->cond, &evt->lock));
    }

    //Auto-reset event
    if(res == 0 && !evt->manual_reset) evt->state = false;
    evt->waiters--;

    cant_fail_ret(pthread_mutex_unlock(&evt->lock));
    return res;
}

HANDLE win_create_event(const char *name, bool initial_state, bool manual_reset) {
    //Allocate the event
    struct sync_event *evt = (struct sync_event*) malloc(sizeof(struct sync_event));
    if(!evt) { winerr_set_errno(); return NULL; }
    evt->sync_obj.wait_fnc = (win_sync_obj_wait_fnc*) evt_wait;
    evt->handle = winhandle_create(evt, (winhandle_destr_fnc*) evt_destr);

    evt->name = name ? strdup(name) : NULL;
    evt->state = initial_state;
    evt->manual_reset = manual_reset;
    evt->waiters = 0;
    cant_fail_ret(pthread_mutex_init(&evt->lock, NULL));
    cant_fail_ret(pthread_cond_init(&evt->cond, NULL));

    //Add event to list
    cant_fail_ret(pthread_rwlock_wrlock(&events_lock));

    evt->prev = NULL;
    evt->next = events_head;
    if(events_head) events_head->prev = evt;
    events_head = evt;

    cant_fail_ret(pthread_rwlock_unlock(&events_lock));

    return evt->handle;
}

void win_set_event(HANDLE handle) {
    struct sync_event *evt = (struct sync_event*) handle->data;

    //Signal the event
    cant_fail_ret(pthread_mutex_lock(&evt->lock));
    evt->state = true;
    cant_fail_ret(pthread_cond_broadcast(&evt->cond));
    cant_fail_ret(pthread_mutex_unlock(&evt->lock));
}

void win_reset_event(HANDLE handle) {
    struct sync_event *evt = (struct sync_event*) handle->data;

    //Reset the event
    cant_fail_ret(pthread_mutex_lock(&evt->lock));
    evt->state = false;
    cant_fail_ret(pthread_cond_broadcast(&evt->cond));
    cant_fail_ret(pthread_mutex_unlock(&evt->lock));
}

__winfnc HANDLE CreateEventA(void *attrs, BOOL manual_reset, BOOL initial_state, const char *name) {
    return win_create_event(name, initial_state, manual_reset);
}
WINAPI(CreateEventA);

__winfnc HANDLE CreateEventW(void *attrs, BOOL manual_reset, BOOL initial_state, const char16_t *name) {
    char *cname = name ? winstr_to_str(name) : NULL;
    HANDLE evt = CreateEventA(attrs, manual_reset, initial_state, cname);
    free(cname);
    return evt;
}
WINAPI(CreateEventW);

__winfnc HANDLE OpenEventW(DWORD access, BOOL inherit, const char16_t *name) {
    char *cname = winstr_to_str(name);
    HANDLE handle = NULL;

    cant_fail_ret(pthread_rwlock_rdlock(&events_lock));
    for(struct sync_event *evt = events_head; evt; evt = evt->next) {
        if(evt->name && strcmp(evt->name, cname) == 0) {
            handle = evt->handle;
            break;
        }
    }
    cant_fail_ret(pthread_rwlock_unlock(&events_lock));

    log_debug("OpenEventW | '%s' -> %s", cname, handle ? "found" : "not found");
    free(cname);
    if(!handle) winerr_set_code(2); /* ERROR_FILE_NOT_FOUND */
    return handle;
}
WINAPI(OpenEventW)

struct sync_mutex {
    struct win_sync_object sync_obj;
    pthread_mutex_t lock;
};

static void mutex_destr(struct sync_mutex *mtx) {
    cant_fail_ret(pthread_mutex_destroy(&mtx->lock));
    free(mtx);
}

static DWORD mutex_wait(struct sync_mutex *mtx, DWORD timeout) {
    if(timeout == INFINITE) {
        cant_fail_ret(pthread_mutex_lock(&mtx->lock));
        return 0;
    }

    struct timespec time;
    win_wait_deadline(&time, timeout);
    int err = pthread_mutex_timedlock(&mtx->lock, &time);
    if(err == ETIMEDOUT) return WAIT_TIMEOUT;
    cant_fail_ret(err);
    return 0;
}

__winfnc HANDLE CreateMutexA(void *attrs, BOOL initial_owner, const char *name) {
    struct sync_mutex *mtx = (struct sync_mutex*) malloc(sizeof(struct sync_mutex));
    if(!mtx) { winerr_set_errno(); return NULL; }
    mtx->sync_obj.wait_fnc = (win_sync_obj_wait_fnc*) mutex_wait;

    //Windows mutexes can be re-acquired by their owning thread
    pthread_mutexattr_t mattr;
    cant_fail_ret(pthread_mutexattr_init(&mattr));
    cant_fail_ret(pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE));
    cant_fail_ret(pthread_mutex_init(&mtx->lock, &mattr));
    cant_fail_ret(pthread_mutexattr_destroy(&mattr));
    if(initial_owner) cant_fail_ret(pthread_mutex_lock(&mtx->lock));

    return winhandle_create(mtx, (winhandle_destr_fnc*) mutex_destr);
}
WINAPI(CreateMutexA)

__winfnc BOOL ReleaseMutex(HANDLE handle) {
    struct sync_mutex *mtx = (struct sync_mutex*) handle->data;
    if(pthread_mutex_unlock(&mtx->lock) != 0) {
        winerr_set_code(288); /* ERROR_NOT_OWNER */
        return FALSE;
    }
    return TRUE;
}
WINAPI(ReleaseMutex)

__winfnc BOOL SetEvent(HANDLE handle) {
    if(handle == INVALID_HANDLE_VALUE) return FALSE;
    win_set_event(handle);
    return TRUE;
}
WINAPI(SetEvent)

__winfnc BOOL ResetEvent(HANDLE handle) {
    if(handle == INVALID_HANDLE_VALUE) return FALSE;
    win_reset_event(handle);
    return TRUE;
}
WINAPI(ResetEvent)

__winfnc DWORD WaitForSingleObject(HANDLE handle, DWORD timeout) {
    return win_wait_sync_obj(handle, timeout);
}
WINAPI(WaitForSingleObject)

#define NUM_FLS_IDXS 128
#define FLS_OUT_OF_INDEXES 0xffffffff

typedef __winfnc void PflsCallbackFunction(void *flsData);

static pthread_rwlock_t fls_lock = PTHREAD_RWLOCK_INITIALIZER;

static int fls_next_free;
static struct {
    int next_idx;
    pthread_key_t key;
    PflsCallbackFunction *callback;
} fls_indices[NUM_FLS_IDXS];

struct fls_value {
    PflsCallbackFunction *callback;
    void *data;
};

__constr static void init_fls() {
    //Initialize indices
    fls_next_free = NUM_FLS_IDXS;
    for(int i = NUM_FLS_IDXS-1; i >= 0; i--) {
        fls_indices[i].next_idx = fls_next_free;
        fls_next_free = i;
    }
}

static void fls_destructor(void *ptr) {
    struct fls_value *val = (struct fls_value*) ptr;
    if(val->callback) val->callback(val->data);
    free(val);
}

__winfnc DWORD FlsAlloc(PflsCallbackFunction *callback) {
    cant_fail_ret(pthread_rwlock_wrlock(&fls_lock));

    //Try to obtain an index
    DWORD idx = FLS_OUT_OF_INDEXES;
    if(fls_next_free < NUM_FLS_IDXS) {
        idx = fls_next_free;
        fls_next_free = fls_indices[idx].next_idx;

        fls_indices[idx].next_idx = -1;
        cant_fail_ret(pthread_key_create(&fls_indices[idx].key, fls_destructor));
        fls_indices[idx].callback = callback;
    } else log_warn("Ran out of FLS indices!");

    cant_fail_ret(pthread_rwlock_unlock(&fls_lock));

    return idx;
}
WINAPI(FlsAlloc);

__winfnc BOOL FlsFree(DWORD idx) {
    cant_fail_ret(pthread_rwlock_wrlock(&fls_lock));

    //Free the index
    BOOL suc;
    if(0 <= idx && idx < NUM_FLS_IDXS && fls_indices[idx].next_idx < 0) {
        cant_fail_ret(pthread_key_delete(fls_indices[idx].key));
        fls_indices[idx].next_idx = fls_next_free;
        fls_next_free = idx;
        suc = TRUE;
    } else suc = FALSE;

    cant_fail_ret(pthread_rwlock_unlock(&fls_lock));

    if(!suc) winerr_set();
    return suc;
}
WINAPI(FlsFree);

__winfnc void *FlsGetValue(DWORD idx) {
    cant_fail_ret(pthread_rwlock_rdlock(&fls_lock));

    //Get the index's value
    void *data = NULL;
    if(0 <= idx && idx < NUM_FLS_IDXS && fls_indices[idx].next_idx < 0) {
        struct fls_value *val = (struct fls_value*) pthread_getspecific(fls_indices[idx].key);
        if(val) data = val->data;
    } else winerr_set();

    cant_fail_ret(pthread_rwlock_unlock(&fls_lock));

    return data;
}
WINAPI(FlsGetValue);

__winfnc BOOL FlsSetValue(DWORD idx, void *data) {
    cant_fail_ret(pthread_rwlock_rdlock(&fls_lock));

    //Set the index's value
    BOOL suc;
    if(0 <= idx && idx < NUM_FLS_IDXS && fls_indices[idx].next_idx < 0) {
        free(pthread_getspecific(fls_indices[idx].key));

        struct fls_value *val = (struct fls_value*) malloc(sizeof(struct fls_value));
        if(!val) {
            winerr_set_errno();
            suc = FALSE;
        } else {
            val->callback = fls_indices[idx].callback;
            val->data = data;
            cant_fail_ret(pthread_setspecific(fls_indices[idx].key, val));
            suc = TRUE;
        }
    } else {
        winerr_set();
        suc = FALSE;
    }

    cant_fail_ret(pthread_rwlock_unlock(&fls_lock));

    return suc;
}
WINAPI(FlsSetValue);

__winfnc DWORD TlsAlloc() { return FlsAlloc(NULL); }
WINAPI(TlsAlloc)

__winfnc BOOL TlsFree(DWORD idx) { return FlsFree(idx); }
WINAPI(TlsFree)

__winfnc void *TlsGetValue(DWORD idx) { return FlsGetValue(idx); }
WINAPI(TlsGetValue)

__winfnc BOOL TlsSetValue(DWORD idx, void *data) { return FlsSetValue(idx, data); }
WINAPI(TlsSetValue)