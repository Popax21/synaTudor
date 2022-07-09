#include <stdlib.h>
#include "internal.h"

static WDF_OBJECT_CONTEXT_TYPE_INFO *resolve_context_type(WDF_OBJECT_CONTEXT_TYPE_INFO *type) {
    if(!type) return NULL;

    do {
        if(type->UniqueType && type->UniqueType != type) type = type->UniqueType;
        else if(type->EvtDriverGetUniqueContextType) type = type->EvtDriverGetUniqueContextType();
        else break;
    } while(true);

    return type;
}

void winwdf_destroy_object(WDFOBJECT obj) {
    if(obj->destr) obj->destr(obj);
    else {
        wdf_cleanup_obj(obj);
        free(obj);
    }
}

void wdf_create_obj(struct wdf_object *parent, struct wdf_object *obj, wdf_obj_destr_fnc *destr, WDF_OBJECT_ATTRIBUTES *attrs) {
    //Initialize object structure
    wdf_init_obj_list(&obj->child_list);

    obj->destr = destr;
    obj->attrs = attrs ? *attrs : (WDF_OBJECT_ATTRIBUTES) {0};

    cant_fail_ret(pthread_mutex_init(&obj->contexts_lock, NULL));
    obj->context_head = (struct wdf_object_context*) malloc(sizeof(struct wdf_object_context));
    if(!obj->context_head) { perror("Couldn't allocate WDF object context"); abort(); }
    obj->context_head->next = NULL;
    obj->context_head->attrs = obj->attrs;
    obj->context_head->type = resolve_context_type(obj->attrs.ContextTypeInfo);
    obj->context_head->data = NULL;

    cant_fail_ret(pthread_mutex_init(&obj->evtqueue_lock, NULL));
    obj->evtqueue_acts_head = NULL;

    //Add to parent object child list
    if(parent) {
        if(parent->child_list.dead) { log_error("Attempted to create object using dead parent object!"); abort(); }
        obj->parent_obj = parent;
        obj->parent_list = &parent->child_list;

        cant_fail_ret(pthread_rwlock_wrlock(&obj->parent_list->lock));

        obj->prev = NULL;
        obj->next = obj->parent_list->head;
        if(obj->parent_list->head) obj->parent_list->head->prev = obj;
        obj->parent_list->head = obj;

        cant_fail_ret(pthread_rwlock_unlock(&obj->parent_list->lock));
    } else {
        obj->parent_obj = NULL;
        obj->parent_list = NULL;
        obj->prev = obj->next = NULL;
    }
}

void wdf_cleanup_obj(struct wdf_object *obj) {
    //Destroy child objects
    wdf_destroy_obj_list(&obj->child_list);

    //Remove from parent object list
    if(obj->parent_list && !obj->parent_list->dead) {
        cant_fail_ret(pthread_rwlock_wrlock(&obj->parent_list->lock));

        if(obj->prev) obj->prev->next = obj->next;
        else obj->parent_list->head = obj->next;
        if(obj->next) obj->next->prev = obj->prev;

        cant_fail_ret(pthread_rwlock_unlock(&obj->parent_list->lock));
    }

    //Clear queued actions
    wdf_evtqueue_clear_obj(obj);

    //Free contexts
    cant_fail_ret(pthread_mutex_lock(&obj->contexts_lock));
    for(struct wdf_object_context *ctx = obj->context_head, *nctx = ctx ? ctx->next : NULL; ctx; ctx = nctx, nctx = ctx ? ctx->next : NULL) {
        if(ctx->attrs.EvtCleanupCallback) ctx->attrs.EvtCleanupCallback(obj);
        if(ctx->attrs.EvtDestroyCallback) ctx->attrs.EvtDestroyCallback(obj);
        free(ctx->data);
        free(ctx);
    }
    cant_fail_ret(pthread_mutex_unlock(&obj->contexts_lock));

    //Free memory
    cant_fail_ret(pthread_mutex_destroy(&obj->evtqueue_lock));
}

void wdf_init_obj_list(struct wdf_object_list *list) {
    cant_fail_ret(pthread_rwlock_init(&list->lock, NULL));
    list->dead = false;
    list->head = NULL;
}

void wdf_destroy_obj_list(struct wdf_object_list *list) {
    //Destroy objects
    cant_fail_ret(pthread_rwlock_wrlock(&list->lock));

    list->dead = true;
    while(list->head) winwdf_destroy_object(list->head);

    cant_fail_ret(pthread_rwlock_unlock(&list->lock));

    //Destroy the lock
    cant_fail_ret(pthread_rwlock_destroy(&list->lock));
}

__winfnc void WdfObjectDelete(WDF_DRIVER_GLOBALS *globals, struct wdf_object *obj) {
    wdf_evtqueue_enqueue(obj, winwdf_destroy_object);
}
WDFFUNC(WdfObjectDelete, 129)

__winfnc NTSTATUS WdfObjectAllocateContext(WDF_DRIVER_GLOBALS *globals, struct wdf_object *obj, WDF_OBJECT_ATTRIBUTES *ctx_attrs, void **out) {
    //Add new context to list
    struct wdf_object_context *ctx = (struct wdf_object_context*) malloc(sizeof(struct wdf_object_context));
    if(!ctx) return winerr_from_errno();

    ctx->attrs = *ctx_attrs;
    ctx->type = resolve_context_type(ctx_attrs->ContextTypeInfo);

    size_t ctx_size = ctx->attrs.ContextSizeOverride ? ctx->attrs.ContextSizeOverride : ctx->type->ContextSize;
    ctx->data = malloc(ctx_size);
    if(!ctx->data) { perror("Couldn't allocate WDF context data"); abort(); }
    memset(ctx->data, 0, ctx_size);

    cant_fail_ret(pthread_mutex_lock(&obj->contexts_lock));
    ctx->next = obj->context_head;
    obj->context_head = ctx;
    cant_fail_ret(pthread_mutex_unlock(&obj->contexts_lock));

    *out = ctx->data;
    return STATUS_SUCCESS;
}
WDFFUNC(WdfObjectAllocateContext, 124)

__winfnc void *WdfObjectGetTypedContextWorker(WDF_DRIVER_GLOBALS *globals, struct wdf_object *obj, WDF_OBJECT_CONTEXT_TYPE_INFO *type) {
    type = resolve_context_type(type);

    //Try to find the context
    void *ctx_data = NULL;
    cant_fail_ret(pthread_mutex_lock(&obj->contexts_lock));
    for(struct wdf_object_context *ctx = obj->context_head; ctx; ctx = ctx->next) {
        if(ctx->type != type) continue;

        if(!ctx->data) {
            //Allocate context data
            size_t ctx_size = ctx->attrs.ContextSizeOverride ? ctx->attrs.ContextSizeOverride : type->ContextSize;
            ctx->data = malloc(ctx_size);
            if(!ctx->data) { perror("Couldn't allocate WDF context data"); abort(); }
            memset(ctx->data, 0, ctx_size);
        }
        ctx_data = ctx->data;
    }

    cant_fail_ret(pthread_mutex_unlock(&obj->contexts_lock));
    return ctx_data;
}
WDFFUNC(WdfObjectGetTypedContextWorker, 123)