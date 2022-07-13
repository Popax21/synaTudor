#ifndef LIBTUDOR_TUDOR_TUDOR_H
#define LIBTUDOR_TUDOR_TUDOR_H

#include <stdbool.h>
#include <libusb.h>

typedef struct _HANDLE *HANDLE;

typedef struct {
    uint32_t PartA;
    uint16_t PartB, PartC;
    uint64_t PartD:16, PartE:48;
} RECGUID;

typedef struct _WINBIO_IDENTITY WINBIO_IDENTITY;
typedef struct _WINBIO_PIPELINE WINBIO_PIPELINE;

extern bool tudor_log_traces;
bool tudor_init();
bool tudor_shutdown();

typedef struct _async_res *tudor_async_res_t;
typedef void tudor_async_cb_fnc(tudor_async_res_t res, bool success, void *ctx);

void tudor_set_async_callback(tudor_async_res_t res, tudor_async_cb_fnc *cb, void *ctx);
void tudor_cancel_async(tudor_async_res_t res);
bool tudor_wait_async(tudor_async_res_t res);
void tudor_cleanup_async(tudor_async_res_t res);

struct tudor_pair_data {
    void *data;
    size_t data_size;
};
extern const struct tudor_pair_data *(*tudor_get_pdata_fnc)(const char *name);
extern void (*tudor_set_pdata_fnc)(const char *name, const struct tudor_pair_data *pdata);

struct tudor_device_state {
    bool pairing_in_process, unpairing_in_process;
    bool update_in_process;
    int init_fails, update_fails;
    uint32_t last_update_timestamp;
};

enum tudor_finger {
    TUDOR_FINGER_UNKNOWN = 0,
    TUDOR_FINGER_RH_THUMB = 1,
    TUDOR_FINGER_RH_INDEX_FINGER = 2,
    TUDOR_FINGER_RH_MIDDLE_FINGER = 3,
    TUDOR_FINGER_RH_RING_FINGER = 4,
    TUDOR_FINGER_RH_LITTLE_FINGER = 5,
    TUDOR_FINGER_LH_THUMB = 6,
    TUDOR_FINGER_LH_INDEX_FINGER = 7,
    TUDOR_FINGER_LH_MIDDLE_FINGER = 8,
    TUDOR_FINGER_LH_RING_FINGER = 9,
    TUDOR_FINGER_LH_LITTLE_FINGER = 10,
    TUDOR_FINGER_ANY = 0xff
};

struct tudor_record {
    struct tudor_record *prev, *next;
    struct tudor_record *res_next;

    WINBIO_IDENTITY *identity;

    RECGUID guid;
    enum tudor_finger finger;
    void *data;
    size_t data_size;
};

struct tudor_device {
    struct tudor_device_state state;

    //Driver data
    HANDLE reg_key;
    struct winwdf_device *wdf_device;
    struct winwdf_file *wdf_file;

    //WinBIO data
    WINBIO_PIPELINE *pipeline;
    HANDLE winbio_file;

    //Enroll data
    bool enrolling;
    RECGUID enroll_guid;
    enum tudor_finger enroll_finger;

    //Database records
    pthread_mutex_t records_lock;
    struct tudor_record *records_head;
    struct tudor_record *result_records_head, *result_records_cursor;
};

bool tudor_open(struct tudor_device *device, libusb_device_handle *usb_dev, struct tudor_device_state *state);
bool tudor_close(struct tudor_device *device);

int tudor_wipe_records(struct tudor_device *device, RECGUID *guid, enum tudor_finger finger);
bool tudor_add_record(struct tudor_device *device, RECGUID guid, enum tudor_finger finger, const void *data, size_t data_size);

bool tudor_enroll_start(struct tudor_device *device, RECGUID guid, enum tudor_finger finger);
bool tudor_enroll_capture(struct tudor_device *device, bool *done, tudor_async_res_t *res);
bool tudor_enroll_commit(struct tudor_device *device, bool *is_duplicate);
bool tudor_enroll_discard(struct tudor_device *device);

bool tudor_verify(struct tudor_device *device, RECGUID guid, enum tudor_finger finger, bool *retry, bool *matches, tudor_async_res_t *res);
bool tudor_identify(struct tudor_device *device, bool *retry, bool *found_match, RECGUID *guid, enum tudor_finger *finger, tudor_async_res_t *res);

#endif