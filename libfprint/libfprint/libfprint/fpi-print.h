#pragma once

#include "fpi-enums.h"
#include "fp-device.h"
#include "fp-print.h"

G_BEGIN_DECLS

/**
 * FpiPrintType:
 * @FPI_PRINT_UNDEFINED: Undefined type, this happens prior to enrollment
 * @FPI_PRINT_RAW: A raw print where the data is directly compared
 * @FPI_PRINT_NBIS: NBIS minutiae comparison
 */
typedef enum {
  FPI_PRINT_UNDEFINED = 0,
  FPI_PRINT_RAW,
  FPI_PRINT_NBIS,
} FpiPrintType;

/**
 * FpiMatchResult:
 * @FPI_MATCH_ERROR: An error occurred during matching
 * @FPI_MATCH_FAIL: The prints did not match
 * @FPI_MATCH_SUCCESS: The prints matched
 */
typedef enum {
  FPI_MATCH_ERROR = -1, /* -1 for g_task_propagate_int */
  FPI_MATCH_FAIL,
  FPI_MATCH_SUCCESS,
} FpiMatchResult;

void     fpi_print_add_print (FpPrint *print,
                              FpPrint *add);

void     fpi_print_set_type (FpPrint     *print,
                             FpiPrintType type);
void     fpi_print_set_device_stored (FpPrint *print,
                                      gboolean device_stored);

gboolean fpi_print_add_from_image (FpPrint *print,
                                   FpImage *image,
                                   GError **error);

FpiMatchResult fpi_print_bz3_match (FpPrint * template,
                                    FpPrint * print,
                                    gint bz3_threshold,
                                    GError **error);

/* Helpers to encode metadata into user ID strings. */
gchar *  fpi_print_generate_user_id (FpPrint *print);
gboolean fpi_print_fill_from_user_id (FpPrint    *print,
                                      const char *user_id);

G_END_DECLS
