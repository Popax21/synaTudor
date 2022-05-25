#ifndef TUDOR_TUDOR_TUDOR_H
#define TUDOR_TUDOR_TUDOR_H

#include <stdbool.h>
#include <libusb.h>
#include "winbio.h"

extern bool tudor_log_traces;
bool tudor_init();
bool tudor_shutdown();

struct tudor_pair_data {
    struct tudor_pair_data *next;
    char *name;
    void *data;
    size_t data_size;
};
extern pthread_mutex_t tudor_pair_data_lock;
extern struct tudor_pair_data *tudor_pair_data_head;
void tudor_add_pair_data(const char *name, const void *data, size_t data_size);

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

    WINBIO_IDENTITY identity;
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
    WINBIO_PIPELINE pipeline;
    HANDLE winbio_file;

    //Enroll data
    bool enrolling;
    GUID enroll_guid;
    enum tudor_finger enroll_finger;

    //Database records
    pthread_mutex_t records_lock;
    struct tudor_record *records_head;
    struct tudor_record *result_records_head, *result_records_cursor;
};

typedef libusb_device_handle *tudor_reenumerate_fnc(void *ctx);

bool tudor_open(struct tudor_device *device, libusb_device_handle *usb_dev, struct tudor_device_state *state, tudor_reenumerate_fnc *reenum_fnc, void *reenum_ctx);
bool tudor_close(struct tudor_device *device);

int tudor_wipe_records(struct tudor_device *device, GUID *guid, enum tudor_finger finger);
void tudor_add_record(struct tudor_device *device, GUID guid, enum tudor_finger finger, const void *data, size_t data_size);

bool tudor_enroll_start(struct tudor_device *device, GUID guid, enum tudor_finger finger);
bool tudor_enroll_capture(struct tudor_device *device, bool *done);
bool tudor_enroll_commit(struct tudor_device *device, bool *is_duplicate);
bool tudor_enroll_discard(struct tudor_device *device);

bool tudor_verify(struct tudor_device *device, GUID guid, enum tudor_finger finger, bool *matches);
bool tudor_identify(struct tudor_device *device, bool *found_match, GUID *guid, enum tudor_finger *finger);

#endif