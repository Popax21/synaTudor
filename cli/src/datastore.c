#include <stdlib.h>
#include <string.h>
#include <tudor/log.h>
#include "datastore.h"

static pthread_mutex_t pdata_lock = PTHREAD_MUTEX_INITIALIZER;
static struct pdata {
    struct pdata *next;
    char *name;
    struct tudor_pair_data data;
} *pdata_head = NULL;

const struct tudor_pair_data *get_pair_data(const char *name) {
    //Find pairing data
    cant_fail_ret(pthread_mutex_lock(&pdata_lock));
    for(struct pdata *pd = pdata_head; pd; pd = pd->next) {
        if(strcmp(pd->name, name) == 0) {
            cant_fail_ret(pthread_mutex_unlock(&pdata_lock));
            return &pd->data;
        }
    }
    cant_fail_ret(pthread_mutex_unlock(&pdata_lock));
    return NULL;
}

void set_pair_data(const char *name, const struct tudor_pair_data *data) {
    //Duplicate pairing data
    struct tudor_pair_data pdata_data = {
        .data = malloc(data->data_size),
        .data_size = data->data_size
    };
    if(!pdata_data.data) {
        perror("Error allocating pairing data buffer");
        abort();
    }
    memcpy(pdata_data.data, data->data, data->data_size);

    //Find existing pairing data
    cant_fail_ret(pthread_mutex_lock(&pdata_lock));
    for(struct pdata *pd = pdata_head; pd; pd = pd->next) {
        if(strcmp(pd->name, name) == 0) {
            free(pd->data.data);
            pd->data = pdata_data;
            cant_fail_ret(pthread_mutex_unlock(&pdata_lock));
            return;
        }
    }

    //Create new pairing data
    struct pdata *pdata = (struct pdata*) malloc(sizeof(struct pdata));
    if(!pdata) {
        perror("Error allocating pairing data");
        abort();
    }

    char *pdata_name = strdup(name);
    if(!pdata_name) {
        perror("Error allocating pairing data name");
        abort();
    }

    *pdata = (struct pdata) {
        .next = pdata_head,
        .name = pdata_name,
        .data = pdata_data
    };
    pdata_head = pdata;

    cant_fail_ret(pthread_mutex_unlock(&pdata_lock));
}

bool load_datastore_pair_data(FILE *file) {
    while(true) {
        //Read name
        int name_len = fgetc(file);
        if(name_len == 0 || name_len == EOF) break;
        if(name_len < 0) {
            perror("Error reading pairing data name");
            return false;
        }

        char *name = (char*) malloc(name_len + 1);
        if(!name) {
            perror("Error allocating pairing data name");
            return false;
        }
        if(fread(name, 1, name_len, file) != name_len) {
            perror("Error reading pairing data name");
            return false;
        }
        name[name_len] = 0;

        //Read data
        size_t data_size;
        if(fread(&data_size, 1, sizeof(size_t), file) != sizeof(size_t)) {
            perror("Error reading pairing data size");
            return false;
        }

        void *data = malloc(data_size);
        if(!data) {
            perror("Error allocating pairing data");
            return false;
        }
        if(fread(data, 1, data_size, file) != data_size) {
            perror("Error reading pairing data");
            return false;
        }

        //Add pairing data
        cant_fail_ret(pthread_mutex_lock(&pdata_lock));

        struct pdata *pdata = (struct pdata*) malloc(sizeof(struct pdata));
        if(!pdata) {
            perror("Error allocating pairing data");
            return false;
        }
        *pdata = (struct pdata) {
            .next = pdata_head,
            .name = name,
            .data.data = data,
            .data.data_size = data_size
        };
        pdata_head = pdata;

        cant_fail_ret(pthread_mutex_unlock(&pdata_lock));
        log_debug("Added pairing data for sensor '%s'...", name);
    }
    return true;
}

bool save_datastore_pair_data(FILE *file) {
    cant_fail_ret(pthread_mutex_lock(&pdata_lock));
    for(struct pdata *pdata = pdata_head; pdata; pdata = pdata->next) {
        //Write name
        int name_len = strlen(pdata->name);
        if(
            fputc((char) name_len, file) == EOF ||
            fwrite(pdata->name, 1, name_len, file) != name_len
        ) {
            perror("Error writing pairing data name");
            return false;
        }

        //Write data
        if(
            fwrite(&pdata->data.data_size, 1, sizeof(size_t), file) != sizeof(size_t) ||
            fwrite(pdata->data.data, 1, pdata->data.data_size, file) != pdata->data.data_size
        ) {
            perror("Error writing pairing data");
            return false;
        }
    }
    cant_fail_ret(pthread_mutex_unlock(&pdata_lock));

    if(fputc(0, file) == EOF) {
        perror("Error writing pairing data end terminator");
        return false;
    }

    return true;
}

void free_pair_data() {
    //Free pairing data
    for(struct pdata *pd = pdata_head, *npd = pd ? pd->next : NULL; pd; pd = npd, npd = pd ? pd->next : NULL) {
        free(pd->name);
        free(pd->data.data);
        free(pd);
    }
    pdata_head = NULL;
}

bool load_datastore_records(FILE *file, struct tudor_device *device) {
    while(true) {
        //Check for end of records
        char ind = fgetc(file);
        if(ind == 0 || ind == EOF) break;

        //Read GUID
        RECGUID guid;
        if(fread(&guid, 1, sizeof(RECGUID), file) != sizeof(RECGUID)) {
            perror("Error reading record GUID");
            return false;
        }

        //Read finger
        enum tudor_finger finger;
        if(fread(&finger, 1, sizeof(enum tudor_finger), file) != sizeof(enum tudor_finger)) {
            perror("Error reading record finger");
            return false;
        }

        //Read data
        size_t data_size;
        if(fread(&data_size, 1, sizeof(size_t), file) != sizeof(size_t)) {
            perror("Error reading record data size");
            return false;
        }

        void *data = malloc(data_size);
        if(!data) {
            perror("Error allocating record data");
            return false;
        }
        if(fread(data, 1, data_size, file) != data_size) {
            perror("Error reading record data");
            return false;
        }

        //Add record
        if(!tudor_add_record(device, guid, finger, data, data_size)) {
            log_error("Duplicate record!");
            return false;
        }

        free(data);
    }

    return true;
}

bool save_datastore_records(FILE *file, struct tudor_device *device) {
    cant_fail_ret(pthread_mutex_lock(&device->records_lock));
    for(struct tudor_record *rec = device->records_head; rec; rec = rec->next) {
        //Write non-end indicator
        if(fputc(1, file) == EOF) {
            perror("Error writing record non-end indicator");
            return false;
        }

        //Write GUID
        if(fwrite(&rec->guid, 1, sizeof(RECGUID), file) != sizeof(RECGUID)) {
            perror("Error writing record GUID");
            return false;
        }

        //Write finger
        if(fwrite(&rec->finger, 1, sizeof(enum tudor_finger), file) != sizeof(enum tudor_finger)) {
            perror("Error writing record finger");
            return false;
        }

        //Write data
        if(
            fwrite(&rec->data_size, 1, sizeof(size_t), file) != sizeof(size_t) ||
            fwrite(rec->data, 1, rec->data_size, file) != rec->data_size
        ) {
            perror("Error writing record data");
            return false;
        }
    }
    cant_fail_ret(pthread_mutex_unlock(&device->records_lock));

    if(fputc(0, file)) {
        perror("Error writing record end terminator");
        return false;
    }

    return true;
}