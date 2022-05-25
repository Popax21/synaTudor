#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <libusb.h>
#include "log.h"
#include "libcrypto.h"
#include "datastore.h"
#include "tudor/tudor.h"

#define TUDOR_VID 0x06cb
#define TUDOR_PID 0x00be

pthread_mutex_t LOG_LOCK = PTHREAD_MUTEX_INITIALIZER;
enum log_level LOG_LEVEL = LOG_INFO;

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

struct open_params {
    libusb_context *usb_ctx;
    int vid, pid;
};

static libusb_device_handle *open_sensor_device(struct open_params *params) {
    int usb_err;

    //Find the sensor device
    libusb_device **usb_devs;
    ssize_t num_usb_devs = libusb_get_device_list(params->usb_ctx, &usb_devs);
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
        if(dev_descr.idVendor != params->vid || dev_descr.idProduct != params->pid) continue;

        sensor_dev = dev;
        log_info("Found sensor USB device [bus %d addr %d vid 0x%04x pid 0x%04x]", (int) libusb_get_bus_number(dev), (int) libusb_get_device_address(dev), (int) dev_descr.idVendor, (int) dev_descr.idProduct);
        break;
    }

    libusb_free_device_list(usb_devs, true);

    if(!sensor_dev) {
        log_error("Couldn't find sensor USB device! [vid 0x%04x pid 0x%04x]", params->vid, params->pid);
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

static bool abort_cmd_loop = false;
static void sigint_handler(int sig) {
    if(abort_cmd_loop) {
        log_warn("Forcefully exitting!");
        exit(EXIT_FAILURE);
    }
    abort_cmd_loop = false;
}

static void run_cmd_loop(struct tudor_device *device) {
    //Command loop
    while(!abort_cmd_loop) {
        //Read command
        puts("");
        puts("Commands:");
        puts("  e - enroll finger");
        puts("  v - verify finger");
        puts("  i - identify finger");
        puts("  q - query information about enrolled fingers");
        puts("  w - wipe enrolled finger(s)");
        puts("  s - shutdown driver");

        printf("> ");
        char cmd = tolower(getchar());
        while(!abort_cmd_loop && getchar() != '\n') continue;
        if(abort_cmd_loop) break;

        //Execute command
        switch(tolower(cmd)) {
            case 'e': {
                //Read identity GUID
                GUID guid = {0};
                printf("Enter identity index: ");
                scanf("%u", &guid.PartA);
                while(!abort_cmd_loop && getchar() != '\n') continue;
                if(abort_cmd_loop) goto cmdend;

                //Read finger
                enum tudor_finger finger;
                printf("Enter finger index (1-5 = right hand thumb - little finger | 6-10 = left hand thumb - little finger): ");
                scanf("%d", (int*) &finger);
                while(!abort_cmd_loop && getchar() != '\n') continue;
                if(abort_cmd_loop) goto cmdend;
                if(finger < TUDOR_FINGER_RH_THUMB || TUDOR_FINGER_LH_LITTLE_FINGER < finger) {
                    printf("Invalid finger index!");
                    goto cmdend;
                }

                //Do enrollment
                if(!tudor_enroll_start(device, guid, finger)) {
                    log_error("Error starting enrollment!");
                    goto cmdend;
                }

                puts("Put your finger on the sensor");
                while(true) {
                    //Update enrollment
                    bool is_done;
                    if(!tudor_enroll_capture(device, &is_done)) {
                        if(!is_done) {
                            log_warn("Retrying enrollment update...");
                            continue;
                        }

                        log_error("Error capturing sample for enrollment!");
                        tudor_enroll_discard(device);
                        goto cmdend;
                    }

                    if(is_done) break;
                    puts("Driver requested more samples");
                }

                //Commit the enrollment
                bool is_dupl;
                if(!tudor_enroll_commit(device, &is_dupl)) {
                    if(is_dupl) {
                        log_warn("Detected duplicate enrollment!");
                        tudor_enroll_discard(device);
                        goto cmdend;
                    }

                    log_error("Error commiting enrollment!");
                    tudor_enroll_discard(device);
                    goto cmdend;
                }

                puts("Successfully enrolled finger");
            } goto cmdend;
            case 'v': {
                //Read identity GUID
                GUID guid = {0};
                printf("Enter identity index: ");
                scanf("%u", &guid.PartA);
                while(!abort_cmd_loop && getchar() != '\n') continue;
                if(abort_cmd_loop) goto cmdend;

                //Capture and verify sample
                puts("Put your finger on the sensor");

                bool matches;
                if(!tudor_verify(device, guid, TUDOR_FINGER_ANY, &matches)) {
                    log_error("Error verify captured sample!");
                    goto cmdend;
                }
                puts(matches ? "VERIFICATION SUCCESS" : "VERIFICATION FAILURE");
            } goto cmdend;
            case 'i': {
                //Capture and identify sample
                puts("Put your finger on the sensor");

                bool found_match;
                GUID match_guid;
                enum tudor_finger match_finger;
                if(!tudor_identify(device, &found_match, &match_guid, &match_finger)) {
                    log_error("Error verify captured sample!");
                    goto cmdend;
                }

                //Print match
                if(found_match) {
                    printf("Found match: identity index=%u finger=%d\n", match_guid.PartA, match_finger);
                } else puts("Found no match");
            } goto cmdend;
            case 'q': {
                //Read identity GUID
                bool all_guids = false;
                GUID guid = {0};
                printf("Enter identity index (invalid number for all): ");
                if(scanf("%u", &guid.PartA) < 1) all_guids = true;
                while(!abort_cmd_loop && getchar() != '\n') continue;
                if(abort_cmd_loop) goto cmdend;

                //Print records
                cant_fail(pthread_mutex_lock(&device->records_lock));

                int num_matched = 0;
                for(struct tudor_record *rec = device->records_head; rec; rec = rec->next) {
                    if(!all_guids && memcmp(&guid, &rec->identity.TemplateGuid, sizeof(GUID)) != 0) continue;
                    printf("RECORD | identity index=%10u finger=%d\n", rec->identity.TemplateGuid.PartA, rec->finger);
                    num_matched++;
                }
                printf("Total: %d record(s)\n", num_matched);

                cant_fail(pthread_mutex_unlock(&device->records_lock));
            } goto cmdend;
            case 'w': {
                //Read identity GUID
                bool all_guids = false;
                GUID guid = {0};
                printf("Enter identity index (invalid number for all): ");
                if(scanf("%u", &guid.PartA) < 1) all_guids = true;
                while(!abort_cmd_loop && getchar() != '\n') continue;
                if(abort_cmd_loop) goto cmdend;

                //Read finger
                enum tudor_finger finger;
                printf("Enter finger index (invalid number for all): ");
                if(scanf("%d", (int*) &finger) < 1) finger = TUDOR_FINGER_ANY;
                while(!abort_cmd_loop && getchar() != '\n') continue;
                if(abort_cmd_loop) goto cmdend;

                //Ask for confirmation
                printf("Do you really want to wipe all enrolled fingers with identity index=%u finger=%d?\n", guid.PartA, finger);
                printf("y/n: ");
                char yn = getchar();
                while(!abort_cmd_loop && getchar() != '\n') continue;
                if(yn != 'y') {
                    puts("Aborted wipe");
                    goto cmdend;
                }

                //Wipe records
                int num_wiped = tudor_wipe_records(device, all_guids ? NULL: &guid, finger);
                printf("Succesfully wiped %d enrolled finger(s)\n", num_wiped);
            } goto cmdend;
            case 's': abort_cmd_loop = true; goto cmdend;
            default: printf("Unknown command '%c'!\n", cmd);
        }
        cmdend:;
    }
}

int main(int argc, char **argv) {
    signal(SIGINT, sigint_handler);

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
    struct open_params open_params = {
        .usb_ctx = usb_ctx,
        .vid = sensor_vid, .pid = sensor_pid
    };

    libusb_device_handle *usb_dev = open_sensor_device(&open_params);
    if(!usb_dev) return EXIT_FAILURE;

    //Drop root privileges (if we have them)
    if(!drop_root_priv()) return EXIT_FAILURE;

    //Start the libusb polling thread
    pthread_t usb_poll_thread;
    cant_fail(pthread_create(&usb_poll_thread, NULL, libusb_poll_thread_func, usb_ctx));

    //Initialize tudor driver
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
    if(!tudor_open(&device, usb_dev, NULL, (tudor_reenumerate_fnc*) open_sensor_device, &open_params)) {
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
    run_cmd_loop(&device);

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

    //Close the USB device
    libusb_poll_thread_exit = true;
    libusb_close(usb_dev);

    //Shutdown libusb
    cant_fail(pthread_join(usb_poll_thread, NULL));
    libusb_exit(usb_ctx);

    return EXIT_SUCCESS;
}