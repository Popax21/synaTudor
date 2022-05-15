#include <openssl/ec.h>
#include <openssl/ecdh.h>
#include <openssl/ecdsa.h>
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

static p256_param_t secp256r1_p  = { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
static p256_param_t secp256r1_a  = { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc };
static p256_param_t secp256r1_b  = { 0x5a, 0xc6, 0x35, 0xd8, 0xaa, 0x3a, 0x93, 0xe7, 0xb3, 0xeb, 0xbd, 0x55, 0x76, 0x98, 0x86, 0xbc, 0x65, 0x1d, 0x06, 0xb0, 0xcc, 0x53, 0xb0, 0xf6, 0x3b, 0xce, 0x3c, 0x3e, 0x27, 0xd2, 0x60, 0x4b };
static p256_param_t secp256r1_Gx = { 0x6b, 0x17, 0xd1, 0xf2, 0xe1, 0x2c, 0x42, 0x47, 0xf8, 0xbc, 0xe6, 0xe5, 0x63, 0xa4, 0x40, 0xf2, 0x77, 0x03, 0x7d, 0x81, 0x2d, 0xeb, 0x33, 0xa0, 0xf4, 0xa1, 0x39, 0x45, 0xd8, 0x98, 0xc2, 0x96 };
static p256_param_t secp256r1_Gy = { 0x4f, 0xe3, 0x42, 0xe2, 0xfe, 0x1a, 0x7f, 0x9b, 0x8e, 0xe7, 0xeb, 0x4a, 0x7c, 0x0f, 0x9e, 0x16, 0x2b, 0xce, 0x33, 0x57, 0x6b, 0x31, 0x5e, 0xce, 0xcb, 0xb6, 0x40, 0x68, 0x37, 0xbf, 0x51, 0xf5 };
static p256_param_t secp256r1_n  = { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbc, 0xe6, 0xfa, 0xad, 0xa7, 0x17, 0x9e, 0x84, 0xf3, 0xb9, 0xca, 0xc2, 0xfc, 0x63, 0x25, 0x51 };

static const EC_GROUP *p256_get_curve() {
    static EC_GROUP *p256_curve = NULL;
    if(p256_curve) return p256_curve;

    //Create the curve (secp256r1)
    BIGNUM *p256_p, *p256_a, *p256_b;
    LIBCRYPTO_ERR(p256_p  = BN_bin2bn(secp256r1_p,  sizeof(secp256r1_p),  NULL));
    LIBCRYPTO_ERR(p256_a  = BN_bin2bn(secp256r1_a,  sizeof(secp256r1_a),  NULL));
    LIBCRYPTO_ERR(p256_b  = BN_bin2bn(secp256r1_b,  sizeof(secp256r1_b),  NULL));
    LIBCRYPTO_ERR(p256_curve = EC_GROUP_new_curve_GFp(p256_p, p256_a, p256_b, NULL));
    BN_free(p256_p);
    BN_free(p256_a);
    BN_free(p256_b);

    //Set the generator point
    BIGNUM *p256_Gx, *p256_Gy, *p256_n;
    LIBCRYPTO_ERR(p256_Gx = BN_bin2bn(secp256r1_Gx, sizeof(secp256r1_Gx), NULL));
    LIBCRYPTO_ERR(p256_Gy = BN_bin2bn(secp256r1_Gy, sizeof(secp256r1_Gy), NULL));
    LIBCRYPTO_ERR(p256_n  = BN_bin2bn(secp256r1_n,  sizeof(secp256r1_n),  NULL));

    EC_POINT *gen_point;
    LIBCRYPTO_ERR(gen_point = EC_POINT_new(p256_curve));
    LIBCRYPTO_ERR(EC_POINT_set_affine_coordinates_GFp(p256_curve, gen_point, p256_Gx, p256_Gy, NULL));
    LIBCRYPTO_ERR(EC_GROUP_set_generator(p256_curve, gen_point, p256_n, NULL));
    EC_POINT_free(gen_point);

    BN_free(p256_Gx);
    BN_free(p256_Gy);
    BN_free(p256_n);

    return p256_curve;
}

struct bcrypt_ecc_algorithm {
    struct bcrypt_algorithm algo;
    ULONG pub_magic, priv_magic;
};

struct p256_key {
    bool has_public, has_private;
    EC_KEY *ec_key;
    p256_param_t pub_x, pub_y;
    p256_param_t priv_d;
};

static NTSTATUS p256_create_key(struct bcrypt_ecc_algorithm *algo, void **out) {
    //Allocate the key
    struct p256_key *key = (struct p256_key*) malloc(sizeof(struct p256_key));
    if(!key) return WINERR_SET_CODE;
    key->has_public = key->has_private = false;

    LIBCRYPTO_ERR(key->ec_key = EC_KEY_new());
    LIBCRYPTO_ERR(EC_KEY_set_group(key->ec_key, p256_get_curve()));

    *out = key;
    return STATUS_SUCCESS;
}

static NTSTATUS p256_generate_key_pair(struct bcrypt_ecc_algorithm *algo, struct p256_key *key, int key_size) {
    if(key_size != 256) return WINERR_SET_CODE;

    //Generate key
    LIBCRYPTO_ERR(EC_KEY_generate_key(key->ec_key));

    //Store key parameters
    const EC_POINT *pub_point = EC_KEY_get0_public_key(key->ec_key);
    BIGNUM *pub_x, *pub_y;
    LIBCRYPTO_ERR(pub_x = BN_new());
    LIBCRYPTO_ERR(pub_y = BN_new());
    LIBCRYPTO_ERR(EC_POINT_get_affine_coordinates_GFp(p256_get_curve(), pub_point, pub_x, pub_y, NULL));
    LIBCRYPTO_ERR(BN_bn2bin(pub_x, key->pub_x));
    LIBCRYPTO_ERR(BN_bn2bin(pub_y, key->pub_y));
    BN_free(pub_x);
    BN_free(pub_y);

    const BIGNUM *priv_key = EC_KEY_get0_private_key(key->ec_key);
    LIBCRYPTO_ERR(BN_bn2bin(priv_key, key->priv_d));

    key->has_public = key->has_private = true;
    return STATUS_SUCCESS;
}

static NTSTATUS p256_import_key(struct bcrypt_ecc_algorithm *algo, struct p256_key *key, const char *import_type, void *buf, size_t buf_size) {
    BCRYPT_ECCKEY_BLOB *ecc_blob = (BCRYPT_ECCKEY_BLOB*) buf;
    p256_param_t *key_params = (p256_param_t*) (ecc_blob + 1);

    if(strcmp(import_type, "ECCPUBLICBLOB") == 0) {
        if(buf_size < sizeof(BCRYPT_ECCKEY_BLOB) + 2*P256_PARAM_SIZE) return STATUS_BUFFER_TOO_SMALL;
        if(ecc_blob->dwMagic != algo->pub_magic || ecc_blob->cbKey != P256_PARAM_SIZE) return WINERR_SET_CODE;

        memcpy(key->pub_x, key_params[0], P256_PARAM_SIZE);
        memcpy(key->pub_y, key_params[1], P256_PARAM_SIZE);

        BIGNUM *pub_x, *pub_y;
        LIBCRYPTO_ERR(pub_x = BN_bin2bn(key->pub_x, P256_PARAM_SIZE, NULL));
        LIBCRYPTO_ERR(pub_y = BN_bin2bn(key->pub_y, P256_PARAM_SIZE, NULL));

        EC_POINT *pub_point;
        LIBCRYPTO_ERR(pub_point = EC_POINT_new(p256_get_curve()));
        if(!EC_POINT_set_affine_coordinates_GFp(p256_get_curve(), pub_point, pub_x, pub_y, NULL)) {
            key->has_public = key->has_private = false;
            return WINERR_SET_CODE;
        }
        LIBCRYPTO_ERR(EC_KEY_set_public_key(key->ec_key, pub_point));
        EC_POINT_free(pub_point);

        BN_free(pub_x);
        BN_free(pub_y);

        key->has_public = true;
        key->has_private = false;

        return STATUS_SUCCESS;
    } else if(strcmp(import_type, "ECCPRIVATEBLOB") == 0) {
        if(buf_size < sizeof(BCRYPT_ECCKEY_BLOB) + 3*P256_PARAM_SIZE) return STATUS_BUFFER_TOO_SMALL;
        if(ecc_blob->dwMagic != algo->priv_magic || ecc_blob->cbKey != P256_PARAM_SIZE) return WINERR_SET_CODE;

        memcpy(key->pub_x, key_params[0], P256_PARAM_SIZE);
        memcpy(key->pub_y, key_params[1], P256_PARAM_SIZE);
        memcpy(key->priv_d, key_params[2], P256_PARAM_SIZE);


        BIGNUM *pub_x, *pub_y, *priv_d;
        LIBCRYPTO_ERR(pub_x = BN_bin2bn(key->pub_x, P256_PARAM_SIZE, NULL));
        LIBCRYPTO_ERR(pub_y = BN_bin2bn(key->pub_y, P256_PARAM_SIZE, NULL));
        LIBCRYPTO_ERR(priv_d = BN_bin2bn(key->priv_d, P256_PARAM_SIZE, NULL));
    
        EC_POINT *pub_point;
        LIBCRYPTO_ERR(pub_point = EC_POINT_new(p256_get_curve()));
        if(BN_is_zero(pub_x) && BN_is_zero(pub_y)) {
            LIBCRYPTO_ERR(EC_POINT_mul(p256_get_curve(), pub_point, priv_d, NULL, NULL, NULL));
            LIBCRYPTO_ERR(EC_POINT_get_affine_coordinates_GFp(p256_get_curve(), pub_point, pub_x, pub_y, NULL));
        } else {
            if(!EC_POINT_set_affine_coordinates_GFp(p256_get_curve(), pub_point, pub_x, pub_y, NULL)) {
                key->has_public = key->has_private = false;
                return WINERR_SET_CODE;
            }
        }
        LIBCRYPTO_ERR(EC_KEY_set_public_key(key->ec_key, pub_point));
        EC_POINT_free(pub_point);

        if(!EC_KEY_set_private_key(key->ec_key, priv_d)) {
            key->has_public = key->has_private = false;
            return WINERR_SET_CODE;
        }

        BN_free(pub_x);
        BN_free(pub_y);
        BN_free(priv_d);

        key->has_public = true;
        key->has_private = true;

        return STATUS_SUCCESS;
    } else return WINERR_SET_CODE;
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
    EC_KEY_free(key->ec_key);
    free(key);
}

static NTSTATUS p256_ecdsa_sign_hash(struct bcrypt_ecc_algorithm *algo, struct p256_key *key, const void *hash, size_t hash_size, void *sig, size_t *sig_size) {
    if(!key->has_private) return WINERR_SET_CODE;

    if(sig && *sig_size >= 2*P256_PARAM_SIZE) {
        //Sign the hash
        ECDSA_SIG *ec_sig;
        LIBCRYPTO_ERR(ec_sig = ECDSA_do_sign(hash, hash_size, key->ec_key));

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

    //Verify the signature
    int ret;
    LIBCRYPTO_ERR_NEG(ret = ECDSA_do_verify((const unsigned char*) hash, (int) hash_size, ec_sig, key->ec_key));

    ECDSA_SIG_free(ec_sig);
    return ret ? STATUS_SUCCESS : STATUS_INVALID_SIGNATURE;
}

static NTSTATUS p256_ecdh_exchange_secret(struct bcrypt_ecc_algorithm *algo, struct p256_key *pub_key, struct p256_key *priv_key, struct bcrypt_secret *secret) {
    if(!pub_key->has_public || !priv_key->has_private) return WINERR_SET_CODE;

    //Allocate secret data
    secret->data = malloc(P256_PARAM_SIZE);
    secret->data_size = P256_PARAM_SIZE;
    if(!secret->data) return winerr_from_errno();

    //Compute secret
    LIBCRYPTO_ERR(ECDH_compute_key(secret->data, P256_PARAM_SIZE, EC_KEY_get0_public_key(pub_key->ec_key), priv_key->ec_key, NULL));

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