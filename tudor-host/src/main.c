#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <tudor/log.h>
#include <tudor/libcrypto.h>
#include <tudor/tudor.h>
#include "sandbox.h"
#include "ipc.h"

static pthread_t usb_thread;
static bool usb_thread_exit = false;

static void *usb_thread_func(void *arg) {
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

static void recv_init_msg(int sock, int *usb_dev_fd, uint8_t *usb_bus, uint8_t *usb_addr) {
    struct ipc_msg_init init_msg;
    ipc_recv_msg(sock, &init_msg, IPC_MSG_INIT, sizeof(init_msg), sizeof(init_msg), usb_dev_fd);

    LOG_LEVEL = init_msg.log_level;

    switch(LOG_LEVEL) {
        case LOG_VERBOSE:
        case LOG_DEBUG:
            setenv("LIBUSB_DEBUG", "1", 1);
            libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
        break;
        case LOG_INFO: libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO); break;
        case LOG_WARN: libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_WARNING); break;
        case LOG_ERROR: libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_ERROR); break;
    }
}

int main() {
    //Configure stdout/stderr buffering
    cant_fail(setvbuf(stdout, NULL, _IOLBF, 1024));
    cant_fail(setvbuf(stderr, NULL, _IOLBF, 1024));

    //Check if stdin is a UNIX socket
    struct stat stdin_stat;
    cant_fail(fstat(STDIN_FILENO, &stdin_stat));
    if(!S_ISSOCK(stdin_stat.st_mode)) {
        fputs("This program isn't intended to be executed directly.\n", stderr);
        return EXIT_FAILURE;
    }

    int stdin_dom;
    socklen_t stdin_dom_sz = sizeof(stdin_dom);
    cant_fail(getsockopt(STDIN_FILENO, SOL_SOCKET, SO_DOMAIN, &stdin_dom, &stdin_dom_sz));
    if(stdin_dom != AF_UNIX) {
        log_error("The given socket isn't a UNIX socket!");
        return EXIT_FAILURE;
    }
    int sock = STDIN_FILENO;

    //Activate sandbox
    activate_sandbox();
    log_info("Activated sandbox");

    //Receive the init message
    int usb_dev_fd;
    uint8_t usb_bus, usb_addr;
    recv_init_msg(sock, &usb_dev_fd, &usb_bus, &usb_addr);
    setup_usb_sbox(usb_dev_fd, usb_bus, usb_addr);
    log_info("Received init message - USB device %hhd-%hhd", usb_bus, usb_addr);

    //Initialize libcrypto
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    log_info("Initialized libcrypto");

    //Initialize libusb
    int usb_err;
    if((usb_err = libusb_set_option(NULL, LIBUSB_OPTION_NO_DEVICE_DISCOVERY, true)) != 0) {
        int err = errno;
        log_error("Error disabling libusb device discovery: %d [%s] (errno %d [%s])", usb_err, libusb_error_name(usb_err), err, strerror(err));
        return EXIT_FAILURE;
    }

    libusb_context *usb_ctx;
    if((usb_err = libusb_init(&usb_ctx)) != 0) {
        int err = errno;
        log_error("Error initializing libusb: %d [%s] (errno %d [%s])", usb_err, libusb_error_name(usb_err), err, strerror(err));
        return EXIT_FAILURE;
    }
    log_info("Initialized libusb");

    //Start the USB thread
    cant_fail_ret(pthread_create(&usb_thread, NULL, usb_thread_func, usb_ctx));
    log_debug("Started USB polling thread");

    //Initialize driver
    if(!tudor_init()) {
        log_error("Couldn't initialize tudor driver!");
        return EXIT_FAILURE;
    }
    log_info("Initialized tudor driver");

    //Open the USB device
    libusb_device_handle *usb_dev;
    if((usb_err = libusb_wrap_sys_device(usb_ctx, usb_dev_fd, &usb_dev)) < 0) {
        int err = errno;
        log_error("Error opening USB device: %d [%s] (errno %d [%s])", usb_err, libusb_error_name(usb_err), err, strerror(err));
        return EXIT_FAILURE;
    }
    log_info("Opened USB device");

    //Open device
    struct tudor_device dev;
    struct tudor_device_state state;
    if(!tudor_open(&dev, usb_dev, &state)) {
        log_error("Couldn't open tudor device!");
        return EXIT_FAILURE;
    }
    log_info("Opend tudor device");

    //Send ready message
    enum ipc_msg_type ready_type = IPC_MSG_READY;
    ipc_send_msg(sock, &ready_type, sizeof(ready_type));
    log_info("Sent ready message");

    //Enter IPC handling loop
    ipc_run_handler_loop(sock);

    //Close device
    usb_thread_exit = true;
    if(!tudor_close(&dev)) {
        log_error("Couldn't close tudor device!");
    }
    log_info("Closed tudor device");

    //Shutdown tudor driver
    if(!tudor_shutdown()) {
        log_error("Couldn't shutdown tudor driver!");
        return EXIT_FAILURE;
    }
    log_info("Shutdown tudor driver");

    //Close the USB device
    libusb_close(usb_dev);
    cant_fail(close(usb_dev_fd));

    //Shutdown libusb
    cant_fail_ret(pthread_join(usb_thread, NULL));
    libusb_exit(usb_ctx);
    log_info("Shutdown libusb");

    return EXIT_SUCCESS;
}