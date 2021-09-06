#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "extracted.h"

typedef struct {
    unsigned int size;
    void *data;
} BLOB;

struct tudorIplParameters {
    uint16_t pixel_bits, frame_header_size, width, x_off, x_size, col_header_size, height, y_off, y_size;
    uint8_t __pad1[2];
    uint32_t config_ver_major, config_ver_minor;
    uint8_t ipl_type;
    uint8_t __pad2[3];
    BLOB iota;
} __attribute__((packed));

struct tudorImage {
    uint8_t __pad1[12];
    uint16_t height, width, pixel_bits;
    uint8_t __pad2[3];
    uint16_t pixel_data[];
} __attribute__((packed));

int __extracted EXTR_tudorIplOpen(void **out, struct tudorIplParameters *params);
int __extracted EXTR_tudorIplProcessFrame(void *ipl, BLOB *frame, BLOB *img);
int __extracted EXTR_tudorIplClose(void *ipl);

int process_frame(struct tudorIplParameters *params, void *frame, uint32_t frame_size, uint8_t **img, int *img_width, int *img_height, bool *enough_coverage) {
    int ret = 0;

    //Open IPL
    void *ipl = NULL;
    if((ret = EXTR_tudorIplOpen(&ipl, params)) != 0) return 0;
    assert(ipl != NULL);

    //Process frame
    struct {
        BLOB blob;
        uint32_t flags;
    } frame_data;
    frame_data.blob.data = frame;
    frame_data.blob.size = frame_size;

    BLOB img_blob = {0};
    if((ret = EXTR_tudorIplProcessFrame(ipl, &frame_data.blob, &img_blob)) != 0) goto exit;
    assert(img_blob.size >= sizeof(struct tudorImage));

    struct tudorImage *timg = (struct tudorImage*) img_blob.data;
    assert(timg->width > 0 && timg->height > 0 && timg->pixel_bits == 8);
    assert(img_blob.size == sizeof(struct tudorImage) + timg->width * timg->height);

    *img_width = timg->width;
    *img_height = timg->height;
    memcpy(*img = (uint8_t*) malloc(*img_width * *img_height), timg->pixel_data, *img_width * *img_height);
    *enough_coverage = (frame_data.flags & 0x80000000) ? false : true;

    free(timg);

    exit:;
    //Close IPL
    EXTR_tudorIplClose(ipl);

    return ret;
}

void free_image(uint8_t *img) {
    free(img);
}