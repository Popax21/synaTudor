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

#include "usb_transport.h"
#define BMKT_MAX_PENDING_SESSIONS 2

typedef enum bmkt_sensor_state {
  BMKT_SENSOR_STATE_UNINIT                        = 0,
  BMKT_SENSOR_STATE_IDLE,
  BMKT_SENSOR_STATE_INIT,
  BMKT_SENSOR_STATE_EXIT,
} bmkt_sensor_state_t;

typedef struct bmkt_sensor_drv bmkt_sensor_drv_t;

typedef struct bmkt_sensor_version
{
  uint32_t build_time;
  uint32_t build_num;
  uint8_t  version_major;
  uint8_t  version_minor;
  uint8_t  target;
  uint8_t  product;
  uint8_t  silicon_rev;
  uint8_t  formal_release;
  uint8_t  platform;
  uint8_t  patch;
  uint8_t  serial_number[6];
  uint16_t security;
  uint8_t  iface;
  uint8_t  device_type;
} bmkt_sensor_version_t;

typedef struct bmkt_sensor
{
  bmkt_usb_transport_t    usb_xport;
  bmkt_sensor_version_t   version;
  bmkt_session_ctx_t      pending_sessions[BMKT_MAX_PENDING_SESSIONS];
  int                     empty_session_idx;
  int                     flags;
  int                     seq_num;
  bmkt_sensor_state_t     sensor_state;
  bmkt_event_cb_t         finger_event_cb;
  void                   *finger_cb_ctx;
  bmkt_general_error_cb_t gen_err_cb;
  void                   *gen_err_cb_ctx;
  bmkt_op_state_t         op_state;
} bmkt_sensor_t;

int bmkt_sensor_open (bmkt_sensor_t          *sensor,
                      bmkt_general_error_cb_t err_cb,
                      void                   *err_cb_ctx);
int bmkt_sensor_close (bmkt_sensor_t *sensor);

int bmkt_sensor_init_fps (bmkt_sensor_t *sensor);

int bmkt_sensor_send_message (bmkt_sensor_t *sensor,
                              uint8_t        msg_id,
                              uint8_t        payload_size,
                              uint8_t       *payload,
                              bmkt_resp_cb_t resp_cb,
                              void          *resp_data);
int bmkt_sensor_handle_response (bmkt_sensor_t   *sensor,
                                 uint8_t         *resp_buf,
                                 int              resp_len,
                                 bmkt_msg_resp_t *msg_resp);

int bmkt_sensor_send_async_read_command (bmkt_sensor_t *sensor);
