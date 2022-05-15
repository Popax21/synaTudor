#include "crypt.h"

static BOOL rsa_aes_import_key(struct crypt_provider *prov, const void *data, size_t data_size, void **key) {
    return TRUE;    
}

static void rsa_aes_destroy_key(struct crypt_provider *prov, void *key) {

}

struct crypt_provider crypt_prov_rsa_aes = {
    .import_key = (crypt_import_key_fnc*) rsa_aes_import_key,
    .destroy_key = (crypt_destroy_key_fnc*) rsa_aes_destroy_key
};