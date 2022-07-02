#ifndef LIBTUDOR_WINAPI_INTERNAL_H
#define LIBTUDOR_WINAPI_INTERNAL_H

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

HANDLE win_create_event(const char *name, bool initial_state, bool manual_reset);
void win_set_event(HANDLE evt);
void win_reset_event(HANDLE evt);

#endif