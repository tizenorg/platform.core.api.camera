/*
* Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/





#ifndef __TIZEN_MULTIMEDIA_CAMERA_PRIVATE_H__
#define	__TIZEN_MULTIMEDIA_CAMERA_PRIVATE_H__
#include <camera.h>
#include <muse_core.h>
#include <muse_camera.h>
#include <mm_camcorder.h>


#ifdef __cplusplus
extern "C" {
#endif

#define CAMERA_PARSE_STRING_SIZE 20

typedef struct _camera_cb_info_s {
	gint fd;
	GThread *msg_recv_thread;
	GThread *msg_handler_thread;
	gint msg_recv_running;
	gint msg_handler_running;
	GCond msg_handler_cond;
	GMutex msg_handler_mutex;
	GQueue *msg_queue;
	GList *idle_event_list;
	GCond idle_event_cond;
	GMutex idle_event_mutex;
	gpointer user_cb[MUSE_CAMERA_EVENT_TYPE_NUM];
	gpointer user_data[MUSE_CAMERA_EVENT_TYPE_NUM];
	gchar recv_msg[MUSE_CAMERA_MSG_MAX_LENGTH];
	GCond api_cond[MUSE_CAMERA_API_MAX];
	GMutex api_mutex[MUSE_CAMERA_API_MAX];
	gint *api_activating;
	gint *api_ret;
	tbm_bufmgr bufmgr;
	gint prev_state;
	gchar *caps;
	media_format_h pkt_fmt;
} camera_cb_info_s;

typedef struct _camera_message_s {
	gchar recv_msg[MUSE_CAMERA_MSG_MAX_LENGTH];
	muse_camera_api_e api;
} camera_message_s;

typedef struct _camera_idle_event_s {
	camera_cb_info_s *cb_info;
	gchar recv_msg[MUSE_CAMERA_MSG_MAX_LENGTH];
	muse_camera_event_e event;
	GMutex event_mutex;
} camera_idle_event_s;

typedef struct _camera_cli_s {
	intptr_t remote_handle;
	MMHandleType client_handle;
	int display_type;
	intptr_t display_handle;
	camera_cb_info_s *cb_info;
#ifdef HAVE_WAYLAND
	MMCamWaylandInfo wl_info;
#endif /* HAVE_WAYLAND */
} camera_cli_s;

typedef struct _camera_media_packet_data {
	int tbm_key;
	tbm_bo bo;
	tbm_bo buffer_bo[BUFFER_MAX_PLANE_NUM];
	int num_buffer_key;
} camera_media_packet_data;


int _camera_get_tbm_surface_format(int in_format, uint32_t *out_format);
int _camera_get_media_packet_mimetype(int in_format, media_format_mimetype_e *mimetype);
int _camera_media_packet_finalize(media_packet_h pkt, int error_code, void *user_data);
int __convert_camera_error_code(const char *func, int code);

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_CAMERA_PRIVATE_H__ */

