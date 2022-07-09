#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <gio/gunixfdlist.h>
#include <tudor/dbus-launcher.h>
#include "dbus.h"

struct host_entry {
    guint id;
    GPid pid;
    bool alive;
};
static GArray *hosts_array;
static guint next_host_id = 0;

static GDBusArgInfo host_id_arg = {
    .ref_count = -1,
    .name = "hostId",
    .signature = "u",
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

//<<<<< HostDied signal >>>>>

static GDBusArgInfo host_died_status_arg = {
    .ref_count = -1,
    .name = TUDOR_HOST_LAUNCHER_HOST_DIED_STATUS_ARG,
    .signature = "i",
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

GDBusSignalInfo launcher_host_died_signal = {
    .ref_count = -1,
    .name = TUDOR_HOST_LAUNCHER_HOST_DIED_SIGNAL,
    .args = (GDBusArgInfo*[]) { &host_id_arg, &host_died_status_arg, NULL },
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

static void host_watch_cb(GPid pid, gint status, gpointer user_data) {
    //Get the process status
    if(WIFEXITED(status)) {
        status = WEXITSTATUS(status);
    } else if(WIFSIGNALED(status)) {
        status = -WTERMSIG(status);
    } else if(WIFSTOPPED(status)) {
        status = -WSTOPSIG(status);
    }

    //Close PID
    g_spawn_close_pid(pid);

    //Clear alive flag
    guint host_id = GPOINTER_TO_UINT(user_data);
    for(int i = 0; i < hosts_array->len; i++) {
        struct host_entry *entry = &g_array_index(hosts_array, struct host_entry, i);
        if(entry->id == host_id) {
            g_assert_cmpint(pid, ==, entry->pid);

            if(entry->alive) {
                g_info("Host process %u died: status %d", host_id, status);

                //Emit signal
                GError *error = NULL;
                g_dbus_connection_emit_signal(dbus_con, NULL, TUDOR_HOST_LAUNCHER_OBJ, TUDOR_HOST_LAUNCHER_INTERF, TUDOR_HOST_LAUNCHER_HOST_DIED_SIGNAL, g_variant_new("(ui)", host_id, status), &error);
                g_assert_no_error(error);
            }

            entry->alive = false;
            return;
        } else g_assert_cmpint(pid, !=, entry->pid);
    }
}

//<<<<< LaunchHost method >>>>>

static GDBusArgInfo launch_pipe_fd_arg = {
    .ref_count = -1,
    .name = "pipeFd",
    .signature = "h",
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

GDBusMethodInfo launcher_launch_method = {
    .ref_count = -1,
    .name = TUDOR_HOST_LAUNCHER_LAUNCH_METHOD,
    .in_args = (GDBusArgInfo*[]) { NULL },
    .out_args = (GDBusArgInfo*[]) { &host_id_arg, &launch_pipe_fd_arg, NULL },
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

static void host_setup(gpointer user_data) {
    setpgrp();
}

void launch_host_call(GDBusMethodInvocation *invoc, GVariant *params) {
    //Create the socket pair
    int fds[2];
    if(socketpair(AF_UNIX, SOCK_DGRAM, 0, fds) < 0) {
        int err = errno;
        g_dbus_method_invocation_return_error(invoc, G_IO_ERROR, G_IO_ERROR_FAILED, "Failed creating IPC socket pair: %d [%s]", err, g_strerror(err));
        return;
    }
    int mod_sock = fds[0], host_sock = fds[1];

    //Start the tudor_host process
    char *argv[] = { "tudor_host", NULL };
    char *envp[] = { NULL };
    GPid pid;

    int fd_out, fd_err;
    g_assert_no_errno(fd_out = dup(STDOUT_FILENO));
    g_assert_no_errno(fd_err = dup(STDERR_FILENO));

    GError *error = NULL;
    gchar *cwd = g_get_current_dir();
    if(!g_spawn_async_with_fds(cwd, argv, envp, G_SPAWN_DO_NOT_REAP_CHILD, host_setup, NULL, &pid, host_sock, fd_out, fd_err, &error)) {
        g_assert_no_errno(close(fd_out));
        g_assert_no_errno(close(fd_err));
        g_assert_no_errno(close(mod_sock));
        g_assert_no_errno(close(host_sock));
        g_free(cwd);

        g_dbus_method_invocation_return_gerror(invoc, error);
        g_clear_error(&error);
        return;

    }
    g_assert_no_errno(close(fd_out));
    g_assert_no_errno(close(fd_err));
    g_assert_no_errno(close(host_sock));
    g_free(cwd);

    //Add to hosts array
    guint host_id = next_host_id++;
    struct host_entry entry = (struct host_entry) { .id = host_id, .pid = pid, .alive = true };
    g_array_append_val(hosts_array, entry);

    //Add child watcher
    g_child_watch_add(pid, host_watch_cb, GUINT_TO_POINTER(host_id));

    //Pass results back to caller
    g_info("Launched Tudor host process ID %u PID %d", host_id, pid);
    g_dbus_method_invocation_return_value_with_unix_fd_list(invoc, g_variant_new("(uh)", host_id, 0), g_unix_fd_list_new_from_array(&mod_sock, 1));
}

//<<<<< KillHost method >>>>>

GDBusMethodInfo launcher_kill_method = {
    .ref_count = -1,
    .name = TUDOR_HOST_LAUNCHER_KILL_METHOD,
    .in_args = (GDBusArgInfo*[]) { &host_id_arg, NULL },
    .out_args = (GDBusArgInfo*[]) { NULL },
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

void kill_host_call(GDBusMethodInvocation *invoc, GVariant *params) {
    //Get the host ID
    if(!g_variant_check_format_string(params, "(u)", FALSE)) {
        g_dbus_method_invocation_return_error(invoc, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Invalid argument format");
        return;
    }

    guint host_id;
    g_variant_get(params, "(u)", &host_id);

    //Find and remove the host entry
    for(int i = 0; i < hosts_array->len; i++) {
        struct host_entry *entry = &g_array_index(hosts_array, struct host_entry, i);
        if(entry->id == host_id) {
            if(entry->alive) {
                g_assert_no_errno(killpg(entry->pid, SIGKILL));
            }
            g_array_remove_index_fast(hosts_array, i);

            g_info("Killed Tudor host process ID %u PID %d", host_id, entry->pid);
            g_dbus_method_invocation_return_value(invoc, NULL);
            return;
        }
    }

    g_dbus_method_invocation_return_error(invoc, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Invalid host ID %u", host_id);
}

void init_launcher() {
    //Create hosts array
    hosts_array = g_array_new(FALSE, FALSE, sizeof(struct host_entry));
}

void uninit_launcher() {
    //Free hosts array
    g_array_unref(hosts_array);
}