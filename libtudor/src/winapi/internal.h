#ifndef LIBTUDOR_WINAPI_INTERNAL_H
#define LIBTUDOR_WINAPI_INTERNAL_H

#include <time.h>
#include "api.h"

//Threading
void win_init_tib();
DWORD win_get_thread_id();

//Synchronization
struct win_sync_object;

#define INFINITE 0xffffffff
#define WAIT_TIMEOUT 0x00000102L

typedef DWORD win_sync_obj_wait_fnc(struct win_sync_object *sync_obj, DWORD timeout);

struct win_sync_object {
    win_sync_obj_wait_fnc *wait_fnc;
};

DWORD win_wait_sync_obj(HANDLE handle, DWORD timeout);

//Converts a relative Win32 millisecond timeout into the absolute CLOCK_REALTIME deadline that
//pthread_cond_timedwait / pthread_timedjoin_np expect.
static inline void win_timeout_to_abstime(DWORD timeout_ms, struct timespec *ts) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += timeout_ms / 1000;
    ts->tv_nsec += (long) (timeout_ms % 1000) * 1000000L;
    if(ts->tv_nsec >= 1000000000L) { ts->tv_sec++; ts->tv_nsec -= 1000000000L; }
}

HANDLE win_create_event(const char *name, bool initial_state, bool manual_reset);
void win_set_event(HANDLE evt);
void win_reset_event(HANDLE evt);

#endif