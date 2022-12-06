#ifndef LIBTUDOR_TUDOR_LIBCRYPTO_H
#define LIBTUDOR_TUDOR_LIBCRYPTO_H

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <tudor/log.h>

static inline void __libcrypto_error(const char *expr) {
    unsigned long err = ERR_get_error();
    log_error("Error in libcrypto: '%s' failed with error %lu [%s]", expr, err, ERR_error_string(err, NULL));
    abort();
}

#define LIBCRYPTO_ERR(expr) { if((expr) <= 0) __libcrypto_error(#expr); }
#define LIBCRYPTO_ERR_NEG(expr) { if((expr) < 0) __libcrypto_error(#expr); }

#endif