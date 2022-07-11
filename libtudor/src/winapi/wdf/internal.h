#ifndef LIBTUDOR_WINAPI_WDF_INTERNAL_H
#define LIBTUDOR_WINAPI_WDF_INTERNAL_H

#include <stdlib.h>
#include <pthread.h>
#include "winapi/api.h"
#include "wdf.h"

//Objects
typedef void wdf_obj_destr_fnc(struct wdf_object *obj);

struct wdf_object_list {
    bool dead;
    pthread_rwlock_t lock;
    struct wdf_object *head;
};

struct wdf_object_context {
    struct wdf_object_context *next;
    WDF_OBJECT_ATTRIBUTES attrs;
    WDF_OBJECT_CONTEXT_TYPE_INFO *type;
    void *data;
};

struct wdf_object {
    //Object lists
    struct wdf_object *parent_obj;
    struct wdf_object_list *parent_list;
    struct wdf_object *prev, *next;

    struct wdf_object_list child_list;

    //Attributes
    WDF_OBJECT_ATTRIBUTES attrs;
    wdf_obj_destr_fnc *destr;

    //Context
    pthread_mutex_t contexts_lock;
    struct wdf_object_context *context_head;

    //Event queue
    pthread_mutex_t evtqueue_lock;
    struct wdf_evtqueue_action *evtqueue_acts_head;
};

void wdf_create_obj(struct wdf_object *parent, struct wdf_object *obj, wdf_obj_destr_fnc *destr, WDF_OBJECT_ATTRIBUTES *attrs);
void wdf_cleanup_obj(struct wdf_object *obj);

void wdf_init_obj_list(struct wdf_object_list *list);
void wdf_destroy_obj_list(struct wdf_object_list *list);

//Event queue
typedef void wdf_evtqueue_action_fnc(struct wdf_object *obj);
void wdf_evtqueue_enqueue(struct wdf_object *obj, wdf_evtqueue_action_fnc *function);
void wdf_evtqueue_clear_obj(struct wdf_object *obj);

//Strings
struct wdf_string;
void wdf_set_string(struct wdf_string *str, const char *val);

//Memory
struct wdf_memory {
    struct wdf_object object;
    void *data;
    size_t data_size;
    bool owns_data;
};

//Drivers
HANDLE wdf_get_driver_reg_key(struct winwdf_driver *driver);

//Devices
struct wdf_device_init;
struct wdf_device_init *wdf_create_device_init(struct winwdf_driver *driver, HANDLE reg_key, libusb_device_handle *usb_dev, struct winwdf_device **out_dev);
HANDLE wdf_get_device_reg_key(struct winwdf_device *driver);

struct wdf_usb_device;
libusb_device_handle *wdf_get_libusb_device(struct winwdf_device *dev);
struct wdf_usb_device * wdf_get_usb_device(struct winwdf_device *dev);
void wdf_set_usb_device(struct winwdf_device *dev, struct wdf_usb_device *usb_dev);

WDF_FILEOBJECT_CONFIG *wdf_get_fs_config(struct winwdf_device *dev);
WDF_OBJECT_ATTRIBUTES *wdf_get_fs_obj_attrs(struct winwdf_device *dev);

//Queues
struct winwdf_queue;

enum wdf_queue_req_type {
    WDF_QUEUE_REQ_CREATE,
    WDF_QUEUE_REQ_READ,
    WDF_QUEUE_REQ_WRITE,
    WDF_QUEUE_REQ_DEVCTRL,
    WDF_QUEUE_REQ_DEVCTRL_INT
};

void wdf_add_device_queue(struct winwdf_device *dev, struct winwdf_queue *queue);
void wdf_remove_device_queue(struct winwdf_device *dev, struct winwdf_queue *queue);
struct winwdf_queue *wdf_get_dispatch_queue(struct winwdf_device *dev, enum wdf_queue_req_type req_type);
void wdf_configure_device_dispatch(struct winwdf_device *dev, struct winwdf_queue *queue, enum wdf_queue_req_type req_type);

void wdf_queue_resume(struct winwdf_queue *queue);
void wdf_queue_stop(struct winwdf_queue *queue);

void wdf_queue_create(struct winwdf_queue *queue, struct winwdf_request *req);
void wdf_queue_read(struct winwdf_queue *queue, struct winwdf_request *req, size_t len);
void wdf_queue_write(struct winwdf_queue *queue, struct winwdf_request *req, size_t len);
void wdf_queue_devctrl(struct winwdf_queue *queue, struct winwdf_request *req, ULONG code, size_t in_len, size_t out_len);
void wdf_queue_int_devctrl(struct winwdf_queue *queue, struct winwdf_request *req, ULONG code, size_t in_len, size_t out_len);

//Requests
typedef NTSTATUS wdf_request_start_fnc(struct winwdf_request *req, void *ctx, struct wdf_object *target, int timeout, void **data);
typedef void wdf_request_cancel_fnc(struct winwdf_request *req, void *ctx, void *data);
typedef void wdf_request_cleanup_fnc(struct winwdf_request *req, void *ctx, void *data);

struct winwdf_request *wdf_create_request(struct wdf_object *parent, WDF_OBJECT_ATTRIBUTES *obj_attrs);
void wdf_reset_request(struct winwdf_request *req);
void wdf_configure_request(struct winwdf_request *req, struct winwdf_queue *io_queue, void *ctx, size_t in_size, size_t out_size, wdf_request_start_fnc *start_fnc, wdf_request_cancel_fnc *cancel_fnc, wdf_request_cleanup_fnc *cleanup_fnc, WDF_REQUEST_PARAMETERS *params);
void wdf_start_request(struct winwdf_request *req, struct wdf_object *target, int timeout);
void wdf_complete_request(struct winwdf_request *req, NTSTATUS status, WDF_REQUEST_COMPLETION_PARAMS *params);

NTSTATUS wdf_get_request_status(struct winwdf_request *req);
void *wdf_get_request_in_buf(struct winwdf_request *req);
void *wdf_get_request_out_buf(struct winwdf_request *req);
ULONG_PTR wdf_get_info(struct winwdf_request *req);

#endif