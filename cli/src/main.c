#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <libusb.h>

#include <tudor/log.h>
#include <tudor/libcrypto.h>
#include <tudor/tudor.h>
#include "datastore.h"
#include "cli.h"

#define TUDOR_VID 0x06cb
#define TUDOR_PID 0x00be

static bool drop_root_priv() {
    if(geteuid() == 0 || getegid() == 0) {
        //Determine UID and GID to drop to
        uid_t uid, euid, nuid = getuid();
        if(nuid == 0) getresuid(&uid, &euid, &nuid);
        if(nuid == 0 && secure_getenv("SUDO_UID")) nuid = (uid_t) strtol(secure_getenv("SUDO_UID"), NULL, 0);

        gid_t gid, egid, ngid = getgid();
        if(ngid == 0) getresgid(&gid, &egid, &ngid);
        if(ngid == 0 && secure_getenv("SUDO_GID")) ngid = (uid_t) strtol(secure_getenv("SUDO_GID"), NULL, 0);

        if(nuid == 0 || ngid == 0) {
            log_error("Running as root and not able to determine UID/GID to switch to!");
            return false;
        }

        //Drop privileges
        log_info("Dropping root privileges... [new uid=%d new gid=%d]", nuid, ngid);
        cant_fail(setresgid(ngid, ngid, ngid));
        cant_fail(setresuid(nuid, nuid, nuid));

        //Be paranoid and really make sure that we dropped privileges
        if(geteuid() == 0 || getegid() == 0) {
            log_error("Somehow wasn't able to drop root privileges!");
            return false;
        }

        if(setuid(0) == 0 || seteuid(0) == 0|| setgid(0) == 0 || setegid(0) == 0) {
            log_error("Was somehow able to regain root privileges!");
            return false;
        }
    }
    return true;
}

static libusb_device_handle *open_sensor_device(libusb_context *usb_ctx, int vid, int pid) {
    int usb_err;

    //Find the sensor device
    libusb_device **usb_devs;
    ssize_t num_usb_devs = libusb_get_device_list(usb_ctx, &usb_devs);
    if(num_usb_devs < 0) {
        log_error("Error getting USB device list: %d [%s]", (int) num_usb_devs, libusb_error_name(num_usb_devs));
        return NULL;
    }

    libusb_device *sensor_dev = NULL;
    for(int i = 0; i < num_usb_devs; i++) {
        libusb_device *dev = usb_devs[i];

        //Get device properties
        struct libusb_device_descriptor dev_descr;
        if((usb_err = libusb_get_device_descriptor(dev, &dev_descr)) != 0) {
            log_error("Error getting USB device descriptor: %d [%s]", usb_err, libusb_error_name(usb_err));
            return NULL;
        }

        //Check VID and PID
        if(dev_descr.idVendor != vid || dev_descr.idProduct != pid) continue;

        sensor_dev = dev;
        log_info("Found sensor USB device [bus %d addr %d vid 0x%04x pid 0x%04x]", (int) libusb_get_bus_number(dev), (int) libusb_get_device_address(dev), (int) dev_descr.idVendor, (int) dev_descr.idProduct);
        break;
    }

    libusb_free_device_list(usb_devs, true);

    if(!sensor_dev) {
        log_error("Couldn't find sensor USB device! [vid 0x%04x pid 0x%04x]", vid, pid);
        return NULL;
    }

    //Open the sensor device
    log_info("Opening sensor USB device...");
    libusb_device_handle *sensor_handle;
    if((usb_err = libusb_open(sensor_dev, &sensor_handle)) != 0) {
        log_error("Error opening sensor USB device: %d [%s]", usb_err, libusb_error_name(usb_err));
        return NULL;
    }

    return sensor_handle;
}

static bool libusb_poll_thread_exit = false;
static void *libusb_poll_thread_func(void *arg) {
    while(!libusb_poll_thread_exit) {
        int usb_err;
        if((usb_err = libusb_handle_events((libusb_context*) arg)) != 0) {
            log_warn("Error in libusb polling thread: %d [%s]", usb_err, libusb_error_name(usb_err));
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    //Parse arguments
    if(argc < 2) {
        log_error("Usage: %s <data store> [flags]", argv[0]);
        return EXIT_FAILURE;
    }

    int sensor_vid = TUDOR_VID, sensor_pid = TUDOR_PID;
    for(int i = 2; i < argc; i++) {
        for(char *p = argv[i]+1; *p; p++) {
            if(*p == 'v' && LOG_LEVEL > LOG_VERBOSE) LOG_LEVEL--; 
            if(*p == 'q' && LOG_LEVEL < LOG_ERROR) LOG_LEVEL++;
            if(*p == 't') tudor_log_traces = true;
            if(*p == 'V') { sensor_vid = strtol(p+1, NULL, 16); break; }
            if(*p == 'P') { sensor_pid = strtol(p+1, NULL, 16); break; }
        }
    }
    sensor_vid &= 0xffff;
    sensor_pid &= 0xffff;

    //Ask if one wants to really use this
    puts(">>>>> WARNING <<<<<");
    puts("Even though the CLI employs sandboxing, its security is in no way comparable to the one found in the libfprint integration.");
    puts("A malicious driver could take over your local user account!");
    puts("This CLI is only intended to be used for debugging and/or small scale tests.");
    printf("Press 'y' to continue, any key to exit: ");
    char chr = 0;
    scanf("%c", &chr);
    if(chr != 'y') { 
        puts("Exiting....");
        return EXIT_FAILURE;
    }

    //Initialize libcrypto
    log_info("Initializing libcrypto...");
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    //Initialize libusb
    log_info("Initializing libusb...");
    libusb_context *usb_ctx;
    int usb_err;
    if((usb_err = libusb_init(&usb_ctx)) != 0) {
        log_error("Error initializing libusb: %d [%s]", usb_err, libusb_error_name(usb_err));
        return EXIT_FAILURE;
    }

    //Open the sensor device
    libusb_device_handle *usb_dev = open_sensor_device(usb_ctx, sensor_vid, sensor_pid);
    if(!usb_dev) return EXIT_FAILURE;

    //Drop root privileges (if we have them)
    if(!drop_root_priv()) return EXIT_FAILURE;

    //Start the libusb polling thread
    pthread_t usb_poll_thread;
    cant_fail_ret(pthread_create(&usb_poll_thread, NULL, libusb_poll_thread_func, usb_ctx));

    //Initialize tudor driver
    tudor_get_pdata_fnc = get_pair_data;
    tudor_set_pdata_fnc = set_pair_data;
    log_info("Initializing tudor driver...");
    if(!tudor_init()) {
        log_error("Error initializing tudor driver!");
        return EXIT_FAILURE;
    }

    //Load data from data store
    log_info("Loading data from data store '%s'...", argv[1]);

    FILE *data_store = fopen(argv[1], "r+");
    if(!data_store && access(argv[1], F_OK)) data_store = fopen(argv[1], "w+");
    if(!data_store) {
        perror("Couldn't open data store file");
        return EXIT_FAILURE;
    }

    if(!load_datastore_pair_data(data_store)) {
        log_error("Couldn't load pairing data from data store!");
        return EXIT_FAILURE;
    }

    //Open the device
    log_info("Opening tudor device...");
    struct tudor_device device;
    if(!tudor_open(&device, usb_dev, NULL)) {
        log_error("Error opening tudor device!");
        return EXIT_FAILURE;
    }

    //Load records
    log_info("Loading device records...");
    if(!load_datastore_records(data_store, &device)) {
        log_error("Couldn't load records from data store!");
        return EXIT_FAILURE;
    }

    //Main command loop
    cli_main_loop(&device);

    //Save data to store
    log_info("Saving data to data store '%s'...", argv[1]);

    if(ftruncate(fileno(data_store), 0) || fseek(data_store, 0, SEEK_SET)) {
        perror("Error writing to data store");
        return EXIT_FAILURE;
    }

    if(!save_datastore_pair_data(data_store)) {
        log_error("Couldn't save pairing data to data store!");
        return EXIT_FAILURE;
    }
    if(!save_datastore_records(data_store, &device)) {
        log_error("Couldn't save records to data store!");
        return EXIT_FAILURE;
    }

    if(fclose(data_store)) {
        perror("Error closing data store");
        return EXIT_FAILURE;
    }

    //Close the device
    log_info("Closing tudor device...");
    if(!tudor_close(&device)) {
        log_error("Error closing tudor device!");
        return EXIT_FAILURE;
    }

    //Shutdown tudor driver
    log_info("Shutting down tudor driver...");
    if(!tudor_shutdown()) {
        log_error("Error shutting down tudor driver!");
        return EXIT_FAILURE;
    }
    free_pair_data();

    //Close the USB device
    libusb_poll_thread_exit = true;
    libusb_close(usb_dev);

    //Shutdown libusb
    cant_fail_ret(pthread_join(usb_poll_thread, NULL));
    libusb_exit(usb_ctx);

    return EXIT_SUCCESS;
}