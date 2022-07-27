#include <stdbool.h>
#include <tudor/dbus-launcher.h>
#include "dbus.h"

static const gchar *pdata_dir;

static GDBusArgInfo sensor_name_arg = {
    .ref_count = -1,
    .name = "sensorName",
    .signature = "s",
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

static GDBusArgInfo pdata_arg = {
    .ref_count = -1,
    .name = "data",
    .signature = "ay",
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

static bool validate_sensor_name(GDBusMethodInvocation *invoc, const gchar *sensor_name) {
    for(; *sensor_name; sensor_name++) {
        if(!g_ascii_isalnum(*sensor_name)) {
            g_dbus_method_invocation_return_error(invoc, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Invalid sensor name");
            return false;
        }
    }
    return true;
}

static GDBusArgInfo has_pdata_arg = {
    .ref_count = -1,
    .name = "hasData",
    .signature = "b",
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

GDBusMethodInfo launcher_load_pdata_method = {
    .ref_count = -1,
    .name = TUDOR_HOST_LAUNCHER_LOAD_PAIRING_DATA_METHOD,
    .in_args = (GDBusArgInfo*[]) { &sensor_name_arg, NULL },
    .out_args = (GDBusArgInfo*[]) { &has_pdata_arg, &pdata_arg, NULL },
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

void load_pdata_call(GDBusMethodInvocation *invoc, GVariant *params) {
    //Parse arguments
    if(!g_variant_check_format_string(params, "(s)", FALSE)) {
        g_dbus_method_invocation_return_error(invoc, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Invalid argument format");
        return;
    }

    gchar *sensor_name;
    g_variant_get(params, "(&s)", &sensor_name);

    //Validate the sensor name
    if(!validate_sensor_name(invoc, sensor_name)) return;

    //Get the pairing data file
    gchar *path = g_strdup_printf("%s/%s.tpd", pdata_dir, sensor_name);
    GFile *pdata_file = g_file_new_for_path(path);
    g_free(path);

    //Check if the pairing data file exists
    GVariant *pdata = NULL;
    if(g_file_query_exists(pdata_file, NULL)) {
        GError *error = NULL;
        GFileInfo *info = g_file_query_info(pdata_file, G_FILE_ATTRIBUTE_STANDARD_SIZE, G_FILE_QUERY_INFO_NONE, NULL, &error);
        if(!info) {
            g_object_unref(pdata_file);
            g_dbus_method_invocation_return_gerror(invoc, error);
            g_clear_error(&error);
            return;
        }

        //Allocate pairing data buffer
        gsize pdata_len = (gsize) g_file_info_get_size(info);
        gpointer pdata_data = g_malloc(pdata_len);
        g_object_unref(info);

        //Load the pairing data
        GFileInputStream *stream = g_file_read(pdata_file, NULL, &error);
        if(!stream) {
            g_object_unref(pdata_file);
            g_dbus_method_invocation_return_gerror(invoc, error);
            g_clear_error(&error);
            return;
        }
        if(!g_input_stream_read_all(G_INPUT_STREAM(stream), pdata_data ? pdata_data : &pdata_data, pdata_len, NULL, NULL, &error)) {
            g_object_unref(stream);
            g_object_unref(pdata_file);
            g_dbus_method_invocation_return_gerror(invoc, error);
            g_clear_error(&error);
            return;
        }
        g_object_unref(stream);

        //Create variant
        pdata = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, pdata_data, pdata_len, 1);
        g_free(pdata_data);

        g_info("Loaded pairing data for sensor '%s' - %lu bytes", sensor_name, pdata_len);
    } else {
        g_info("No stored pairing data for sensor '%s'", sensor_name);
    }

    //Return result
    bool has_pdata = pdata != NULL;
    if(!pdata) pdata = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, NULL, 0, 1);
    g_dbus_method_invocation_return_value(invoc, g_variant_new("(b@ay)", has_pdata, pdata));
}

GDBusMethodInfo launcher_store_pdata_method = {
    .ref_count = -1,
    .name = TUDOR_HOST_LAUNCHER_STORE_PAIRING_DATA_METHOD,
    .in_args = (GDBusArgInfo*[]) { &sensor_name_arg, &pdata_arg, NULL },
    .out_args = (GDBusArgInfo*[]) { NULL },
    .annotations = (GDBusAnnotationInfo*[]) { NULL }
};

void store_pdata_call(GDBusMethodInvocation *invoc, GVariant *params) {
    //Parse arguments
    if(!g_variant_check_format_string(params, "(say)", FALSE)) {
        g_dbus_method_invocation_return_error(invoc, G_DBUS_ERROR, G_DBUS_ERROR_INVALID_ARGS, "Invalid argument format");
        return;
    }

    gchar *sensor_name;
    GVariant *pdata;
    g_variant_get(params, "(&s@ay)", &sensor_name, &pdata);

    //Validate the sensor name
    if(!validate_sensor_name(invoc, sensor_name)) return;

    //Get the pairing data file
    gchar *path = g_strdup_printf("%s/%s.tpd", pdata_dir, sensor_name);
    GFile *pdata_file = g_file_new_for_path(path);
    g_free(path);

    //Write data to file
    gsize pdata_len;
    const void *pdata_data = (const void*) g_variant_get_fixed_array(pdata, &pdata_len, 1);

    gboolean suc;
    GError *error = NULL;
    GFileOutputStream *stream = g_file_replace(pdata_file, NULL, false, G_FILE_CREATE_PRIVATE | G_FILE_CREATE_REPLACE_DESTINATION, NULL, &error);
    if(stream) {
        suc = g_output_stream_write_all(G_OUTPUT_STREAM(stream), pdata_data ? pdata_data : &pdata_data, pdata_len, NULL, NULL, &error);
        g_object_unref(stream);
    } else suc = false;

    //Return result
    if(suc) {
        g_info("Stored pairing data for sensor '%s' - %lu bytes", sensor_name, pdata_len);
        g_dbus_method_invocation_return_value(invoc, NULL);
    } else {
        g_dbus_method_invocation_return_gerror(invoc, error);
        g_clear_error(&error);
    }

    g_object_unref(pdata_file);
    g_variant_unref(pdata);
}

void init_pdata() {
    //Get the pairing data directory
    pdata_dir = g_getenv("STATE_DIRECTORY");
    if(!pdata_dir) g_error("State directory environment variable not set!");
}

void uninit_pdata() {}