#ifndef TUDOR_HOST_LAUNCHER_PDATA_H
#define TUDOR_HOST_LAUNCHER_PDATA_H

#include <gio/gio.h>

extern GDBusMethodInfo launcher_load_pdata_method;
extern GDBusMethodInfo launcher_store_pdata_method;

void init_pdata();
void uninit_pdata();

void load_pdata_call(GDBusMethodInvocation *invoc, GVariant *params);
void store_pdata_call(GDBusMethodInvocation *invoc, GVariant *params);

#endif