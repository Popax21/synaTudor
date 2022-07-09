#include <fcntl.h>
#include <errno.h>
#include <tudor/log.h>

#include "noredirect.h"
#include <sys/stat.h>

#undef stat

int stat(const char *path, struct stat *buf) {
    //FIXME
    log_debug("Hooking stat for path '%s'", path);
    errno = -EACCES;
    return -1;
}

int stat64(const char *path, struct stat64 *buf) {
    //FIXME
    log_debug("Hooking stat for path '%s'", path);
    errno = -EACCES;
    return -1;
}

int statx(int dir_fd, const char *path, int flags, unsigned int mask, struct statx *buf) {
    //FIXME
    log_debug("Hooking statx for path '%s'", path);
    errno = -EACCES;
    return -1;
}