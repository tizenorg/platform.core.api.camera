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

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUFFER_MAX_PLANE_NUM
#undef BUFFER_MAX_PLANE_NUM
#endif /* BUFFER_MAX_PLANE_NUM */

#define BUFFER_MAX_PLANE_NUM    4
#define CAMERA_PARSE_STRING_SIZE 20

#define PREVIEW_CB_TYPE_USER 0x0000000F
#define PREVIEW_CB_TYPE_EVAS 0x000000F0

#define CHECK_PREVIEW_CB(cb_info, cb_type) ((cb_info)->preview_cb_flag & cb_type)
#define SET_PREVIEW_CB_TYPE(cb_info, cb_type) ((cb_info)->preview_cb_flag |= cb_type)
#define UNSET_PREVIEW_CB_TYPE(cb_info, cb_type) ((cb_info)->preview_cb_flag &= ~cb_type)


typedef struct _camera_stream_data_s {
	union {
		struct {
			unsigned char *yuv;
			unsigned int length_yuv;
		} yuv420, yuv422;
		struct {
			unsigned char *y;
			unsigned int length_y;
			unsigned char *uv;
			unsigned int length_uv;
		} yuv420sp;
		struct {
			unsigned char *y;
			unsigned int length_y;
			unsigned char *u;
			unsigned int length_u;
			unsigned char *v;
			unsigned int length_v;
		} yuv420p, yuv422p;
		struct {
			unsigned char *data;
			unsigned int length_data;
		} encoded;
	} data;                         /**< pointer of captured stream */
	int data_type;                  /**< data type */
	unsigned int length_total;      /**< total length of stream buffer (in byte)*/
	unsigned int num_planes;        /**< number of planes */
	MMPixelFormatType format;       /**< image format */
	int width;                      /**< width of video buffer */
	int height;                     /**< height of video buffer */
	unsigned int timestamp;         /**< timestamp of stream buffer (msec)*/
	void *bo[BUFFER_MAX_PLANE_NUM]; /**< TBM buffer object */
	void *internal_buffer;          /**< Internal buffer pointer */
	int stride[BUFFER_MAX_PLANE_NUM];    /**< Stride of each plane */
	int elevation[BUFFER_MAX_PLANE_NUM]; /**< Elevation of each plane */
} camera_stream_data_s;

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
	media_format_h pkt_fmt;
	int preview_cb_flag;
	GMutex mp_data_mutex;
	void *evas_info;
	GMutex evas_mutex;
	gboolean run_evas_render;
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

#ifdef HAVE_WAYLAND
typedef struct _camera_wl_info_s {
	int parent_id;
	int window_x;
	int window_y;
	int window_width;
	int window_height;
	void *evas_obj;
} camera_wl_info_s;
#endif /* HAVE_WAYLAND */

typedef struct _camera_cli_s {
	intptr_t remote_handle;
	intptr_t display_handle;
	camera_cb_info_s *cb_info;
#ifdef HAVE_WAYLAND
	camera_wl_info_s wl_info;
#endif /* HAVE_WAYLAND */
} camera_cli_s;

typedef struct _camera_media_packet_data {
	int tbm_key;
	tbm_bo bo;
	tbm_bo buffer_bo[BUFFER_MAX_PLANE_NUM];
	int num_buffer_key;
	int ref_cnt;
} camera_media_packet_data;


int _camera_get_tbm_surface_format(int in_format, uint32_t *out_format);
int _camera_get_media_packet_mimetype(int in_format, media_format_mimetype_e *mimetype);
int _camera_media_packet_finalize(media_packet_h pkt, int error_code, void *user_data);
int _camera_start_evas_rendering(camera_h camera);
int _camera_stop_evas_rendering(camera_h camera, bool keep_screen);

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_CAMERA_PRIVATE_H__ */

