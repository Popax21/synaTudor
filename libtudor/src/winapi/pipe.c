#include "internal.h"

static pthread_mutex_t pipes_lock = PTHREAD_MUTEX_INITIALIZER;
static struct winpipe {
    struct winpipe *prev, *next;
    char *name;
    int refcnt;

    pthread_mutex_t lock;
    bool dying;

    int num_listeners;
    pthread_cond_t listen_cond;
} *pipes_head;

static NTSTATUS pipe_read(struct winpipe *pipe, OVERLAPPED *ovlp, void *buf, size_t *buf_size, void **op_ctx) {
    return WINERR_SET_CODE;
}

static NTSTATUS pipe_write(struct winpipe *pipe, OVERLAPPED *ovlp, const void *buf, size_t *buf_size, void **op_ctx) {
    return WINERR_SET_CODE;
}

static void pipe_destroy(struct winpipe *pipe) {
    cant_fail_ret(pthread_mutex_lock(&pipe->lock));
    if(--pipe->refcnt > 0) {
        cant_fail_ret(pthread_mutex_unlock(&pipe->lock));
        return;
    }
    pipe->dying = true;

    //Remove from pipes list
    cant_fail_ret(pthread_mutex_unlock(&pipes_lock));
    if(pipe->prev) pipe->prev->next = pipe->next;
    else pipes_head = pipe->next;
    if(pipe->next) pipe->next->prev = pipe->prev;
    cant_fail_ret(pthread_mutex_unlock(&pipes_lock));

    //Signal server listen functions
    cant_fail_ret(pthread_cond_broadcast(&pipe->listen_cond));
    while(pipe->num_listeners > 0) cant_fail_ret(pthread_cond_wait(&pipe->listen_cond, &pipe->lock));

    //Free memory
    cant_fail_ret(pthread_cond_destroy(&pipe->listen_cond));
    cant_fail_ret(pthread_mutex_unlock(&pipe->lock));
    cant_fail_ret(pthread_mutex_destroy(&pipe->lock));

    free(pipe->name);
    free(pipe);    
}

__winfnc HANDLE CreateNamedPipeW(const char16_t *name, DWORD open_mode, DWORD pipe_mode, DWORD max_instances, DWORD out_buf_size, DWORD in_buf_size, DWORD default_timeout, void *security_attrs) {
    //Create the pipe
    struct winpipe *pipe = (struct winpipe*) malloc(sizeof(struct winpipe));
    if(!pipe) { winerr_set_errno(); return NULL; }
    pipe->name = winstr_to_str(name);
    pipe->refcnt = 1;

    cant_fail_ret(pthread_mutex_init(&pipe->lock, NULL));
    pipe->dying = false;

    pipe->num_listeners = 0;
    cant_fail_ret(pthread_cond_init(&pipe->listen_cond, NULL));

    //Add to pipes list
    cant_fail_ret(pthread_mutex_lock(&pipes_lock));
    pipe->prev = NULL;
    pipe->next = pipes_head;
    if(pipes_head) pipes_head->prev = pipe;
    pipes_head = pipe;
    cant_fail_ret(pthread_mutex_unlock(&pipes_lock));

    return winio_create_file(pipe, (open_mode & FILE_FLAG_OVERLAPPED) != 0, (winio_read_fnc*) pipe_read, (winio_write_fnc*) pipe_write, NULL, NULL, NULL, (winio_destroy_fnc*) pipe_destroy);
}
WINAPI(CreateNamedPipeW)

__winfnc BOOL ConnectNamedPipe(HANDLE handle, OVERLAPPED *ovlp) {
    struct winpipe *pipe = (struct winpipe*) winio_get_file_context(handle);

    cant_fail_ret(pthread_mutex_lock(&pipe->lock));
    while(!pipe->dying) cant_fail_ret(pthread_cond_wait(&pipe->listen_cond, &pipe->lock));
    cant_fail_ret(pthread_cond_broadcast(&pipe->listen_cond));
    cant_fail_ret(pthread_mutex_unlock(&pipe->lock));

    return FALSE;
}
WINAPI(ConnectNamedPipe)

__winfnc BOOL DisconnectNamedPipe(HANDLE handle) {
    return FALSE;
}
WINAPI(DisconnectNamedPipe)