#ifndef LIBTUDOR_WINAPI_CRYPT_CRYPT_H
#define LIBTUDOR_WINAPI_CRYPT_CRYPT_H

#include <tudor/libcrypto.h>
#include "wincrypt.h"
#include "winapi/api.h"

struct crypt_provider;
struct crypt_key;

typedef BOOL crypt_import_key_fnc(struct crypt_provider *prov, const BLOBHEADER *data, size_t data_size, void **key);
typedef void crypt_destroy_key_fnc(struct crypt_provider *prov, void *key);

struct crypt_provider {
    crypt_import_key_fnc *import_key;
    crypt_destroy_key_fnc *destroy_key;
};

struct crypt_key {
    struct crypt_provider *prov;
    void *key_data;

    void *plain_data;
    size_t plain_size;
};

extern struct crypt_provider crypt_prov_rsa_aes;

struct crypt_hash_algorithm;
struct crypt_hash;

typedef BOOL crypt_create_hash_fnc(struct crypt_hash_algorithm *algo, struct crypt_key *key, void **hash);
typedef BOOL crypt_duplicate_hash_fnc(struct crypt_hash_algorithm *algo, void *hash, void **nhash);
typedef BOOL crypt_get_hash_param_fnc(struct crypt_hash_algorithm *algo, void *hash, DWORD param, void *data, size_t *data_size);
typedef BOOL crypt_set_hash_param_fnc(struct crypt_hash_algorithm *algo, void *hash, DWORD param, const void *data);
typedef BOOL crypt_update_hash_fnc(struct crypt_hash_algorithm *algo, void *hash, const void *data, size_t data_size);
typedef void crypt_destroy_hash_fnc(struct crypt_hash_algorithm *algo, void *hash);

struct crypt_hash_algorithm {
    crypt_create_hash_fnc *create_hash;
    crypt_duplicate_hash_fnc *duplicate_hash;
    crypt_get_hash_param_fnc *get_hash_param;
    crypt_set_hash_param_fnc *set_hash_param;
    crypt_update_hash_fnc *update_hash;
    crypt_destroy_hash_fnc *destroy_hash;
};

struct crypt_hash {
    struct crypt_hash_algorithm *algo;
    void *hash_data;
};

extern struct evp_md_algorithm crypt_hash_algo_sha1, crypt_hash_algo_sha256, crypt_hash_algo_sha384, crypt_hash_algo_sha512;
extern struct crypt_hash_algorithm crypt_hash_algo_hmac;

#endif