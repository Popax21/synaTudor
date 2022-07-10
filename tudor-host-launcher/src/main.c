#include <stdlib.h>
#include <tudor/dbus-launcher.h>
#include "dbus.h"
#include "launch.h"
#include "pdata.h"

GDBusConnection *dbus_con;

static GDBusInterfaceInfo launcher_interf = {
    .ref_count = -1,
    .name = TUDOR_HOST_LAUNCHER_INTERF,
    .methods = (GDBusMethodInfo*[]) { &launcher_launch_method, &launcher_kill_method, &launcher_load_pdata_method, &launcher_store_pdata_method, NULL },
    .signals = (GDBusSignalInfo*[]) { &launcher_host_died_signal, NULL },
    .properties = (GDBusPropertyInfo*[]) { NULL },
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

static void launch_method_call(GDBusConnection *con, const gchar *sender, const gchar *obj_path, const gchar *if_name, const gchar *method_name, GVariant *params, GDBusMethodInvocation *invoc, gpointer user_data) {
    //Dispatch call
    if(strcmp(method_name, TUDOR_HOST_LAUNCHER_LAUNCH_METHOD) == 0) {
        launch_host_call(invoc, params);
    } else if(strcmp(method_name, TUDOR_HOST_LAUNCHER_KILL_METHOD) == 0) {
        kill_host_call(invoc, params);
    } else if(strcmp(method_name, TUDOR_HOST_LAUNCHER_LOAD_PAIRING_DATA_METHOD) == 0) {
        load_pdata_call(invoc, params);
    } else if(strcmp(method_name, TUDOR_HOST_LAUNCHER_STORE_PAIRING_DATA_METHOD) == 0) {
        store_pdata_call(invoc, params);
    } else {
        g_dbus_method_invocation_return_error(invoc, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_SIGNATURE, "Unknown method '%s'", method_name);
    }
}

static GDBusInterfaceVTable launch_vtable = {
    launch_method_call,
    NULL, NULL
};

static void name_acquired(GDBusConnection *con, const gchar *name, gpointer user_data) {
    g_info("Acquired DBus name '%s'", name);
}

static void name_lost(GDBusConnection *con, const gchar *name, gpointer user_data) {
    g_warning("Lost DBus name '%s'!", name);
    g_main_loop_quit((GMainLoop*) user_data);
}

int main() {
    //Connect to DBus
    GError *error = NULL;
    dbus_con = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
    g_assert_no_error(error);

    //Initialize modules
    init_launcher();
    init_pdata();

    //Register launcher object
    g_dbus_connection_register_object(dbus_con, TUDOR_HOST_LAUNCHER_OBJ, &launcher_interf, &launch_vtable, NULL, NULL, &error);
    g_assert_no_error(error);

    //Create the main loop
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    //Claim our DBus name
    guint owner_id = g_bus_own_name_on_connection(dbus_con, TUDOR_HOST_LAUNCHER_SERVICE, G_BUS_NAME_OWNER_FLAGS_NONE, name_acquired, name_lost, loop, NULL);

    //Main loop
    g_main_loop_run(loop);

    //Cleanup DBus
    g_bus_unown_name(owner_id);
    g_clear_object(&dbus_con);

    //Uninitialize modules
    uninit_launcher();
    uninit_pdata();

    return EXIT_SUCCESS;
}