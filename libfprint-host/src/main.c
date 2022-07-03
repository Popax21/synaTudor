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

static char sensor_name[IPC_SENSOR_NAME_SIZE+1] = {0};
static size_t pair_data_size;
static uint8_t pair_data_buf[IPC_MAX_PAIR_DATA_SIZE];

static void recv_init_msg(int sock, struct usb_dev_params *usb_params) {
    struct ipc_msg_init *init_msg = (struct ipc_msg_init*) malloc(sizeof(struct ipc_msg_init) + IPC_MAX_PAIR_DATA_SIZE);
    if(!init_msg) abort_perror("Couldn't allocate memory for init message");

    pair_data_size = ipc_recv_msg(sock, init_msg, IPC_MSG_INIT, sizeof(struct ipc_msg_init), sizeof(struct ipc_msg_init) + IPC_MAX_PAIR_DATA_SIZE) - sizeof(struct ipc_msg_init);

    LOG_LEVEL = (enum log_level) init_msg->log_level;
    *usb_params = (struct usb_dev_params) {
        .bus = init_msg->usb_bus,
        .addr = init_msg->usb_addr
    };

    strncpy(sensor_name, init_msg->sensor_name, IPC_SENSOR_NAME_SIZE);
    memcpy(pair_data_buf, init_msg->pair_data, pair_data_size);

    free(init_msg);
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
        .hdr.cmsg_len = SOL_SOCKET,
        .hdr.cmsg_type = SCM_RIGHTS,
        .hdr.cmsg_len = sizeof(int),
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

static void send_ready_msg(int sock) {
    struct ipc_msg_ready *ready_msg = (struct ipc_msg_ready*) malloc(sizeof(struct ipc_msg_ready) + pair_data_size);
    if(!ready_msg) abort_perror("Couldn't allocate memory for ready message");

    ready_msg->type = IPC_MSG_READY;
    strncpy(ready_msg->sensor_name, sensor_name, IPC_SENSOR_NAME_SIZE);
    memcpy(ready_msg->pair_data, pair_data_buf, pair_data_size);

    ipc_send_msg(sock, ready_msg, sizeof(struct ipc_msg_ready) + pair_data_size);
    free(ready_msg);
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

    //Start the USB thread
    start_usb_thread();
    log_info("Started USB thread");

    //Activate sandbox
    int seccomp_notif_fd;
    activate_sandbox(&seccomp_notif_fd);
    transfer_sandbox_notif_fd(sock, seccomp_notif_fd);
    log_info("Activated sandbox");

    //Initialize libcrypto
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    log_info("Initialized libcrypto");

    //Initialize libusb
    init_libusb();
    log_info("Initialized libusb");

    //Initialize driver
    if(!tudor_init()) {
        log_error("Couldn't initialize tudor driver!");
        return EXIT_FAILURE;
    }
    log_info("Initialized tudor driver");

    //Register init pairing data
    struct tudor_pair_data *init_pair_data = NULL;
    if(pair_data_size > 0) {
        tudor_add_pair_data(sensor_name, pair_data_buf, pair_data_size);
        cant_fail(pthread_mutex_lock(&tudor_pair_data_lock));
        init_pair_data = tudor_pair_data_head;
        cant_fail(pthread_mutex_unlock(&tudor_pair_data_lock));
    }

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

    //Get pairing data for sensor
    cant_fail(pthread_mutex_lock(&tudor_pair_data_lock));

    struct tudor_pair_data *pair_data = tudor_pair_data_head;
    if(pair_data && pair_data->next && pair_data == init_pair_data) pair_data = pair_data->next;

    if(!pair_data){
        log_error("No pairing data found for sensor!");
        return EXIT_FAILURE;
    }

    strncpy(sensor_name, pair_data->name, IPC_SENSOR_NAME_SIZE);
    memcpy(pair_data_buf, pair_data->data, pair_data_size = pair_data->data_size);

    cant_fail(pthread_mutex_unlock(&tudor_pair_data_lock));

    //Send ready message
    send_ready_msg(sock);
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