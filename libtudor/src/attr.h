#ifndef LIBTUDOR_ATTR_H
#define LIBTUDOR_ATTR_H

#define __packed  __attribute__((packed))
#define __align(a)  __attribute__((aligned(a)))

#define __constr  __attribute__((constructor))

#define __cdecl /* no need to use any attributes */
#define __winfnc __attribute__((ms_abi))

#endif