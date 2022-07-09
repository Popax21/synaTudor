#include <fcntl.h>
#include <errno.h>
#include <tudor/log.h>

int name_to_handle_at(int dir_fd, const char *path, struct file_handle *handle, int *mount_id, int flags) {
    //FIXME
    log_debug("Hooking name_to_handle_at for path '%s' handle %p", path, handle);
    errno = -ENOSYS;
    return -1;
}

int open_by_handle_at(int mount_fd, struct file_handle *handle, int flags) {
    //FIXME
    log_debug("Hooking open_by_handle_at for handle %p", handle);
    errno = -ENOSYS;
    return -1;
}