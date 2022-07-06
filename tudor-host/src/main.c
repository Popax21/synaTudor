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
    ipc_recv_msg(sock, &init_msg, IPC_MSG_INIT, sizeof(init_msg), sizeof(init_msg));

    LOG_LEVEL = init_msg.log_level;
    *usb_params = (struct usb_dev_params) {
        .bus = init_msg.usb_bus,
        .addr = init_msg.usb_addr
    };
}

static void transfer_sandbox_notif_fd(int sock, int notif_fd) {
    enum ipc_msg_type msg_payload = IPC_MSG_SANDBOX;
    struct iovec iov = {
        .iov_base = &msg_payload,
        .iov_len = sizeof(msg_payload)
    };

    struct {
        struct cmsghdr hdr;
        int fd;
    } cmsg = {
        .hdr.cmsg_level = SOL_SOCKET,
        .hdr.cmsg_type = SCM_RIGHTS,
        .hdr.cmsg_len = CMSG_LEN(sizeof(int)),
        .fd = notif_fd
    };

    struct msghdr msg_hdr = {
        .msg_name = NULL,
        .msg_namelen = 0,
        .msg_iov = &iov,
        .msg_iovlen = 1,
        .msg_control = &cmsg,
        .msg_controllen = sizeof(cmsg)
    };

    cant_fail(sendmsg(sock, &msg_hdr, 0));
}

int main() {
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
    int seccomp_notif_fd;
    activate_sandbox(&seccomp_notif_fd);
    transfer_sandbox_notif_fd(sock, seccomp_notif_fd);
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