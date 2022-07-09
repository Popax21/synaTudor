#include "internal.h"

pthread_mutex_t tudor_pair_data_lock = PTHREAD_MUTEX_INITIALIZER;
struct tudor_pair_data *tudor_pair_data_head = NULL;

void tudor_add_pair_data(const char *name, const void *data, size_t data_size) {
    log_debug("Adding pairing data for sensor '%s'...", name);

    cant_fail_ret(pthread_mutex_lock(&tudor_pair_data_lock));

    //Allocate pairing data
    struct tudor_pair_data *pair_data = (struct tudor_pair_data*) malloc(sizeof(struct tudor_pair_data));
    if(!pair_data) { perror("Error allocating pairing data"); abort(); }

    pair_data->name = strdup(name);
    if(!pair_data->name) { perror("Error allocating pairing data name"); abort(); }
    pair_data->data = malloc(data_size);
    if(!pair_data->data) { perror("Error allocating pairing data buffer"); abort(); }
    pair_data->data_size = data_size;
    memcpy(pair_data->data, data, data_size);

    pair_data->next = tudor_pair_data_head;
    tudor_pair_data_head = pair_data;

    cant_fail_ret(pthread_mutex_unlock(&tudor_pair_data_lock));
}