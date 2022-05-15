#include <openssl/evp.h>
#include <openssl/rand.h>
#include "bcrypt.h"

#define BCRYPT_KEY_DATA_BLOB_MAGIC 0x4d42444b
#define BCRYPT_KEY_DATA_BLOB_VERSION1 1

typedef struct {
    ULONG dwMagic;
    ULONG dwVersion;
    ULONG cbKeyData;
} BCRYPT_KEY_DATA_BLOB_HEADER;

#define BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO_VERSION 1
typedef struct {
    ULONG cbSize;
    ULONG dwInfoVersion;
    UCHAR *pbNonce;
    ULONG cbNonce;
    UCHAR *pbAuthData;
    ULONG cbAuthData;
    UCHAR *pbTag;
    ULONG cbTag;
    UCHAR *pbMacContext;
    ULONG cbMacContext;
    ULONG cbAAD;
    ULONGLONG cbData;
    ULONG dwFlags;
} BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO;

struct aes_key {
    bool has_key;
    int key_size;
    BYTE key[32];
};

static NTSTATUS aes_create_key(struct bcrypt_algorithm *algo, struct aes_key **out) {
    //Allocate the key
    struct aes_key *key = (struct aes_key*) malloc(sizeof(struct aes_key));
    if(!key) return WINERR_SET_CODE;
    key->has_key = false;

    *out = key;
    return STATUS_SUCCESS;
}

static NTSTATUS aes_generate_sym_key(struct bcrypt_algorithm *algo, struct aes_key *key, const void *secret, size_t secret_size) {
    //"Generate" key
    if(secret_size != 16 && secret_size != 24 && secret_size != 32) return WINERR_SET_CODE;
    memcpy(key->key, secret, secret_size);
    key->has_key = true;
    key->key_size = secret_size*8;
    return STATUS_SUCCESS;
}

static NTSTATUS aes_import_key(struct bcrypt_algorithm *algo, struct aes_key *key, const char *import_type, void *buf, size_t buf_size) {
    if(strcmp(import_type, "KeyDataBlob") == 0) {
        //Check buffer size
        if(buf_size < sizeof(BCRYPT_KEY_DATA_BLOB_HEADER)) return STATUS_BUFFER_TOO_SMALL;

        BCRYPT_KEY_DATA_BLOB_HEADER *blob = (BCRYPT_KEY_DATA_BLOB_HEADER*) buf;
        if(blob->dwMagic != BCRYPT_KEY_DATA_BLOB_MAGIC || blob->dwVersion != BCRYPT_KEY_DATA_BLOB_VERSION1) return WINERR_SET_CODE;
        if(buf_size < sizeof(BCRYPT_KEY_DATA_BLOB_HEADER) + blob->cbKeyData) return STATUS_BUFFER_TOO_SMALL; 

        //Import key
        if(blob->cbKeyData != 16 && blob->cbKeyData != 24 && blob->cbKeyData != 32) return WINERR_SET_CODE;
        memcpy(key->key, blob+1, blob->cbKeyData);
        key->has_key = true;
        key->key_size = blob->cbKeyData*8;

        return STATUS_SUCCESS;
    } else return WINERR_SET_CODE;
}

static NTSTATUS aes_export_key(struct bcrypt_algorithm *algo, struct aes_key *key, const char *export_type, void *buf, size_t *buf_size) {
    if(strcmp(export_type, "KeyDataBlob") == 0) {
        if(!key->has_key) return WINERR_SET_CODE;

        //Check buffer size
        size_t exp_size = sizeof(BCRYPT_KEY_DATA_BLOB_HEADER) + key->key_size / 8;
        if(buf && *buf_size < exp_size) return STATUS_BUFFER_TOO_SMALL;
        else if(!buf) { *buf_size = exp_size; return STATUS_SUCCESS; }

        //Initialize blob
        BCRYPT_KEY_DATA_BLOB_HEADER *blob = (BCRYPT_KEY_DATA_BLOB_HEADER*) buf;
        blob->dwMagic = BCRYPT_KEY_DATA_BLOB_MAGIC;
        blob->dwVersion = BCRYPT_KEY_DATA_BLOB_VERSION1;
        blob->cbKeyData = key->key_size / 8;

        //Export key
        memcpy(blob+1, key->key, blob->cbKeyData);

        return STATUS_SUCCESS;
    } else return WINERR_SET_CODE;
}

static void aes_destroy_key(struct bcrypt_algorithm *algo, struct aes_key *key) {
    free(key);
}

static EVP_CIPHER_CTX *create_aes_ctx(struct aes_key *key, struct bcrypt_object *prov_obj, const void *pad_info, const void *iv, size_t iv_size, bool enc) {
    //Get chaining mode cipher
    struct bcrypt_property *chain_prop = bcrypt_find_obj_prop(prov_obj, "ChainingMode");
    if(!chain_prop) return NULL;
    char *chain_mode = winstr_to_str((const char16_t*) chain_prop->val);

    //Determine cipher
    const EVP_CIPHER *cipher = NULL;
    switch(key->key_size) {
        case 128: {
            if(strcmp(chain_mode, "ChainingModeCBC") == 0) cipher = EVP_aes_128_cbc();
            if(strcmp(chain_mode, "ChainingModeCFB") == 0) cipher = EVP_aes_128_cfb();
            if(strcmp(chain_mode, "ChainingModeECB") == 0) cipher = EVP_aes_128_ecb();
            if(strcmp(chain_mode, "ChainingModeGCM") == 0) cipher = EVP_aes_128_gcm();
        } break;
        case 192: {
            if(strcmp(chain_mode, "ChainingModeCBC") == 0) cipher = EVP_aes_192_cbc();
            if(strcmp(chain_mode, "ChainingModeCFB") == 0) cipher = EVP_aes_192_cfb();
            if(strcmp(chain_mode, "ChainingModeECB") == 0) cipher = EVP_aes_192_ecb();
            if(strcmp(chain_mode, "ChainingModeGCM") == 0) cipher = EVP_aes_192_gcm();
        } break;
        case 256: {
            if(strcmp(chain_mode, "ChainingModeCBC") == 0) cipher = EVP_aes_256_cbc();
            if(strcmp(chain_mode, "ChainingModeCFB") == 0) cipher = EVP_aes_256_cfb();
            if(strcmp(chain_mode, "ChainingModeECB") == 0) cipher = EVP_aes_256_ecb();
            if(strcmp(chain_mode, "ChainingModeGCM") == 0) cipher = EVP_aes_256_gcm();
        } break;
        default: abort();
    }
    free(chain_mode);
    if(!cipher) return NULL;

    //Handle GCM
    BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO *auth_info = NULL;
    if(EVP_CIPHER_mode(cipher) == EVP_CIPH_GCM_MODE) {
        if(!pad_info) return NULL;
        auth_info = (BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO*) pad_info;
        if(auth_info->cbSize < sizeof(BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO) || auth_info->dwInfoVersion != BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO_VERSION) return NULL;
        if(auth_info->dwFlags) {
            log_warn("create_aes_ctx | Authentication cipher info flags used!");
            return NULL;
        }

        //Override IV with nonce
        if(!iv) {
            if(!auth_info->pbNonce) return NULL;
            iv = auth_info->pbNonce;
            iv_size = auth_info->cbNonce;
        } else{
            if(auth_info->pbNonce) return NULL;
        }
    }

    //Check IV size
    if(EVP_CIPHER_iv_length(cipher) != iv_size) return NULL;

    //Create context
    EVP_CIPHER_CTX *ctx;
    LIBCRYPTO_ERR(ctx = EVP_CIPHER_CTX_new());
    LIBCRYPTO_ERR(EVP_CipherInit_ex(ctx, cipher, NULL, (const unsigned char*) key->key, (const unsigned char*) iv, enc));

    return ctx;
}

static NTSTATUS aes_encrypt(struct bcrypt_algorithm *algo, struct aes_key *key, struct bcrypt_object *prov_obj, const void *pad_info, const void *iv, size_t iv_size, const void *in, size_t in_size, void *out, size_t *out_size) {
    if(!key->has_key) return WINERR_SET_CODE;

    //Check buffer size
    if(out && *out_size < in_size) return STATUS_BUFFER_TOO_SMALL;
    *out_size = in_size;
    if(!out) return STATUS_SUCCESS;

    //Determine cipher
    EVP_CIPHER_CTX *ctx = create_aes_ctx(key, prov_obj, pad_info, iv, iv_size, true);
    if(!ctx) return WINERR_SET_CODE;

    //Handle GCM AAD
    int sz;
    if(EVP_CIPHER_CTX_mode(ctx) == EVP_CIPH_GCM_MODE) {
        BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO *auth_info = (BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO*) pad_info;
        LIBCRYPTO_ERR(EVP_CipherUpdate(ctx, NULL, &sz, (const unsigned char*) auth_info->pbAuthData, (int) auth_info->cbAuthData));
    }

    //Encrypt data
    *out_size = 0;
    LIBCRYPTO_ERR(EVP_CipherUpdate(ctx, (unsigned char*) out, &sz, (const unsigned char*) in, (int) in_size));
    *out_size += sz;
    LIBCRYPTO_ERR(EVP_CipherFinal_ex(ctx, (unsigned char*) out + sz, &sz));
    *out_size += sz;

    //Get GCM tag
    if(EVP_CIPHER_CTX_mode(ctx) == EVP_CIPH_GCM_MODE) {
        BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO *auth_info = (BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO*) pad_info;
        LIBCRYPTO_ERR(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, auth_info->cbTag, auth_info->pbTag));
    }

    EVP_CIPHER_CTX_free(ctx);

    return STATUS_SUCCESS;
}

static NTSTATUS aes_decrypt(struct bcrypt_algorithm *algo, struct aes_key *key, struct bcrypt_object *prov_obj, const void *pad_info, const void *iv, size_t iv_size, const void *in, size_t in_size, void *out, size_t *out_size) {
    if(!key->has_key) return WINERR_SET_CODE;

    //Check buffer size
    if(out && *out_size < in_size) return STATUS_BUFFER_TOO_SMALL;
    *out_size = in_size;
    if(!out) return STATUS_SUCCESS;

    //Create context
    EVP_CIPHER_CTX *ctx = create_aes_ctx(key, prov_obj, pad_info, iv, iv_size, false);
    if(!ctx) return WINERR_SET_CODE;

    //Handle GCM AAD
    int sz;
    if(EVP_CIPHER_CTX_mode(ctx) == EVP_CIPH_GCM_MODE) {
        BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO *auth_info = (BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO*) pad_info;
        LIBCRYPTO_ERR(EVP_CipherUpdate(ctx, NULL, &sz, (const unsigned char*) auth_info->pbAuthData, (int) auth_info->cbAuthData));
    }

    //Decrypt data
    *out_size = 0;
    LIBCRYPTO_ERR(EVP_CipherUpdate(ctx, (unsigned char*) out, &sz, (const unsigned char*) in, (int) in_size));
    *out_size += sz;

    //Set GCM tag
    if(EVP_CIPHER_CTX_mode(ctx) == EVP_CIPH_GCM_MODE) {
        BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO *auth_info = (BCRYPT_AUTHENTICATED_CIPHER_MODE_INFO*) pad_info;
        LIBCRYPTO_ERR(EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, auth_info->cbTag, auth_info->pbTag));
    }

    //Finalize decryption
    if(!EVP_CipherFinal_ex(ctx, (unsigned char*) out + sz, &sz)) return STATUS_INVALID_SIGNATURE;
    *out_size += sz;

    EVP_CIPHER_CTX_free(ctx);

    return STATUS_SUCCESS;
}

struct bcrypt_algorithm bcrypt_algo_aes = {
    .name = "AES",
    .min_key_size = 16, .max_key_size = 32, .key_size_step = 8,
    .min_tag_size = 12, .max_tag_size = 16, .tag_size_step = 1,

    .create_key = (bcrypt_create_key_fnc*) aes_create_key,
    .generate_sym_key = (bcrypt_generate_sym_key_fnc*) aes_generate_sym_key,
    .generate_key_pair = NULL,
    .import_key = (bcrypt_import_key_fnc*) aes_import_key,
    .export_key = (bcrypt_export_key_fnc*) aes_export_key,
    .finalize_key = NULL,
    .destroy_key = (bcrypt_destroy_key_fnc*) aes_destroy_key,

    .encrypt = (bcrypt_encrypt_fnc*) aes_encrypt,
    .decrypt = (bcrypt_decrypt_fnc*) aes_decrypt,

    .sign_hash = NULL,
    .verify_hash = NULL,
    .exchange_secret = NULL
};