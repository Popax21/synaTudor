#ifndef TUDOR_TUDOR_DBUS_LAUNCHER_H
#define TUDOR_TUDOR_DBUS_LAUNCHER_H

#define TUDOR_HOST_LAUNCHER_SERVICE "net.reactivated.TudorHostLauncher"
#define TUDOR_HOST_LAUNCHER_OBJ "/net/reactivated/TudorHostLauncher"
#define TUDOR_HOST_LAUNCHER_INTERF "net.reactivated.TudorHostLauncher"

#define TUDOR_HOST_LAUNCHER_LAUNCH_METHOD "LaunchHost"
#define TUDOR_HOST_LAUNCHER_KILL_METHOD "KillHost"
#define TUDOR_HOST_LAUNCHER_ADOPT_METHOD "AdoptHost"
#define TUDOR_HOST_LAUNCHER_ORPHAN_METHOD "OrphanHost"
#define TUDOR_HOST_LAUNCHER_LOAD_PAIRING_DATA_METHOD "LoadPairingData"
#define TUDOR_HOST_LAUNCHER_STORE_PAIRING_DATA_METHOD "StorePairingData"

#define TUDOR_HOST_LAUNCHER_HOST_DIED_SIGNAL "HostDied"
#define TUDOR_HOST_LAUNCHER_HOST_DIED_STATUS_ARG "status"

#endif