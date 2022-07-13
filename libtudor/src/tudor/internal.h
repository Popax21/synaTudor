#ifndef LIBTUDOR_TUDOR_INTERNAL_H
#define LIBTUDOR_TUDOR_INTERNAL_H

#include <tudor/tudor.h>
#include "winbio.h"
#include "loader.h"
#include "wdf.h"

struct async_args_enroll {
    bool *done;
};

struct async_args_verify {
    RECGUID guid;
    enum tudor_finger finger;
    bool *retry;
    bool *matches;
};

struct async_args_identify {
    bool *retry;
    bool *found_match;
    RECGUID *guid;
    enum tudor_finger *finger;
};

struct _async_res {
    struct tudor_device *dev;
    OVERLAPPED *ovlp;
    union {
        struct async_args_enroll enroll;
        struct async_args_verify verify;
        struct async_args_identify identify; 
    } args;

    pthread_mutex_t lock;
    int success;
    pthread_cond_t compl_cond;
    tudor_async_cb_fnc *cb_fnc;
    void *cb_ctx;
};

tudor_async_res_t async_new_res(struct tudor_device *dev, OVERLAPPED *ovlp);
void async_complete_op(tudor_async_res_t res, bool success);

struct windrv_dll {
    struct winmodule module;
    uint8_t *pe_image, *pe_image_end;
    struct dll_image image;
    bool is_adapter, is_driver;
};

#define NUM_WINDRV_DLLS 2
extern struct windrv_dll tudor_windrv_dlls[];

#define WINBIO_CALL_PIPELINE(fnc, ...) if((hres = fnc(__VA_ARGS__)) != ERROR_SUCCESS) { log_error("Error in WINBIO pipeline function '%s': 0x%x!", #fnc, hres); return false; }

extern struct windrv_dll *tudor_adapter_dll, *tudor_driver_dll;
extern struct winwdf_driver *tudor_wdf_driver;
extern WINBIO_SENSOR_INTERFACE *tudor_sensor_adapter;
extern WINBIO_ENGINE_INTERFACE *tudor_engine_adapter;
extern WINBIO_STORAGE_INTERFACE *tudor_storage_adapter;

bool tudor_reg_handler(void *ctx, void *ctx_obj, const char *key_name, const char *val_name, bool is_write, void *buf, size_t *buf_size, enum winreg_val_type *val_type);

#endif