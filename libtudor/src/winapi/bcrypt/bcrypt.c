#include <openssl/evp.h>
#include "bcrypt.h"

typedef struct {
    ULONG dwMinLength;
    ULONG dwMaxLength;
    ULONG dwIncrement;
} BCRYPT_KEY_LENGTHS_STRUCT;

#define BCRYPTBUFFER_VERSION 0

#define KDF_HASH_ALGORITHM 0x0
#define KDF_SECRET_PREPEND 0x1
#define KDF_SECRET_APPEND 0x2
#define KDF_HMAC_KEY 0x3
#define KDF_TLS_PRF_LABEL 0x4
#define KDF_TLS_PRF_SEED 0x5

typedef struct {
  ULONG cbBuffer;
  ULONG BufferType;
  void *pvBuffer;
} BCryptBuffer;

typedef struct {
    ULONG ulVersion;
    ULONG cBuffers;
    BCryptBuffer *pBuffers;
} BCryptBufferDesc;

#define NUM_BCRYPT_ALGORITHMS 3
struct bcrypt_algorithm *bcrypt_algos[] = {
    &bcrypt_algo_aes,
    (struct bcrypt_algorithm*) &bcrypt_algo_ecdh_p256, (struct bcrypt_algorithm*) &bcrypt_algo_ecdsa_p256
};

static void add_obj_prop(struct bcrypt_object *obj, const char *name, const void *val, size_t val_size) {
    //Allocate property
    struct bcrypt_property* prop = (struct bcrypt_property*) malloc(sizeof(struct bcrypt_property));
    if(!prop) { perror("Couldn't allocate memory for bcrypt object property"); abort(); }

    prop->name = strdup(name);
    if(!prop->name) { perror("Couldn't allocate memory for bcrypt object property name"); abort(); }

    prop->val = malloc(val_size);
    prop->val_size = val_size;
    if(!prop->val) { perror("Couldn't allocate memory for bcrypt object property value"); abort(); }
    memcpy(prop->val, val, val_size);

    //Add to property list
    prop->next = obj->prop_head;
    obj->prop_head = prop;
}

static void destroy_obj(struct bcrypt_object *obj) {
    //Free properties
    for(struct bcrypt_property *p = obj->prop_head, *np = p ? p->next : NULL; p; p = np, np = p ? p->next : NULL) {
        free(p->name);
        free(p->val);
        free(p);
    }
}

static NTSTATUS create_key(struct bcrypt_algo_wrap *algo, struct bcrypt_key **out) {
    if(!algo->algo->create_key) return WINERR_SET_CODE;

    //Allocate key
    struct bcrypt_key *key = (struct bcrypt_key*) malloc(sizeof(struct bcrypt_key));
    if(!key) return winerr_from_errno();
    *key = (struct bcrypt_key) {0};
    key->algo = algo->algo;
    key->algo_wrap = algo;
    key->key_data = NULL;

    //Create key
    NTSTATUS status = algo->algo->create_key(algo->algo, &key->key_data);
    if(status != STATUS_SUCCESS) {
        free(key);
        return status;
    }

    *out = key;
    return STATUS_SUCCESS;
}

static void destroy_key(struct bcrypt_key *key) {
    if(key->algo->destroy_key) key->algo->destroy_key(key->algo, key->key_data);
    destroy_obj(&key->obj);
    free(key);
}

__winfnc NTSTATUS BCryptGetProperty(struct bcrypt_object *obj, const char16_t *name, UCHAR *out, ULONG out_size, ULONG *res_size, ULONG flags) {
    //Find property
    char *cname = winstr_to_str(name);
    struct bcrypt_property *prop = bcrypt_find_obj_prop(obj, cname);

    if(!prop) { 
        log_warn("BCryptGetProperty | Unknown property '%s'!", cname);
        return WINERR_SET_CODE;
    } else free(cname);

    //Copy property value
    if(!out) { *res_size = (ULONG) prop->val_size; return STATUS_SUCCESS; }
    *res_size = (out_size < prop->val_size) ? out_size : (ULONG) prop->val_size;
    memcpy(out, prop->val, *res_size);

    return STATUS_SUCCESS;
}
WINAPI(BCryptGetProperty)

__winfnc NTSTATUS BCryptSetProperty(struct bcrypt_object *obj, const char16_t *name, UCHAR *in, ULONG in_size, ULONG flags) {
    //Find existing property
    char *cname = winstr_to_str(name);
    struct bcrypt_property *prop = bcrypt_find_obj_prop(obj, cname);

    if(!prop) {
        //Create new property
        prop = (struct bcrypt_property*) malloc(sizeof(struct bcrypt_property));
        if(!prop) { free(cname); return winerr_from_errno(); }
        prop->name = cname;
        prop->val = NULL;
        prop->val_size = 0;

        prop->next = obj->prop_head;
        obj->prop_head = prop;
    } else free(cname);

    //Set property value
    free(prop->val);
    prop->val = malloc(in_size);
    prop->val_size = in_size;
    if(!prop->val) return winerr_from_errno();
    memcpy(prop->val, in, in_size);

    return STATUS_SUCCESS;
}
WINAPI(BCryptSetProperty)

__winfnc NTSTATUS BCryptOpenAlgorithmProvider(struct bcrypt_algo_wrap **out, const char16_t *alg_id, const char16_t *impl, ULONG flags) {
    char *alg_cid = winstr_to_str(alg_id);

    //Try to find the algorithm
    struct bcrypt_algorithm *algo = NULL;
    for(int i = 0; i < NUM_BCRYPT_ALGORITHMS; i++) {
        if(strcmp(alg_cid, bcrypt_algos[i]->name) == 0) {
            algo = bcrypt_algos[i];
            break;
        }
    }

    if(!algo) {
        log_warn("BCryptOpenAlgorithmProvider | algorithm '%s' not found!", alg_cid);
        free(alg_cid);
        return WINERR_SET_CODE;
    } else free(alg_cid);

    //Create the algorithm wraper
    struct bcrypt_algo_wrap *wrap = (struct bcrypt_algo_wrap*) malloc(sizeof(struct bcrypt_algo_wrap));
    if(!wrap) return winerr_from_errno();
    *wrap = (struct bcrypt_algo_wrap) {0};
    wrap->algo = algo;

    //Add default properties
    add_obj_prop(&wrap->obj, "AlgorithmName", alg_id, (winstr_len(alg_id)+1) * sizeof(char16_t));

    BCRYPT_KEY_LENGTHS_STRUCT kls = { .dwMinLength = (ULONG) algo->min_key_size, .dwMaxLength = (ULONG) algo->max_key_size, .dwIncrement = (ULONG) algo->key_size_step };
    add_obj_prop(&wrap->obj, "KeyLengths", &kls, sizeof(kls));

    BCRYPT_KEY_LENGTHS_STRUCT tls = { .dwMinLength = (ULONG) algo->min_tag_size, .dwMaxLength = (ULONG) algo->max_tag_size, .dwIncrement = (ULONG) algo->tag_size_step };
    add_obj_prop(&wrap->obj, "AuthTagLength", &tls, sizeof(tls));

    *out = wrap;
    return STATUS_SUCCESS;
}
WINAPI(BCryptOpenAlgorithmProvider)

__winfnc NTSTATUS BCryptCloseAlgorithmProvider(struct bcrypt_algo_wrap *algo, ULONG flags) {
    destroy_obj(&algo->obj);
    return STATUS_SUCCESS;
}
WINAPI(BCryptCloseAlgorithmProvider)

__winfnc NTSTATUS BCryptGenerateSymmetricKey(struct bcrypt_algo_wrap *algo, struct bcrypt_key **out, UCHAR *key_obj, ULONG key_obj_size, UCHAR *secret, ULONG secret_size, ULONG flags) {
    if(!algo->algo->generate_sym_key) return WINERR_SET_CODE;

    //Create and generate key
    NTSTATUS status;

    struct bcrypt_key *key;
    status = create_key(algo, &key);
    if(status != STATUS_SUCCESS) return status;

    status = algo->algo->generate_sym_key(algo->algo, key->key_data, secret, secret_size);
    if(status != STATUS_SUCCESS) {
        destroy_key(key);
        return status;
    }

    *out = key;
    return STATUS_SUCCESS;
}
WINAPI(BCryptGenerateSymmetricKey)

__winfnc NTSTATUS BCryptGenerateKeyPair(struct bcrypt_algo_wrap *algo, struct bcrypt_key **out, ULONG key_len, ULONG flags) {
    if(!algo->algo->generate_key_pair) return WINERR_SET_CODE;

    //Create and generate key
    NTSTATUS status;

    struct bcrypt_key *key;
    status = create_key(algo, &key);
    if(status != STATUS_SUCCESS) return status;

    status = algo->algo->generate_key_pair(algo->algo, key->key_data, (int) key_len);
    if(status != STATUS_SUCCESS) {
        destroy_key(key);
        return status;
    }

    *out = key;
    return STATUS_SUCCESS;
}
WINAPI(BCryptGenerateKeyPair)

__winfnc NTSTATUS BCryptFinalizeKeyPair(struct bcrypt_key *key, ULONG flags) {
    if(key->algo->finalize_key) return key->algo->finalize_key(key->algo, key->key_data);
    return STATUS_SUCCESS;
}
WINAPI(BCryptFinalizeKeyPair)

__winfnc NTSTATUS BCryptImportKey(struct bcrypt_algo_wrap *algo, struct bcrypt_key *import_key, const char16_t *blob_type, struct bcrypt_key **out, UCHAR *in, ULONG in_size, ULONG flags) {
    if(!algo->algo->import_key) return WINERR_SET_CODE;
    if(import_key) {
        log_warn("BCryptImportKey called with encrypted input data!");
        return WINERR_SET_CODE;
    }

    //Allocate key
    NTSTATUS status;

    struct bcrypt_key *key;
    status = create_key(algo, &key);
    if(status != STATUS_SUCCESS) return status;

    char *import_type = winstr_to_str(blob_type);
    status = algo->algo->import_key(algo->algo, key->key_data, import_type, in, in_size);
    free(import_type);
    if(status != STATUS_SUCCESS) {
        destroy_key(key);
        return status;
    }

    *out = key;
    return STATUS_SUCCESS;
}
WINAPI(BCryptImportKey)

__winfnc NTSTATUS BCryptImportKeyPair(struct bcrypt_algo_wrap *algo, struct bcrypt_key *import_key, const char16_t *blob_type, struct bcrypt_key **out, UCHAR *in, ULONG in_size, ULONG flags) {
    if(!algo->algo->import_key) return WINERR_SET_CODE;
    if(import_key) {
        log_warn("BCryptImportKeyPair called with encrypted input data!");
        return WINERR_SET_CODE;
    }

    //Allocate key
    NTSTATUS status;

    struct bcrypt_key *key;
    status = create_key(algo, &key);
    if(status != STATUS_SUCCESS) return status;

    char *import_type = winstr_to_str(blob_type);
    status = algo->algo->import_key(algo->algo, key->key_data, import_type, in, in_size);
    free(import_type);
    if(status != STATUS_SUCCESS) {
        destroy_key(key);
        return status;
    }

    *out = key;
    return STATUS_SUCCESS;
}
WINAPI(BCryptImportKeyPair)

__winfnc NTSTATUS BCryptExportKey(struct bcrypt_key *key, struct bcrypt_key *export_key, const char16_t *blob_type, UCHAR *out, ULONG out_size, ULONG *res_size, ULONG flags) {
    if(!key->algo->export_key) return WINERR_SET_CODE;
    if(export_key) {
        log_warn("BCryptExportKey called with encrypted output data!");
        return WINERR_SET_CODE;
    }

    char *export_type = winstr_to_str(blob_type);
    size_t buf_size = out_size;
    NTSTATUS status = key->algo->export_key(key->algo, key->key_data, export_type, out, &buf_size);
    if(status == STATUS_SUCCESS) *res_size = (ULONG) buf_size;
    free(export_type);
    return status;
}
WINAPI(BCryptExportKey)

__winfnc NTSTATUS BCryptDestroyKey(struct bcrypt_key *key) {
    destroy_key(key);
    return STATUS_SUCCESS;
}
WINAPI(BCryptDestroyKey);

__winfnc NTSTATUS BCryptEncrypt(struct bcrypt_key *key, UCHAR *in, ULONG in_size, void *pad_info, UCHAR *iv, ULONG iv_size, UCHAR *out, ULONG out_size, ULONG *res_size, ULONG flags) {
    if(!key->algo->encrypt) return WINERR_SET_CODE;
    if(flags) {
        log_warn("BCryptEncrypt | Tried to use flags: 0x%x!", flags);
        return WINERR_SET_CODE;
    }

    size_t sz = out_size;
    NTSTATUS status = key->algo->encrypt(key->algo, key->key_data, &key->algo_wrap->obj, pad_info, iv, iv_size, in, in_size, out, &sz);
    if(status == STATUS_SUCCESS) *res_size = (ULONG) sz;
    return status;
}
WINAPI(BCryptEncrypt)

__winfnc NTSTATUS BCryptDecrypt(struct bcrypt_key *key, UCHAR *in, ULONG in_size, void *pad_info, UCHAR *iv, ULONG iv_size, UCHAR *out, ULONG out_size, ULONG *res_size, ULONG flags) {
    if(!key->algo->decrypt) return WINERR_SET_CODE;
    if(flags) {
        log_warn("BCryptDecrypt | Tried to use flags: 0x%x!", flags);
        return WINERR_SET_CODE;
    }

    size_t sz = out_size;
    NTSTATUS status = key->algo->decrypt(key->algo, key->key_data, &key->algo_wrap->obj, pad_info, iv, iv_size, in, in_size, out, &sz);
    if(status == STATUS_SUCCESS) *res_size = (ULONG) sz;
    return status;
}
WINAPI(BCryptDecrypt)

__winfnc NTSTATUS BCryptSignHash(struct bcrypt_key *key, void *pad_info, UCHAR *hash, ULONG hash_size, UCHAR *sig, ULONG buf_size, ULONG *sig_size, ULONG flags) {
    if(!key->algo->sign_hash) return WINERR_SET_CODE;
    if(flags) {
        log_warn("BCryptSignHash | Tried to use flags: 0x%x!", flags);
        return WINERR_SET_CODE;
    }

    size_t sz = buf_size;
    NTSTATUS status = key->algo->sign_hash(key->algo, key->key_data, hash, hash_size, sig, &sz);
    if(status == STATUS_SUCCESS) *sig_size = (ULONG) sz;
    return status;
}
WINAPI(BCryptSignHash)

__winfnc NTSTATUS BCryptVerifySignature(struct bcrypt_key *key, void *pad_info, UCHAR *hash, ULONG hash_size, UCHAR *sig, ULONG sig_size, ULONG flags) {
    if(!key->algo->verify_hash) return WINERR_SET_CODE;
    if(flags) {
        log_warn("BCryptVerifySignature | Tried to use flags: 0x%x!", flags);
        return WINERR_SET_CODE;
    }

    return key->algo->verify_hash(key->algo, key->key_data, hash, hash_size, sig, sig_size);
}
WINAPI(BCryptVerifySignature)

__winfnc NTSTATUS BCryptSecretAgreement(struct bcrypt_key *priv_key, struct bcrypt_key *pub_key, struct bcrypt_secret **out, ULONG flags) {
    if(pub_key->algo != priv_key->algo) return WINERR_SET_CODE;
    if(!pub_key->algo->exchange_secret) return WINERR_SET_CODE;

    //Allocate secret
    struct bcrypt_secret *secret = (struct bcrypt_secret*) malloc(sizeof(struct bcrypt_secret));
    if(!secret) return WINERR_SET_CODE;
    *secret = (struct bcrypt_secret) {0};
    secret->data = NULL;
    secret->data_size = 0;

    //Exchange secret
    NTSTATUS status = pub_key->algo->exchange_secret(pub_key->algo, pub_key->key_data, priv_key->key_data, secret);

    if(status != STATUS_SUCCESS) {
        free(secret->data);
        free(secret);
    } else *out = secret;

    return status;
}
WINAPI(BCryptSecretAgreement)

__winfnc NTSTATUS BCryptDestroySecret(struct bcrypt_secret *secret) {
    destroy_obj(&secret->obj);
    free(secret->data);
    free(secret);
    return STATUS_SUCCESS;
}
WINAPI(BCryptDestroySecret)

static const EVP_MD *determine_hash_md(BCryptBufferDesc *params) {
    const EVP_MD *hash_md = NULL;
    for(int i = 0; i < params->cBuffers; i++) {
        BCryptBuffer *buf = &params->pBuffers[i];
        if(buf->BufferType != KDF_HASH_ALGORITHM) continue;

        char *name = winstr_to_str((const char16_t*) buf->pvBuffer);
        if(strcmp(name, "SHA1") == 0) hash_md = EVP_sha1();
        else if(strcmp(name, "SHA256") == 0) hash_md = EVP_sha256();
        else if(strcmp(name, "SHA384") == 0) hash_md = EVP_sha384();
        else if(strcmp(name, "SHA512") == 0) hash_md = EVP_sha512();
        else log_warn("Unknown hash algorithm '%s'!", name);
        free(name);
    }
    return hash_md;
}

__winfnc NTSTATUS BCryptDeriveKey(struct bcrypt_secret *secret, const char16_t *kdf, BCryptBufferDesc *params, UCHAR *key, ULONG key_size, ULONG *res_size, ULONG flags) {
    if(params && params->ulVersion != BCRYPTBUFFER_VERSION) return WINERR_SET_CODE;
    if(flags) {
        log_warn("BCryptDeriveKey | Tried to use flags: 0x%x!", flags);
        return WINERR_SET_CODE;
    }

    //Apply KDF
    char *ckdf = winstr_to_str(kdf);
    if(strcmp(ckdf, "HASH") == 0) {
        free(ckdf);
        if(!params) return WINERR_SET_CODE;

        //Determine hash function
        const EVP_MD *hash_md = determine_hash_md(params);
        if(!hash_md) hash_md = EVP_sha1();

        if(key && key_size >= EVP_MD_size(hash_md)) return STATUS_BUFFER_TOO_SMALL;
        else if(!key) {
            *res_size = EVP_MD_size(hash_md);
            return STATUS_SUCCESS;
        }

        //Calculate hash
        EVP_MD_CTX *ctx;
        LIBCRYPTO_ERR(ctx = EVP_MD_CTX_new());
        LIBCRYPTO_ERR(EVP_DigestInit_ex(ctx, hash_md, NULL));

        for(int i = 0; i < params->cBuffers; i++) {
            BCryptBuffer *buf = &params->pBuffers[i];
            if(buf->BufferType != KDF_SECRET_PREPEND) continue;
            LIBCRYPTO_ERR(EVP_DigestUpdate(ctx, buf->pvBuffer, buf->cbBuffer));
        }

        LIBCRYPTO_ERR(EVP_DigestUpdate(ctx, secret->data, secret->data_size));

        for(int i = 0; i < params->cBuffers; i++) {
            BCryptBuffer *buf = &params->pBuffers[i];
            if(buf->BufferType != KDF_SECRET_APPEND) continue;
            LIBCRYPTO_ERR(EVP_DigestUpdate(ctx, buf->pvBuffer, buf->cbBuffer));
        }

        LIBCRYPTO_ERR(EVP_DigestFinal_ex(ctx, key, res_size));
        EVP_MD_CTX_free(ctx);

        return STATUS_SUCCESS;
    } else if(strcmp(ckdf, "HMAC") == 0) {
        free(ckdf);
        if(!params) return WINERR_SET_CODE;

        //Determine hash function
        const EVP_MD *hash_md = determine_hash_md(params);
        if(!hash_md) hash_md = EVP_sha1();

        if(key && key_size >= EVP_MD_size(hash_md)) return STATUS_BUFFER_TOO_SMALL;
        else if(!key) {
            *res_size = EVP_MD_size(hash_md);
            return STATUS_SUCCESS;
        }

        //Determine HMAC key
        void *hmac_key = NULL;
        size_t hmac_key_size = 0;
        for(int i = 0; i < params->cBuffers; i++) {
            BCryptBuffer *buf = &params->pBuffers[i];
            if(buf->BufferType != KDF_HMAC_KEY) continue;
            hmac_key = buf->pvBuffer;
            hmac_key_size = buf->cbBuffer;
        }
        if(!hmac_key) return WINERR_SET_CODE;

        EVP_PKEY *hmac_pkey;
        LIBCRYPTO_ERR(hmac_pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, hmac_key, (int) hmac_key_size));

        //Calculate HMAC
        EVP_MD_CTX *ctx;
        LIBCRYPTO_ERR(ctx = EVP_MD_CTX_new());
        LIBCRYPTO_ERR(EVP_DigestSignInit(ctx, NULL, hash_md, NULL, hmac_pkey));

        for(int i = 0; i < params->cBuffers; i++) {
            BCryptBuffer *buf = &params->pBuffers[i];
            if(buf->BufferType != KDF_SECRET_PREPEND) continue;
            LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, buf->pvBuffer, buf->cbBuffer));
        }

        LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, secret->data, secret->data_size));

        for(int i = 0; i < params->cBuffers; i++) {
            BCryptBuffer *buf = &params->pBuffers[i];
            if(buf->BufferType != KDF_SECRET_APPEND) continue;
            LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, buf->pvBuffer, buf->cbBuffer));
        }

        size_t sz;
        LIBCRYPTO_ERR(EVP_DigestSignFinal(ctx, key, &sz));
        *res_size = (ULONG) sz;

        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(hmac_pkey);

        return STATUS_SUCCESS;
    } else if(strcmp(ckdf, "TLS_PRF") == 0) {
        free(ckdf);
        if(!params) return WINERR_SET_CODE;

        //Determine hash function
        const EVP_MD *hash_md = determine_hash_md(params);
        if(!hash_md) hash_md = EVP_sha256();

        //Determine PRF label and seed
        const char *prf_label = NULL;
        void *prf_seed = NULL;
        size_t prf_seed_size = 0;
        for(int i = 0; i < params->cBuffers; i++) {
            BCryptBuffer *buf = &params->pBuffers[i];
            if(buf->BufferType == KDF_TLS_PRF_LABEL) {
                prf_label = (const char*) buf->pvBuffer;
            } else if(buf->BufferType == KDF_TLS_PRF_SEED) {
                prf_seed = buf->pvBuffer;
                prf_seed_size = buf->cbBuffer;
            }
        }
        if(!prf_label | !prf_seed) return WINERR_SET_CODE;
        int prf_label_len = strlen(prf_label);

        //Calculate PRF
        void *prf_a = malloc(EVP_MD_size(hash_md));
        if(!prf_a) return winerr_from_errno();

        void *prf_k = malloc(EVP_MD_size(hash_md));
        if(!prf_k) { free(prf_a); return winerr_from_errno(); }

        EVP_MD_CTX *ctx;
        LIBCRYPTO_ERR(ctx = EVP_MD_CTX_new());

        EVP_PKEY *pkey;
        LIBCRYPTO_ERR(pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, secret->data, (int) secret->data_size));

        //Initialize A(1)
        size_t sig_sz = EVP_MD_size(hash_md);
        LIBCRYPTO_ERR(EVP_DigestSignInit(ctx, NULL, hash_md, NULL, pkey));
        LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, prf_label, prf_label_len));
        LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, prf_seed, prf_seed_size));
        LIBCRYPTO_ERR(EVP_DigestSignFinal(ctx, (unsigned char*) prf_a, &sig_sz));

        //Generate derived key
        for(ssize_t rem_size = key_size; rem_size > 0; rem_size -= EVP_MD_size(hash_md)) {
            //Generate key block
            LIBCRYPTO_ERR(EVP_DigestSignInit(ctx, NULL, hash_md, NULL, pkey));
            LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, prf_a, EVP_MD_size(hash_md)));
            LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, prf_label, prf_label_len));
            LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, prf_seed, prf_seed_size));
            LIBCRYPTO_ERR(EVP_DigestSignFinal(ctx, (unsigned char*) prf_k, &sig_sz));

            //Update A(i-1) -> A(i)
            LIBCRYPTO_ERR(EVP_DigestSignInit(ctx, NULL, hash_md, NULL, pkey));
            LIBCRYPTO_ERR(EVP_DigestSignUpdate(ctx, prf_a, EVP_MD_size(hash_md)));
            LIBCRYPTO_ERR(EVP_DigestSignFinal(ctx, (unsigned char*) prf_a, &sig_sz));

            //Append to output key
            size_t copy_sz = (rem_size < EVP_MD_size(hash_md)) ? rem_size : EVP_MD_size(hash_md);
            memcpy(key, prf_k, copy_sz);
            key += copy_sz;
        }
        EVP_MD_CTX_free(ctx);
        EVP_PKEY_free(pkey);

        free(prf_a);
        free(prf_k);

        *res_size = key_size;
        return STATUS_SUCCESS;
    } else if(strcmp(ckdf, "TRUNCATE") == 0) {
        free(ckdf);

        //Copy raw bytes, but swap endian (BE -> LE)
        ULONG num_copy = (secret->data_size < key_size) ? secret->data_size : key_size;
        for(int i = 0; i < num_copy; i++) {
            key[i] = ((BYTE*) secret->data)[secret->data_size-i-1];
        }
        *res_size = num_copy;

        return STATUS_SUCCESS;
    } else {
        log_warn("BCryptDeriveKey | Unknown KDF '%s'!", ckdf);
        free(ckdf);
        return WINERR_SET_CODE;
    }
}
WINAPI(BCryptDeriveKey)