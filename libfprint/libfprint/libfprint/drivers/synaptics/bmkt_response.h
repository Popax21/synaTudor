/*
 * Synaptics MiS Fingerprint Sensor Response Data Interface
 * Copyright (C) 2019 Synaptics Inc
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#pragma once

#include "bmkt.h"

/** List of response message IDs */
#define BMKT_RSP_CONTINUOUS_IMAGE_CAPTURE_FAIL 0x02
#define BMKT_RSP_CONTINUOUS_IMAGE_CAPTURE_READY 0x03
#define BMKT_RSP_CONTINUOUS_IMAGE_CAPTURE_STOPPED 0x05
#define BMKT_RSP_SENSOR_MODULE_TEST_READY 0x07
#define BMKT_RSP_SENSOR_MODULE_TEST_FAIL 0x09
#define BMKT_RSP_SENSOR_MODULE_TEST_REPORT 0x0A
#define BMKT_RSP_NEXT_TEST_REPORT_CHUNK 0x0C

/*! \addtogroup init
 *  Response IDs returned by fingerprint initialization operation
 *  @{
 */
/** Failed to initialize fingerprint sensor module */
#define BMKT_RSP_FPS_INIT_FAIL 0x12
/** Successfully initialized fingerprint sensor module */
#define BMKT_RSP_FPS_INIT_OK 0x13
/*! @} */

/*! \addtogroup mode
 *  Response IDs returned by get fingerprint mode operation
 *  @{
 */
/** Failed to get fingerprint sensor module’s current operational mode */
#define BMKT_RSP_FPS_MODE_FAIL 0x22
/**
 * BMKT_RSP_FPS_MODE_REPORT:
 * Response containing the current operational mode of the fingerprint sensor module
 * <br>Payload data represented in \ref bmkt_fps_mode_resp_t struct
 */
#define BMKT_RSP_FPS_MODE_REPORT 0x23
/*! @} */

/*! \addtogroup setseclevel
 *  Response IDs returned by set security level operation
 *  @{
 */
/** Failed to set fingerprint sensor module security level */
#define BMKT_RSP_SET_SECURITY_LEVEL_FAIL 0x32
/**
 * BMKT_RSP_SET_SECURITY_LEVEL_REPORT:
 * Security level of the fingerprint sensor module was set successfully
 * <br>Contains payload data represented in \ref bmkt_set_sec_level_resp_t struct
 */
#define BMKT_RSP_SET_SECURITY_LEVEL_REPORT 0x33
/*! @} */

/*! \addtogroup getseclevel
 *  Response IDs returned by get security level operation
 *  @{
 */
/** Failed to get fingerprint sensor module security level */
#define BMKT_RSP_GET_SECURITY_LEVEL_FAIL 0x35
/**
 * BMKT_RSP_GET_SECURITY_LEVEL_REPORT:
 * Returns the current security level of the fingerprint sensor module
 * <br>Contains payload data represented in \ref bmkt_set_sec_level_resp_t struct
 */
#define BMKT_RSP_GET_SECURITY_LEVEL_REPORT 0x36
/*! @} */

/*! \addtogroup cancelop
 *  Response IDs returned by cancel_operation operation
 *  @{
 */
/**
 * BMKT_RSP_CANCEL_OP_OK:
 * Successfully canceled the current operation and returned
 * fingerprint sensor module to idle mode
 */
#define BMKT_RSP_CANCEL_OP_OK 0x42
/** Failed to cancel the current operation */
#define BMKT_RSP_CANCEL_OP_FAIL 0x43
/*! @} */

/*! \addtogroup enrollment
 *  Response IDs returned by enrollment operation
 *  @{
 */
/**
 * BMKT_RSP_ENROLL_READY:
 * Fingerprint enrollment session has begun and the user can place
 * their finger on the sensor
 */
#define BMKT_RSP_ENROLL_READY 0x54
/** Progress of the currently on-going fingerprint enrollment session */
#define BMKT_RSP_ENROLL_REPORT 0x55
/** Enrollment has been paused */
#define BMKT_RSP_ENROLL_PAUSED 0x56
/** Enrollment has been resume */
#define BMKT_RSP_ENROLL_RESUMED 0x57
/** The current enrollment session has encountered an error */
#define BMKT_RSP_ENROLL_FAIL 0x58
/**
 * BMKT_RSP_ENROLL_OK:
 * User has been successfully enrolled into the fingerprint sensor module
 * <br>Contains payload data represented in \ref bmkt_enroll_resp_t struct
 */
#define BMKT_RSP_ENROLL_OK 0x59

/**
 * BMKT_RSP_CAPTURE_COMPLETE:
 * Fingerprint image capture is complete and it is safe for the user
 * to lift their finger off the sensor
 */
#define BMKT_RSP_CAPTURE_COMPLETE 0x60
/*! @} */

/*! \addtogroup identify
 *  Response IDs returned by identify operation.
 *  @{
 */
/* Fingerprint identification session has begun */
#define BMKT_RSP_ID_READY 0x62
/* Identification has failed */
#define BMKT_RSP_ID_FAIL 0x63
/**
 * BMKT_RSP_ID_OK:
 * User has been successfully identified
 * <br>Contains payload data represented in \ref bmkt_auth_resp struct
 */
#define BMKT_RSP_ID_OK 0x64
/*! @} */

/*! \addtogroup verify
 *  Response IDs returned by identify operation.
 *  @{
 */
/** Fingerprint verification session has begun */
#define BMKT_RSP_VERIFY_READY 0x66
/** Verification has failed */
#define BMKT_RSP_VERIFY_FAIL 0x67
/**
 * BMKT_RSP_VERIFY_OK:
 * User’s identity has been successfully verified
 * <br>Contains payload data represented in \ref bmkt_auth_resp struct
 */
#define BMKT_RSP_VERIFY_OK 0x68
/*! @} */

/**
 * BMKT_RSP_TEMPLATE_RECORDS_REPORT:
 * Response ID returned by get enrolled users templates record operation
 * <br>Returns list of template records containing user IDs and corresponding finger IDs
 * <br>Payload data represented in \ref bmkt_enroll_templates_resp_t struct
 */
#define BMKT_RSP_TEMPLATE_RECORDS_REPORT 0x75

/**
 * BMKT_RSP_QUERY_RESPONSE_COMPLETE:
 * Response ID returned by get next query response operation
 * <br>Complete sequence of messages containing the template records query response has been sent
 */
#define BMKT_RSP_QUERY_RESPONSE_COMPLETE 0x76

/**
 * BMKT_RSP_GET_ENROLLED_FINGERS_REPORT:
 * Response ID returned by get enrolled fingers operation
 * <br> Returns list of IDs of enrolled fingers for a specific user,
 * along with template record status corresponding to each enrolled finger
 * <br>Contains payload data represented in \ref bmkt_enrolled_fingers_resp_t struct
 */
#define BMKT_RSP_GET_ENROLLED_FINGERS_REPORT 0x77

/*! \addtogroup dbcapacity
 *  Response IDs returned by get database capacity operation
 *  @{
 */
/**
 * BMKT_RSP_DATABASE_CAPACITY_REPORT:
 * Response specifying total capacity of fingerprint template database and
 * how much free capacity is remaining along with how many templates are corrupted and
 * how many bad (permanently unusable) storage slots are there.
 * <br>Payload data represented in \ref bmkt_get_db_capacity_resp_t struct
 */
#define BMKT_RSP_DATABASE_CAPACITY_REPORT 0x78
/** Failed to execute database query */
#define BMKT_RSP_QUERY_FAIL 0x79
/*! @} */

/*! \addtogroup deluser
 *  Response IDs returned by delete fingerprint of specific user operation
 *  @{
 */
/** Failed to delete a user’s fingerprint template from the database */
#define BMKT_RSP_DEL_USER_FP_FAIL 0x82
/**
 * BMKT_RSP_DEL_USER_FP_OK:
 * Fingerprint template successfully deleted from the database.
 * Returns the user ID and finger ID deleted. If value of finger ID is set equal to 0,
 * then all fingerprint templates for that user have been deleted from the database
 * <br>Payload data represented in \ref bmkt_del_user_resp_t struct
 */
#define BMKT_RSP_DEL_USER_FP_OK 0x83
/*! @} */

/*! \addtogroup delfulldb
 *  Response IDs returned by delete entire fingerprint template DB operation
 *  @{
 */
/** Failed to erase entire fingerprint template database */
#define BMKT_RSP_DEL_FULL_DB_FAIL 0x85
/** Successfully erased entire fingerprint template database */
#define BMKT_RSP_DEL_FULL_DB_OK 0x86
/**
 * BMKT_RSP_DELETE_PROGRESS:
 * Notify progress made during the on-going deletion of the full template database
 * <br>Payload data represented in \ref bmkt_del_all_users_resp_t struct
 */
#define BMKT_RSP_DELETE_PROGRESS 0x87
/*! @} */

/**
 * BMKT_RSP_REPEAT_LAST_BMKT_RSP_FAIL:
 * Response ID returned by repeate last response operation
 * <br>Failed to retrieve and re-send last response
 */
#define BMKT_RSP_REPEAT_LAST_BMKT_RSP_FAIL 0x93

/*! \addtogroup pwrdwn
 *  Response IDs returned by power down notify operation
 *  @{
 */
/** Fingerprint sensor module is ready to be powered down */
#define BMKT_RSP_POWER_DOWN_READY 0xA2
/** Failed to go into power down mode */
#define BMKT_RSP_POWER_DOWN_FAIL 0xA3
/*! @} */

/*! \addtogroup versioninfo
 *  Response IDs returned by get version operation
 *  @{
 */
/**
 * BMKT_RSP_VERSION_INFO:
 * System version information of the fingerprint sensor module
 * <br>Payload data represented in \ref bmkt_get_version_resp_t struct
 */
#define BMKT_RSP_VERSION_INFO 0xB2
/* Failed to retrieve and send last response */
#define BMKT_RSP_GET_VERSION_FAIL 0xB3
/*! @} */

/**
 * BMKT_RSP_GENERAL_ERROR:
 * Not tied to a specific command-response session.
 * <br>Could be caused by corrupt or truncated command message
 */
#define BMKT_RSP_GENERAL_ERROR 0xC1
#define BMKT_RSP_DISABLE_PAIRING_FAIL 0xC3
#define BMKT_RSP_DISABLE_PAIRING_OK 0xC4
#define BMKT_RSP_QUERY_PAIRING_FAIL 0xC6
#define BMKT_RSP_SENSOR_PAIRING_REPORT 0xC7

/*! \addtogroup versioninfo
 *  Response IDs returned by get sensor module status operation
 *  @{
 */
/**
 * BMKT_RSP_SENSOR_STATUS_REPORT:
 * Response returning the current status of the sensor module
 * <br>Payload data represented in bmkt_XXX struct
 */
#define BMKT_RSP_SENSOR_STATUS_REPORT 0xD2
/** Failed to retrieve sensor status */
#define BMKT_RSP_SENSOR_STATUS_FAIL 0xD3
/*! @} */

/**
 * BMKT_RSP_SEND_NEXT_USER_ID:
 * Response ID returned by identify user in order operation
 * <br>Notify to send the next batch of user IDs in the priority list
 */
#define BMKT_RSP_SEND_NEXT_USER_ID 0xE2
/**
 * BMKT_RSP_RETRIEVE_FINAL_RESULT_FAIL:
 * Response IDs returned by retrieve final result operation
 * <br>Failed to retrieve and re-send cached final result
 */
#define BMKT_RSP_RETRIEVE_FINAL_RESULT_FAIL 0xE5

/**
 * Response payload data structure returned by sensor initialization operation.
 */
typedef struct bmkt_init_resp
{
  uint8_t finger_presence;                                      /**< Indicates finger existence on the sensor during startup */
} bmkt_init_resp_t;

/**
 * bmkt_enroll_resp:
 * Response payload data structure returned by enrollment operation.
 */
typedef struct bmkt_enroll_resp
{
  int     progress;                             /**< Shows current progress status [0-100] */
  uint8_t finger_id;                            /**< User's finger id [1-10] */
  uint8_t user_id[BMKT_MAX_USER_ID_LEN];        /**< User name to be enrolled */
} bmkt_enroll_resp_t;

/**
 * bmkt_auth_resp:
 * Response payload data structure returned by identify and verify operations.
 */
struct bmkt_auth_resp
{
  double  match_result;                         /**< match result returned by matcher */
  uint8_t finger_id;                            /**< Matched templates's finger id */
  uint8_t user_id[BMKT_MAX_USER_ID_LEN];        /**< Matched template's user id */
};

typedef struct bmkt_auth_resp bmkt_verify_resp_t;   /**< Returned by verify */
typedef struct bmkt_auth_resp bmkt_identify_resp_t; /**< Returned by identify */

/**
 * bmkt_fps_mode_resp:
 * Response payload data structure returned by get fingerprint mode operation.
 */
typedef struct bmkt_fps_mode_resp
{
  uint8_t mode;                     /**< One of the Level I bmkt_mode_t values */
  uint8_t level2_mode;              /**< One of the Level II bmkt_mode_level2_t values */
  uint8_t cmd_id;                   /**< Message ID of command being executed when bmkt_get_fps_mode was called */
  uint8_t finger_presence;          /**< Finger presence status value finger on sensor 1 / finger not on sensor 0 */
} bmkt_fps_mode_resp_t;

/**
 * bmkt_get_version_resp:
 * Response payload data structure returned by get version operation.
 */
typedef struct bmkt_get_version_resp
{
  uint8_t part[BMKT_PART_NUM_LEN];                  /**< Software Part Number */
  uint8_t year;                                     /**< Software Version Year */
  uint8_t week;                                     /**< Software Version Week */
  uint8_t patch;                                    /**< Software Version Patch Level */
  uint8_t supplier_id[BMKT_SUPPLIER_ID_LEN];        /**< Software Supplier Identification */
} bmkt_get_version_resp_t;

/**
 * bmkt_get_db_capacity_resp:
 * Response payload data structure returned by get DB capacity operation.
 */
typedef struct bmkt_get_db_capacity_resp
{
  uint8_t total;                    /**< Total Available Capacity: Total number of template records that can be stored */
  uint8_t empty;                    /**< Free Capacity: Number of template records that can still be stored */
  uint8_t bad_slots;                /**< Number of bad template storage slots */
  uint8_t corrupt_templates;        /**< Number of corrupt templates */
} bmkt_get_db_capacity_resp_t;

/**
 * bmkt_sec_level:
 * Security level values.
 */
typedef enum bmkt_sec_level {
  BMKT_SECURITY_LEVEL_LOW     = 0x10,
  BMKT_SECURITY_LEVEL_MEDIUM  = 0x40,
  BMKT_SECURITY_LEVEL_HIGH    = 0x60,
} bmkt_sec_level_t;

/**
 * bmkt_set_sec_level_resp:
 * Response payload data structure returned by get/set security level operations.
 */
typedef struct bmkt_set_sec_level_resp
{
  bmkt_sec_level_t sec_level;       /**< One of the bmkt_sec_level_t values */
} bmkt_set_sec_level_resp_t;

/**
 * bmkt_del_all_users_resp:
 * Response payload data structure returned by delete all enrolled users operation.
 */
typedef struct bmkt_del_all_users_resp
{
  int progress;         /**< Progress indicator as a percentage */
} bmkt_del_all_users_resp_t;

/**
 * bmkt_del_user_resp:
 * Response payload data structure returned by delete enrolled user operation.
 */
typedef struct bmkt_del_user_resp
{
  int progress;         /**< Progress indicator as a percentage */
} bmkt_del_user_resp_t;

/**
 * bmkt_enroll_template:
 * Structure of enrolled users template record data.
 */
typedef struct bmkt_enroll_template
{
  uint8_t user_id_len;                              /**< Length of user_id string */
  uint8_t template_status;                          /**< Template record status  */
  uint8_t finger_id;                                /**< ID of enrolled finger */
  uint8_t user_id[BMKT_MAX_USER_ID_LEN + 1];        /**< Name of the enrolled user */
} bmkt_enroll_template_t;

/**
 * bmkt_enroll_templates_resp:
 * Response payload data structure returned by get enrolled user list operation.
 */
typedef struct bmkt_enroll_templates_resp
{
  uint8_t                total_query_messages;                                   /**< Total query response messages */
  uint8_t                query_sequence;                                         /**< Query response sequence number */
  bmkt_enroll_template_t templates[BMKT_MAX_NUM_TEMPLATES_INTERNAL_FLASH];       /**< Enrolled user template records list */
} bmkt_enroll_templates_resp_t;

/**
 * bmkt_enrolled_fingers:
 * Structure of template record status corresponding to each enrolled finger.
 */
typedef struct bmkt_enrolled_fingers
{
  uint8_t finger_id;                /**< ID of enrolled finger */
  uint8_t template_status;          /**< Template record status of finger_id */
} bmkt_enrolled_fingers_t;

/**
 * bmkt_enrolled_fingers_resp:
 * Response payload data structure returned by get enrolled fingers operation.
 */
typedef struct bmkt_enrolled_fingers_resp
{
  bmkt_enrolled_fingers_t fingers[10];       /**< List of enroled fingers, max number of supported fingers per user is 10 */
} bmkt_enrolled_fingers_resp_t;

/**
 * bmkt_response_data_t:
 * Union combining all response payload data types.
 */
typedef union
{
  bmkt_init_resp_t             init_resp;
  bmkt_enroll_resp_t           enroll_resp;
  bmkt_verify_resp_t           verify_resp;
  bmkt_identify_resp_t         id_resp;
  bmkt_fps_mode_resp_t         fps_mode_resp;
  bmkt_get_version_resp_t      get_version_resp;
  bmkt_get_db_capacity_resp_t  db_cap_resp;
  bmkt_set_sec_level_resp_t    sec_level_resp;
  bmkt_del_all_users_resp_t    del_all_users_resp;
  bmkt_enroll_templates_resp_t enroll_templates_resp;
  bmkt_del_user_resp_t         del_user_resp;
  bmkt_enrolled_fingers_resp_t enrolled_fingers_resp;
} bmkt_response_data_t;

/**
 * bmkt_response:
 * Structure to abstract different response structure types in one API
 * to be used in bmkt_resp_cb_t callback function.
 */
typedef struct bmkt_response
{
  int                  response_id;     /**< Response message ID, one of th BMKT_RSP_XXX */
  int                  result;          /**< Operation execution result code */
  int                  complete;        /**< Operation completion status 1: complete / 0: not completed */
  bmkt_response_data_t response;        /**< Operation specific response union */
} bmkt_response_t;
