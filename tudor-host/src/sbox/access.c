#include <unistd.h>
#include <errno.h>
#include <tudor/log.h>

int access(const char *path, int mode) {
    //FIXME
    log_debug("Hooking access test for path '%s' mode %d", path, mode);
    errno = EACCES;
    return -1;
}