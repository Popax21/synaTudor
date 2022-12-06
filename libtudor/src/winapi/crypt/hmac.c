#include <assert.h>
#include <openssl/evp.h>
#include "crypt.h"

struct hmac_hash {
    EVP_MD_CTX *hmac_ctx;
    EVP_PKEY *hmac_pkey;

    bool has_algo, is_completed, is_dirty;

    ALG_ID hash_algo;
    size_t hash_size;

    void *key_data;
    size_t key_size;

    void *inner_data, *outer_data;
    size_t inner_size, outer_size;
};

static BOOL hmac_create_hash(struct crypt_hash_algorithm *algo, struct crypt_key *key, struct hmac_hash **out) {
    if(!key || !key->plain_data) { winerr_set_errno(); return FALSE; }

    //Allocate the hash
    struct hmac_hash *hash = (struct hmac_hash*) malloc(sizeof(struct hmac_hash));
    if(!hash) { winerr_set_errno(); return FALSE; }
    hash->has_algo = hash->is_completed = hash->is_dirty = false;

    hash->key_data = malloc(key->plain_size);
    hash->key_size = key->plain_size;
    if(!hash->key_data) {
        winerr_set_errno();
        free(hash);
        return FALSE;
    }
    memcpy(hash->key_data, key->plain_data, key->plain_size);

    hash->inner_data = malloc(64);
    hash->inner_size = 64;
    if(!hash->inner_data) {
        winerr_set_errno();
        free(hash->key_data);
        free(hash);
        return FALSE;
    }
    memset(hash->inner_data, 0x36, 64);

    hash->outer_data = malloc(64);
    hash->outer_size = 64;
    if(!hash->outer_data) {
        winerr_set_errno();
        free(hash->key_data);
        free(hash->inner_data);
        free(hash);
        return FALSE;
    }
    memset(hash->outer_data, 0x5c, 64);

    //Create the hash context
    LIBCRYPTO_ERR(hash->hmac_ctx = EVP_MD_CTX_new());
    hash->hmac_pkey = NULL;

    *out = hash;
    return TRUE;
}

static BOOL hmac_flush(struct hmac_hash *hash) {
    if(!hash->has_algo || hash->is_completed) { winerr_set_errno(); return FALSE; }
    if(!hash->is_dirty) return TRUE;

    //Initialize the hash context
    const EVP_MD *evp;
    switch(hash->hash_algo) {
        case CALG_SHA1: evp = EVP_sha1(); break;
        case CALG_SHA_256: evp = EVP_sha256(); break;
        case CALG_SHA_384: evp = EVP_sha384(); break;
        case CALG_SHA_512: evp = EVP_sha512(); break;
        default: abort();
    }

    if(hash->hmac_pkey) EVP_PKEY_free(hash->hmac_pkey);
    LIBCRYPTO_ERR(hash->hmac_pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, (const unsigned char*) hash->key_data, hash->key_size));
    LIBCRYPTO_ERR(EVP_DigestSignInit(hash->hmac_ctx, NULL, evp, NULL, hash->hmac_pkey));
    if(hash->inner_size > 0) LIBCRYPTO_ERR(EVP_DigestSignUpdate(hash->hmac_ctx, hash->inner_data, hash->inner_size));

    hash->is_dirty = false;
    hash->is_completed = false;
    return TRUE;
}

static BOOL hmac_get_hash_param(struct crypt_hash_algorithm *algo, struct hmac_hash *hash, DWORD param, void *data, size_t *data_size) {
    switch(param) {
        case HP_ALGID: {
            if(data && *data_size >= sizeof(ALG_ID)) {
                *((ALG_ID*) data) = CALG_HMAC;
            } else if(data) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
            *data_size = sizeof(ALG_ID);
            return TRUE;
        }
        case HP_HASHVAL: {
            if(!hmac_flush(hash)) return FALSE;

            if(data && *data_size >= hash->hash_size) {
                if(hash->outer_size > 0) LIBCRYPTO_ERR(EVP_DigestSignUpdate(hash->hmac_ctx, hash->outer_data, hash->outer_size));
                LIBCRYPTO_ERR(EVP_DigestSignFinal(hash->hmac_ctx, (unsigned char*) data, data_size));
                assert(*data_size == hash->hash_size);
                hash->is_completed = TRUE;
            } else if(data) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
            *data_size = hash->hash_size;
            return TRUE;
        }
        case HP_HASHSIZE: {
            if(!hash->has_algo) { winerr_set_errno(); return FALSE; }
            if(data && *data_size >= sizeof(ULONG)) {
                *((ULONG*) data) = (ULONG) hash->hash_size;
            } else if(data) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
            *data_size = sizeof(ULONG);
            return TRUE;
        }
        case HP_HMAC_INFO: {
            if(data && *data_size >= sizeof(HMAC_INFO)) {
                HMAC_INFO *info = (HMAC_INFO*) data;
                info->pbInnerString = (BYTE*) hash->inner_data;
                info->cbInnerString = (ULONG) hash->inner_size;
                info->pbOuterString = (BYTE*) hash->outer_data;
                info->cbOuterString = (ULONG) hash->outer_size;
            } else if(data) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }
            *data_size = sizeof(HMAC_INFO);
            return TRUE;
        }
        default: return FALSE;
    }
}

static BOOL hmac_set_hash_param(struct crypt_hash_algorithm *algo, struct hmac_hash *hash, DWORD param, void *data) {
    switch(param) {
        case HP_HASHVAL: {
            log_warn("hmac_set_hash_param | Tried to set hash value!");
            winerr_set_errno();
            return FALSE;
        }
        case HP_HMAC_INFO: {
            if(hash->is_completed) { winerr_set_errno(); return FALSE; }
            HMAC_INFO *info = (HMAC_INFO*) data;

            //Set algorithm
            switch(info->HashAlgid) {
                case CALG_SHA1: hash->hash_size = 20; break;
                case CALG_SHA_256: hash->hash_size = 32; break;
                case CALG_SHA_384: hash->hash_size = 48; break;
                case CALG_SHA_512: hash->hash_size = 64; break;
                default: {
                    log_warn("hmac_set_hash_param | Tried to use unsupported hash algorithm 0x%x", hash->hash_algo);
                    return FALSE;
                }
            }
            hash->hash_algo = info->HashAlgid;

            //Copy inner and outer strings
            hash->inner_data = malloc(info->cbInnerString);
            hash->inner_size = info->cbInnerString;
            if(!hash->inner_data) { winerr_set_errno(); return FALSE; }
            memcpy(hash->inner_data, info->pbInnerString, hash->inner_size);

            hash->outer_data = malloc(info->cbOuterString);
            hash->outer_size = info->cbOuterString;
            if(!hash->outer_data) { winerr_set_errno(); return FALSE; }
            memcpy(hash->outer_data, info->pbOuterString, hash->outer_size);

            hash->has_algo = true;
            hash->is_dirty = true;
            return TRUE;
        }
        default: winerr_set_errno(); return FALSE;
    }
}

static BOOL hmac_update_hash(struct crypt_hash_algorithm *algo, struct hmac_hash *hash, void *data, size_t data_size) {
    if(!hmac_flush(hash)) return FALSE;
    LIBCRYPTO_ERR(EVP_DigestSignUpdate(hash->hmac_ctx, (const unsigned char*) data, data_size));
    return TRUE;
}

static void hmac_destroy_hash(struct crypt_hash_algorithm *algo, struct hmac_hash *hash) {
    EVP_MD_CTX_free(hash->hmac_ctx);
    if(hash->hmac_pkey) EVP_PKEY_free(hash->hmac_pkey);
    free(hash->key_data);
    free(hash->inner_data);
    free(hash->outer_data);
    free(hash);
}

struct crypt_hash_algorithm crypt_hash_algo_hmac = {
    .create_hash = (crypt_create_hash_fnc*) hmac_create_hash,
    .get_hash_param = (crypt_get_hash_param_fnc*) hmac_get_hash_param,
    .set_hash_param = (crypt_set_hash_param_fnc*) hmac_set_hash_param,
    .update_hash = (crypt_update_hash_fnc*) hmac_update_hash,
    .destroy_hash = (crypt_destroy_hash_fnc*) hmac_destroy_hash,
};