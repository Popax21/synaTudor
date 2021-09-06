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

#include <glib.h>
#include "bmkt_response.h"
#include "bmkt_message.h"

static uint8_t
extract8 (const uint8_t *buf, int *offset)
{
  uint8_t ret = 0;
  int off = 0;

  if (offset)
    off = *offset;

  ret = *(buf + off);

  if (offset)
    *offset += 1;

  return ret;
}


static int
parse_error_response (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  if (msg_resp->payload_len != 2)
    return BMKT_UNRECOGNIZED_MESSAGE;

  resp->result = (msg_resp->payload[0] << 8) | msg_resp->payload[1];

  return BMKT_SUCCESS;
}

static int
parse_init_ok (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  bmkt_init_resp_t *init_resp = &resp->response.init_resp;

  if (msg_resp->payload_len != 1)
    return BMKT_UNRECOGNIZED_MESSAGE;

  init_resp->finger_presence = extract8 (msg_resp->payload, NULL);

  return BMKT_SUCCESS;
}


static int
parse_fps_mode_report (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  int offset = 0;
  bmkt_fps_mode_resp_t *fps_mode_resp = &resp->response.fps_mode_resp;

  if (msg_resp->payload_len != sizeof (bmkt_fps_mode_resp_t))
    return BMKT_UNRECOGNIZED_MESSAGE;

  fps_mode_resp->mode = extract8 (msg_resp->payload, &offset);
  fps_mode_resp->level2_mode = extract8 (msg_resp->payload, &offset);
  fps_mode_resp->cmd_id = extract8 (msg_resp->payload, &offset);
  fps_mode_resp->finger_presence = extract8 (msg_resp->payload, &offset);

  return BMKT_SUCCESS;
}

static int
parse_enroll_report (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  bmkt_enroll_resp_t *enroll_resp = &resp->response.enroll_resp;

  if (msg_resp->payload_len != 1)
    return BMKT_UNRECOGNIZED_MESSAGE;

  enroll_resp->progress = extract8 (msg_resp->payload, NULL);

  return BMKT_SUCCESS;
}

static int
parse_enroll_ok (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  bmkt_enroll_resp_t *enroll_resp = &resp->response.enroll_resp;

  if (msg_resp->payload_len < 1 || msg_resp->payload_len > (BMKT_MAX_USER_ID_LEN + 1))
    return BMKT_UNRECOGNIZED_MESSAGE;

  enroll_resp->finger_id = msg_resp->payload[0];
  memcpy (enroll_resp->user_id, &msg_resp->payload[1], msg_resp->payload_len - 1);

  return BMKT_SUCCESS;
}

static int
parse_auth_ok (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  bmkt_identify_resp_t *id_resp = &resp->response.id_resp;

  if (msg_resp->payload_len < 3 || msg_resp->payload_len > (BMKT_MAX_USER_ID_LEN + 3))
    return BMKT_UNRECOGNIZED_MESSAGE;

  id_resp->match_result = (double) msg_resp->payload[0] + 0.01 * (double) msg_resp->payload[1];
  id_resp->finger_id = msg_resp->payload[2];
  memcpy (id_resp->user_id, &msg_resp->payload[3], msg_resp->payload_len - 3);

  return BMKT_SUCCESS;
}

static int
parse_security_level_report (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  bmkt_set_sec_level_resp_t *sec_level_resp = &resp->response.sec_level_resp;

  if (msg_resp->payload_len != 1)
    return BMKT_UNRECOGNIZED_MESSAGE;

  sec_level_resp->sec_level = extract8 (msg_resp->payload, NULL);

  return BMKT_SUCCESS;
}

static int
parse_del_all_users_progress_report (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  bmkt_del_all_users_resp_t *del_all_users_resp = &resp->response.del_all_users_resp;

  if (msg_resp->payload_len != 1)
    return BMKT_UNRECOGNIZED_MESSAGE;

  del_all_users_resp->progress = extract8 (msg_resp->payload, NULL);

  return BMKT_SUCCESS;
}

static int
parse_db_cap_report (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  bmkt_get_db_capacity_resp_t *db_cap_resp = &resp->response.db_cap_resp;
  int offset = 0;

  if (msg_resp->payload_len < 2 || msg_resp->payload_len > 4)
    return BMKT_UNRECOGNIZED_MESSAGE;

  db_cap_resp->total = extract8 (msg_resp->payload, &offset);
  db_cap_resp->empty = extract8 (msg_resp->payload, &offset);

  if (msg_resp->payload_len == 4)
    {
      db_cap_resp->bad_slots = extract8 (msg_resp->payload, &offset);
      db_cap_resp->corrupt_templates = extract8 (msg_resp->payload, &offset);
    }

  return BMKT_SUCCESS;
}

static int
parse_get_enrolled_fingers_report (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  int offset = 0;
  int i = 0;

  if (msg_resp->payload_len < 2)
    return BMKT_UNRECOGNIZED_MESSAGE;
  /* 2 bytes per finger so calculate the total number of fingers to process*/
  int num_fingers = (msg_resp->payload_len) / 2;

  bmkt_enrolled_fingers_resp_t *get_enrolled_fingers_resp = &resp->response.enrolled_fingers_resp;

  for (i = 0; i < num_fingers; i++)
    {
      get_enrolled_fingers_resp->fingers[i].finger_id = extract8 (msg_resp->payload, &offset);
      get_enrolled_fingers_resp->fingers[i].template_status = extract8 (msg_resp->payload, &offset);

    }
  return BMKT_SUCCESS;
}
static int
parse_get_enrolled_users_report (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  int offset = 0;
  int i = 0;

  /* the payload is 2 bytes + template data */
  if (msg_resp->payload_len < 2)
    return BMKT_UNRECOGNIZED_MESSAGE;

  bmkt_enroll_templates_resp_t *get_enroll_templates_resp = &resp->response.enroll_templates_resp;

  get_enroll_templates_resp->total_query_messages = extract8 (msg_resp->payload, &offset);
  get_enroll_templates_resp->query_sequence = extract8 (msg_resp->payload, &offset);

  int n = 0;

  for (n = 0; n < BMKT_MAX_NUM_TEMPLATES_INTERNAL_FLASH; n++)
    {
      if (offset >= msg_resp->payload_len)
        break;
      get_enroll_templates_resp->templates[n].user_id_len = extract8 (msg_resp->payload, &offset) - 2;
      if(get_enroll_templates_resp->templates[n].user_id_len > BMKT_MAX_USER_ID_LEN)
        return BMKT_UNRECOGNIZED_MESSAGE;
      get_enroll_templates_resp->templates[n].template_status = extract8 (msg_resp->payload, &offset);
      get_enroll_templates_resp->templates[n].finger_id = extract8 (msg_resp->payload, &offset);
      for (i = 0; i < get_enroll_templates_resp->templates[n].user_id_len; i++)
        get_enroll_templates_resp->templates[n].user_id[i] = extract8 (msg_resp->payload, &offset);
      get_enroll_templates_resp->templates[n].user_id[i] = '\0';
    }

  return BMKT_SUCCESS;
}

static int
parse_get_version_report (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  bmkt_get_version_resp_t *get_version_resp = &resp->response.get_version_resp;
  int offset = 0;

  if (msg_resp->payload_len != 15)
    return BMKT_UNRECOGNIZED_MESSAGE;

  memcpy (get_version_resp->part, msg_resp->payload, BMKT_PART_NUM_LEN);
  offset += BMKT_PART_NUM_LEN;
  get_version_resp->year = extract8 (msg_resp->payload, &offset);
  get_version_resp->week = extract8 (msg_resp->payload, &offset);
  get_version_resp->patch = extract8 (msg_resp->payload, &offset);
  memcpy (get_version_resp->supplier_id, msg_resp->payload + offset, BMKT_SUPPLIER_ID_LEN);

  return BMKT_SUCCESS;
}

int
bmkt_compose_message (uint8_t *cmd, int *cmd_len, uint8_t msg_id, uint8_t seq_num,
                      uint8_t payload_size, const uint8_t *payload)
{
  int message_len = BMKT_MESSAGE_HEADER_LEN + payload_size;

  if (*cmd_len < message_len)
    return BMKT_OUT_OF_MEMORY;

  cmd[BMKT_MESSAGE_HEADER_ID_FIELD] = BMKT_MESSAGE_HEADER_ID;
  cmd[BMKT_MESSAGE_SEQ_NUM_FIELD] = seq_num;
  cmd[BMKT_MESSAGE_ID_FIELD] = msg_id;
  cmd[BMKT_MESSAGE_PAYLOAD_LEN_FIELD] = payload_size;
  memcpy (&cmd[BMKT_MESSAGE_PAYLOAD_FIELD], payload, payload_size);

  *cmd_len = message_len;

  return BMKT_SUCCESS;
}

int
bmkt_parse_message_header (uint8_t *resp_buf, int resp_len, bmkt_msg_resp_t *msg_resp)
{
  if (resp_buf[BMKT_MESSAGE_HEADER_ID_FIELD] != BMKT_MESSAGE_HEADER_ID)
    return BMKT_CORRUPT_MESSAGE;

  msg_resp->seq_num = resp_buf[BMKT_MESSAGE_SEQ_NUM_FIELD];
  msg_resp->msg_id = resp_buf[BMKT_MESSAGE_ID_FIELD];
  msg_resp->payload_len = resp_buf[BMKT_MESSAGE_PAYLOAD_LEN_FIELD];
  if (msg_resp->payload_len > 0)
    msg_resp->payload = &resp_buf[BMKT_MESSAGE_PAYLOAD_FIELD];
  else
    msg_resp->payload = NULL;

  return BMKT_SUCCESS;
}

int
bmkt_parse_message_payload (bmkt_msg_resp_t *msg_resp, bmkt_response_t *resp)
{
  int ret = BMKT_SUCCESS;

  memset (resp, 0, sizeof (bmkt_response_t));

  resp->response_id = msg_resp->msg_id;

  switch(msg_resp->msg_id)
    {
    case BMKT_RSP_CONTINUOUS_IMAGE_CAPTURE_FAIL:
    case BMKT_RSP_SENSOR_MODULE_TEST_FAIL:
    case BMKT_RSP_FPS_INIT_FAIL:
    case BMKT_RSP_FPS_MODE_FAIL:
    case BMKT_RSP_SET_SECURITY_LEVEL_FAIL:
    case BMKT_RSP_GET_SECURITY_LEVEL_FAIL:
    case BMKT_RSP_CANCEL_OP_FAIL:
    case BMKT_RSP_ENROLL_FAIL:
    case BMKT_RSP_ID_FAIL:
    case BMKT_RSP_VERIFY_FAIL:
    case BMKT_RSP_QUERY_FAIL:
    case BMKT_RSP_DEL_USER_FP_FAIL:
    case BMKT_RSP_DEL_FULL_DB_FAIL:
    case BMKT_RSP_REPEAT_LAST_BMKT_RSP_FAIL:
    case BMKT_RSP_POWER_DOWN_FAIL:
    case BMKT_RSP_GET_VERSION_FAIL:
    case BMKT_RSP_DISABLE_PAIRING_FAIL:
    case BMKT_RSP_QUERY_PAIRING_FAIL:
    case BMKT_RSP_SENSOR_STATUS_FAIL:
    case BMKT_RSP_RETRIEVE_FINAL_RESULT_FAIL:
      ret = parse_error_response (msg_resp, resp);
      resp->complete = 1;
      break;

    case BMKT_RSP_FPS_INIT_OK:
      ret = parse_init_ok (msg_resp, resp);
      resp->complete = 1;
      break;

    case BMKT_RSP_CANCEL_OP_OK:
    case BMKT_RSP_DEL_FULL_DB_OK:
    case BMKT_RSP_DEL_USER_FP_OK:
      /* responses with a payload of 0
         so the response indicates success */
      resp->result = BMKT_SUCCESS;
      resp->complete = 1;
      break;

    case BMKT_RSP_FPS_MODE_REPORT:
      // parse_fps_mode
      ret = parse_fps_mode_report (msg_resp, resp);
      resp->complete = 1;
      break;

    case BMKT_RSP_GET_SECURITY_LEVEL_REPORT:
    case BMKT_RSP_SET_SECURITY_LEVEL_REPORT:
      /* parse security level result */
      ret = parse_security_level_report (msg_resp, resp);
      resp->complete = 1;
      break;

    case BMKT_RSP_DELETE_PROGRESS:
      ret = parse_del_all_users_progress_report (msg_resp, resp);
      break;

    case BMKT_RSP_CAPTURE_COMPLETE:
      resp->result = BMKT_SUCCESS;
      break;

    case BMKT_RSP_ENROLL_READY:
      resp->result = BMKT_SUCCESS;
      break;

    case BMKT_RSP_ENROLL_REPORT:
      ret = parse_enroll_report (msg_resp, resp);
      break;

    case BMKT_RSP_ENROLL_OK:
      resp->complete = 1;
      ret = parse_enroll_ok (msg_resp, resp);
      break;

    case BMKT_RSP_ID_OK:
    case BMKT_RSP_VERIFY_OK:
      ret = parse_auth_ok (msg_resp, resp);
      resp->complete = 1;
      break;

    case BMKT_RSP_GET_ENROLLED_FINGERS_REPORT:
      ret = parse_get_enrolled_fingers_report (msg_resp, resp);
      resp->complete = 1;
      break;

    case BMKT_RSP_DATABASE_CAPACITY_REPORT:
      resp->complete = 1;
      ret = parse_db_cap_report (msg_resp, resp);
      break;

    case BMKT_RSP_TEMPLATE_RECORDS_REPORT:
      ret = parse_get_enrolled_users_report (msg_resp, resp);
      break;

    case BMKT_RSP_QUERY_RESPONSE_COMPLETE:
      resp->complete = 1;
      break;

    case BMKT_RSP_VERSION_INFO:
      ret = parse_get_version_report (msg_resp, resp);
      resp->complete = 1;
      break;

    case BMKT_RSP_POWER_DOWN_READY:
      resp->complete = 1;
      break;
    }

  return ret;
}
