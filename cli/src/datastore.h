#ifndef CLI_DATASTORE_H
#define CLI_DATASTORE_H

#include <stdio.h>
#include <tudor/tudor.h>

bool load_datastore_pair_data(FILE *file);
bool save_datastore_pair_data(FILE *file);

bool load_datastore_records(FILE *file, struct tudor_device *device);
bool save_datastore_records(FILE *file, struct tudor_device *device);

#endif