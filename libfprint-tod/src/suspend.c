#include <gio/gunixfdmessage.h>
#include "suspend.h"
#include "ipc.h"
#include "open.h"

#define LOGIND_SERVICE "org.freedesktop.login1"
#define LOGIND_INTERF "org.freedesktop.login1.Manager"
#define LOGIND_OBJ "/org/freedesktop/login1"
#define LOGIND_PREPARE_SLEEP_SIGNAL "PrepareForSleep"
#define LOGIND_SLEEP_INHIBIT_METHOD "Inhibit"

static void suspend_close_cb(GObject *src_obj, GAsyncResult *res, gpointer user_data) {
    GTask *task = G_TASK(res);
    gint sleep_inhib = GPOINTER_TO_INT(user_data);

    //Check for errors
    GError *error = NULL;
    g_task_propagate_int(task, &error);
    if(error) {
        g_warning("Error closing tudor device on suspend: %s (%s code %d)", error->message, g_quark_to_string(error->domain), error->code);
        g_clear_error(&error);
    }

    //Close the sleep inhibitor
    close_sleep_inhibitor(sleep_inhib);
}

static void suspend_dev(FpiDeviceTudor *tdev, gint sleep_inhib) {
    //Check if the host process is running
    if(tdev->host_has_id && !tdev->host_dead) {
        g_info("Closing tudor device on suspend... (host ID %d)", tdev->host_id);

        //Close the device
        close_device(tdev, suspend_close_cb, GINT_TO_POINTER(sleep_inhib));
    } else {
        close_sleep_inhibitor(sleep_inhib);
    }
}

struct suspend_cancel_params {
    FpiDeviceTudor *tdev;
    gint sleep_inhib;
    GSource *monitor_src, *timeout_src;
};

static void suspend_params_free(struct suspend_cancel_params *params) {
    g_source_unref(params->monitor_src);
    g_source_unref(params->timeout_src);
    g_object_unref(params->tdev);
    g_slice_free(struct suspend_cancel_params, params);
}

static gboolean suspend_monitor_cb(gpointer user_data) {
    struct suspend_cancel_params *params = (struct suspend_cancel_params*) user_data;

    //Check the current action
    if(fpi_device_get_current_action(FP_DEVICE(params->tdev)) != FPI_DEVICE_ACTION_NONE) return G_SOURCE_CONTINUE;
    g_debug("Suspending tudor device...");

    //Destroy timeout source
    g_source_destroy(params->timeout_src);

    //Suspend the device
    suspend_dev(params->tdev, params->sleep_inhib);

    return G_SOURCE_REMOVE;
}

static void suspend_timeout_cb(FpDevice *dev, gpointer user_data) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(dev);

    g_warning("Tudor device suspend timeout hit! Killing host process...");

    //Kill the host
    GError *error = NULL;
    if(!kill_host_process(tdev, &error)) {
        g_warning("Could not kill tudor host process because of suspend timeout: %s (%s code %d)", error->message, g_quark_to_string(error->domain), error->code);
        g_clear_error(&error);
    }
}

static void suspend_signal_cb(GDBusConnection *con, const gchar *sender, const gchar *obj, const gchar *interf, const gchar *signal, GVariant *params, gpointer user_data) {
    FpiDeviceTudor *tdev = FPI_DEVICE_TUDOR(user_data);

    //Check and get parameters
    if(!g_variant_check_format_string(params, "(b)", FALSE)) {
        g_warning("Received invalid parameters from logind PrepareForSleep signal!");
        return;
    }

    gboolean enter_sleep;
    g_variant_get(params, "(b)", &enter_sleep);

    if(!enter_sleep) return;

    //Steal the sleep inhibitor
    gint sleep_inhib = tdev->host_sleep_inhib;
    tdev->host_sleep_inhib = -1;

    //Cancel the current action
    if(fpi_device_get_current_action(FP_DEVICE(tdev)) != FPI_DEVICE_ACTION_NONE) {
        g_warning("Cancelling current tudor device action on suspend...");

        if(tdev->cancel_handler_id != 0) {
            //Invoke the cancel handler manually
            invoke_cancel_handler(tdev);
        }

        //Wait for action to complete, but also add a timeout
        struct suspend_cancel_params *params = g_slice_new(struct suspend_cancel_params);
        params->tdev = g_object_ref(tdev);
        params->timeout_src = fpi_device_add_timeout(FP_DEVICE(tdev), SUSPEND_CANCEL_TIMEOUT_SECS * 1000, suspend_timeout_cb, params, NULL);
        params->monitor_src = g_idle_source_new();
        g_source_set_callback(params->monitor_src, suspend_monitor_cb, params, (GDestroyNotify) suspend_params_free);
        g_source_attach(params->monitor_src, NULL);
    } else {
        //Immediatly suspend the device
        suspend_dev(tdev, sleep_inhib);
    }
}

void register_suspend_monitor(FpiDeviceTudor *tdev) {
    //Register a signal listener
    g_dbus_connection_signal_subscribe(tdev->dbus_con,
        LOGIND_SERVICE, LOGIND_INTERF, LOGIND_PREPARE_SLEEP_SIGNAL, LOGIND_OBJ,
        NULL, G_DBUS_SIGNAL_FLAGS_NONE,
        suspend_signal_cb, tdev, NULL
    );
}

bool create_sleep_inhibitor(FpiDeviceTudor *tdev, GError **error) {
    //Close the sleep inhibitor
    if(tdev->host_sleep_inhib >= 0) {
        g_assert_no_errno(close(tdev->host_sleep_inhib));
        tdev->host_sleep_inhib = -1;
    }

    //Request a sleep inhibitor from logind
    GUnixFDList *fds;
    GVariant *rets = g_dbus_connection_call_with_unix_fd_list_sync(tdev->dbus_con,
        LOGIND_SERVICE, LOGIND_OBJ, LOGIND_INTERF,
        LOGIND_SLEEP_INHIBIT_METHOD, g_variant_new("(ssss)", "sleep", "net.reactivated.Fprint", "Suspend tudor device", "delay"), G_VARIANT_TYPE("(h)"), G_DBUS_CALL_FLAGS_NONE,
        G_MAXINT,
        NULL, &fds,
        NULL, error
    );
    if(!rets) return false;

    //Get the inhibitor FD
    int fd_idx;
    g_variant_get(rets, "(h)", &fd_idx);
    g_variant_unref(rets);

    tdev->host_sleep_inhib = g_unix_fd_list_get(fds, fd_idx, error);
    g_object_unref(fds);
    if(tdev->host_sleep_inhib < 0) return false;

    g_debug("Got sleep inhibitor for tudor device: %d", tdev->host_sleep_inhib);
    return true;
}