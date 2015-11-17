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

#define MAX_DETECTED_FACE 20

#define CAMERA_PARSE_STRING_SIZE 20

typedef enum {
	_CAMERA_EVENT_TYPE_STATE_CHANGE,
	_CAMERA_EVENT_TYPE_FOCUS_CHANGE,
	_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE,
	_CAMERA_EVENT_TYPE_PREVIEW,
	_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW,
	_CAMERA_EVENT_TYPE_CAPTURE,
	_CAMERA_EVENT_TYPE_ERROR,
	_CAMERA_EVENT_TYPE_HDR_PROGRESS,
	_CAMERA_EVENT_TYPE_INTERRUPTED,
	_CAMERA_EVENT_TYPE_FACE_DETECTION,
	_CAMERA_EVENT_TYPE_VIDEO_FRAME_RENDER_ERROR,
	_CAMERA_EVENT_TYPE_NUM
}_camera_event_e;

typedef struct _camera_cb_data {
	int event_type;
	void *handle;
} camera_cb_data;

typedef struct _callback_cb_info {
	GThread *msg_rcv_thread;
	GThread *message_handler_thread;
	gint rcv_thread_running;
	gint message_handler_running;
	gint fd;
	gint id;
	gpointer user_cb[MUSE_CAMERA_EVENT_TYPE_NUM];
	gpointer user_cb_completed[MUSE_CAMERA_EVENT_TYPE_NUM];
	gpointer user_data[MUSE_CAMERA_EVENT_TYPE_NUM];
	gchar recv_msg[MUSE_CAMERA_MSG_MAX_LENGTH];
	GCond *pCond;
	GMutex *pMutex;
	GCond message_handler_cond;
	GMutex message_handler_mutex;
	GList *idle_event_list;
	GCond idle_event_cond;
	GMutex idle_event_mutex;
	gint *activating;
	gint *ret;
	tbm_bufmgr bufmgr;
	GQueue *message_queue;
	gint prev_state;
	media_format_h pkt_fmt;
} callback_cb_info_s;

typedef struct _camera_message_s {
	gchar recv_msg[MUSE_CAMERA_MSG_MAX_LENGTH];
	muse_camera_api_e api;
} camera_message_s;

typedef struct _camera_event_s {
	callback_cb_info_s *cb_info;
	gchar recv_msg[MUSE_CAMERA_MSG_MAX_LENGTH];
	muse_camera_event_e event;
} camera_event_s;

typedef struct _camera_idle_event_s {
	callback_cb_info_s *cb_info;
	gchar recv_msg[MUSE_CAMERA_MSG_MAX_LENGTH];
	muse_camera_event_e event;
	GMutex event_mutex;
} camera_idle_event_s;

typedef struct _camera_cli_s {
	intptr_t remote_handle;
	MMHandleType client_handle;
	intptr_t cli_display_handle;
	callback_cb_info_s *cb_info;
#ifdef HAVE_WAYLAND
	MMCamWaylandInfo *wl_info;
#endif /* #ifdef HAVE_WAYLAND */
} camera_cli_s;

typedef struct _camera_media_packet_data {
	int tbm_key;
	tbm_bo bo;
	tbm_bo buffer_bo[BUFFER_MAX_PLANE_NUM];
	int num_buffer_key;
} camera_media_packet_data;

typedef enum {
	MUSE_CAMERA_CLIENT_SYNC_CB_HANDLER,
	MUSE_CAMERA_CLIENT_USER_CALLBACK,
	MUSE_CAMERA_CLIENT_MAX
} muse_cli_camera_api_e;

int _camera_get_mm_handle(camera_h camera , MMHandleType *handle);
int _camera_set_relay_mm_message_callback(camera_h camera, MMMessageCallback callback, void *user_data);
int __camera_start_continuous_focusing(camera_h camera);
int _camera_set_use(camera_h camera, bool used);
bool _camera_is_used(camera_h camera);
int _camera_get_tbm_surface_format(int in_format, uint32_t *out_format);
int _camera_get_media_packet_mimetype(int in_format, media_format_mimetype_e *mimetype);
int _camera_media_packet_finalize(media_packet_h pkt, int error_code, void *user_data);
int __convert_camera_error_code(const char* func, int code);

#ifdef __cplusplus
}
#endif

#endif //__TIZEN_MULTIMEDIA_CAMERA_PRIVATE_H__

