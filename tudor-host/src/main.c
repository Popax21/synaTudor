#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <tudor/log.h>
#include <tudor/libcrypto.h>
#include <tudor/tudor.h>
#include "sandbox.h"
#include "usb.h"
#include "ipc.h"

static void recv_init_msg(int sock, struct usb_dev_params *usb_params) {
    struct ipc_msg_init init_msg;
    ipc_recv_msg(sock, &init_msg, IPC_MSG_INIT, sizeof(init_msg), sizeof(init_msg), NULL);

    LOG_LEVEL = init_msg.log_level;
    *usb_params = (struct usb_dev_params) {
        .bus = init_msg.usb_bus,
        .addr = init_msg.usb_addr
    };

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

    //Receive the init message
    int sock = STDIN_FILENO;
    struct usb_dev_params usb_params;
    recv_init_msg(sock, &usb_params);
    log_info("Received and handled init message");

    //Initialize libcrypto
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    log_info("Initialized libcrypto");

    //Initialize libusb
    init_libusb();
    log_info("Initialized libusb");

    //Activate sandbox
    sandbox_ipc_sock = sock;
    activate_sandbox();
    log_info("Activated sandbox");

    //Initialize driver
    if(!tudor_init()) {
        log_error("Couldn't initialize tudor driver!");
        return EXIT_FAILURE;
    }
    log_info("Initialized tudor driver");

    //Open USB device
    libusb_device_handle *usb_dev = open_usb_dev(&usb_params);
    if(!usb_dev) {
        log_error("Couldn't open USB device");
        return EXIT_FAILURE;
    }
    log_info("Opened USB device");

    //Open device
    struct tudor_device dev;
    struct tudor_device_state state;
    if(!tudor_open(&dev, usb_dev, &state, (tudor_reenumerate_fnc*) open_usb_dev, &usb_params)) {
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

    notify_usb_shutdown();

    //Close device
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

    //Shutdown libusb
    exit_libusb();
    log_info("Shutdown libusb");

    return EXIT_SUCCESS;
}