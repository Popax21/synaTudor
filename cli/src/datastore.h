#ifndef CLI_DATASTORE_H
#define CLI_DATASTORE_H

#include <stdio.h>
#include <tudor/tudor.h>

const struct tudor_pair_data *get_pair_data(const char *name);
void set_pair_data(const char *name, const struct tudor_pair_data *data);

bool load_datastore_pair_data(FILE *file);
bool save_datastore_pair_data(FILE *file);
void free_pair_data();

bool load_datastore_records(FILE *file, struct tudor_device *device);
bool save_datastore_records(FILE *file, struct tudor_device *device);

#endif