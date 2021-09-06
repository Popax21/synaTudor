/*
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

#define BMKT_MESSAGE_HEADER_ID 0xFE
#define BMKT_MESSAGE_HEADER_LEN (4)
#define BMKT_MESSAGE_CRC32_LEN (4)
#define BMKT_MESSAGE_HEADER_ID_FIELD 0
#define BMKT_MESSAGE_SEQ_NUM_FIELD 1
#define BMKT_MESSAGE_ID_FIELD 2
#define BMKT_MESSAGE_PAYLOAD_LEN_FIELD 3
#define BMKT_MESSAGE_PAYLOAD_FIELD 4

// Command messages
#define BMKT_CMD_CONTINUOUS_IMAGE_CAPTURE 0x01
#define BMKT_CMD_CONTINUOUS_IMAGE_CAPTURE_STOP 0x04
#define BMKT_CMD_SENSOR_MODULE_TEST 0x06
#define BMKT_CMD_SENSOR_MODULE_TEST_START 0x08
#define BMKT_CMD_NEXT_TEST_REPORT_CHUNK 0x0B
#define BMKT_CMD_FPS_INIT 0x11
#define BMKT_CMD_GET_FPS_MODE 0x21
#define BMKT_CMD_SET_SECURITY_LEVEL 0x31
#define BMKT_CMD_GET_SECURITY_LEVEL 0x34
#define BMKT_CMD_CANCEL_OP 0x41
#define BMKT_CMD_ENROLL_USER 0x51
#define BMKT_CMD_ENROLL_PAUSE 0x52
#define BMKT_CMD_ENROLL_RESUME 0x53
#define BMKT_CMD_ID_USER 0x61
#define BMKT_CMD_VERIFY_USER 0x65
#define BMKT_CMD_GET_TEMPLATE_RECORDS 0x71
#define BMKT_CMD_GET_NEXT_QUERY_RESPONSE 0x72
#define BMKT_CMD_GET_ENROLLED_FINGERS 0x73
#define BMKT_CMD_GET_DATABASE_CAPACITY 0x74
#define BMKT_CMD_DEL_USER_FP 0x81
#define BMKT_CMD_DEL_FULL_DB 0x84
#define BMKT_CMD_REPEAT_LAST_RSP 0x92
#define BMKT_CMD_POWER_DOWN_NOTIFY 0xA1
#define BMKT_CMD_GET_VERSION 0xB1
#define BMKT_CMD_DISABLE_PAIRING 0xC2
#define BMKT_CMD_QUERY_PAIRING 0xC5
#define BMKT_CMD_SENSOR_STATUS 0xD1
#define BMKT_CMD_ID_USER_IN_ORDER 0xE1
#define BMKT_CMD_ID_NEXT_USER 0xE3
#define BMKT_CMD_VERIFY_USER_IN_ORDER 0xF1
#define BMKT_CMD_VERIFY_FINGERS_IN_ORDER 0xF2
#define BMKT_CMD_GET_FINAL_RESULT 0xE4

#define BMKT_EVT_FINGER_REPORT 0x91

#define BMKT_EVT_FINGER_STATE_NOT_ON_SENSOR 0x00
#define BMKT_EVT_FINGER_STATE_ON_SENSOR 0x01

typedef struct bmkt_msg_resp
{
  uint8_t  msg_id;
  uint8_t  seq_num;
  uint8_t  payload_len;
  uint8_t *payload;
  int      result;
} bmkt_msg_resp_t;

int bmkt_compose_message (uint8_t       *cmd,
                          int           *cmd_len,
                          uint8_t        msg_id,
                          uint8_t        seq_num,
                          uint8_t        payload_size,
                          const uint8_t *payload);

int bmkt_parse_message_header (uint8_t         *resp_buf,
                               int              resp_len,
                               bmkt_msg_resp_t *msg_resp);
int bmkt_parse_message_payload (bmkt_msg_resp_t *msg_resp,
                                bmkt_response_t *resp);
