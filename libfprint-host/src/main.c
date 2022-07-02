#include <stdlib.h>
#include "sandbox.h"

int main() {
    //Activate sandbox
    int seccomp_notif_fd;
    activate_sandbox(NULL, &seccomp_notif_fd);

    return EXIT_SUCCESS;
}