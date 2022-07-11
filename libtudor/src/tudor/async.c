#include "internal.h"

#define SUCCESS_PENDING -1

tudor_async_res_t async_new_res(struct tudor_device *dev, OVERLAPPED *ovlp) {
    tudor_async_res_t res = (tudor_async_res_t) malloc(sizeof(struct _async_res));
    if(!res) { perror("Couldn't allocate async result"); abort(); }
    *res = (struct _async_res) {0};
    *res = (struct _async_res) {
        .dev = dev,
        .ovlp = ovlp,
        .success = SUCCESS_PENDING,
        .cb_fnc = NULL,
        .cb_ctx = NULL
    };
    cant_fail_ret(pthread_mutex_init(&res->lock, NULL));
    cant_fail_ret(pthread_cond_init(&res->compl_cond, NULL));
    return res;
}

void async_complete_op(tudor_async_res_t res, bool success) {
    cant_fail_ret(pthread_mutex_lock(&res->lock));
    res->success = success;
    cant_fail_ret(pthread_cond_signal(&res->compl_cond));

    //Store callback data
    tudor_async_cb_fnc *cb_fnc = res->cb_fnc;
    void *cb_ctx = res->cb_ctx;

    cant_fail_ret(pthread_mutex_unlock(&res->lock));

    //Invoke callback
    if(cb_fnc) cb_fnc(res, success, cb_ctx);
}

void tudor_set_async_callback(tudor_async_res_t res, tudor_async_cb_fnc *cb, void *ctx) {
    cant_fail_ret(pthread_mutex_lock(&res->lock));
    if(res->success != SUCCESS_PENDING) cb(res, res->success, ctx);
    else {
        res->cb_fnc = cb;
        res->cb_ctx = ctx;
    }
    cant_fail_ret(pthread_mutex_unlock(&res->lock));
}

void tudor_cancel_async(tudor_async_res_t res) {
    winio_cancel_overlapped(res->ovlp);
}

bool tudor_wait_async(tudor_async_res_t res) {
    cant_fail_ret(pthread_mutex_lock(&res->lock));
    while(res->success == SUCCESS_PENDING) cant_fail_ret(pthread_cond_wait(&res->compl_cond, &res->lock));
    cant_fail_ret(pthread_mutex_unlock(&res->lock));
    return (bool) res->success;
}

void tudor_cleanup_async(tudor_async_res_t res) {
    cant_fail_ret(pthread_cond_destroy(&res->compl_cond));
    cant_fail_ret(pthread_mutex_destroy(&res->lock));
    winio_cleanup_overlapped(res->ovlp);
    free(res);
}