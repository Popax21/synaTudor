#ifndef TUDOR_TUDOR_DBUS_LAUNCHER_H
#define TUDOR_TUDOR_DBUS_LAUNCHER_H

#define TUDOR_HOST_LAUNCHER_SERVICE "net.reactivated.TudorHostLauncher"
#define TUDOR_HOST_LAUNCHER_OBJ "/net/reactivated/TudorHostLauncher"
#define TUDOR_HOST_LAUNCHER_INTERF "net.reactivated.TudorHostLauncher"

#define TUDOR_HOST_LAUNCHER_HOST_ID_ARG "hostId"

#define TUDOR_HOST_LAUNCHER_LAUNCH_METHOD "LaunchHost"
#define TUDOR_HOST_LAUNCHER_LAUNCH_PIPE_FD_ARG "pipeFd"

#define TUDOR_HOST_LAUNCHER_KILL_METHOD "KillHost"

#define TUDOR_HOST_LAUNCHER_HOST_DIED_SIGNAL "HostDied"
#define TUDOR_HOST_LAUNCHER_HOST_DIED_STATUS_ARG "status"

#endif