#include <errno.h>

#include "noredirect.h"
#include <sys/statfs.h>
#undef statfs

int statfs(const char *path, struct statfs *buf) {
    errno = ENOSYS;
    return -1;
}

int statfs64(const char *path, struct statfs64 *buf) {
    errno = ENOSYS;
    return -1;
}