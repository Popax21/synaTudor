#include "sandbox.h"

struct utsname sbox_utsname;

int uname(struct utsname *oname) {
    *oname = sbox_utsname;
    return 0;
}