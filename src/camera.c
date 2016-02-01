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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mm.h>
#include <mm_types.h>
#include <camera.h>
#include <muse_camera.h>
#include <muse_camera_msg.h>
#include <muse_core_ipc.h>
#include <muse_core_module.h>
#include <camera_private.h>
#include <muse_core.h>
#include <dlog.h>
#include <Elementary.h>
#include <tbm_surface_internal.h>
#include <Evas.h>
#ifdef HAVE_WAYLAND
#include <Ecore_Wayland.h>
#include <wayland-client.h>
#include <tizen-extension-client-protocol.h>
#else
#include <Ecore.h>
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_CAMERA"


#ifdef HAVE_WAYLAND
static void __global(void *data, struct wl_registry *registry,
	uint32_t name, const char *interface, uint32_t version)
{
	struct tizen_surface **tz_surface = NULL;

	if (!data) {
		LOGE("NULL data");
		return;
	}

	tz_surface = (struct tizen_surface **)data;

	if (!interface) {
		LOGW("NULL interface");
		return;
	}

	LOGI("interface %s", interface);

	if (strcmp(interface, "tizen_surface") == 0) {
		LOGD("binding tizen surface for wayland");

		*tz_surface = wl_registry_bind(registry, name, &tizen_surface_interface, version);
		if (*tz_surface == NULL) {
			LOGE("failed to bind");
		}

		LOGD("done");
	}

	return;
}

static void __global_remove(void *data, struct wl_registry *wl_registry, uint32_t name)
{
	LOGD("enter");
	return;
}

static const struct wl_registry_listener _camera_wl_registry_listener =
{
	__global,
	__global_remove
};

void __parent_id_getter(void *data, struct tizen_resource *tizen_resource, uint32_t id)
{
	if (!data) {
		LOGE("NULL data");
		return;
	}

	*((unsigned int *)data) = id;

    LOGD("[CLIENT] got parent_id [%u] from server", id);

	return;
}

static const struct tizen_resource_listener _camera_tz_resource_listener =
{
	__parent_id_getter
};

int _get_wl_info(Evas_Object *obj, camera_wl_info_s *wl_info)
{
	int ret = CAMERA_ERROR_NONE;
	Ecore_Wl_Window *window = NULL;
	struct wl_display *display = NULL;
	struct wl_surface *surface = NULL;
	struct wl_registry *registry = NULL;
	struct tizen_surface *tz_surface = NULL;
	struct tizen_resource *tz_resource = NULL;

	if (!obj || !wl_info) {
		LOGE("NULL parameter %p %p", obj, wl_info);
		return CAMERA_ERROR_INVALID_OPERATION;
	}

	window = elm_win_wl_window_get(obj);
	if (!window) {
		LOGE("failed to get wayland window");
		ret = CAMERA_ERROR_INVALID_OPERATION;
		goto _DONE;
	}

	surface = (struct wl_surface *)ecore_wl_window_surface_get(window);
	if (!surface) {
		LOGE("failed to get wayland surface");
		ret = CAMERA_ERROR_INVALID_OPERATION;
		goto _DONE;
	}

	display = (struct wl_display *)ecore_wl_display_get();
	if (!display) {
		LOGE("failed to get wayland display");
		ret = CAMERA_ERROR_INVALID_OPERATION;
		goto _DONE;
	}

	registry = wl_display_get_registry(display);
	if (!registry) {
		LOGE("failed to get wayland registry");
		ret = CAMERA_ERROR_INVALID_OPERATION;
		goto _DONE;
	}

	wl_registry_add_listener(registry, &_camera_wl_registry_listener, &tz_surface);

	wl_display_dispatch(display);
	wl_display_roundtrip(display);

	if (!tz_surface) {
		LOGE("failed to get tizen surface");
		ret = CAMERA_ERROR_INVALID_OPERATION;
		goto _DONE;
	}

	/* Get parent_id which is unique in a entire systemw. */
	tz_resource = tizen_surface_get_tizen_resource(tz_surface, surface);
	if (!tz_resource) {
		LOGE("failed to get tizen resurce");
		ret = CAMERA_ERROR_INVALID_OPERATION;
		goto _DONE;
	}

	wl_info->parent_id = 0;

	tizen_resource_add_listener(tz_resource, &_camera_tz_resource_listener, &wl_info->parent_id);

	wl_display_roundtrip(display);

	if (wl_info->parent_id > 0) {
		ret = CAMERA_ERROR_NONE;

		wl_info->evas_obj = obj;

		evas_object_geometry_get(obj, &wl_info->window_x, &wl_info->window_y,
			&wl_info->window_width, &wl_info->window_height);

		LOGD("evas object : %p, parent id : %u, window : %d,%d,%dx%d",
			wl_info->evas_obj, wl_info->parent_id,
			wl_info->window_x, wl_info->window_y,
			wl_info->window_width, wl_info->window_height);
	} else {
		ret = CAMERA_ERROR_INVALID_OPERATION;
		LOGE("failed to get parent id");
	}

_DONE:
	if (tz_surface) {
		tizen_surface_destroy(tz_surface);
		tz_surface = NULL;
	}

	if (tz_resource) {
		tizen_resource_destroy(tz_resource);
		tz_resource = NULL;
	}

	if (registry) {
		wl_registry_destroy(registry);
		registry = NULL;
	}

	return ret;
}
#endif /* HAVE_WAYLAND */

static int _import_tbm_key(tbm_bufmgr bufmgr, unsigned int tbm_key, tbm_bo *bo, tbm_bo_handle *bo_handle)
{
	tbm_bo tmp_bo = NULL;
	tbm_bo_handle tmp_bo_handle = {NULL, };

	if (bufmgr == NULL || bo == NULL || bo_handle == NULL || tbm_key == 0) {
		LOGE("invalid parameter - bufmgr %p, bo %p, bo_handle %p, key %d",
		     bufmgr, bo, bo_handle, tbm_key);
		return false;
	}

	tmp_bo = tbm_bo_import(bufmgr, tbm_key);
	if (tmp_bo == NULL) {
		LOGE("bo import failed - bufmgr %p, key %d", bufmgr, tbm_key);
		return false;
	}

	tmp_bo_handle = tbm_bo_map(tmp_bo, TBM_DEVICE_CPU, TBM_OPTION_READ);
	if (tmp_bo_handle.ptr == NULL) {
		LOGE("bo map failed %p", tmp_bo);
		tbm_bo_unref(tmp_bo);
		tmp_bo = NULL;
		return false;
	}

	/* set bo and bo_handle */
	*bo = tmp_bo;
	*bo_handle = tmp_bo_handle;

	return true;
}

static void _release_imported_bo(tbm_bo *bo)
{
	if (bo == NULL || *bo == NULL) {
		LOGW("NULL bo");
		return;
	}

	tbm_bo_unmap(*bo);
	tbm_bo_unref(*bo);
	*bo = NULL;

	return;
}

static int _client_wait_for_cb_return(muse_camera_api_e api, camera_cb_info_s *cb_info, int time_out)
{
	int ret = CAMERA_ERROR_NONE;
	gint64 end_time;

	LOGD("Enter api : %d", api);

	g_mutex_lock(&(cb_info->api_mutex[api]));

	if (cb_info->api_activating[api] == 0) {
		end_time = g_get_monotonic_time() + time_out * G_TIME_SPAN_SECOND;
		if (g_cond_wait_until(&(cb_info->api_cond[api]), &(cb_info->api_mutex[api]), end_time)) {
			ret = cb_info->api_ret[api];
			cb_info->api_activating[api] = 0;

			LOGD("return value : 0x%x", ret);
		} else {
			ret = CAMERA_ERROR_INVALID_OPERATION;

			LOGE("api %d was TIMED OUT!", api);
		}
	} else {
		ret = cb_info->api_ret[api];
		cb_info->api_activating[api] = 0;

		LOGD("condition is already checked for the api[%d], return[0x%x]", api, ret);
	}

	g_mutex_unlock(&(cb_info->api_mutex[api]));

	return ret;
}

int _camera_get_tbm_surface_format(int in_format, uint32_t *out_format)
{
	if (in_format <= MM_PIXEL_FORMAT_INVALID ||
	    in_format >= MM_PIXEL_FORMAT_NUM ||
	    out_format == NULL) {
		LOGE("INVALID_PARAMETER : in_format %d, out_format ptr %p", in_format, out_format);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	switch (in_format) {
	case MM_PIXEL_FORMAT_NV12:
	case MM_PIXEL_FORMAT_NV12T:
		*out_format = TBM_FORMAT_NV12;
		break;
	case MM_PIXEL_FORMAT_NV16:
		*out_format = TBM_FORMAT_NV16;
		break;
	case MM_PIXEL_FORMAT_NV21:
		*out_format = TBM_FORMAT_NV21;
		break;
	case MM_PIXEL_FORMAT_YUYV:
		*out_format = TBM_FORMAT_YUYV;
		break;
	case MM_PIXEL_FORMAT_UYVY:
	case MM_PIXEL_FORMAT_ITLV_JPEG_UYVY:
		*out_format = TBM_FORMAT_UYVY;
		break;
	case MM_PIXEL_FORMAT_422P:
		*out_format = TBM_FORMAT_YUV422;
		break;
	case MM_PIXEL_FORMAT_I420:
		*out_format = TBM_FORMAT_YUV420;
		break;
	case MM_PIXEL_FORMAT_YV12:
		*out_format = TBM_FORMAT_YVU420;
		break;
	case MM_PIXEL_FORMAT_RGB565:
		*out_format = TBM_FORMAT_RGB565;
		break;
	case MM_PIXEL_FORMAT_RGB888:
		*out_format = TBM_FORMAT_RGB888;
		break;
	case MM_PIXEL_FORMAT_RGBA:
		*out_format = TBM_FORMAT_RGBA8888;
		break;
	case MM_PIXEL_FORMAT_ARGB:
		*out_format = TBM_FORMAT_ARGB8888;
		break;
	default:
		LOGE("invalid in_format %d", in_format);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	return CAMERA_ERROR_NONE;
}


int _camera_get_media_packet_mimetype(int in_format, media_format_mimetype_e *mimetype)
{
	if (in_format <= MM_PIXEL_FORMAT_INVALID ||
	    in_format >= MM_PIXEL_FORMAT_NUM ||
	    mimetype == NULL) {
		LOGE("INVALID_PARAMETER : in_format %d, mimetype ptr %p", in_format, mimetype);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	switch (in_format) {
	case MM_PIXEL_FORMAT_NV12:
	case MM_PIXEL_FORMAT_NV12T:
		*mimetype = MEDIA_FORMAT_NV12;
		break;
	case MM_PIXEL_FORMAT_NV16:
		*mimetype = MEDIA_FORMAT_NV16;
		break;
	case MM_PIXEL_FORMAT_NV21:
		*mimetype = MEDIA_FORMAT_NV21;
		break;
	case MM_PIXEL_FORMAT_YUYV:
		*mimetype = MEDIA_FORMAT_YUYV;
		break;
	case MM_PIXEL_FORMAT_UYVY:
	case MM_PIXEL_FORMAT_ITLV_JPEG_UYVY:
		*mimetype = MEDIA_FORMAT_UYVY;
		break;
	case MM_PIXEL_FORMAT_422P:
		*mimetype = MEDIA_FORMAT_422P;
		break;
	case MM_PIXEL_FORMAT_I420:
		*mimetype = MEDIA_FORMAT_I420;
		break;
	case MM_PIXEL_FORMAT_YV12:
		*mimetype = MEDIA_FORMAT_YV12;
		break;
	case MM_PIXEL_FORMAT_RGB565:
		*mimetype = MEDIA_FORMAT_RGB565;
		break;
	case MM_PIXEL_FORMAT_RGB888:
		*mimetype = MEDIA_FORMAT_RGB888;
		break;
	case MM_PIXEL_FORMAT_RGBA:
		*mimetype = MEDIA_FORMAT_RGBA;
		break;
	case MM_PIXEL_FORMAT_ARGB:
		*mimetype = MEDIA_FORMAT_ARGB;
		break;
	default:
		LOGE("invalid in_format %d", in_format);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	return CAMERA_ERROR_NONE;
}

int _camera_media_packet_finalize(media_packet_h pkt, int error_code, void *user_data)
{
	int i = 0;
	int ret = 0;
	camera_cb_info_s *cb_info = (camera_cb_info_s *)user_data;
	camera_media_packet_data *mp_data = NULL;
	tbm_surface_h tsurf = NULL;

	if (pkt == NULL || cb_info == NULL) {
		LOGE("invalid parameter buffer %p, cb_info %p", pkt, cb_info);
		return MEDIA_PACKET_FINALIZE;
	}

	ret = media_packet_get_extra(pkt, (void **)&mp_data);
	if (ret != MEDIA_PACKET_ERROR_NONE) {
		LOGE("media_packet_get_extra failed 0x%x", ret);
		return MEDIA_PACKET_FINALIZE;
	}

	/*LOGD("mp_data %p", mp_data);*/

	if (mp_data) {
		int tbm_key = mp_data->tbm_key;

		/* release imported bo */
		for (i = 0 ; i < mp_data->num_buffer_key ; i++) {
			tbm_bo_unref(mp_data->buffer_bo[i]);
			mp_data->buffer_bo[i] = NULL;
		}

		/* unmap and unref tbm bo */
		_release_imported_bo(&mp_data->bo);

		/* return buffer */
		muse_camera_msg_send1_no_return(MUSE_CAMERA_API_RETURN_BUFFER,
			cb_info->fd, cb_info, INT, tbm_key);
		g_free(mp_data);
		mp_data = NULL;
	}

	ret = media_packet_get_tbm_surface(pkt, &tsurf);
	if (ret != MEDIA_PACKET_ERROR_NONE) {
		LOGE("media_packet_get_tbm_surface failed 0x%x", ret);
		return MEDIA_PACKET_FINALIZE;
	}

	if (tsurf) {
		tbm_surface_destroy(tsurf);
		tsurf = NULL;
	}

	return MEDIA_PACKET_FINALIZE;
}

static void _client_user_callback(camera_cb_info_s *cb_info, char *recv_msg, muse_camera_event_e event)
{
	int param1 = 0;
	int param2 = 0;
	int tbm_key = 0;
	tbm_bo bo = NULL;
	tbm_bo_handle bo_handle = {NULL, };

	if (recv_msg == NULL || event >= MUSE_CAMERA_EVENT_TYPE_NUM) {
		LOGE("invalid parameter - camera msg %p, event %d", recv_msg, event);
		return;
	}

	LOGD("get camera msg %s, event %d", recv_msg, event);

	if (event == MUSE_CAMERA_EVENT_TYPE_PREVIEW) {
		if (cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_PREVIEW] == NULL &&
			cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] == NULL) {
			LOGW("all preview callback from user are NULL");
			return;
		}
	} else if (cb_info->user_cb[event] == NULL) {
		LOGW("user callback for event %d is not set", event);
		return;
	}

	switch (event) {
	case MUSE_CAMERA_EVENT_TYPE_STATE_CHANGE:
		{
			int previous = 0;
			int current = 0;
			int by_policy = 0;

			muse_camera_msg_get(previous, recv_msg);
			muse_camera_msg_get(current, recv_msg);
			muse_camera_msg_get(by_policy, recv_msg);

			LOGD("STATE CHANGE - previous %d, current %d, by_policy %d",
			     previous, current, by_policy);

			((camera_state_changed_cb)cb_info->user_cb[event])((camera_state_e)previous,
				(camera_state_e)current, (bool)by_policy, cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOCUS_CHANGE:
		{
			int state = 0;

			muse_camera_msg_get(state, recv_msg);

			LOGD("FOCUS state - %d", state);

			((camera_focus_changed_cb)cb_info->user_cb[event])((camera_focus_state_e)state, cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE:
		LOGD("CAPTURE_COMPLETED");
		((camera_capture_completed_cb)cb_info->user_cb[event])(cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_PREVIEW:
	case MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW:
		{
			camera_preview_data_s frame;
			unsigned char *buf_pos = NULL;
			camera_stream_data_s *stream = NULL;
			int i = 0;
			int total_size = 0;
			int num_buffer_key = 0;
			int buffer_key[BUFFER_MAX_PLANE_NUM] = {0, };
			tbm_bo buffer_bo[BUFFER_MAX_PLANE_NUM] = {NULL, };
			tbm_bo_handle buffer_bo_handle[BUFFER_MAX_PLANE_NUM] = {{.ptr = NULL}, };
			camera_media_packet_data *mp_data = NULL;

			muse_camera_msg_get(tbm_key, recv_msg);
			muse_camera_msg_get(num_buffer_key, recv_msg);
			muse_camera_msg_get_array(buffer_key, recv_msg);

			memset(&frame, 0x0, sizeof(camera_preview_data_s));

			if (tbm_key <= 0) {
				LOGE("invalid key %d", tbm_key);
				break;
			}

			/* import tbm bo and get virtual address */
			if (!_import_tbm_key(cb_info->bufmgr, tbm_key, &bo, &bo_handle)) {
				LOGE("failed to import key %d", tbm_key);

				muse_camera_msg_send1_no_return(MUSE_CAMERA_API_RETURN_BUFFER,
					cb_info->fd, cb_info, INT, tbm_key);
				break;
			}

			buf_pos = (unsigned char *)bo_handle.ptr;

			/* get stream info */
			stream = (camera_stream_data_s *)buf_pos;

			for (i = 0 ; i < num_buffer_key ; i++) {
				/* import buffer bo and get virtual address */
				if (!_import_tbm_key(cb_info->bufmgr, buffer_key[i], &buffer_bo[i], &buffer_bo_handle[i])) {
					LOGE("failed to import buffer key %d", buffer_key[i]);

					/* release imported bo */
					for (i -= 1 ; i >= 0 ; i--)
						_release_imported_bo(&buffer_bo[i]);

					_release_imported_bo(&bo);

					/* send return buffer */
					muse_camera_msg_send1_no_return(MUSE_CAMERA_API_RETURN_BUFFER,
						cb_info->fd, cb_info, INT, tbm_key);
					return;
				}
			}

			if (cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_PREVIEW]) {
				/* set frame info */
				if (stream->format == MM_PIXEL_FORMAT_ITLV_JPEG_UYVY)
					frame.format  = MM_PIXEL_FORMAT_UYVY;
				else
					frame.format = stream->format;
				frame.width = stream->width;
				frame.height = stream->height;
				frame.timestamp = stream->timestamp;
				frame.num_of_planes = stream->num_planes;

				if (num_buffer_key == 0) {
					/* non-zero copy */
					buf_pos += sizeof(camera_stream_data_s);

					if (stream->format == MM_PIXEL_FORMAT_ENCODED_H264) {
						frame.data.encoded_plane.data = buf_pos;
						frame.data.encoded_plane.size = stream->data.encoded.length_data;
						total_size = stream->data.encoded.length_data;
					} else {
						switch (stream->num_planes) {
						case 1:
							frame.data.single_plane.yuv = buf_pos;
							frame.data.single_plane.size = stream->data.yuv420.length_yuv;
							total_size = stream->data.yuv420.length_yuv;
							break;
						case 2:
							frame.data.double_plane.y = buf_pos;
							frame.data.double_plane.y_size = stream->data.yuv420sp.length_y;
							buf_pos += stream->data.yuv420sp.length_y;
							frame.data.double_plane.uv = buf_pos;
							frame.data.double_plane.uv_size = stream->data.yuv420sp.length_uv;
							total_size = stream->data.yuv420sp.length_y + \
								stream->data.yuv420sp.length_uv;
							break;
						case 3:
							frame.data.triple_plane.y = buf_pos;
							frame.data.triple_plane.y_size = stream->data.yuv420p.length_y;
							buf_pos += stream->data.yuv420p.length_y;
							frame.data.triple_plane.u = buf_pos;
							frame.data.triple_plane.u_size = stream->data.yuv420p.length_u;
							buf_pos += stream->data.yuv420p.length_u;
							frame.data.triple_plane.v = buf_pos;
							frame.data.triple_plane.v_size = stream->data.yuv420p.length_v;
							total_size = stream->data.yuv420p.length_y + \
								stream->data.yuv420p.length_u + \
								stream->data.yuv420p.length_v;
							break;
						default:
							break;
						}
					}
				} else {
					/* zero copy */
					switch (stream->num_planes) {
					case 1:
						frame.data.single_plane.yuv = buffer_bo_handle[0].ptr;
						frame.data.single_plane.size = stream->data.yuv420.length_yuv;
						total_size = stream->data.yuv420.length_yuv;
						break;
					case 2:
						frame.data.double_plane.y = buffer_bo_handle[0].ptr;
						if (stream->num_planes == (unsigned int)num_buffer_key)
							frame.data.double_plane.uv = buffer_bo_handle[1].ptr;
						else
							frame.data.double_plane.uv = buffer_bo_handle[0].ptr + stream->data.yuv420sp.length_y;
						frame.data.double_plane.y_size = stream->data.yuv420sp.length_y;
						frame.data.double_plane.uv_size = stream->data.yuv420sp.length_uv;
						total_size = stream->data.yuv420sp.length_y + \
							stream->data.yuv420sp.length_uv;
						break;
					case 3:
						frame.data.triple_plane.y = buffer_bo_handle[0].ptr;
						if (stream->num_planes == (unsigned int)num_buffer_key) {
							frame.data.triple_plane.u = buffer_bo_handle[1].ptr;
							frame.data.triple_plane.v = buffer_bo_handle[2].ptr;
						} else {
							frame.data.triple_plane.u = buffer_bo_handle[0].ptr + stream->data.yuv420p.length_y;
							frame.data.triple_plane.v = buffer_bo_handle[1].ptr + stream->data.yuv420p.length_u;
						}
						frame.data.triple_plane.y_size = stream->data.yuv420p.length_y;
						frame.data.triple_plane.u_size = stream->data.yuv420p.length_u;
						frame.data.triple_plane.v_size = stream->data.yuv420p.length_v;
						total_size = stream->data.yuv420p.length_y + \
							stream->data.yuv420p.length_u + \
							stream->data.yuv420p.length_v;
						break;
					default:
						break;
					}
				}

				/*
				LOGD("PREVIEW_CB - format %d, %dx%d, size %d plane num %d",
				     frame.format, frame.width, frame.height, total_size, frame.num_of_planes);
				*/

				((camera_preview_cb)cb_info->user_cb[event])(&frame, cb_info->user_data[event]);

				/*LOGD("PREVIEW_CB retuned");*/
			}

			if (cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW]) {
				media_packet_h pkt = NULL;
				tbm_surface_h tsurf = NULL;
				uint32_t bo_format = 0;
				int ret = 0;
				media_format_mimetype_e mimetype = MEDIA_FORMAT_NV12;
				bool make_pkt_fmt = false;
				tbm_surface_info_s tsurf_info;

				memset(&tsurf_info, 0x0, sizeof(tbm_surface_info_s));

				/* unmap buffer bo */
				for (i = 0 ; i < num_buffer_key ; i++) {
					if (buffer_bo[i])
						tbm_bo_unmap(buffer_bo[i]);
				}

				/* create tbm surface */
				for (i = 0 ; i < BUFFER_MAX_PLANE_NUM ; i++)
					tsurf_info.planes[i].stride = stream->stride[i];

				/* get tbm surface format */
				ret = _camera_get_tbm_surface_format(stream->format, &bo_format);
				ret |= _camera_get_media_packet_mimetype(stream->format, &mimetype);

				if (num_buffer_key > 0 && ret == CAMERA_ERROR_NONE) {
					tsurf_info.width = stream->width;
					tsurf_info.height = stream->height;
					tsurf_info.format = bo_format;
					tsurf_info.bpp = tbm_surface_internal_get_bpp(bo_format);
					tsurf_info.num_planes = tbm_surface_internal_get_num_planes(bo_format);

					switch (bo_format) {
					case TBM_FORMAT_NV12:
					case TBM_FORMAT_NV21:
						tsurf_info.planes[0].size = stream->stride[0] * stream->elevation[0];
						tsurf_info.planes[1].size = stream->stride[1] * stream->elevation[1];
						tsurf_info.planes[0].offset = 0;
						if (num_buffer_key == 1)
							tsurf_info.planes[1].offset = tsurf_info.planes[0].size;
						tsurf_info.size = tsurf_info.planes[0].size + tsurf_info.planes[1].size;
						break;
					case TBM_FORMAT_YUV420:
					case TBM_FORMAT_YVU420:
						tsurf_info.planes[0].size = stream->stride[0] * stream->elevation[0];
						tsurf_info.planes[1].size = stream->stride[1] * stream->elevation[1];
						tsurf_info.planes[2].size = stream->stride[2] * stream->elevation[2];
						tsurf_info.planes[0].offset = 0;
						if (num_buffer_key == 1) {
							tsurf_info.planes[1].offset = tsurf_info.planes[0].size;
							tsurf_info.planes[2].offset = tsurf_info.planes[0].size + tsurf_info.planes[1].size;
						}
						tsurf_info.size = tsurf_info.planes[0].size + tsurf_info.planes[1].size + tsurf_info.planes[2].size;
						break;
					case TBM_FORMAT_UYVY:
					case TBM_FORMAT_YUYV:
						tsurf_info.planes[0].size = (stream->stride[0] * stream->elevation[0]) << 1;
						tsurf_info.planes[0].offset = 0;
						tsurf_info.size = tsurf_info.planes[0].size;
						break;
					default:
						break;
					}

					tsurf = tbm_surface_internal_create_with_bos(&tsurf_info, buffer_bo, num_buffer_key);
					/*LOGD("tbm surface %p", tsurf);*/
				}

				if (tsurf) {
					/* check media packet format */
					if (cb_info->pkt_fmt) {
						int pkt_fmt_width = 0;
						int pkt_fmt_height = 0;
						media_format_mimetype_e pkt_fmt_mimetype = MEDIA_FORMAT_NV12;

						media_format_get_video_info(cb_info->pkt_fmt, &pkt_fmt_mimetype, &pkt_fmt_width, &pkt_fmt_height, NULL, NULL);
						if (pkt_fmt_mimetype != mimetype ||
						    pkt_fmt_width != stream->width ||
						    pkt_fmt_height != stream->height) {
							LOGW("different format. current 0x%x, %dx%d, new 0x%x, %dx%d",
							     pkt_fmt_mimetype, pkt_fmt_width, pkt_fmt_height, mimetype, stream->width, stream->height);
							media_format_unref(cb_info->pkt_fmt);
							cb_info->pkt_fmt = NULL;
							make_pkt_fmt = true;
						}
					} else {
						make_pkt_fmt = true;
					}

					/* create packet format */
					if (make_pkt_fmt) {
						LOGW("make new pkt_fmt - mimetype 0x%x, %dx%d", mimetype, stream->width, stream->height);
						ret = media_format_create(&cb_info->pkt_fmt);
						if (ret == MEDIA_FORMAT_ERROR_NONE) {
							ret = media_format_set_video_mime(cb_info->pkt_fmt, mimetype);
							ret |= media_format_set_video_width(cb_info->pkt_fmt, stream->width);
							ret |= media_format_set_video_height(cb_info->pkt_fmt, stream->height);
							LOGW("media_format_set_video_mime,width,height ret : 0x%x", ret);
						} else {
							LOGW("media_format_create failed");
						}
					}

					/* create media packet */
					ret = media_packet_create_from_tbm_surface(cb_info->pkt_fmt,
						tsurf, (media_packet_finalize_cb)_camera_media_packet_finalize,
						(void *)cb_info, &pkt);
					if (ret != MEDIA_PACKET_ERROR_NONE) {
						LOGE("media_packet_create_from_tbm_surface failed");

						tbm_surface_destroy(tsurf);
						tsurf = NULL;
					}
				} else {
					LOGE("failed to create tbm surface %dx%d, format %d, num_buffer_key %d",
					     stream->width, stream->height, stream->format, num_buffer_key);
				}

				if (pkt) {
					/*LOGD("media packet %p, internal buffer %p", pkt, stream->internal_buffer);*/

					mp_data = g_new0(camera_media_packet_data, 1);
					if (mp_data) {
						mp_data->tbm_key = tbm_key;
						mp_data->num_buffer_key = num_buffer_key;
						mp_data->bo = bo;
						for (i = 0 ; i < num_buffer_key ; i++)
							mp_data->buffer_bo[i] = buffer_bo[i];

						/* set media packet data */
						ret = media_packet_set_extra(pkt, (void *)mp_data);
						if (ret != MEDIA_PACKET_ERROR_NONE) {
							LOGE("media_packet_set_extra failed");

							if (mp_data) {
								g_free(mp_data);
								mp_data = NULL;
							}

							media_packet_destroy(pkt);
							pkt = NULL;
						} else {
							int e_type = MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW;

							/* set timestamp : msec -> nsec */
							if (media_packet_set_pts(pkt, (uint64_t)(stream->timestamp) * 1000000) != MEDIA_PACKET_ERROR_NONE)
								LOGW("media_packet_set_pts failed");

							/* call media packet callback */
							((camera_media_packet_preview_cb)cb_info->user_cb[e_type])(pkt, cb_info->user_data[e_type]);
						}
					} else {
						LOGE("failed to alloc media packet data");
					}
				}
			}

			/* send message for preview callback return */
			muse_camera_msg_send_no_return(MUSE_CAMERA_API_PREVIEW_CB_RETURN, cb_info->fd, cb_info);

			if (mp_data == NULL) {
				/* release imported bo */
				for (i = 0 ; i < num_buffer_key ; i++)
					_release_imported_bo(&buffer_bo[i]);

				/* unmap and unref tbm bo */
				_release_imported_bo(&bo);

				/* return buffer */
				muse_camera_msg_send1_no_return(MUSE_CAMERA_API_RETURN_BUFFER,
					cb_info->fd, cb_info, INT, tbm_key);

				/*LOGD("return buffer Done");*/
			}
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_HDR_PROGRESS:
		{
			int percent = 0;

			muse_camera_msg_get(percent, recv_msg);

			LOGD("HDR progress - %d \%", percent);

			((camera_attr_hdr_progress_cb)cb_info->user_cb[event])(percent, cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_INTERRUPTED:
		{
			int policy = 0;
			int previous = 0;
			int current = 0;

			muse_camera_msg_get(policy, recv_msg);
			muse_camera_msg_get(previous, recv_msg);
			muse_camera_msg_get(current, recv_msg);

			LOGD("INTERRUPTED - policy %d, state previous %d, current %d",
			     policy, previous, current);

			((camera_interrupted_cb)cb_info->user_cb[event])((camera_policy_e)policy,
				(camera_state_e)previous, (camera_state_e)current, cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FACE_DETECTION:
		{
			int count = 0;
			camera_detected_face_s *faces = NULL;

			muse_camera_msg_get(count, recv_msg);
			muse_camera_msg_get(tbm_key, recv_msg);

			if (count > 0 && tbm_key > 0) {
				LOGD("FACE_DETECTION - count %d, tbm_key %d", count, tbm_key);

				if (!_import_tbm_key(cb_info->bufmgr, tbm_key, &bo, &bo_handle))
					break;

				/* set face info */
				faces = bo_handle.ptr;

				((camera_face_detected_cb)cb_info->user_cb[event])(faces, count, cb_info->user_data[event]);

#if 0
				{
					int i = 0;

					for (i = 0 ; i < count ; i++) {
						LOGD("id[%2d] - score %d, position (%d,%d,%dx%d)",
						     i, faces[i].score, faces[i].x, faces[i].y, faces[i].width, faces[i].height);
					}
				}
#endif

				/* release bo */
				_release_imported_bo(&bo);

				/* return buffer */
				muse_camera_msg_send1_no_return(MUSE_CAMERA_API_RETURN_BUFFER,
					cb_info->fd, cb_info, INT, tbm_key);

				LOGD("return buffer done");
			} else {
				LOGE("invalid message - count %d, key %d", count, tbm_key);
			}
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_ERROR:
		{
			int error = 0;
			int current_state = 0;

			muse_camera_msg_get(error, recv_msg);
			muse_camera_msg_get(current_state, recv_msg);

			LOGE("ERROR - error 0x%x, current_state %d", error, current_state);

			((camera_error_cb)cb_info->user_cb[event])((camera_error_e)error,
				(camera_state_e)current_state, cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_RESOLUTION:
		muse_camera_msg_get(param1, recv_msg);
		muse_camera_msg_get(param2, recv_msg);

		LOGD("SUPPORTED_PREVIEW_RESOLUTION - %d x %d", param1, param2);

		if (((camera_supported_preview_resolution_cb)cb_info->user_cb[event])(param1, param2, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_PREVIEW_RESOLUTION");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_RESOLUTION:
		muse_camera_msg_get(param1, recv_msg);
		muse_camera_msg_get(param2, recv_msg);

		LOGD("SUPPORTED_CAPTURE_RESOLUTION - %d x %d", param1, param2);

		if (((camera_supported_capture_resolution_cb)cb_info->user_cb[event])(param1, param2, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_CAPTURE_RESOLUTION");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_FORMAT:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_CAPTURE_FORMAT - %d ", param1);

		if (((camera_supported_capture_format_cb)cb_info->user_cb[event])((camera_pixel_format_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_CAPTURE_FORMAT");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_FORMAT:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_PREVIEW_FORMAT - %d ", param1);

		if (((camera_supported_preview_format_cb)cb_info->user_cb[event])((camera_pixel_format_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_PREVIEW_FORMAT");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_AF_MODE:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_AF_MODE - %d ", param1);

		if (((camera_attr_supported_af_mode_cb)cb_info->user_cb[event])((camera_attr_af_mode_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_AF_MODE");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_EXPOSURE_MODE - %d ", param1);

		if (((camera_attr_supported_exposure_mode_cb)cb_info->user_cb[event])((camera_attr_exposure_mode_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_EXPOSURE_MODE");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_ISO:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_ISO - %d ", param1);

		if (((camera_attr_supported_iso_cb)cb_info->user_cb[event])((camera_attr_iso_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_ISO");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_WHITEBALANCE:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_WHITEBALANCE - %d ", param1);

		if (((camera_attr_supported_whitebalance_cb)cb_info->user_cb[event])((camera_attr_whitebalance_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_WHITEBALANCE");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EFFECT:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_EFFECT - %d ", param1);

		if (((camera_attr_supported_effect_cb)cb_info->user_cb[event])((camera_attr_effect_mode_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_EFFECT");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_SCENE_MODE:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_SCENE_MODE - %d ", param1);

		if (((camera_attr_supported_scene_mode_cb)cb_info->user_cb[event])((camera_attr_scene_mode_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_SCENE_MODE");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FLASH_MODE:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_FLASH_MODE - %d ", param1);

		if (((camera_attr_supported_flash_mode_cb)cb_info->user_cb[event])((camera_attr_flash_mode_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_FLASH_MODE");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_FPS - %d ", param1);

		if (((camera_attr_supported_fps_cb)cb_info->user_cb[event])((camera_attr_fps_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_FPS");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS_BY_RESOLUTION:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_FPS_BY_RESOLUTION - %d ", param1);

		if (((camera_attr_supported_fps_cb)cb_info->user_cb[event])((camera_attr_fps_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_FPS_BY_RESOLUTION");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_FLIP:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_STREAM_FLIP - %d ", param1);

		if (((camera_attr_supported_stream_flip_cb)cb_info->user_cb[event])((camera_flip_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_STREAM_FLIP");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_ROTATION:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_STREAM_ROTATION - %d ", param1);

		if (((camera_attr_supported_stream_rotation_cb)cb_info->user_cb[event])((camera_rotation_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_STREAM_ROTATION");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_THEATER_MODE:
		muse_camera_msg_get(param1, recv_msg);

		LOGD("SUPPORTED_THEATER_MODE - %d ", param1);

		if (((camera_attr_supported_theater_mode_cb)cb_info->user_cb[event])((camera_attr_theater_mode_e)param1, cb_info->user_data[event]) == false) {
			cb_info->user_cb[event] = NULL;
			cb_info->user_data[event] = NULL;
			LOGD("stop foreach callback for SUPPORTED_THEATER_MODE");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_CAPTURE:
		{
			camera_image_data_s *rImage = NULL;
			camera_image_data_s *rPostview = NULL;
			camera_image_data_s *rThumbnail = NULL;
			unsigned char *buf_pos = NULL;
			int is_postview = 0;
			int is_thumbnail = 0;

			muse_camera_msg_get(tbm_key, recv_msg);
			muse_camera_msg_get(is_postview, recv_msg);
			muse_camera_msg_get(is_thumbnail, recv_msg);

			LOGD("camera capture callback came in. key %d, postview %d, thumbnail %d",
			     tbm_key, is_postview, is_thumbnail);

			if (tbm_key <= 0) {
				LOGE("invalid key %d", tbm_key);
				break;
			}

			/* import tbm bo and get virtual address */
			if (!_import_tbm_key(cb_info->bufmgr, tbm_key, &bo, &bo_handle))
				break;

			buf_pos = (unsigned char *)bo_handle.ptr;
			rImage = (camera_image_data_s *)buf_pos;
			rImage->data = buf_pos + sizeof(camera_image_data_s);
			buf_pos += sizeof(camera_image_data_s) + rImage->size;

			if (is_postview) {
				rPostview = (camera_image_data_s *)buf_pos;
				LOGD("rPostview->size : %d", rPostview->size);
				rPostview->data = buf_pos + sizeof(camera_image_data_s);
				buf_pos += sizeof(camera_image_data_s) + rPostview->size;
			}

			if (is_thumbnail) {
				rThumbnail = (camera_image_data_s *)buf_pos;
				LOGD("rThumbnail->size : %d", rThumbnail->size);
				rThumbnail->data = buf_pos + sizeof(camera_image_data_s);
				buf_pos += sizeof(camera_image_data_s) + rThumbnail->size;
			}

			LOGD("image info %dx%d, size : %d", rImage->height, rImage->width, rImage->size);

			((camera_capturing_cb)cb_info->user_cb[event])(rImage, rPostview, rThumbnail, cb_info->user_data[event]);

			/* unmap and unref tbm bo */
			_release_imported_bo(&bo);

			/* return buffer */
			muse_camera_msg_send1_no_return(MUSE_CAMERA_API_RETURN_BUFFER,
				cb_info->fd, cb_info, INT, tbm_key);

			LOGD("return buffer done");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_VIDEO_FRAME_RENDER_ERROR:
		break;
	default:
		LOGW("Unknown event : %d", event);
		break;
	}

	return;
}

static bool _camera_idle_event_callback(void *data)
{
	camera_cb_info_s *cb_info = NULL;
	camera_idle_event_s *cam_idle_event = (camera_idle_event_s *)data;

	if (cam_idle_event == NULL) {
		LOGE("cam_idle_event is NULL");
		return false;
	}

	/* lock event */
	g_mutex_lock(&cam_idle_event->event_mutex);

	cb_info = cam_idle_event->cb_info;
	if (cb_info == NULL) {
		LOGW("camera cb_info is NULL. event %d", cam_idle_event->event);
		goto IDLE_EVENT_CALLBACK_DONE;
	}

	/* remove event from list */
	g_mutex_lock(&cb_info->idle_event_mutex);
	if (cb_info->idle_event_list)
		cb_info->idle_event_list = g_list_remove(cb_info->idle_event_list, (gpointer)cam_idle_event);

	/*LOGD("remove camera idle event %p, %p", cam_idle_event, cb_info->idle_event_list);*/
	g_mutex_unlock(&cb_info->idle_event_mutex);

	/* user callback */
	_client_user_callback(cam_idle_event->cb_info, cam_idle_event->recv_msg, cam_idle_event->event);

	/* send signal for waiting thread */
	g_cond_signal(&cb_info->idle_event_cond);

IDLE_EVENT_CALLBACK_DONE:
	/* unlock and release event */
	g_mutex_unlock(&cam_idle_event->event_mutex);
	g_mutex_clear(&cam_idle_event->event_mutex);

	free(cam_idle_event);
	cam_idle_event = NULL;

	return false;
}

static void *_camera_msg_handler_func(gpointer data)
{
	int ret = 0;
	int api = 0;
	int event = 0;
	int event_class = 0;
	camera_message_s *cam_msg = NULL;
	camera_idle_event_s *cam_idle_event = NULL;
	camera_cb_info_s *cb_info = (camera_cb_info_s *)data;

	if (cb_info == NULL) {
		LOGE("cb_info NULL");
		return NULL;
	}

	LOGD("start");

	g_mutex_lock(&cb_info->msg_handler_mutex);

	while (g_atomic_int_get(&cb_info->msg_handler_running)) {
		if (g_queue_is_empty(cb_info->msg_queue)) {
			LOGD("signal wait...");
			g_cond_wait(&cb_info->msg_handler_cond, &cb_info->msg_handler_mutex);
			LOGD("signal received");

			if (g_atomic_int_get(&cb_info->msg_handler_running) == 0) {
				LOGD("stop event thread");
				break;
			}
		}

		cam_msg = (camera_message_s *)g_queue_pop_head(cb_info->msg_queue);

		g_mutex_unlock(&cb_info->msg_handler_mutex);

		if (cam_msg == NULL) {
			LOGE("NULL message");
			g_mutex_lock(&cb_info->msg_handler_mutex);
			continue;
		}

		api = cam_msg->api;

		if (api < MUSE_CAMERA_API_MAX) {
			g_mutex_lock(&cb_info->api_mutex[api]);

			if (muse_camera_msg_get(ret, cam_msg->recv_msg)) {
				cb_info->api_ret[api] = ret;
				cb_info->api_activating[api] = 1;

				LOGD("camera api %d - return 0x%x", ret);

				g_cond_signal(&cb_info->api_cond[api]);
			} else {
				LOGE("failed to get camera ret for api %d, msg %s", api, cam_msg->recv_msg);
			}

			g_mutex_unlock(&cb_info->api_mutex[api]);
		} else if (api == MUSE_CAMERA_CB_EVENT) {
			event = -1;
			event_class = -1;

			if (!muse_camera_msg_get(event, cam_msg->recv_msg) ||
			    !muse_camera_msg_get(event_class, cam_msg->recv_msg)) {
				LOGE("failed to get camera event %d, class %d", event, event_class);

				g_free(cam_msg);
				cam_msg = NULL;

				g_mutex_lock(&cb_info->msg_handler_mutex);
				continue;
			}

			switch (event_class) {
			case MUSE_CAMERA_EVENT_CLASS_THREAD_SUB:
				_client_user_callback(cb_info, cam_msg->recv_msg, event);
				break;
			case MUSE_CAMERA_EVENT_CLASS_THREAD_MAIN:
				cam_idle_event = (camera_idle_event_s *)malloc(sizeof(camera_idle_event_s));
				if (cam_idle_event == NULL) {
					LOGE("cam_idle_event alloc failed");
					break;
				}

				cam_idle_event->event = event;
				cam_idle_event->cb_info = cb_info;
				g_mutex_init(&cam_idle_event->event_mutex);
				memcpy(cam_idle_event->recv_msg, cam_msg->recv_msg, sizeof(cam_idle_event->recv_msg));

				LOGD("add camera event[%d, %p] to IDLE", event, cam_idle_event);

				g_mutex_lock(&cb_info->idle_event_mutex);
				cb_info->idle_event_list = g_list_append(cb_info->idle_event_list, (gpointer)cam_idle_event);
				g_mutex_unlock(&cb_info->idle_event_mutex);

				g_idle_add_full(G_PRIORITY_DEFAULT,
					(GSourceFunc)_camera_idle_event_callback,
					(gpointer)cam_idle_event,
					NULL);
				break;
			default:
				LOGE("unknown camera event class %d", event_class);
				break;
			}
		} else {
			LOGE("unknown camera api[%d] message[%s]", api, cam_msg->recv_msg);
		}

		free(cam_msg);
		cam_msg = NULL;

		g_mutex_lock(&cb_info->msg_handler_mutex);
	}

	/* remove remained event */
	while (!g_queue_is_empty(cb_info->msg_queue)) {
		cam_msg = (camera_message_s *)g_queue_pop_head(cb_info->msg_queue);
		if (cam_msg) {
			LOGD("remove camera message %p", cam_msg);
			free(cam_msg);
			cam_msg = NULL;
		} else {
			LOGW("NULL camera message");
		}
	}

	g_mutex_unlock(&cb_info->msg_handler_mutex);

	LOGD("return");

	return NULL;
}

static void _camera_remove_idle_event_all(camera_cb_info_s *cb_info)
{
	camera_idle_event_s *cam_idle_event = NULL;
	gboolean ret = TRUE;
	GList *list = NULL;
	gint64 end_time = 0;

	if (cb_info == NULL) {
		LOGE("cb_info is NULL");
		return;
	}

	g_mutex_lock(&cb_info->idle_event_mutex);

	if (cb_info->idle_event_list == NULL) {
		LOGD("No idle event is remained.");
	} else {
		list = cb_info->idle_event_list;

		while (list) {
			cam_idle_event = list->data;
			list = g_list_next(list);

			if (!cam_idle_event) {
				LOGW("Fail to remove idle event. The event is NULL");
			} else {
				if (g_mutex_trylock(&cam_idle_event->event_mutex)) {
					ret = g_idle_remove_by_data(cam_idle_event);

					LOGD("remove idle event [%p], ret[%d]", cam_idle_event, ret);

					if (ret == FALSE) {
						cam_idle_event->cb_info = NULL;
						LOGW("idle callback for event %p will be called later", cam_idle_event);
					}

					cb_info->idle_event_list = g_list_remove(cb_info->idle_event_list, (gpointer)cam_idle_event);

					g_mutex_unlock(&cam_idle_event->event_mutex);

					if (ret == TRUE) {
						g_mutex_clear(&cam_idle_event->event_mutex);

						free(cam_idle_event);
						cam_idle_event = NULL;

						LOGD("remove idle event done");
					}
				} else {
					LOGW("event lock failed. it's being called...");

					end_time = g_get_monotonic_time() + G_TIME_SPAN_MILLISECOND * 100;

					if (g_cond_wait_until(&cb_info->idle_event_cond, &cb_info->idle_event_mutex, end_time))
						LOGW("signal received");
					else
						LOGW("timeout");
				}
			}
		}

		g_list_free(cb_info->idle_event_list);
		cb_info->idle_event_list = NULL;
	}

	g_mutex_unlock(&cb_info->idle_event_mutex);

	return;
}

static void *_camera_msg_recv_func(gpointer data)
{
	int i = 0;
	int ret = 0;
	int api = 0;
	int api_class = 0;
	int num_token = 0;
	int str_pos = 0;
	int prev_pos = 0;
	char *recv_msg = NULL;
	char **parse_str = NULL;
	camera_cb_info_s *cb_info = (camera_cb_info_s *)data;

	if (cb_info == NULL) {
		LOGE("cb_info NULL");
		return NULL;
	}

	LOGD("start");

	parse_str = (char **)malloc(sizeof(char *) * CAMERA_PARSE_STRING_SIZE);
	if (parse_str == NULL) {
		LOGE("parse_str malloc failed");
		return NULL;
	}

	for (i = 0 ; i < CAMERA_PARSE_STRING_SIZE ; i++) {
		parse_str[i] = (char *)malloc(sizeof(char) * MUSE_CAMERA_MSG_MAX_LENGTH);
		if (parse_str[i] == NULL) {
			LOGE("parse_str[%d] malloc failed", i);
			goto CB_HANDLER_EXIT;
		}
	}

	recv_msg = cb_info->recv_msg;

	while (g_atomic_int_get(&cb_info->msg_recv_running)) {
		ret = muse_core_ipc_recv_msg(cb_info->fd, recv_msg);
		if (ret <= 0)
			break;
		recv_msg[ret] = '\0';

		str_pos = 0;
		prev_pos = 0;
		num_token = 0;

		/*LOGD("recvMSg : %s, length : %d", recv_msg, ret);*/

		/* Need to split the combined entering msgs.
		    This module supports up to 200 combined msgs. */
		for (str_pos = 0; str_pos < ret; str_pos++) {
			if (recv_msg[str_pos] == '}') {
				memset(parse_str[num_token], 0x0, sizeof(char) * MUSE_CAMERA_MSG_MAX_LENGTH);
				strncpy(parse_str[num_token], recv_msg + prev_pos, str_pos - prev_pos + 1);
				LOGD("splitted msg : [%s], Index : %d", parse_str[num_token], num_token);
				prev_pos = str_pos+1;
				num_token++;
			}
		}

		/*LOGD("num_token : %d", num_token);*/

		/* Re-construct to the useful single msg. */
		for (i = 0; i < num_token; i++) {
			if (i >= CAMERA_PARSE_STRING_SIZE) {
				LOGE("invalid token index %d", i);
				break;
			}

			api = -1;
			api_class = -1;

			if (!muse_camera_msg_get(api, parse_str[i])) {
				LOGE("failed to get camera api");
				continue;
			}

			if (muse_camera_msg_get(api_class, parse_str[i]))
				LOGD("camera api_class[%d]", api_class);

			if (api_class == MUSE_CAMERA_API_CLASS_IMMEDIATE) {
				g_mutex_lock(&cb_info->api_mutex[api]);

				if (!muse_camera_msg_get(ret, parse_str[i])) {
					LOGE("failed to get camera ret");
					g_mutex_unlock(&cb_info->api_mutex[api]);
					continue;
				}

				cb_info->api_ret[api] = ret;
				cb_info->api_activating[api] = 1;

				if (api == MUSE_CAMERA_API_CREATE) {
					if (ret != CAMERA_ERROR_NONE) {
						g_atomic_int_set(&cb_info->msg_recv_running, 0);
						LOGE("camera create error 0x%x. close client cb handler", ret);
					}
				} else if (api == MUSE_CAMERA_API_DESTROY) {
					if (ret == CAMERA_ERROR_NONE) {
						g_atomic_int_set(&cb_info->msg_recv_running, 0);
						LOGD("camera destroy done. close client cb handler");
					}
				}

				g_cond_signal(&cb_info->api_cond[api]);
				g_mutex_unlock(&cb_info->api_mutex[api]);
			} else if (api_class == MUSE_CAMERA_API_CLASS_THREAD_SUB || api == MUSE_CAMERA_CB_EVENT) {
				camera_message_s *cam_msg = g_new0(camera_message_s, 1);
				if (cam_msg == NULL) {
					LOGE("failed to alloc cam_msg");
					continue;
				}

				cam_msg->api = api;
				memcpy(cam_msg->recv_msg, parse_str[i], sizeof(cam_msg->recv_msg));

				LOGD("add camera message to queue : api %d", api);

				g_mutex_lock(&cb_info->msg_handler_mutex);
				g_queue_push_tail(cb_info->msg_queue, (gpointer)cam_msg);
				g_cond_signal(&cb_info->msg_handler_cond);
				g_mutex_unlock(&cb_info->msg_handler_mutex);
			} else {
				LOGW("unknown camera api %d and api_class %d", api, api_class);
			}
		}

	}

	LOGD("client cb exit");

CB_HANDLER_EXIT:
	if (parse_str) {
		for (i = 0 ; i < CAMERA_PARSE_STRING_SIZE ; i++) {
			if (parse_str[i]) {
				free(parse_str[i]);
				parse_str[i] = NULL;
			}
		}

		free(parse_str);
		parse_str = NULL;
	}

	return NULL;
}

static camera_cb_info_s *_client_callback_new(gint sockfd)
{
	camera_cb_info_s *cb_info = NULL;
	gint *tmp_activating = NULL;
	gint *tmp_ret = NULL;
	gint i = 0;

	g_return_val_if_fail(sockfd > 0, NULL);

	cb_info = g_new0(camera_cb_info_s, 1);
	if (cb_info == NULL) {
		LOGE("cb_info failed");
		goto ErrorExit;
	}

	g_mutex_init(&cb_info->msg_handler_mutex);
	g_cond_init(&cb_info->msg_handler_cond);
	g_mutex_init(&cb_info->idle_event_mutex);
	g_cond_init(&cb_info->idle_event_cond);

	for (i = 0 ; i < MUSE_CAMERA_API_MAX ; i++) {
		g_mutex_init(&cb_info->api_mutex[i]);
		g_cond_init(&cb_info->api_cond[i]);
	}

	tmp_activating = g_new0(gint, MUSE_CAMERA_API_MAX);
	if (tmp_activating == NULL) {
		LOGE("tmp_activating failed");
		goto ErrorExit;
	}

	tmp_ret = g_new0(gint, MUSE_CAMERA_API_MAX);
	if (tmp_ret == NULL) {
		LOGE("tmp_ret failed");
		goto ErrorExit;
	}

	cb_info->msg_queue = g_queue_new();
	if (cb_info->msg_queue == NULL) {
		LOGE("msg_queue new failed");
		goto ErrorExit;
	}

	g_atomic_int_set(&cb_info->msg_handler_running, 1);
	cb_info->msg_handler_thread = g_thread_try_new("camera_msg_handler",
		_camera_msg_handler_func, (gpointer)cb_info, NULL);
	if (cb_info->msg_handler_thread == NULL) {
		LOGE("message handler thread creation failed");
		goto ErrorExit;
	}

	cb_info->fd = sockfd;
	cb_info->api_activating = tmp_activating;
	cb_info->api_ret = tmp_ret;

	g_atomic_int_set(&cb_info->msg_recv_running, 1);
	cb_info->msg_recv_thread = g_thread_try_new("camera_msg_recv",
		_camera_msg_recv_func, (gpointer)cb_info, NULL);
	if (cb_info->msg_recv_thread == NULL) {
		LOGE("message receive thread creation failed");
		goto ErrorExit;
	}

	return cb_info;

ErrorExit:
	if (cb_info) {
		if (cb_info->msg_handler_thread) {
			g_mutex_lock(&cb_info->msg_handler_mutex);
			g_atomic_int_set(&cb_info->msg_handler_running, 0);
			g_cond_signal(&cb_info->msg_handler_cond);
			g_mutex_unlock(&cb_info->msg_handler_mutex);

			g_thread_join(cb_info->msg_handler_thread);
			g_thread_unref(cb_info->msg_handler_thread);
			cb_info->msg_handler_thread = NULL;
		}

		for (i = 0 ; i < MUSE_CAMERA_API_MAX ; i++) {
			g_mutex_clear(&cb_info->api_mutex[i]);
			g_cond_clear(&cb_info->api_cond[i]);
		}

		g_mutex_clear(&cb_info->msg_handler_mutex);
		g_cond_clear(&cb_info->msg_handler_cond);
		g_mutex_clear(&cb_info->idle_event_mutex);
		g_cond_clear(&cb_info->idle_event_cond);

		if (cb_info->msg_queue) {
			g_queue_free(cb_info->msg_queue);
			cb_info->msg_queue = NULL;
		}

		g_free(cb_info);
		cb_info = NULL;
	}

	if (tmp_activating) {
		g_free(tmp_activating);
		tmp_activating = NULL;
	}
	if (tmp_ret) {
		g_free(tmp_ret);
		tmp_ret = NULL;
	}

	return NULL;
}

static void _client_callback_destroy(camera_cb_info_s *cb_info)
{
	gint i = 0;

	g_return_if_fail(cb_info != NULL);

	LOGD("MSG receive thread[%p] destroy", cb_info->msg_recv_thread);

	g_thread_join(cb_info->msg_recv_thread);
	g_thread_unref(cb_info->msg_recv_thread);
	cb_info->msg_recv_thread = NULL;

	LOGD("msg thread removed");

	g_mutex_lock(&cb_info->msg_handler_mutex);
	g_atomic_int_set(&cb_info->msg_handler_running, 0);
	g_cond_signal(&cb_info->msg_handler_cond);
	g_mutex_unlock(&cb_info->msg_handler_mutex);

	g_thread_join(cb_info->msg_handler_thread);
	g_thread_unref(cb_info->msg_handler_thread);
	cb_info->msg_handler_thread = NULL;

	g_queue_free(cb_info->msg_queue);
	cb_info->msg_queue = NULL;

	for (i = 0 ; i < MUSE_CAMERA_API_MAX ; i++) {
		g_mutex_clear(&cb_info->api_mutex[i]);
		g_cond_clear(&cb_info->api_cond[i]);
	}

	g_mutex_clear(&cb_info->msg_handler_mutex);
	g_cond_clear(&cb_info->msg_handler_cond);
	g_mutex_clear(&cb_info->idle_event_mutex);
	g_cond_clear(&cb_info->idle_event_cond);

	LOGD("event thread removed");

	if (cb_info->fd > -1) {
		muse_core_connection_close(cb_info->fd);
		cb_info->fd = -1;
	}

	if (cb_info->bufmgr) {
		tbm_bufmgr_deinit(cb_info->bufmgr);
		cb_info->bufmgr = NULL;
	}
	if (cb_info->api_activating) {
		g_free(cb_info->api_activating);
		cb_info->api_activating = NULL;
	}
	if (cb_info->api_ret) {
		g_free(cb_info->api_ret);
		cb_info->api_ret = NULL;
	}
	if (cb_info->pkt_fmt) {
		media_format_unref(cb_info->pkt_fmt);
		cb_info->pkt_fmt = NULL;
	}

	g_free(cb_info);
	cb_info = NULL;

	return;
}

int camera_create(camera_device_e device, camera_h *camera)
{
	int sock_fd = -1;
	char *sndMsg;
	int ret = CAMERA_ERROR_NONE;
	int pid = 0;
	camera_cli_s *pc = NULL;
	tbm_bufmgr bufmgr = NULL;

	muse_camera_api_e api = MUSE_CAMERA_API_CREATE;
	muse_core_api_module_e muse_module = MUSE_CAMERA;
	int device_type = (int)device;

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	bufmgr = tbm_bufmgr_init(-1);
	if (bufmgr == NULL) {
		LOGE("get tbm bufmgr failed");
		return CAMERA_ERROR_INVALID_OPERATION;
	}

	sock_fd = muse_core_client_new();
	if (sock_fd < 0) {
		LOGE("muse_core_client_new failed - returned fd %d", sock_fd);
		ret = CAMERA_ERROR_INVALID_OPERATION;
		goto ErrorExit;
	}

	pid = getpid();

	sndMsg = muse_core_msg_json_factory_new(api,
		MUSE_TYPE_INT, "module", muse_module,
		MUSE_TYPE_INT, PARAM_DEVICE_TYPE, device_type,
		MUSE_TYPE_INT, "pid", pid,
		0);

	muse_core_ipc_send_msg(sock_fd, sndMsg);
	muse_core_msg_json_factory_free(sndMsg);

	pc = g_new0(camera_cli_s, 1);
	if (pc == NULL) {
		LOGE("camera_cli_s alloc failed");
		ret = CAMERA_ERROR_OUT_OF_MEMORY;
		goto ErrorExit;
	}

	pc->cb_info = _client_callback_new(sock_fd);
	if (pc->cb_info == NULL) {
		LOGE("cb_info alloc failed");
		ret = CAMERA_ERROR_OUT_OF_MEMORY;
		goto ErrorExit;
	}

	LOGD("cb info : %d", pc->cb_info->fd);

	ret = _client_wait_for_cb_return(api, pc->cb_info, CALLBACK_TIME_OUT);
	if (ret == CAMERA_ERROR_NONE) {
		intptr_t handle = 0;
		muse_camera_msg_get_pointer(handle, pc->cb_info->recv_msg);
		if (handle == 0) {
			LOGE("Receiving Handle Failed!!");
			ret = CAMERA_ERROR_INVALID_OPERATION;
			goto ErrorExit;
		}

		pc->remote_handle = handle;
		pc->cb_info->bufmgr = bufmgr;

		ret = camera_set_display((camera_h)pc, CAMERA_DISPLAY_TYPE_NONE, NULL);
		if (ret != CAMERA_ERROR_NONE) {
			LOGE("init display failed 0x%x", ret);
			goto ErrorExit;
		}

		LOGD("camera create 0x%x", pc->remote_handle);
		*camera = (camera_h)pc;
	} else {
		goto ErrorExit;
	}

	return ret;

ErrorExit:
	tbm_bufmgr_deinit(bufmgr);
	bufmgr = NULL;

	if (pc) {
		_client_callback_destroy(pc->cb_info);
		pc->cb_info = NULL;
		g_free(pc);
		pc = NULL;
	}

	LOGE("camera create error : 0x%x", ret);

	return ret;
}

int camera_destroy(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	muse_camera_api_e api = MUSE_CAMERA_API_DESTROY;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd = 0;

	LOGD("ENTER");

	if (pc->cb_info == NULL) {
		LOGE("cb_info NULL, INVALID_PARAMETER");
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	sock_fd = pc->cb_info->fd;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	if (ret == CAMERA_ERROR_NONE) {
		_camera_remove_idle_event_all(pc->cb_info);
		_client_callback_destroy(pc->cb_info);
		pc->cb_info = NULL;

		g_free(pc);
		pc = NULL;
	} else {
		LOGE("camera destroy error : 0x%x", ret);
	}

	return ret;
}

int camera_start_preview(camera_h camera)
{
	int ret = CAMERA_ERROR_NONE;
	muse_camera_api_e api = MUSE_CAMERA_API_START_PREVIEW;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd = 0;

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	LOGD("start");

	sock_fd = pc->cb_info->fd;

	muse_camera_msg_send_longtime(api, sock_fd, pc->cb_info, ret);

	if (ret != CAMERA_ERROR_NONE) {
		LOGE("start preview failed 0x%x", ret);
		return ret;
	}

	LOGD("ret : 0x%x", ret);

	return CAMERA_ERROR_NONE;
}

int camera_stop_preview(camera_h camera)
{
	int ret = CAMERA_ERROR_NONE;
	int sock_fd = 0;
	camera_cli_s *pc = NULL;
	muse_camera_api_e api = MUSE_CAMERA_API_STOP_PREVIEW;

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	sock_fd = pc->cb_info->fd;

	LOGD("Enter");

	/* send stop preview message */
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	LOGD("ret : 0x%x", ret);

	return ret;
}

int camera_start_capture(camera_h camera, camera_capturing_cb capturing_cb, camera_capture_completed_cb completed_cb, void *user_data)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_START_CAPTURE;
	int sock_fd;
	int is_capturing_cb = 0;
	int is_completed_cb = 0;
	LOGD("Enter, handle :%x", pc->remote_handle);

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	sock_fd = pc->cb_info->fd;

	if (capturing_cb != NULL) {
		is_capturing_cb = 1;
		pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = capturing_cb;
		pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = user_data;
	}

	if (completed_cb != NULL) {
		is_completed_cb = 1;
		pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = completed_cb;
		pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = user_data;
	}

	muse_camera_msg_send2(api, sock_fd, pc->cb_info, ret, INT, is_capturing_cb, INT, is_completed_cb);
	LOGD("is_capturing_cb :%d, is_completed_cb : %d", is_capturing_cb, is_completed_cb);
	LOGD("ret : 0x%x", ret);
	return ret;
}

bool camera_is_supported_continuous_capture(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SUPPORT_CONTINUOUS_CAPTURE;
	int sock_fd;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return (bool)ret;
}

int camera_start_continuous_capture(camera_h camera, int count, int interval, camera_capturing_cb capturing_cb, camera_capture_completed_cb completed_cb , void *user_data)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_START_CONTINUOUS_CAPTURE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = capturing_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = user_data;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = completed_cb;

	muse_camera_msg_send2(api, sock_fd, pc->cb_info, ret, INT, count, INT, interval);

	LOGD("ret : 0x%x", ret);

	return ret;
}

int camera_stop_continuous_capture(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}


	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_STOP_CONTINUOUS_CAPTURE;
	LOGD("Enter,  handle :%x", pc->remote_handle);
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

bool camera_is_supported_face_detection(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SUPPORT_FACE_DETECTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return (bool)ret;
}

bool camera_is_supported_zero_shutter_lag(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SUPPORT_ZERO_SHUTTER_LAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return (bool)ret;
}

bool camera_is_supported_media_packet_preview_cb(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SUPPORT_MEDIA_PACKET_PREVIEW_CB;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return (bool)ret;
}

int camera_get_device_count(camera_h camera, int *device_count)
{
	if (camera == NULL || device_count == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_DEVICE_COUNT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_device_count;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_device_count, pc->cb_info->recv_msg);
		*device_count = get_device_count;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_start_face_detection(camera_h camera, camera_face_detected_cb callback, void *user_data)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_START_FACE_DETECTION;

	LOGD("Enter, handle :%x", pc->remote_handle);
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FACE_DETECTION] = callback;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FACE_DETECTION] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_stop_face_detection(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_STOP_FACE_DETECTION;
	LOGD("Enter,  handle :%x", pc->remote_handle);
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_state(camera_h camera, camera_state_e * state)
{
	if (camera == NULL || state == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_STATE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_state;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_state, pc->cb_info->recv_msg);
		*state = (camera_state_e)get_state;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_start_focusing(camera_h camera, bool continuous)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_START_FOCUSING;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int is_continuous = (int)continuous;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, is_continuous);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_cancel_focusing(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_CANCEL_FOCUSING;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_display(camera_h camera, camera_display_type_e type, camera_display_h display)
{
	int ret = CAMERA_ERROR_NONE;
	void *set_display_handle = NULL;
	Evas_Object *obj = NULL;
	const char *object_type = NULL;
#ifdef HAVE_WAYLAND
	camera_wl_info_s *wl_info = NULL;
#endif /* HAVE_WAYLAND */

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (type < CAMERA_DISPLAY_TYPE_OVERLAY || type > CAMERA_DISPLAY_TYPE_NONE) {
		LOGE("invalid type %d", type);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (type != CAMERA_DISPLAY_TYPE_NONE && display == NULL) {
		LOGE("display type[%d] is not NONE, but display handle is NULL", type);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_DISPLAY;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x display : 0x%x", pc->remote_handle, display);

	if (type == CAMERA_DISPLAY_TYPE_NONE) {
		set_display_handle = 0;
		LOGD("display type NONE");
	} else {
		obj = (Evas_Object *)display;
		object_type = evas_object_type_get(obj);
		if (object_type) {
			if (type == CAMERA_DISPLAY_TYPE_OVERLAY && !strcmp(object_type, "elm_win")) {
#ifdef HAVE_WAYLAND
				/* get wayland parent id */
				if (_get_wl_info(obj, &pc->wl_info) != CAMERA_ERROR_NONE) {
					LOGE("failed to get wayland info");
					return CAMERA_ERROR_INVALID_OPERATION;
				}

				set_display_handle = (void *)&pc->wl_info;
#else /* HAVE_WAYLAND */
				/* x window overlay surface */
				set_display_handle = (void *)elm_win_xwindow_get(obj);
#endif /* HAVE_WAYLAND */
				LOGD("display type OVERLAY : handle %p", set_display_handle);
			} else if (type == CAMERA_DISPLAY_TYPE_EVAS && !strcmp(object_type, "image")) {
				/* evas object surface */
				set_display_handle = (void *)display;
				LOGD("display type EVAS : handle %p", set_display_handle);
			} else {
				LOGE("unknown evas object [%p,%s] or type [%d] mismatch", obj, object_type, type);
				return CAMERA_ERROR_INVALID_PARAMETER;
			}
		} else {
			LOGE("failed to get evas object type from %p", obj);
			return CAMERA_ERROR_INVALID_PARAMETER;
		}
	}

	pc->display_handle = (intptr_t)set_display_handle;

	if (type == CAMERA_DISPLAY_TYPE_OVERLAY) {
#ifdef HAVE_WAYLAND
		wl_info = &pc->wl_info;
		muse_camera_msg_send_array_and_value(api, sock_fd, pc->cb_info, ret,
			wl_info, 5, sizeof(int), INT, type);

		LOGD("wayland parent id : %d, window %d,%d,%dx%d",
			wl_info->parent_id, wl_info->window_x, wl_info->window_y,
			wl_info->window_width, wl_info->window_height);
#else /* HAVE_WAYLAND */
		muse_camera_msg_send2(api, sock_fd, pc->cb_info, ret, INT, type, INT, set_display_handle);

		LOGD("x id : %d", (int)set_display_handle);
#endif /* HAVE_WAYLAND */
	} else
		muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, type);

	if (ret != CAMERA_ERROR_NONE)
		LOGE("set display error 0x%x", ret);

	return ret;
}

int camera_set_preview_resolution(camera_h camera,  int width, int height)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_PREVIEW_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send2(api, sock_fd, pc->cb_info, ret, INT, width, INT, height);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_set_capture_resolution(camera_h camera,  int width, int height)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_CAPTURE_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send2(api, sock_fd, pc->cb_info, ret, INT, width, INT, height);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_capture_format(camera_h camera, camera_pixel_format_e format)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	int set_format = (int)format;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_CAPTURE_FORMAT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x, capture_format: %d", pc->remote_handle, set_format);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_format);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_preview_format(camera_h camera, camera_pixel_format_e format)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	int set_format = (int)format;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_PREVIEW_FORMAT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x, capture_format: %d", pc->remote_handle, set_format);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_format);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_preview_resolution(camera_h camera,  int *width, int *height)
{
	if (camera == NULL || width == NULL || height == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}


	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_PREVIEW_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_width, pc->cb_info->recv_msg);
		muse_camera_msg_get(get_height, pc->cb_info->recv_msg);
		*width = get_width;
		*height = get_height;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_display_rotation(camera_h camera, camera_rotation_e rotation)
{
	int ret = CAMERA_ERROR_NONE;
	int set_rotation = (int)rotation;
	camera_cli_s *pc = NULL;

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (rotation < CAMERA_ROTATION_NONE || rotation > CAMERA_ROTATION_270) {
		LOGE("Invalid rotation %d", rotation);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send1(MUSE_CAMERA_API_SET_DISPLAY_ROTATION,
		pc->cb_info->fd, pc->cb_info, ret, INT, set_rotation);

	return ret;
}

int camera_get_display_rotation(camera_h camera, camera_rotation_e *rotation)
{
	int ret = CAMERA_ERROR_NONE;
	int get_rotation = CAMERA_ROTATION_NONE;
	camera_cli_s *pc = NULL;

	if (camera == NULL || rotation == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send(MUSE_CAMERA_API_GET_DISPLAY_ROTATION,
		pc->cb_info->fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_rotation, pc->cb_info->recv_msg);
		*rotation = (camera_rotation_e)get_rotation;
	}

	return ret;
}

int camera_set_display_flip(camera_h camera, camera_flip_e flip)
{
	int ret = CAMERA_ERROR_NONE;
	int set_flip = (int)flip;
	camera_cli_s *pc = NULL;

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (flip < CAMERA_FLIP_NONE || flip > CAMERA_FLIP_BOTH) {
		LOGE("Invalid flip %d", flip);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send1(MUSE_CAMERA_API_SET_DISPLAY_FLIP,
		pc->cb_info->fd, pc->cb_info, ret, INT, set_flip);

	return ret;
}

int camera_get_display_flip(camera_h camera, camera_flip_e *flip)
{
	int ret = CAMERA_ERROR_NONE;
	int get_flip = CAMERA_FLIP_NONE;
	camera_cli_s *pc = NULL;

	if (camera == NULL || flip == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send(MUSE_CAMERA_API_GET_DISPLAY_FLIP,
		pc->cb_info->fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_flip, pc->cb_info->recv_msg);
		*flip = (camera_flip_e)get_flip;
	}

	return ret;
}

int camera_set_display_visible(camera_h camera, bool visible)
{
	int ret = CAMERA_ERROR_NONE;
	int set_visible = (int)visible;
	camera_cli_s *pc = NULL;

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send1(MUSE_CAMERA_API_SET_DISPLAY_VISIBLE,
		pc->cb_info->fd, pc->cb_info, ret, INT, set_visible);

	return ret;
}

int camera_is_display_visible(camera_h camera, bool *visible)
{
	int ret = CAMERA_ERROR_NONE;
	int get_visible = true;
	camera_cli_s *pc = NULL;

	if (camera == NULL || visible == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send(MUSE_CAMERA_API_IS_DISPLAY_VISIBLE,
		pc->cb_info->fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_visible, pc->cb_info->recv_msg);
		*visible = (bool)get_visible;
	}

	return ret;
}

int camera_set_display_mode(camera_h camera, camera_display_mode_e mode)
{
	int ret = CAMERA_ERROR_NONE;
	int set_mode = (int)mode;
	camera_cli_s *pc = NULL;

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (mode < CAMERA_DISPLAY_MODE_LETTER_BOX || mode > CAMERA_DISPLAY_MODE_CROPPED_FULL) {
		LOGE("Invalid mode %d", mode);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send1(MUSE_CAMERA_API_SET_DISPLAY_MODE,
		pc->cb_info->fd, pc->cb_info, ret, INT, set_mode);

	return ret;
}

int camera_get_display_mode(camera_h camera, camera_display_mode_e *mode)
{
	int ret = CAMERA_ERROR_NONE;
	int get_mode = CAMERA_DISPLAY_MODE_LETTER_BOX;
	camera_cli_s *pc = NULL;

	if (camera == NULL || mode == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	pc = (camera_cli_s *)camera;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send(MUSE_CAMERA_API_GET_DISPLAY_MODE,
		pc->cb_info->fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recv_msg);
		*mode = (camera_display_mode_e)get_mode;
	}

	return ret;
}

int camera_get_capture_resolution(camera_h camera, int *width, int *height)
{
	if (camera == NULL || width == NULL || height == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_CAPTURE_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_width, pc->cb_info->recv_msg);
		muse_camera_msg_get(get_height, pc->cb_info->recv_msg);
		*width = get_width;
		*height = get_height;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_capture_format(camera_h camera, camera_pixel_format_e *format)
{
	if (camera == NULL || format == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_CAPTURE_FORMAT;
	int get_format;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_format, pc->cb_info->recv_msg);
		*format = (camera_pixel_format_e)get_format;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_preview_format(camera_h camera, camera_pixel_format_e *format)
{
	if (camera == NULL || format == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_PREVIEW_FORMAT;
	int get_format;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_format, pc->cb_info->recv_msg);
		*format = (camera_pixel_format_e)get_format;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_facing_direction(camera_h camera, camera_facing_direction_e *facing_direciton)
{
	if (camera == NULL || facing_direciton == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_FACING_DIRECTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_facing_direction;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_facing_direction, pc->cb_info->recv_msg);
		*facing_direciton = (camera_facing_direction_e)get_facing_direction;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_preview_cb(camera_h camera, camera_preview_cb callback, void *user_data)
{
	if (camera == NULL || callback == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_PREVIEW_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_PREVIEW] = callback;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_PREVIEW] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_unset_preview_cb(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_PREVIEW_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_PREVIEW] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_PREVIEW] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_media_packet_preview_cb(camera_h camera, camera_media_packet_preview_cb callback, void *user_data)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (camera_is_supported_media_packet_preview_cb(camera) == false) {
		LOGE("NOT SUPPORTED");
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	if (callback == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - callback", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_MEDIA_PACKET_PREVIEW_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = callback;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_unset_media_packet_preview_cb(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_MEDIA_PACKET_PREVIEW_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_state_changed_cb(camera_h camera, camera_state_changed_cb callback, void *user_data)
{
	if (camera == NULL || callback == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_STATE_CHANGED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_STATE_CHANGE] = callback;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_STATE_CHANGE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}
int camera_unset_state_changed_cb(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_STATE_CHANGED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_interrupted_cb(camera_h camera, camera_interrupted_cb callback, void *user_data)
{
	if (camera == NULL || callback == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_INTERRUPTED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_INTERRUPTED] = callback;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_INTERRUPTED] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_unset_interrupted_cb(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_INTERRUPTED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_INTERRUPTED] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_INTERRUPTED] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_focus_changed_cb(camera_h camera, camera_focus_changed_cb callback, void *user_data)
{
	if (camera == NULL || callback == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOCUS_CHANGED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = callback;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_unset_focus_changed_cb(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_FOCUS_CHANGED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_error_cb(camera_h camera, camera_error_cb callback, void *user_data)
{
	if (camera == NULL || callback == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_ERROR_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_ERROR] = callback;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_ERROR] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_unset_error_cb(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_ERROR_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_ERROR] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_ERROR] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_foreach_supported_preview_resolution(camera_h camera, camera_supported_preview_resolution_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOREACH_SUPPORTED_PREVIEW_RESOLUTION;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_RESOLUTION] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_RESOLUTION] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_foreach_supported_capture_resolution(camera_h camera, camera_supported_capture_resolution_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOREACH_SUPPORTED_CAPTURE_RESOLUTION;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_RESOLUTION] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_RESOLUTION] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_foreach_supported_capture_format(camera_h camera, camera_supported_capture_format_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOREACH_SUPPORTED_CAPTURE_FORMAT;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_FORMAT] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_FORMAT] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_foreach_supported_preview_format(camera_h camera, camera_supported_preview_format_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOREACH_SUPPORTED_PREVIEW_FORMAT;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_FORMAT] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_FORMAT] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_get_recommended_preview_resolution(camera_h camera, int *width, int *height)
{
	if (camera == NULL || width == NULL || height == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_RECOMMENDED_PREVIEW_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_width, pc->cb_info->recv_msg);
		muse_camera_msg_get(get_height, pc->cb_info->recv_msg);
		*width = get_width;
		*height = get_height;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_lens_orientation(camera_h camera, int *angle)
{
	if (camera == NULL || angle == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_LENS_ORIENTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_angle;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_angle, pc->cb_info->recv_msg);
		*angle = get_angle;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_set_theater_mode(camera_h camera, camera_attr_theater_mode_e mode)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_THEATER_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_mode = (int)mode;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_get_theater_mode(camera_h camera, camera_attr_theater_mode_e *mode)
{
	if (camera == NULL || mode == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_THEATER_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recv_msg);
		*mode = (camera_attr_theater_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_foreach_supported_theater_mode(camera_h camera, camera_attr_supported_theater_mode_cb foreach_cb, void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_THEATER_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_THEATER_MODE] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_THEATER_MODE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	LOGD("Finish, return :%x", ret);

	return ret;
}

int camera_attr_set_preview_fps(camera_h camera,  camera_attr_fps_e fps)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_PREVIEW_FPS;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_fps = (int)fps;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_fps);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_image_quality(camera_h camera,  int quality)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_IMAGE_QUALITY;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, quality);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_get_preview_fps(camera_h camera,  camera_attr_fps_e *fps)
{
	if (camera == NULL || fps == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_PREVIEW_FPS;
	int get_fps;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_fps, pc->cb_info->recv_msg);
		*fps = (camera_attr_fps_e)get_fps;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_image_quality(camera_h camera, int *quality)
{
	if (camera == NULL || quality == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_IMAGE_QUALITY;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_quality;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_quality, pc->cb_info->recv_msg);
		*quality = get_quality;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_encoded_preview_bitrate(camera_h camera, int *bitrate)
{
	if (camera == NULL || bitrate == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_ENCODED_PREVIEW_BITRATE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_bitrate;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_bitrate, pc->cb_info->recv_msg);
		*bitrate = get_bitrate;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_encoded_preview_bitrate(camera_h camera, int bitrate)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_ENCODED_PREVIEW_BITRATE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_bitrate = bitrate;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_bitrate);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_encoded_preview_gop_interval(camera_h camera, int *interval)
{
	if (camera == NULL || interval == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_ENCODED_PREVIEW_GOP_INTERVAL;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_gop_interval;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_gop_interval, pc->cb_info->recv_msg);
		*interval = get_gop_interval;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_encoded_preview_gop_interval(camera_h camera, int interval)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_ENCODED_PREVIEW_GOP_INTERVAL;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_gop_interval = interval;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_gop_interval);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_zoom(camera_h camera, int zoom)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_ZOOM;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, zoom);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_set_af_mode(camera_h camera,  camera_attr_af_mode_e mode)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_AF_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_mode = (int)mode;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_set_af_area(camera_h camera, int x, int y)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_AF_AREA;
	int sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send2(api, sock_fd, pc->cb_info, ret, INT, x, INT, y);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_clear_af_area(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_CLEAR_AF_AREA;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_exposure_mode(camera_h camera,  camera_attr_exposure_mode_e mode)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (mode < CAMERA_ATTR_EXPOSURE_MODE_OFF || mode > CAMERA_ATTR_EXPOSURE_MODE_CUSTOM) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_EXPOSURE_MODE;
	int set_mode = (int)mode;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_exposure(camera_h camera, int value)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_EXPOSURE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, value);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_iso(camera_h camera, camera_attr_iso_e iso)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_ISO;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_iso = (int)iso;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_iso);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_brightness(camera_h camera, int level)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_BRIGHTNESS;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, level);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_contrast(camera_h camera, int level)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, level);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_whitebalance(camera_h camera, camera_attr_whitebalance_e wb)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (wb < CAMERA_ATTR_WHITE_BALANCE_NONE || wb > CAMERA_ATTR_WHITE_BALANCE_CUSTOM) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_WHITEBALANCE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_whitebalance = (int)wb;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_whitebalance);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_effect(camera_h camera, camera_attr_effect_mode_e effect)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_EFFECT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_effect = (int)effect;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_effect);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_scene_mode(camera_h camera, camera_attr_scene_mode_e mode)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_SCENE_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_mode = (int)mode;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_enable_tag(camera_h camera, bool enable)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_ENABLE_TAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_enable = (int)enable;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_enable);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_tag_image_description(camera_h camera, const char *description)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (description == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_TAG_IMAGE_DESCRIPTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, STRING, description);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_tag_orientation(camera_h camera,  camera_attr_tag_orientation_e orientation)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_TAG_ORIENTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_orientation = (int)orientation;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_orientation);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_tag_software(camera_h camera,  const char *software)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (software == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_TAG_SOFTWARE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, STRING, software);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_geotag(camera_h camera, double latitude , double longitude, double altitude)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_GEOTAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	double set_geotag[3] = { latitude, longitude, altitude };

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send_array(api, sock_fd, pc->cb_info, ret,
									set_geotag, sizeof(set_geotag), sizeof(double));
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_remove_geotag(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_REMOVE_GEOTAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_flash_mode(camera_h camera, camera_attr_flash_mode_e mode)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_FLASH_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_mode = (int)mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_zoom(camera_h camera, int *zoom)
{
	if (camera == NULL || zoom == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_ZOOM;
	int get_zoom;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_zoom, pc->cb_info->recv_msg);
		*zoom = get_zoom;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_zoom_range(camera_h camera, int *min, int *max)
{
	if (camera == NULL || min == NULL || max == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_ZOOM_RANGE;
	int get_min;
	int get_max;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_min, pc->cb_info->recv_msg);
		muse_camera_msg_get(get_max, pc->cb_info->recv_msg);
		*min = get_min;
		*max = get_max;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_af_mode(camera_h camera, camera_attr_af_mode_e *mode)
{
	if (camera == NULL || mode == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_AF_MODE;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recv_msg);
		*mode = (camera_attr_af_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_exposure_mode(camera_h camera, camera_attr_exposure_mode_e *mode)
{
	if (camera == NULL || mode == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_EXPOSURE_MODE;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recv_msg);
		*mode = (camera_attr_exposure_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_get_exposure(camera_h camera, int *value)
{
	if (camera == NULL || value == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_EXPOSURE;
	int get_value;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_value, pc->cb_info->recv_msg);
		*value = get_value;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_exposure_range(camera_h camera, int *min, int *max)
{
	if (camera == NULL || min == NULL || max == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_EXPOSURE_RANGE;
	int get_min;
	int get_max;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_min, pc->cb_info->recv_msg);
		muse_camera_msg_get(get_max, pc->cb_info->recv_msg);
		*min = get_min;
		*max = get_max;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_iso(camera_h camera, camera_attr_iso_e *iso)
{
	if (camera == NULL || iso == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_ISO;
	int get_iso;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_iso, pc->cb_info->recv_msg);
		*iso = (camera_attr_iso_e)get_iso;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_brightness(camera_h camera,  int *level)
{
	if (camera == NULL || level == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_BRIGHTNESS;
	int get_level;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_level, pc->cb_info->recv_msg);
		*level = get_level;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_brightness_range(camera_h camera, int *min, int *max)
{
	if (camera == NULL || min == NULL || max == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_BRIGHTNESS_RANGE;
	int get_min;
	int get_max;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_min, pc->cb_info->recv_msg);
		muse_camera_msg_get(get_max, pc->cb_info->recv_msg);
		*min = get_min;
		*max = get_max;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_contrast(camera_h camera,  int *level)
{
	if (camera == NULL || level == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_level;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_level, pc->cb_info->recv_msg);
		*level = get_level;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_contrast_range(camera_h camera, int *min , int *max)
{
	if (camera == NULL || min == NULL || max == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_CONTRAST_RANGE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_min;
	int get_max;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_min, pc->cb_info->recv_msg);
		muse_camera_msg_get(get_max, pc->cb_info->recv_msg);
		*min = get_min;
		*max = get_max;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_whitebalance(camera_h camera,  camera_attr_whitebalance_e *wb)
{
	if (camera == NULL || wb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_WHITEBALANCE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_wb;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_wb, pc->cb_info->recv_msg);
		*wb = (camera_attr_whitebalance_e)get_wb;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_effect(camera_h camera, camera_attr_effect_mode_e *effect)
{
	if (camera == NULL || effect == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_EFFECT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_effect;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_effect, pc->cb_info->recv_msg);
		*effect = (camera_attr_effect_mode_e)get_effect;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_scene_mode(camera_h camera,  camera_attr_scene_mode_e *mode)
{
	if (camera == NULL || mode == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_SCENE_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recv_msg);
		*mode = (camera_attr_scene_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_is_enabled_tag(camera_h camera,  bool *enable)
{
	if (camera == NULL || enable == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_ENABLED_TAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_enabled, pc->cb_info->recv_msg);
		*enable = (bool)get_enabled;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_tag_image_description(camera_h camera,  char **description)
{
	if (camera == NULL || description == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_TAG_IMAGE_DESCRIPTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	char get_description[MUSE_CAMERA_MSG_MAX_LENGTH] = {0,};

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get_string(get_description, pc->cb_info->recv_msg);
		*description = strdup(get_description);
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_tag_orientation(camera_h camera, camera_attr_tag_orientation_e *orientation)
{
	if (camera == NULL || orientation == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_TAG_ORIENTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_orientation;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_orientation, pc->cb_info->recv_msg);
		*orientation = (camera_attr_tag_orientation_e)get_orientation;
		LOGD("success, orientation : %d", *orientation);
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_tag_software(camera_h camera, char **software)
{
	if (camera == NULL || software == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_TAG_SOFTWARE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	char get_software[MUSE_CAMERA_MSG_MAX_LENGTH] = {0,};

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get_string(get_software, pc->cb_info->recv_msg);
		*software = strdup(get_software);
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_geotag(camera_h camera, double *latitude , double *longitude, double *altitude)
{
	if (camera == NULL || latitude == NULL || longitude == NULL || altitude == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_GEOTAG;
	double get_geotag[3] = {0,};
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int valid = 0;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get_array(get_geotag, pc->cb_info->recv_msg);
		*latitude = get_geotag[0];
		*longitude = get_geotag[1];
		*altitude = get_geotag[2];
	} else {
		LOGE("Returned value is not valid : 0x%x", valid);
	}

	LOGD("ret : 0x%x", ret);

	return ret;
}


int camera_attr_get_flash_mode(camera_h camera,  camera_attr_flash_mode_e *mode)
{
	if (camera == NULL || mode == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_FLASH_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recv_msg);
		*mode = (camera_attr_flash_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_flash_state(camera_device_e device, camera_flash_state_e *state)
{
	int sock_fd = -1;
	char *sndMsg;
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = NULL;
	int get_flash_state = 0;

	/* create muse connection */
	muse_camera_api_e api = MUSE_CAMERA_API_GET_FLASH_STATE;
	muse_core_api_module_e muse_module = MUSE_CAMERA;
	int device_type = (int)device;

	sock_fd = muse_core_client_new();
	if (sock_fd < 0) {
		LOGE("muse_core_client_new failed - returned fd %d", sock_fd);
		ret = CAMERA_ERROR_INVALID_OPERATION;
		goto Exit;
	}

	sndMsg = muse_core_msg_json_factory_new(api,
		MUSE_TYPE_INT, "module", muse_module,
		MUSE_TYPE_INT, PARAM_DEVICE_TYPE, device_type,
		0);

	muse_core_ipc_send_msg(sock_fd, sndMsg);
	muse_core_msg_json_factory_free(sndMsg);

	pc = g_new0(camera_cli_s, 1);
	if (pc == NULL) {
		LOGE("camera_cli_s alloc failed");
		ret = CAMERA_ERROR_OUT_OF_MEMORY;
		goto Exit;
	}

	pc->cb_info = _client_callback_new(sock_fd);
	if (pc->cb_info == NULL) {
		LOGE("cb_info alloc failed");
		ret = CAMERA_ERROR_OUT_OF_MEMORY;
		goto Exit;
	}

	ret = _client_wait_for_cb_return(api, pc->cb_info, CALLBACK_TIME_OUT);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_flash_state, pc->cb_info->recv_msg);
		*state = (camera_flash_state_e)get_flash_state;
	}

	LOGD("Flash state : %d\n", *state);

Exit:
	/* release resources */
	if (pc) {
		g_atomic_int_set(&pc->cb_info->msg_recv_running, 0);
		g_atomic_int_set(&pc->cb_info->msg_handler_running, 0);
		_client_callback_destroy(pc->cb_info);
		pc->cb_info = NULL;
		g_free(pc);
		pc = NULL;
	}

	return ret;
}

int camera_attr_foreach_supported_af_mode(camera_h camera, camera_attr_supported_af_mode_cb foreach_cb, void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_AF_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_AF_MODE] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_AF_MODE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_exposure_mode(camera_h camera, camera_attr_supported_exposure_mode_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_EXPOSURE_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_iso(camera_h camera, camera_attr_supported_iso_cb foreach_cb, void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_ISO;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_ISO] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_ISO] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_whitebalance(camera_h camera, camera_attr_supported_whitebalance_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_WHITEBALANCE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_WHITEBALANCE] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_WHITEBALANCE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_effect(camera_h camera, camera_attr_supported_effect_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_EFFECT;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EFFECT] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EFFECT] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_scene_mode(camera_h camera, camera_attr_supported_scene_mode_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_SCENE_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_SCENE_MODE] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_SCENE_MODE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_flash_mode(camera_h camera, camera_attr_supported_flash_mode_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_FLASH_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FLASH_MODE] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FLASH_MODE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_fps(camera_h camera, camera_attr_supported_fps_cb foreach_cb , void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_FPS;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, handle :%x", pc->remote_handle);
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("Enter, handle :%x", pc->remote_handle);
	return ret;
}

int camera_attr_foreach_supported_fps_by_resolution(camera_h camera, int width, int height, camera_attr_supported_fps_cb foreach_cb, void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_FPS_BY_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, handle :%x", pc->remote_handle);
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS_BY_RESOLUTION] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS_BY_RESOLUTION] = user_data;

	muse_camera_msg_send2(api, sock_fd, pc->cb_info, ret, INT, width, INT, height);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_foreach_supported_stream_flip(camera_h camera, camera_attr_supported_stream_flip_cb foreach_cb, void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_STREAM_FLIP;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, handle :%x", pc->remote_handle);
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_FLIP] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_FLIP] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_stream_rotation(camera_h camera, camera_attr_supported_stream_rotation_cb foreach_cb, void *user_data)
{
	if (camera == NULL || foreach_cb == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_STREAM_ROTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, handle :%x", pc->remote_handle);
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_ROTATION] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_ROTATION] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_stream_rotation(camera_h camera , camera_rotation_e rotation)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_STREAM_ROTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_rotation = (int)rotation;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_rotation);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_stream_rotation(camera_h camera , camera_rotation_e *rotation)
{
	if (camera == NULL || rotation == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_STREAM_ROTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_rotation;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_rotation, pc->cb_info->recv_msg);
		*rotation = (camera_rotation_e)get_rotation;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_stream_flip(camera_h camera , camera_flip_e flip)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_STREAM_FLIP;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_flip = (int)flip;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_flip);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_stream_flip(camera_h camera , camera_flip_e *flip)
{
	if (camera == NULL || flip == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_STREAM_FLIP;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_flip;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_flip, pc->cb_info->recv_msg);
		*flip = (camera_flip_e)get_flip;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_set_hdr_mode(camera_h camera, camera_attr_hdr_mode_e mode)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_HDR_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_mode = (int)mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_hdr_mode(camera_h camera, camera_attr_hdr_mode_e *mode)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (mode == NULL) {
		LOGE("CAMERA_ERROR_NOT_SUPPORTED(0x%08x) - mode", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_HDR_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recv_msg);
		*mode = (camera_attr_hdr_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


bool camera_attr_is_supported_hdr_capture(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_SUPPORTED_HDR_CAPTURE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return (bool)ret;
}


int camera_attr_set_hdr_capture_progress_cb(camera_h camera, camera_attr_hdr_progress_cb callback, void *user_data)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (callback == NULL) {
		LOGE("CAMERA_ERROR_NOT_SUPPORTED(0x%08x) - callback", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_HDR_CAPTURE_PROGRESS_CB;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, handle :%x", pc->remote_handle);

	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_HDR_PROGRESS] = callback;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_HDR_PROGRESS] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_unset_hdr_capture_progress_cb(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_UNSET_HDR_CAPTURE_PROGRESS_CB;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, handle :%x", pc->remote_handle);

	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_HDR_PROGRESS] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_HDR_PROGRESS] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_enable_anti_shake(camera_h camera, bool enable)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_ENABLE_ANTI_SHAKE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_enable = (int)enable;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_enable);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_is_enabled_anti_shake(camera_h camera , bool *enabled)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (enabled == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - enabled", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_ENABLED_ANTI_SHAKE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_enabled, pc->cb_info->recv_msg);
		*enabled = (bool)get_enabled;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


bool camera_attr_is_supported_anti_shake(camera_h camera)
{

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_SUPPORTED_ANTI_SHAKE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_enable_video_stabilization(camera_h camera, bool enable)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_ENABLE_VIDEO_STABILIZATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_enable = (int)enable;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_enable);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_is_enabled_video_stabilization(camera_h camera, bool *enabled)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (enabled == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - enabled", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_ENABLED_VIDEO_STABILIZATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_enabled, pc->cb_info->recv_msg);
		*enabled = (bool)get_enabled;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


bool camera_attr_is_supported_video_stabilization(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_SUPPORTED_VIDEO_STABILIZATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_enable_auto_contrast(camera_h camera, bool enable)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_ENABLE_AUTO_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_enable = (int)enable;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_enable);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_is_enabled_auto_contrast(camera_h camera, bool *enabled)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (enabled == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - enabled", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_ENABLED_AUTO_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_enabled, pc->cb_info->recv_msg);
		*enabled = (bool)get_enabled;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


bool camera_attr_is_supported_auto_contrast(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_SUPPORTED_AUTO_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_disable_shutter_sound(camera_h camera, bool disable)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_DISABLE_SHUTTER_SOUND;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_disable = (int)disable;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_disable);
	LOGD("ret : 0x%x", ret);
	return ret;
}
