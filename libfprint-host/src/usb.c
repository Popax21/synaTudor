#include <stdbool.h>
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
            log_warn("Error in USB polling thread: %d [%s]", usb_err, libusb_error_name(usb_err));
        }
    }

    return NULL;
}

void start_usb_thread() {
    cant_fail(pthread_mutex_lock(&usb_thread_lock));
    cant_fail(pthread_create(&usb_thread, NULL, usb_thread_func, NULL));
}

void notify_usb_shutdown() {
    usb_thread_exit = true;
}

void init_libusb() {
    int usb_err = libusb_init(&usb_ctx);
    if(usb_err != 0) {
        log_error("Error initializing libusb: %d [%s]", usb_err, libusb_error_name(usb_err));
        abort();
    }
    cant_fail(pthread_mutex_unlock(&usb_thread_lock));
}

void exit_libusb() {
    cant_fail(pthread_join(usb_thread, NULL));
    libusb_exit(usb_ctx);
}

libusb_device_handle *open_usb_dev(struct usb_dev_params *params) {
    int usb_err;

    //Find the sensor device
    libusb_device **usb_devs;
    ssize_t num_usb_devs = libusb_get_device_list(usb_ctx, &usb_devs);
    if(num_usb_devs < 0) {
        log_error("Couldn't get USB device list: %d [%s]", (int) num_usb_devs, libusb_error_name(num_usb_devs));
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
        log_error("Couldn't open sensor USB device: %d [%s]", usb_err, libusb_error_name(usb_err));
        return NULL;
    }

    return sensor_handle;
}