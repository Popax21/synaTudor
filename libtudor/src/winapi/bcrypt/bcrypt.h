#ifndef LIBTUDOR_WINAPI_BCRYPT_BCRYPT_H
#define LIBTUDOR_WINAPI_BCRYPT_BCRYPT_H

#include <tudor/libcrypto.h>
#include "winapi/api.h"

struct bcrypt_property {
    struct bcrypt_property *next;

    char *name;
    void *val;
    size_t val_size;
};

struct bcrypt_object {
    struct bcrypt_property *prop_head;
};

static inline struct bcrypt_property *bcrypt_find_obj_prop(struct bcrypt_object *obj, const char *name) {
    for(struct bcrypt_property *p = obj->prop_head; p; p = p->next) {
        if(strcmp(p->name, name) == 0) return p;
    }
    return NULL;
}

struct bcrypt_algorithm;
struct bcrypt_key;
struct bcrypt_secret;

typedef NTSTATUS bcrypt_create_key_fnc(struct bcrypt_algorithm *algo, void **key);
typedef NTSTATUS bcrypt_generate_sym_key_fnc(struct bcrypt_algorithm *algo, void *key, const void *secret, size_t secret_size);
typedef NTSTATUS bcrypt_generate_key_pair_fnc(struct bcrypt_algorithm *algo, void *key, int key_size);
typedef NTSTATUS bcrypt_import_key_fnc(struct bcrypt_algorithm *algo, void *key, const char *import_type, void *buf, size_t buf_size);
typedef NTSTATUS bcrypt_export_key_fnc(struct bcrypt_algorithm *algo, void *key, const char *export_type, void *buf, size_t *buf_size);
typedef NTSTATUS bcrypt_finalize_key_fnc(struct bcrypt_algorithm *algo, void *key);
typedef void bcrypt_destroy_key_fnc(struct bcrypt_algorithm *algo, void *key);

typedef NTSTATUS bcrypt_encrypt_fnc(struct bcrypt_algorithm *algo, struct bcrypt_key *key, struct bcrypt_object *prov_obj, void *pad_info, const void *iv, size_t iv_size, const void *in, size_t in_size, void *out, size_t *out_size);
typedef NTSTATUS bcrypt_decrypt_fnc(struct bcrypt_algorithm *algo, struct bcrypt_key *key, struct bcrypt_object *prov_obj, void *pad_info, const void *iv, size_t iv_size, const void *in, size_t in_size, void *out, size_t *out_size);

typedef NTSTATUS bcrypt_sign_hash_fnc(struct bcrypt_algorithm *algo, void *key, const void *hash, size_t hash_size, void *sig, size_t *sig_size);
typedef NTSTATUS bcrypt_verify_hash_fnc(struct bcrypt_algorithm *algo, void *key, const void *hash, size_t hash_size, const void *sig, size_t sig_size);

typedef NTSTATUS bcrypt_exchange_secret_fnc(struct bcrypt_algorithm *algo, void *pub_key, void *priv_key, struct bcrypt_secret *secret);

struct bcrypt_algorithm {
    const char *name;
    size_t min_key_size, max_key_size, key_size_step;
    size_t min_tag_size, max_tag_size, tag_size_step;

    bcrypt_create_key_fnc *create_key;
    bcrypt_generate_sym_key_fnc *generate_sym_key;
    bcrypt_generate_key_pair_fnc *generate_key_pair;
    bcrypt_import_key_fnc *import_key;
    bcrypt_export_key_fnc *export_key;
    bcrypt_finalize_key_fnc *finalize_key;
    bcrypt_destroy_key_fnc *destroy_key;

    bcrypt_encrypt_fnc *encrypt;
    bcrypt_decrypt_fnc *decrypt;

    bcrypt_sign_hash_fnc *sign_hash;
    bcrypt_verify_hash_fnc *verify_hash;
    bcrypt_exchange_secret_fnc *exchange_secret;
};

struct bcrypt_algo_wrap {
    struct bcrypt_object obj;
    struct bcrypt_algorithm *algo;
};

struct bcrypt_key {
    struct bcrypt_object obj;
    struct bcrypt_algorithm *algo;
    struct bcrypt_algo_wrap *algo_wrap;
    void *key_data;
};

struct bcrypt_secret {
    struct bcrypt_object obj;
    void *data;
    size_t data_size;
};

extern struct bcrypt_algorithm bcrypt_algo_aes;
extern struct bcrypt_ecc_algorithm bcrypt_algo_ecdh_p256, bcrypt_algo_ecdsa_p256;

#endif