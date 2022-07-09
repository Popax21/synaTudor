#include "internal.h"

struct wdf_evtqueue_action {
    struct wdf_evtqueue_action *next;
    struct wdf_object *object;
    struct wdf_evtqueue_action *obj_next;
    wdf_evtqueue_action_fnc *action;
};

static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static __thread bool flushing_queue = false;
static struct wdf_evtqueue_action *queue_head = NULL;

void winwdf_event_queue_flush() {
    WIN_CLOBBER_NONVOL_REGS

    cant_fail_ret(pthread_mutex_lock(&queue_mutex));
    flushing_queue = true;

    //Flush the queue
    if(queue_head) log_debug("Flushing WDF event queue...");
    while(queue_head) {
        struct wdf_evtqueue_action *act = queue_head;
        queue_head = act->next;

        if(act->object) {
            act->object->evtqueue_acts_head = act->obj_next;
            act->action(act->object);
        }

        free(act);
    }

    flushing_queue = false;
    cant_fail_ret(pthread_mutex_unlock(&queue_mutex));
}

void wdf_evtqueue_enqueue(struct wdf_object *obj, wdf_evtqueue_action_fnc *action) {
    WIN_CLOBBER_NONVOL_REGS

    //Create action object
    struct wdf_evtqueue_action *act = (struct wdf_evtqueue_action*) malloc(sizeof(struct wdf_evtqueue_action));
    if(!act) { perror("Couldn't allocate memory for WDF event queue action"); abort(); }
    act->object = obj;
    act->action = action;

    //Enqueue action
    if(!flushing_queue) cant_fail_ret(pthread_mutex_lock(&queue_mutex));
    act->next = queue_head;
    queue_head = act;
    if(!flushing_queue) cant_fail_ret(pthread_mutex_unlock(&queue_mutex));

    cant_fail_ret(pthread_mutex_lock(&obj->evtqueue_lock));
    act->obj_next = obj->evtqueue_acts_head;
    obj->evtqueue_acts_head = act;
    cant_fail_ret(pthread_mutex_unlock(&obj->evtqueue_lock));
}

void wdf_evtqueue_clear_obj(struct wdf_object *obj) {
    cant_fail_ret(pthread_mutex_lock(&obj->evtqueue_lock));

    //Orphan queued actions
    for(struct wdf_evtqueue_action *act = obj->evtqueue_acts_head; act; act = act->obj_next) act->object = NULL;
    obj->evtqueue_acts_head = NULL;

    cant_fail_ret(pthread_mutex_unlock(&obj->evtqueue_lock));
}