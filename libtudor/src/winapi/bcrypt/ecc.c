#include <assert.h>
#include <openssl/evp.h>
#include <openssl/core_names.h>
#include <openssl/param_build.h>
#include <openssl/ec.h>
#include "bcrypt.h"

#define BCRYPT_ECDH_PUBLIC_P256_MAGIC 0x314b4345
#define BCRYPT_ECDH_PRIVATE_P256_MAGIC 0x324b4345
#define BCRYPT_ECDSA_PUBLIC_P256_MAGIC 0x31534345
#define BCRYPT_ECDSA_PRIVATE_P256_MAGIC 0x32534345

typedef struct {
    ULONG dwMagic;
    ULONG cbKey;
} BCRYPT_ECCKEY_BLOB;

#define P256_PARAM_SIZE 32
typedef unsigned char p256_param_t[P256_PARAM_SIZE];

#define P256_CURVE_NID NID_X9_62_prime256v1

static const EC_GROUP *p256_get_curve() {
    static EC_GROUP *p256_curve = NULL;
    if(!p256_curve) LIBCRYPTO_ERR(p256_curve = EC_GROUP_new_by_curve_name(P256_CURVE_NID));
    return p256_curve;
}

struct bcrypt_ecc_algorithm {
    struct bcrypt_algorithm algo;
    ULONG pub_magic, priv_magic;
};

struct p256_key {
    bool has_public, has_private;
    EVP_PKEY *ec_key;
    p256_param_t pub_x, pub_y;
    p256_param_t priv_d;
};

static NTSTATUS p256_create_key(struct bcrypt_ecc_algorithm *algo, void **out) {
    //Allocate the key
    struct p256_key *key = (struct p256_key*) malloc(sizeof(struct p256_key));
    if(!key) return WINERR_SET_CODE;
    key->has_public = key->has_private = false;

    LIBCRYPTO_ERR(key->ec_key = EVP_PKEY_new());

    *out = key;
    return STATUS_SUCCESS;
}

static NTSTATUS p256_generate_key_pair(struct bcrypt_ecc_algorithm *algo, struct p256_key *key, int key_size) {
    if(key_size != 256) return WINERR_SET_CODE;

    //Generate key
    EVP_PKEY_CTX *keygen_ctx;
    LIBCRYPTO_ERR(keygen_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL));
    LIBCRYPTO_ERR(EVP_PKEY_keygen_init(keygen_ctx));
    LIBCRYPTO_ERR(EVP_PKEY_CTX_set_ec_paramgen_curve_nid(keygen_ctx, P256_CURVE_NID));
    LIBCRYPTO_ERR(EVP_PKEY_keygen(keygen_ctx, &key->ec_key));
    EVP_PKEY_CTX_free(keygen_ctx);

    //Store key parameters
    BIGNUM *pub_x = NULL, *pub_y = NULL;
    LIBCRYPTO_ERR(EVP_PKEY_get_bn_param(key->ec_key, OSSL_PKEY_PARAM_EC_PUB_X, &pub_x));
    LIBCRYPTO_ERR(EVP_PKEY_get_bn_param(key->ec_key, OSSL_PKEY_PARAM_EC_PUB_Y, &pub_y));
    LIBCRYPTO_ERR(BN_bn2bin(pub_x, key->pub_x));
    LIBCRYPTO_ERR(BN_bn2bin(pub_y, key->pub_y));
    BN_free(pub_x);
    BN_free(pub_y);

    BIGNUM *priv_key = NULL;
    LIBCRYPTO_ERR(EVP_PKEY_get_bn_param(key->ec_key, OSSL_PKEY_PARAM_PRIV_KEY, &priv_key));
    LIBCRYPTO_ERR(BN_bn2bin(priv_key, key->priv_d));
    BN_free(priv_key);

    key->has_public = key->has_private = true;
    return STATUS_SUCCESS;
}

static NTSTATUS p256_import_key(struct bcrypt_ecc_algorithm *algo, struct p256_key *key, const char *import_type, void *buf, size_t buf_size) {
    NTSTATUS status;
    BCRYPT_ECCKEY_BLOB *ecc_blob = (BCRYPT_ECCKEY_BLOB*) buf;
    p256_param_t *key_params = (p256_param_t*) (ecc_blob + 1);

    BIGNUM *pub_x, *pub_y, *priv_d;
    if(strcmp(import_type, "ECCPUBLICBLOB") == 0) {
        if(buf_size < sizeof(BCRYPT_ECCKEY_BLOB) + 2*P256_PARAM_SIZE) return STATUS_BUFFER_TOO_SMALL;
        if(ecc_blob->dwMagic != algo->pub_magic || ecc_blob->cbKey != P256_PARAM_SIZE) return WINERR_SET_CODE;

        //Parse public key parameters
        LIBCRYPTO_ERR(pub_x = BN_bin2bn(key_params[0], P256_PARAM_SIZE, NULL));
        LIBCRYPTO_ERR(pub_y = BN_bin2bn(key_params[1], P256_PARAM_SIZE, NULL));
        priv_d = NULL;
    } else if(strcmp(import_type, "ECCPRIVATEBLOB") == 0) {
        if(buf_size < sizeof(BCRYPT_ECCKEY_BLOB) + 3*P256_PARAM_SIZE) return STATUS_BUFFER_TOO_SMALL;
        if(ecc_blob->dwMagic != algo->priv_magic || ecc_blob->cbKey != P256_PARAM_SIZE) return WINERR_SET_CODE;

        //Parse private key parameters
        LIBCRYPTO_ERR(pub_x = BN_bin2bn(key_params[0], P256_PARAM_SIZE, NULL));
        LIBCRYPTO_ERR(pub_y = BN_bin2bn(key_params[1], P256_PARAM_SIZE, NULL));
        LIBCRYPTO_ERR(priv_d = BN_bin2bn(key_params[2], P256_PARAM_SIZE, NULL));
    
        status = STATUS_SUCCESS;
        if(BN_is_zero(pub_x) && BN_is_zero(pub_y)) {
            //Derive public key
            EC_POINT *pub_point;
            LIBCRYPTO_ERR(pub_point = EC_POINT_new(p256_get_curve()));
            LIBCRYPTO_ERR(EC_POINT_mul(p256_get_curve(), pub_point, priv_d, NULL, NULL, NULL));
            LIBCRYPTO_ERR(EC_POINT_get_affine_coordinates(p256_get_curve(), pub_point, pub_x, pub_y, NULL));
            EC_POINT_free(pub_point);
        }
    } else return WINERR_SET_CODE;

    //Create the public key point and encode it
    EC_POINT *pub_point;
    LIBCRYPTO_ERR(pub_point = EC_POINT_new(p256_get_curve()));
    if(!EC_POINT_set_affine_coordinates(p256_get_curve(), pub_point, pub_x, pub_y, NULL)) {
        EC_POINT_free(pub_point);
        status = WINERR_SET_CODE;
        goto err;
    }

    if(EC_POINT_is_on_curve(p256_get_curve(), pub_point, NULL) <= 0) {
        EC_POINT_free(pub_point);
        status = WINERR_SET_CODE;
        goto err;
    }

    size_t pub_buf_sz;
    LIBCRYPTO_ERR(pub_buf_sz = EC_POINT_point2oct(p256_get_curve(), pub_point, POINT_CONVERSION_UNCOMPRESSED, NULL, 0, NULL));
    unsigned char *pub_buf = malloc(pub_buf_sz);
    if(!pub_buf) abort_perror("Failed to allocate EC public key buffer");
    LIBCRYPTO_ERR(EC_POINT_point2oct(p256_get_curve(), pub_point, POINT_CONVERSION_UNCOMPRESSED, pub_buf, pub_buf_sz, NULL));

    EC_POINT_free(pub_point);

    //Build parameters
    OSSL_PARAM_BLD *bld;
    LIBCRYPTO_ERR(bld = OSSL_PARAM_BLD_new());

    LIBCRYPTO_ERR(OSSL_PARAM_BLD_push_utf8_ptr(bld, OSSL_PKEY_PARAM_GROUP_NAME, SN_X9_62_prime256v1, 0));
    LIBCRYPTO_ERR(OSSL_PARAM_BLD_push_octet_string(bld, OSSL_PKEY_PARAM_PUB_KEY, pub_buf, pub_buf_sz));
    LIBCRYPTO_ERR(OSSL_PARAM_BLD_push_utf8_ptr(bld, OSSL_PKEY_PARAM_ENCODED_PUBLIC_KEY, OSSL_PKEY_EC_POINT_CONVERSION_FORMAT_UNCOMPRESSED, 0));
    if(priv_d) LIBCRYPTO_ERR(OSSL_PARAM_BLD_push_BN(bld, OSSL_PKEY_PARAM_PRIV_KEY, priv_d));

    OSSL_PARAM *params = OSSL_PARAM_BLD_to_param(bld);
    free(pub_buf);

    //Create the key from the parameters
    EVP_PKEY_CTX *ctx;
    LIBCRYPTO_ERR(ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL));
    LIBCRYPTO_ERR(EVP_PKEY_fromdata_init(ctx));

    int r;
    LIBCRYPTO_ERR_NEG(r = EVP_PKEY_fromdata(ctx, &key->ec_key, priv_d ? EVP_PKEY_KEYPAIR : EVP_PKEY_PUBLIC_KEY, params));
    status = (r > 0) ? STATUS_SUCCESS : WINERR_SET_CODE;

    EVP_PKEY_CTX_free(ctx);
    OSSL_PARAM_free(params);

    err:;
    if(status == STATUS_SUCCESS) {
        //Store key parameters
        LIBCRYPTO_ERR(BN_bn2binpad(pub_x, key->pub_x, P256_PARAM_SIZE));
        LIBCRYPTO_ERR(BN_bn2binpad(pub_x, key->pub_y, P256_PARAM_SIZE));
        if(priv_d) LIBCRYPTO_ERR(BN_bn2binpad(priv_d, key->priv_d, P256_PARAM_SIZE));

        key->has_public = true;
        key->has_private = priv_d != NULL;
    } else key->has_public = key->has_private = false;

    BN_free(pub_x);
    BN_free(pub_y);
    if(priv_d) BN_free(priv_d);

    return status;
}

static NTSTATUS p256_export_key(struct bcrypt_ecc_algorithm *algo, struct p256_key *key, const char *export_type, void *buf, size_t *buf_size) {
    BCRYPT_ECCKEY_BLOB *ecc_blob = (BCRYPT_ECCKEY_BLOB*) buf;
    p256_param_t *key_params = (p256_param_t*) (ecc_blob + 1);

    if(strcmp(export_type, "ECCPUBLICBLOB") == 0) {
        if(!key->has_public) return WINERR_SET_CODE;
        if(buf && *buf_size >= sizeof(BCRYPT_ECCKEY_BLOB) + 2*P256_PARAM_SIZE) {
            ecc_blob->dwMagic = algo->pub_magic;
            ecc_blob->cbKey = P256_PARAM_SIZE;
            memcpy(key_params[0], key->pub_x, P256_PARAM_SIZE);
            memcpy(key_params[1], key->pub_y, P256_PARAM_SIZE);
        } else if(buf) return STATUS_BUFFER_TOO_SMALL;
        *buf_size = sizeof(BCRYPT_ECCKEY_BLOB) + 2*P256_PARAM_SIZE;
        return STATUS_SUCCESS;
    } else if(strcmp(export_type, "ECCPRIVATEBLOB") == 0) {
        if(!key->has_public || !key->has_private) return WINERR_SET_CODE;
        if(buf && *buf_size >= sizeof(BCRYPT_ECCKEY_BLOB) + 3*P256_PARAM_SIZE) {
            ecc_blob->dwMagic = algo->pub_magic;
            ecc_blob->cbKey = P256_PARAM_SIZE;
            memcpy(key_params[0], key->pub_x, P256_PARAM_SIZE);
            memcpy(key_params[1], key->pub_y, P256_PARAM_SIZE);
            memcpy(key_params[2], key->priv_d, P256_PARAM_SIZE);
        } else if(buf) return STATUS_BUFFER_TOO_SMALL;
        *buf_size = sizeof(BCRYPT_ECCKEY_BLOB) + 3*P256_PARAM_SIZE;
        return STATUS_SUCCESS;
    } else return WINERR_SET_CODE;
}

static void p256_destroy_key(struct bcrypt_ecc_algorithm *algo, struct p256_key *key) {
    EVP_PKEY_free(key->ec_key);
    free(key);
}

static NTSTATUS p256_ecdsa_sign_hash(struct bcrypt_ecc_algorithm *algo, struct p256_key *key, const void *hash, size_t hash_size, void *sig, size_t *sig_size) {
    if(!key->has_private) return WINERR_SET_CODE;

    if(sig && *sig_size >= 2*P256_PARAM_SIZE) {
        //Setup a signing context
        EVP_PKEY_CTX *ctx;
        LIBCRYPTO_ERR(ctx = EVP_PKEY_CTX_new(key->ec_key, NULL));
        LIBCRYPTO_ERR(EVP_PKEY_sign_init(ctx));

        //Sign the hash
        size_t buf_size;
        LIBCRYPTO_ERR(EVP_PKEY_sign(ctx, NULL, &buf_size, hash, hash_size));

        unsigned char *buf = malloc(buf_size), *bptr = buf;
        if(!buf) abort_perror("Failed to allocate ECDSA signature buffer");
        LIBCRYPTO_ERR(EVP_PKEY_sign(ctx, buf, &buf_size, hash, hash_size));

        EVP_PKEY_CTX_free(ctx);

        //Decode the signature
        ECDSA_SIG *ec_sig;
        LIBCRYPTO_ERR(ec_sig = d2i_ECDSA_SIG(NULL, (const unsigned char**) &bptr, buf_size));
        free(buf);

        //Store output 
        const BIGNUM *r = ECDSA_SIG_get0_r(ec_sig), *s = ECDSA_SIG_get0_s(ec_sig);
        p256_param_t *out = (p256_param_t*) sig;
        LIBCRYPTO_ERR(BN_bn2bin(r, out[0]));
        LIBCRYPTO_ERR(BN_bn2bin(s, out[1]));

        ECDSA_SIG_free(ec_sig);
    } else if(sig) return STATUS_BUFFER_TOO_SMALL;
    *sig_size = 2*P256_PARAM_SIZE;
    return STATUS_SUCCESS;
}

static NTSTATUS p256_ecdsa_verify_hash(struct bcrypt_ecc_algorithm *algo, struct p256_key *key, const void *hash, size_t hash_size, const void *sig, size_t sig_size) {
    if(!key->has_public) return WINERR_SET_CODE;

    if(sig_size != 2*P256_PARAM_SIZE) return STATUS_INVALID_SIGNATURE;

    //Create the signature
    ECDSA_SIG *ec_sig;
    LIBCRYPTO_ERR(ec_sig = ECDSA_SIG_new());

    const p256_param_t *params = (const p256_param_t*) sig;
    BIGNUM *sig_r, *sig_s;
    LIBCRYPTO_ERR(sig_r = BN_new());
    LIBCRYPTO_ERR(sig_s = BN_new());
    LIBCRYPTO_ERR(BN_bin2bn(params[0], P256_PARAM_SIZE, sig_r));
    LIBCRYPTO_ERR(BN_bin2bn(params[1], P256_PARAM_SIZE, sig_s));
    LIBCRYPTO_ERR(ECDSA_SIG_set0(ec_sig, sig_r, sig_s));

    //Encode the signature
    int buf_size;
    unsigned char *buf = NULL;
    LIBCRYPTO_ERR(buf_size = i2d_ECDSA_SIG(ec_sig, &buf));

    ECDSA_SIG_free(ec_sig);

    //Create a verification context and verify the signature
    EVP_PKEY_CTX *ctx;
    LIBCRYPTO_ERR(ctx = EVP_PKEY_CTX_new(key->ec_key, NULL));
    LIBCRYPTO_ERR(EVP_PKEY_verify_init(ctx));

    int ret;
    LIBCRYPTO_ERR_NEG(ret = EVP_PKEY_verify(ctx, buf, buf_size, (const unsigned char*) hash, (int) hash_size));

    EVP_PKEY_CTX_free(ctx);
    OPENSSL_free(buf);

    return ret ? STATUS_SUCCESS : STATUS_INVALID_SIGNATURE;
}

static NTSTATUS p256_ecdh_exchange_secret(struct bcrypt_ecc_algorithm *algo, struct p256_key *pub_key, struct p256_key *priv_key, struct bcrypt_secret *secret) {
    if(!pub_key->has_public || !priv_key->has_private) return WINERR_SET_CODE;

    //Allocate secret data
    secret->data = malloc(P256_PARAM_SIZE);
    secret->data_size = P256_PARAM_SIZE;
    if(!secret->data) return winerr_from_errno();

    //Create a derivation context
    EVP_PKEY_CTX *ctx;
    LIBCRYPTO_ERR(ctx = EVP_PKEY_CTX_new(priv_key->ec_key, NULL));
    LIBCRYPTO_ERR(EVP_PKEY_derive_init(ctx));
    LIBCRYPTO_ERR(EVP_PKEY_derive_set_peer(ctx, pub_key->ec_key));

    //Compute secret
    LIBCRYPTO_ERR(EVP_PKEY_derive(ctx, NULL, &secret->data_size));
    assert(secret->data_size == P256_PARAM_SIZE);
    LIBCRYPTO_ERR(EVP_PKEY_derive(ctx, secret->data, &secret->data_size));

    EVP_PKEY_CTX_free(ctx);

    return STATUS_SUCCESS;
}

struct bcrypt_ecc_algorithm 
    bcrypt_algo_ecdh_p256 = {
        .algo = {
            .name = "ECDH_P256",
            .min_key_size = 32, .max_key_size = 32, .key_size_step = 0,
            .min_tag_size =  0, .max_tag_size =  0, .tag_size_step = 0,

            .create_key = (bcrypt_create_key_fnc*) p256_create_key,
            .generate_sym_key = NULL,
            .generate_key_pair = (bcrypt_generate_key_pair_fnc*) p256_generate_key_pair,
            .import_key = (bcrypt_import_key_fnc*) p256_import_key,
            .export_key = (bcrypt_export_key_fnc*) p256_export_key,
            .finalize_key = NULL,
            .destroy_key = (bcrypt_destroy_key_fnc*) p256_destroy_key,

            .encrypt = NULL,
            .decrypt = NULL,

            .sign_hash = NULL,
            .verify_hash = NULL,
            .exchange_secret = (bcrypt_exchange_secret_fnc*) p256_ecdh_exchange_secret
        },
        .pub_magic = BCRYPT_ECDH_PUBLIC_P256_MAGIC, .priv_magic = BCRYPT_ECDH_PRIVATE_P256_MAGIC
    }, 
    bcrypt_algo_ecdsa_p256 = {
        .algo = {
            .name = "ECDSA_P256",
            .min_key_size = 32, .max_key_size = 32, .key_size_step = 0,
            .min_tag_size =  0, .max_tag_size =  0, .tag_size_step = 0,

            .create_key = (bcrypt_create_key_fnc*) p256_create_key,
            .generate_sym_key = NULL,
            .generate_key_pair = (bcrypt_generate_key_pair_fnc*) p256_generate_key_pair,
            .import_key = (bcrypt_import_key_fnc*) p256_import_key,
            .export_key = (bcrypt_export_key_fnc*) p256_export_key,
            .finalize_key = NULL,
            .destroy_key = (bcrypt_destroy_key_fnc*) p256_destroy_key,

            .encrypt = NULL,
            .decrypt = NULL,

            .sign_hash = (bcrypt_sign_hash_fnc*) p256_ecdsa_sign_hash,
            .verify_hash = (bcrypt_verify_hash_fnc*) p256_ecdsa_verify_hash,
            .exchange_secret = NULL
        },
        .pub_magic = BCRYPT_ECDSA_PUBLIC_P256_MAGIC, .priv_magic = BCRYPT_ECDSA_PRIVATE_P256_MAGIC
    }
;