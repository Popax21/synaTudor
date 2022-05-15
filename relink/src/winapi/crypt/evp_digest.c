#include <openssl/evp.h>
#include "crypt.h"

struct evp_md_algorithm {
    struct crypt_hash_algorithm algo;
    ALG_ID algid;
    const EVP_MD *(*get_evp_md)();
};

struct evp_md_hash {
    const EVP_MD *evp_md;
    EVP_MD_CTX *evp_ctx;
    bool is_completed;
};

static BOOL evp_md_create_hash(struct evp_md_algorithm *algo, struct crypt_key *key, struct evp_md_hash **out) {
    if(key) { winerr_set_errno(); return FALSE; }

    //Allocate hash
    struct evp_md_hash *hash = (struct evp_md_hash*) malloc(sizeof(struct evp_md_hash));
    if(!hash) { winerr_set_errno(); return FALSE; }
    hash->evp_md = algo->get_evp_md();
    hash->is_completed = false;

    LIBCRYPTO_ERR(hash->evp_ctx = EVP_MD_CTX_new());
    LIBCRYPTO_ERR(EVP_DigestInit_ex(hash->evp_ctx, hash->evp_md, NULL));

    *out = hash;
    return TRUE;
}

static BOOL evp_md_duplicate_hash(struct evp_md_algorithm *algo, struct evp_md_hash *hash, struct evp_md_hash **out) {
    //Allocate hash
    struct evp_md_hash *nhash = (struct evp_md_hash*) malloc(sizeof(struct evp_md_hash));
    if(!nhash) { winerr_set_errno(); return FALSE; }
    nhash->evp_md = hash->evp_md;
    nhash->is_completed = hash->is_completed;

    LIBCRYPTO_ERR(nhash->evp_ctx = EVP_MD_CTX_new());
    LIBCRYPTO_ERR(EVP_MD_CTX_copy(nhash->evp_ctx, hash->evp_ctx));

    *out = nhash;
    return TRUE;
}

static BOOL evp_md_get_hash_param(struct evp_md_algorithm *algo, struct evp_md_hash *hash, DWORD param, void *data, size_t *data_size) {
    switch(param) {
        case HP_ALGID: {
            if(data && *data_size >= sizeof(ALG_ID)) {
                *((ALG_ID*) data) = algo->algid;
            } else if(data) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
            *data_size = sizeof(ALG_ID);
            return TRUE;
        }
        case HP_HASHVAL: {
            if(hash->is_completed) { winerr_set(); return FALSE; }
            if(data && *data_size >= EVP_MD_size(hash->evp_md)) {
                unsigned int s;
                LIBCRYPTO_ERR(EVP_DigestFinal_ex(hash->evp_ctx, (unsigned char*) data, &s));
                hash->is_completed = TRUE;
            } else if(data) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
            *data_size = EVP_MD_size(hash->evp_md);
            return TRUE;
        }
        case HP_HASHSIZE: {
            if(data && *data_size >= sizeof(ULONG)) {
                *((ULONG*) data) = (ULONG) EVP_MD_size(hash->evp_md);
            } else if(data) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
            *data_size = sizeof(ULONG);
            return TRUE;
        }
        default: return FALSE;
    }
}

static BOOL evp_md_set_hash_param(struct evp_md_algorithm *algo, struct evp_md_hash *hash, DWORD param, const void *data) {
    switch(param) {
        case HP_HASHVAL: {
            log_warn("evp_md_set_hash_param | Tried to set hash value!");
            winerr_set_errno();
            return FALSE;
        }
        default: winerr_set_errno(); return FALSE;
    }
}

static BOOL evp_md_update_hash(struct evp_md_algorithm *algo, struct evp_md_hash *hash, const void *data, size_t data_size) {
    if(hash->is_completed) { winerr_set(); return FALSE; }
    LIBCRYPTO_ERR(EVP_DigestUpdate(hash->evp_ctx, data, data_size));
    return TRUE;
}

static void evp_md_destroy_hash(struct evp_md_algorithm *algo, struct evp_md_hash *hash) {
    EVP_MD_CTX_free(hash->evp_ctx);
    free(hash);
}

struct evp_md_algorithm
    crypt_hash_algo_sha1 = {
        .algo = {
            .create_hash = (crypt_create_hash_fnc*) evp_md_create_hash,
            .duplicate_hash = (crypt_duplicate_hash_fnc*) evp_md_duplicate_hash,
            .get_hash_param = (crypt_get_hash_param_fnc*) evp_md_get_hash_param,
            .set_hash_param = (crypt_set_hash_param_fnc*) evp_md_set_hash_param,
            .update_hash = (crypt_update_hash_fnc*) evp_md_update_hash,
            .destroy_hash = (crypt_destroy_hash_fnc*) evp_md_destroy_hash
        },
        .algid = CALG_SHA1,
        .get_evp_md = EVP_sha1
    },
    crypt_hash_algo_sha256 = {
        .algo = {
            .create_hash = (crypt_create_hash_fnc*) evp_md_create_hash,
            .duplicate_hash = (crypt_duplicate_hash_fnc*) evp_md_duplicate_hash,
            .get_hash_param = (crypt_get_hash_param_fnc*) evp_md_get_hash_param,
            .set_hash_param = (crypt_set_hash_param_fnc*) evp_md_set_hash_param,
            .update_hash = (crypt_update_hash_fnc*) evp_md_update_hash,
            .destroy_hash = (crypt_destroy_hash_fnc*) evp_md_destroy_hash
        },
        .algid = CALG_SHA_256,
        .get_evp_md = EVP_sha256
    },
    crypt_hash_algo_sha384 = {
        .algo = {
            .create_hash = (crypt_create_hash_fnc*) evp_md_create_hash,
            .duplicate_hash = (crypt_duplicate_hash_fnc*) evp_md_duplicate_hash,
            .get_hash_param = (crypt_get_hash_param_fnc*) evp_md_get_hash_param,
            .set_hash_param = (crypt_set_hash_param_fnc*) evp_md_set_hash_param,
            .update_hash = (crypt_update_hash_fnc*) evp_md_update_hash,
            .destroy_hash = (crypt_destroy_hash_fnc*) evp_md_destroy_hash
        },
        .algid = CALG_SHA_384,
        .get_evp_md = EVP_sha384
    },
    crypt_hash_algo_sha512 = {
        .algo = {
            .create_hash = (crypt_create_hash_fnc*) evp_md_create_hash,
            .duplicate_hash = (crypt_duplicate_hash_fnc*) evp_md_duplicate_hash,
            .get_hash_param = (crypt_get_hash_param_fnc*) evp_md_get_hash_param,
            .set_hash_param = (crypt_set_hash_param_fnc*) evp_md_set_hash_param,
            .update_hash = (crypt_update_hash_fnc*) evp_md_update_hash,
            .destroy_hash = (crypt_destroy_hash_fnc*) evp_md_destroy_hash
        },
        .algid = CALG_SHA_512,
        .get_evp_md = EVP_sha512
    }
;