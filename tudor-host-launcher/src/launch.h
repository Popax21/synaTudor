#ifndef TUDOR_HOST_LAUNCHER_LAUNCH_H
#define TUDOR_HOST_LAUNCHER_LAUNCH_H

#include <gio/gio.h>

extern GDBusMethodInfo launcher_launch_method;
extern GDBusMethodInfo launcher_kill_method;
extern GDBusSignalInfo launcher_host_died_signal;

void init_launcher();
void uninit_launcher();

void launch_host_call(GDBusMethodInvocation *invoc, GVariant *params);
void kill_host_call(GDBusMethodInvocation *invoc, GVariant *params);

#endif