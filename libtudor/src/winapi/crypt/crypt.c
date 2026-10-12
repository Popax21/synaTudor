#include <openssl/ecdsa.h>
#include <openssl/rand.h>
#include "crypt.h"

__winfnc BOOL CryptAcquireContextA(struct crypt_provider **prov, const char *cont_name, const char *prov_name, DWORD prov_type, DWORD flags) {
    switch(prov_type) {
        case PROV_RSA_FULL:
        case PROV_RSA_AES: *prov = &crypt_prov_rsa_aes; return TRUE;
        default: {
            log_warn("CryptAcquireContextA | Couldn't find provider for container '%s' provider '%s' provider type 0x%x flags 0x%x", cont_name, prov_name, prov_type, flags);
            return FALSE;
        }
    }
}
WINAPI(CryptAcquireContextA)

__winfnc BOOL CryptAcquireContextW(struct crypt_provider **prov, const char16_t *cont_name, const char16_t *prov_name, DWORD prov_type, DWORD flags) {
    return CryptAcquireContextA(prov, NULL, NULL, prov_type, flags);
}
WINAPI(CryptAcquireContextW)

__winfnc BOOL CryptReleaseContext(struct crypt_provider *prov, DWORD flags) {
    return TRUE;
}
WINAPI(CryptReleaseContext)

__winfnc BOOL CryptImportKey(struct crypt_provider *prov, const BLOBHEADER *data, DWORD data_len, struct crypt_key *pub_key, DWORD flags, struct crypt_key **out) {
    if(!prov->import_key) { winerr_set(); return FALSE; }
    if(pub_key) {
        log_warn("CryptImportKey called with encrypted key data!");
        winerr_set();
        return FALSE;
    }

    if(data_len < sizeof(BLOBHEADER)) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }

    //Allocate key
    struct crypt_key *key = (struct crypt_key*) malloc(sizeof(struct crypt_key));
    if(!key) { winerr_set_errno(); return FALSE; }
    key->prov = prov;
    key->key_data = NULL;
    key->mode = CRYPT_MODE_CBC;
    memset(key->iv, 0, sizeof(key->iv));
    key->cipher_ctx = NULL;

    if(data->bType == type_PLAINTEXTKEYBLOB) {
        //Copy plaintext data
        PLAINTEXTKEYBLOB *blob = (PLAINTEXTKEYBLOB*) data;
        if(data_len < sizeof(PLAINTEXTKEYBLOB) + blob->dwKeySize) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }

        key->plain_data = malloc(blob->dwKeySize);
        key->plain_size = blob->dwKeySize;
        if(!key->plain_data) {
            winerr_set_errno();
            free(key);
            return FALSE;
        }
        memcpy(key->plain_data, blob+1, blob->dwKeySize);
    } else {
        key->plain_data = NULL;
        key->plain_size = 0;
    }

    //Import key
    if(!prov->import_key(prov, data, data_len, &key->key_data)) {
        winerr_set_errno();
        free(key->plain_data);
        free(key);
        return FALSE;
    }

    *out = key;
    return TRUE;
}
WINAPI(CryptImportKey)

__winfnc BOOL CryptDestroyKey(struct crypt_key *key) {
    if(key->prov->destroy_key) key->prov->destroy_key(key->prov, key->key_data);
    EVP_CIPHER_CTX_free(key->cipher_ctx);
    free(key->plain_data);
    free(key);
    return TRUE;
}
WINAPI(CryptDestroyKey)

__winfnc BOOL CryptSetKeyParam(struct crypt_key *key, DWORD param, const BYTE *data, DWORD flags) {
    switch(param) {
        case KP_IV:
            memcpy(key->iv, data, sizeof(key->iv));
            break;
        case KP_MODE:
            key->mode = *(const DWORD*) data;
            break;
        case KP_PADDING:
            //Padding is never applied, see key_cipher_run
            break;
        default:
            log_warn("CryptSetKeyParam | unsupported param %u", param);
            winerr_set();
            return FALSE;
    }

    //A new IV / mode starts a new message
    EVP_CIPHER_CTX_free(key->cipher_ctx);
    key->cipher_ctx = NULL;
    return TRUE;
}
WINAPI(CryptSetKeyParam)

static const EVP_CIPHER *key_cipher(struct crypt_key *key) {
    bool ecb = key->mode == CRYPT_MODE_ECB;
    switch(key->plain_size) {
        case 16: return ecb ? EVP_aes_128_ecb() : EVP_aes_128_cbc();
        case 24: return ecb ? EVP_aes_192_ecb() : EVP_aes_192_cbc();
        case 32: return ecb ? EVP_aes_256_ecb() : EVP_aes_256_cbc();
        default: return NULL;
    }
}

//The driver encrypts multi-part data across several calls, so the cipher
//context lives on the key until the Final call to keep CBC chaining intact.
//Padding is never applied: the driver only passes whole blocks.
static BOOL key_cipher_run(struct crypt_key *key, int enc, BOOL final, BYTE *data, DWORD *data_len) {
    if(key->cipher_ctx && key->cipher_enc != enc) {
        EVP_CIPHER_CTX_free(key->cipher_ctx);
        key->cipher_ctx = NULL;
    }

    if(!key->cipher_ctx) {
        const EVP_CIPHER *cipher = key->plain_data ? key_cipher(key) : NULL;
        if(!cipher) {
            log_warn("%s | unsupported key (size %zu mode %u)", enc ? "CryptEncrypt" : "CryptDecrypt", key->plain_size, key->mode);
            winerr_set();
            return FALSE;
        }

        key->cipher_ctx = EVP_CIPHER_CTX_new();
        if(!key->cipher_ctx || !EVP_CipherInit_ex(key->cipher_ctx, cipher, NULL, key->plain_data, key->mode == CRYPT_MODE_ECB ? NULL : key->iv, enc)) {
            log_warn("Error initializing cipher: %s", ERR_error_string(ERR_get_error(), NULL));
            winerr_set();
            return FALSE;
        }
        EVP_CIPHER_CTX_set_padding(key->cipher_ctx, 0);
        key->cipher_enc = enc;
    }

    int out_len = 0, final_len = 0;
    if(!EVP_CipherUpdate(key->cipher_ctx, data, &out_len, data, *data_len) ||
        (final && !EVP_CipherFinal_ex(key->cipher_ctx, data + out_len, &final_len))) {
        log_warn("Error running cipher: %s", ERR_error_string(ERR_get_error(), NULL));
        winerr_set();
        return FALSE;
    }

    if(final) {
        EVP_CIPHER_CTX_free(key->cipher_ctx);
        key->cipher_ctx = NULL;
    }

    *data_len = out_len + final_len;
    return TRUE;
}

__winfnc BOOL CryptEncrypt(struct crypt_key *key, struct crypt_hash *hash, BOOL final, DWORD flags, BYTE *data, DWORD *data_len, DWORD buf_len) {
    if(hash) { log_warn("CryptEncrypt called with a hash object"); winerr_set(); return FALSE; }

    //Size query: without padding the output is as long as the input
    if(!data) return TRUE;
    if(*data_len > buf_len) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }

    return key_cipher_run(key, 1, final, data, data_len);
}
WINAPI(CryptEncrypt)

__winfnc BOOL CryptDecrypt(struct crypt_key *key, struct crypt_hash *hash, BOOL final, DWORD flags, BYTE *data, DWORD *data_len) {
    if(hash) { log_warn("CryptDecrypt called with a hash object"); winerr_set(); return FALSE; }
    return key_cipher_run(key, 0, final, data, data_len);
}
WINAPI(CryptDecrypt)

__winfnc BOOL CryptCreateHash(struct crypt_provider *prov, ALG_ID alg_id, struct crypt_key *key, DWORD flags, struct crypt_hash **out) {
    //Get the algorithm
    struct crypt_hash_algorithm *algo;
    switch(alg_id) {
        case CALG_SHA1: algo = (struct crypt_hash_algorithm*) &crypt_hash_algo_sha1; break;
        case CALG_SHA_256: algo = (struct crypt_hash_algorithm*) &crypt_hash_algo_sha256; break;
        case CALG_SHA_384: algo = (struct crypt_hash_algorithm*) &crypt_hash_algo_sha384; break;
        case CALG_SHA_512: algo = (struct crypt_hash_algorithm*) &crypt_hash_algo_sha512; break;
        case CALG_HMAC: algo = &crypt_hash_algo_hmac; break;
        default: {
            log_warn("CryptCreateHash | Couldn't find algorithm id 0x%x flags 0x%x", alg_id, flags);
            return FALSE;
        }
    }

    //Allocate and create hash
    struct crypt_hash *hash = (struct crypt_hash*) malloc(sizeof(struct crypt_hash));
    if(!hash) { winerr_set_errno(); return FALSE; }
    hash->algo = algo;
    hash->hash_data = NULL;

    if(!algo->create_hash(algo, key, &hash->hash_data)) {
        free(hash);
        return FALSE;
    }

    *out = hash;
    return TRUE;
}
WINAPI(CryptCreateHash)

__winfnc BOOL CryptDestroyHash(struct crypt_hash *hash) {
    if(hash->algo->destroy_hash) hash->algo->destroy_hash(hash->algo, hash->hash_data);
    free(hash);
    return TRUE;
}
WINAPI(CryptDestroyHash)

__winfnc BOOL CryptDuplicateHash(struct crypt_hash *hash, DWORD *reserved, DWORD flags, struct crypt_hash **out) {
    //Allocate and duplicate hash
    struct crypt_hash *nhash = (struct crypt_hash*) malloc(sizeof(struct crypt_hash));
    if(!nhash) { winerr_set_errno(); return FALSE; }
    nhash->algo = hash->algo;
    nhash->hash_data = NULL;

    if(!hash->algo->duplicate_hash(hash->algo, hash->hash_data, &nhash->hash_data)) {
        free(nhash);
        return FALSE;
    }

    *out = nhash;
    return TRUE;
}
WINAPI(CryptDuplicateHash)

__winfnc BOOL CryptGetHashParam(struct crypt_hash *hash, DWORD param, BYTE *data, DWORD *data_len, DWORD flags) {
    size_t buf_size = *data_len;
    BOOL suc = hash->algo->get_hash_param(hash->algo, hash->hash_data, param, data, &buf_size);
    *data_len = (size_t) buf_size;
    return suc;
}
WINAPI(CryptGetHashParam)

__winfnc BOOL CryptSetHashParam(struct crypt_hash *hash, DWORD param, const BYTE *data, DWORD flags) {
    return hash->algo->set_hash_param(hash->algo, hash->hash_data, param, data);
}
WINAPI(CryptSetHashParam)

__winfnc BOOL CryptHashData(struct crypt_hash *hash, const BYTE *data, DWORD data_len, DWORD flags) {
    return hash->algo->update_hash(hash->algo, hash->hash_data, data, data_len);
}
WINAPI(CryptHashData)

__winfnc BOOL CryptEncodeObject(DWORD cert_enc_type, const char *struct_type, void *struct_info, BYTE *enc, DWORD *enc_size) {
    //Handle integer constants
    if(!(((uintptr_t) struct_type) & (128llu << sizeof(uintptr_t)))) {
        switch((uintptr_t) struct_type) {
            case X509_ECC_SIGNATURE: {
                CERT_ECC_SIGNATURE *ecc_sig = (CERT_ECC_SIGNATURE*) struct_info;

                //Create signature
                ECDSA_SIG *sig;
                LIBCRYPTO_ERR(sig = ECDSA_SIG_new());

                BIGNUM *sig_r, *sig_s;
                LIBCRYPTO_ERR(sig_r = BN_new());
                LIBCRYPTO_ERR(sig_s = BN_new());
                LIBCRYPTO_ERR(BN_lebin2bn(ecc_sig->r.pbData, ecc_sig->r.cbData, sig_r));
                LIBCRYPTO_ERR(BN_lebin2bn(ecc_sig->s.pbData, ecc_sig->s.cbData, sig_s));
                LIBCRYPTO_ERR(ECDSA_SIG_set0(sig, sig_r, sig_s));

                //Encode signature
                int sig_size = i2d_ECDSA_SIG(sig, NULL);
                if(enc && *enc_size >= sig_size) {
                    i2d_ECDSA_SIG(sig, &enc);
                } else if(enc) {
                    ECDSA_SIG_free(sig);
                    winerr_set_code(ERROR_INSUFFICIENT_BUFFER);
                    return FALSE;
                }

                ECDSA_SIG_free(sig);
                *enc_size = sig_size;
                return TRUE;
            }
            default: {
                log_debug("CryptEncodeObject | Unsupported arguments! cert_enc_type %x struct_type %lx", cert_enc_type, (uintptr_t) struct_type);
                winerr_set();
                return FALSE;
            }
        }
    }

    log_debug("CryptEncodeObject | Unsupported arguments! cert_enc_type %x struct_type '%s'", cert_enc_type, struct_type);
    winerr_set();
    return FALSE;
}
WINAPI(CryptEncodeObject)

__winfnc BOOL CryptDecodeObject(DWORD cert_enc_type, const char *struct_type, const BYTE *enc, DWORD enc_size, DWORD flags, void *struct_info, DWORD *struct_info_size) {
    //Handle integer constants
    if(!(((uintptr_t) struct_type) & (128llu << sizeof(uintptr_t)))) {
        switch((uintptr_t) struct_type) {
            case X509_ECC_SIGNATURE: {
                //Decode signature
                ECDSA_SIG *sig;
                LIBCRYPTO_ERR(sig = d2i_ECDSA_SIG(NULL, &enc, (long) enc_size));

                //Store signature
                const BIGNUM *sig_r = ECDSA_SIG_get0_r(sig), *sig_s = ECDSA_SIG_get0_s(sig);
                int sig_size = sizeof(CERT_ECC_SIGNATURE) + BN_num_bytes(sig_r) + BN_num_bytes(sig_s);
                if(struct_info && *struct_info_size >= sig_size) {
                    CERT_ECC_SIGNATURE *ecc_sig = (CERT_ECC_SIGNATURE*) struct_info;
                    ecc_sig->r.cbData = BN_num_bytes(sig_r);
                    ecc_sig->r.pbData = (BYTE*) struct_info + sizeof(CERT_ECC_SIGNATURE);
                    ecc_sig->s.cbData = BN_num_bytes(sig_s);
                    ecc_sig->s.pbData = (BYTE*) struct_info + sizeof(CERT_ECC_SIGNATURE) + ecc_sig->r.cbData;
                    LIBCRYPTO_ERR(BN_bn2lebinpad(sig_r, ecc_sig->r.pbData, ecc_sig->r.cbData));
                    LIBCRYPTO_ERR(BN_bn2lebinpad(sig_s, ecc_sig->s.pbData, ecc_sig->s.cbData));
                } else if(struct_info) { winerr_set_code(ERROR_INSUFFICIENT_BUFFER); return FALSE; }

                *struct_info_size = sig_size;
                ECDSA_SIG_free(sig);
                return TRUE;
            }
            default: {
                log_debug("CryptDecodeObject | Unsupported arguments! cert_enc_type %x struct_type %lx", cert_enc_type, (uintptr_t) struct_type);
                winerr_set();
                return FALSE;
            }
        }
    }

    log_debug("CryptDecodeObject | Unsupported arguments! cert_enc_type %x struct_type '%s'", cert_enc_type, struct_type);
    winerr_set();
    return FALSE;
}
WINAPI(CryptDecodeObject)

__winfnc BOOL CryptProtectData(DATA_BLOB *in, const char16_t *descr, DATA_BLOB *entropy, void *reserved, void *prompt_struct, DWORD flags, DATA_BLOB *out) {
    out->cbData = in->cbData;
    out->pbData = (BYTE*) malloc(in->cbData);
    if(!out->pbData) { winerr_set_errno(); return FALSE; }
    memcpy(out->pbData, in->pbData, in->cbData);
    return TRUE;
}
WINAPI(CryptProtectData)

__winfnc BOOL CryptUnprotectData(DATA_BLOB *in, const char16_t *descr, DATA_BLOB *entropy, void *reserved, void *prompt_struct, DWORD flags, DATA_BLOB *out) {
    out->cbData = in->cbData;
    out->pbData = (BYTE*) malloc(in->cbData);
    if(!out->pbData) { winerr_set_errno(); return FALSE; }
    memcpy(out->pbData, in->pbData, in->cbData);
    return TRUE;
}
WINAPI(CryptUnprotectData)

__winfnc BOOL CryptProtectMemory(void *data, DWORD size, DWORD flags) {
    return TRUE;
}
WINAPI(CryptProtectMemory);

__winfnc BOOL CryptUnprotectMemory(void *data, DWORD size, DWORD flags) {
    return TRUE;
}
WINAPI(CryptUnprotectMemory);

__winfnc BOOL CryptGenRandom(HANDLE prov, DWORD len, BYTE *buf) {
    LIBCRYPTO_ERR(RAND_bytes(buf, (int) len));
    return TRUE;
}
WINAPI(CryptGenRandom)