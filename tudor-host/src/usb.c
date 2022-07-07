#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <tudor/log.h>
#include "usb.h"

static libusb_context *usb_ctx;

static pthread_t usb_thread;
static pthread_mutex_t usb_thread_lock = PTHREAD_MUTEX_INITIALIZER;
static bool usb_thread_exit = false;

static void *usb_thread_func(void *arg) {
    //Acquire USB lock
    cant_fail(pthread_mutex_lock(&usb_thread_lock));

    //Polling loop
    while(!usb_thread_exit) {
        int usb_err;
        if((usb_err = libusb_handle_events((libusb_context*) arg)) != 0) {
            int err = errno;
            log_warn("Error in USB polling thread: %d [%s] (errno %d [%s])", usb_err, libusb_error_name(usb_err), err, strerror(err));
        }
    }

    return NULL;
}

static void usb_log_cb(libusb_context *ctx, enum libusb_log_level level, const char *str) {
    switch(level) {
        case LIBUSB_LOG_LEVEL_NONE: break;
        case LIBUSB_LOG_LEVEL_DEBUG: log_debug("%s", str); break;
        case LIBUSB_LOG_LEVEL_INFO: log_info("%s", str); break;
        case LIBUSB_LOG_LEVEL_WARNING: log_warn("%s", str); break;
        case LIBUSB_LOG_LEVEL_ERROR: log_error("%s", str); break;
    }
}

void init_libusb() {
    //Set log callback
    libusb_set_log_cb(NULL, usb_log_cb, LIBUSB_LOG_CB_GLOBAL);

    //Init libusb
    int usb_err = libusb_init(&usb_ctx);
    if(usb_err != 0) {
        int err = errno;
        log_error("Error initializing libusb: %d [%s] (errno %d [%s])", usb_err, libusb_error_name(usb_err), err, strerror(err));
        abort();
    }
    cant_fail(pthread_mutex_unlock(&usb_thread_lock));

    //Start the USB thread
    cant_fail(pthread_mutex_lock(&usb_thread_lock));
    cant_fail(pthread_create(&usb_thread, NULL, usb_thread_func, NULL));
}

void exit_libusb() {
    cant_fail(pthread_join(usb_thread, NULL));
    libusb_exit(usb_ctx);
}

void notify_usb_shutdown() {
    usb_thread_exit = true;
}

libusb_device_handle *open_usb_dev(struct usb_dev_params *params) {
    int usb_err;

    //Find the sensor device
    libusb_device **usb_devs;
    ssize_t num_usb_devs = libusb_get_device_list(usb_ctx, &usb_devs);
    if(num_usb_devs < 0) {
        int err = errno;
        log_error("Couldn't get USB device list: %d [%s] (errno %d [%s])", (int) num_usb_devs, libusb_error_name(num_usb_devs), err, strerror(err));
        return NULL;
    }

    libusb_device *sensor_dev = NULL;
    for(int i = 0; i < num_usb_devs; i++) {
        libusb_device *dev = usb_devs[i];

        //Check bus and address
        if(libusb_get_bus_number(dev) != params->bus || libusb_get_device_address(dev) != params->addr) continue;

        sensor_dev = dev;
        log_info("Found sensor USB device [bus %d addr %d]", (int) params->bus, (int) params->addr);
        break;
    }

    libusb_free_device_list(usb_devs, true);

    if(!sensor_dev) {
        log_error("Couldn't find sensor USB device! [bus %d addr %d]", (int) params->bus, (int) params->addr);
        return NULL;
    }

    //Open the sensor device
    log_info("Opening sensor USB device...");
    libusb_device_handle *sensor_handle;
    if((usb_err = libusb_open(sensor_dev, &sensor_handle)) != 0) {
        int err = errno;
        log_error("Couldn't open sensor USB device: %d [%s] (errno %d [%s])", usb_err, libusb_error_name(usb_err), err, strerror(err));
        return NULL;
    }

    return sensor_handle;
}