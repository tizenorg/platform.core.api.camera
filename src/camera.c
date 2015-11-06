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
#include <camera_private.h>
#include <muse_core.h>
//#include <glib.h>
#include <dlog.h>
#include <Elementary.h>
#include <mm_camcorder_client.h>
#include <Evas.h>
#ifdef HAVE_WAYLAND
#include <Ecore_Wayland.h>
#else
#include <Ecore.h>
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_CAMERA_CLIENT"


int __convert_camera_error_code(const char *func, int code)
{
	int ret = CAMERA_ERROR_NONE;
	const char *errorstr = NULL;

	switch (code) {
	case MM_ERROR_NONE:
		ret = CAMERA_ERROR_NONE;
		errorstr = "ERROR_NONE";
		break;
	case MM_ERROR_CAMCORDER_INVALID_ARGUMENT:
	case MM_ERROR_COMMON_INVALID_ATTRTYPE:
		ret = CAMERA_ERROR_INVALID_PARAMETER;
		errorstr = "INVALID_PARAMETER";
		break;
	case MM_ERROR_CAMCORDER_NOT_INITIALIZED:
	case MM_ERROR_CAMCORDER_INVALID_STATE:
		ret = CAMERA_ERROR_INVALID_STATE;
		errorstr = "INVALID_STATE";
		break;
	case MM_ERROR_CAMCORDER_DEVICE_NOT_FOUND:
		ret = CAMERA_ERROR_DEVICE_NOT_FOUND;
		errorstr = "DEVICE_NOT_FOUND";
		break;
	case MM_ERROR_CAMCORDER_DEVICE_BUSY:
	case MM_ERROR_CAMCORDER_DEVICE_OPEN:
	case MM_ERROR_CAMCORDER_CMD_IS_RUNNING:
		ret = CAMERA_ERROR_DEVICE_BUSY;
		errorstr = "DEVICE_BUSY";
		break;
	case MM_ERROR_CAMCORDER_DEVICE:
	case MM_ERROR_CAMCORDER_DEVICE_IO:
	case MM_ERROR_CAMCORDER_DEVICE_TIMEOUT:
	case MM_ERROR_CAMCORDER_DEVICE_WRONG_JPEG:
	case MM_ERROR_CAMCORDER_DEVICE_LACK_BUFFER:
		ret = CAMERA_ERROR_DEVICE;
		errorstr = "ERROR_DEVICE";
		break;
	case MM_ERROR_CAMCORDER_GST_CORE:
	case MM_ERROR_CAMCORDER_GST_LIBRARY:
	case MM_ERROR_CAMCORDER_GST_RESOURCE:
	case MM_ERROR_CAMCORDER_GST_STREAM:
	case MM_ERROR_CAMCORDER_GST_STATECHANGE:
	case MM_ERROR_CAMCORDER_GST_NEGOTIATION:
	case MM_ERROR_CAMCORDER_GST_LINK:
	case MM_ERROR_CAMCORDER_GST_FLOW_ERROR:
	case MM_ERROR_CAMCORDER_ENCODER:
	case MM_ERROR_CAMCORDER_ENCODER_BUFFER:
	case MM_ERROR_CAMCORDER_ENCODER_WRONG_TYPE:
	case MM_ERROR_CAMCORDER_ENCODER_WORKING:
	case MM_ERROR_CAMCORDER_INTERNAL:
	case MM_ERROR_CAMCORDER_RESPONSE_TIMEOUT:
	case MM_ERROR_CAMCORDER_DSP_FAIL:
	case MM_ERROR_CAMCORDER_AUDIO_EMPTY:
	case MM_ERROR_CAMCORDER_CREATE_CONFIGURE:
	case MM_ERROR_CAMCORDER_FILE_SIZE_OVER:
	case MM_ERROR_CAMCORDER_DISPLAY_DEVICE_OFF:
	case MM_ERROR_CAMCORDER_INVALID_CONDITION:
		ret = CAMERA_ERROR_INVALID_OPERATION;
		errorstr = "INVALID_OPERATION";
		break;
	case MM_ERROR_CAMCORDER_RESOURCE_CREATION:
	case MM_ERROR_COMMON_OUT_OF_MEMORY:
		ret = CAMERA_ERROR_OUT_OF_MEMORY;
		errorstr = "OUT_OF_MEMORY";
		break;
	case MM_ERROR_POLICY_BLOCKED:
		ret = CAMERA_ERROR_SOUND_POLICY;
		errorstr = "ERROR_SOUND_POLICY";
		break;
	case MM_ERROR_POLICY_BLOCKED_BY_CALL:
		ret = CAMERA_ERROR_SOUND_POLICY_BY_CALL;
		errorstr = "ERROR_SOUND_POLICY_BY_CALL";
		break;
	case MM_ERROR_POLICY_BLOCKED_BY_ALARM:
		ret = CAMERA_ERROR_SOUND_POLICY_BY_ALARM;
		errorstr = "ERROR_SOUND_POLICY_BY_ALARM";
		break;
	case MM_ERROR_POLICY_RESTRICTED:
		ret = CAMERA_ERROR_SECURITY_RESTRICTED;
		errorstr = "ERROR_RESTRICTED";
		break;
	case MM_ERROR_CAMCORDER_DEVICE_REG_TROUBLE:
		ret = CAMERA_ERROR_ESD;
		errorstr = "ERROR_ESD";
		break;
	case MM_ERROR_COMMON_INVALID_PERMISSION:
		ret = CAMERA_ERROR_PERMISSION_DENIED;
		errorstr = "ERROR_PERMISSION_DENIED";
		break;
	case MM_ERROR_COMMON_OUT_OF_ARRAY:
	case MM_ERROR_COMMON_OUT_OF_RANGE:
	case MM_ERROR_COMMON_ATTR_NOT_EXIST:
	case MM_ERROR_CAMCORDER_NOT_SUPPORTED:
		ret = CAMERA_ERROR_NOT_SUPPORTED;
		errorstr = "ERROR_NOT_SUPPORTED";
		break;
	default:
		ret = CAMERA_ERROR_INVALID_OPERATION;
		errorstr = "INVALID_OPERATION";
	}

	if (code != MM_ERROR_NONE) {
		LOGE("[%s] %s(0x%08x) : core frameworks error code(0x%08x)", func ? func : "NULL_FUNC", errorstr, ret, code);
	}

	return ret;
}

#ifdef HAVE_WAYLAND
static MMCamWaylandInfo *_get_wl_info(Evas_Object *obj)
{
	MMCamWaylandInfo *wl_info = NULL;

	if (obj == NULL) {
		LOGE("evas object is NULL");
		return NULL;
	}

	wl_info = g_new0(MMCamWaylandInfo, 1);
	if (wl_info == NULL) {
		LOGE("wl_info alloc failed : %d", sizeof(MMCamWaylandInfo));
		return NULL;
	}

	wl_info->evas_obj = (void *)obj;
	wl_info->window = (void *)elm_win_wl_window_get(obj);
	wl_info->surface = (void *)ecore_wl_window_surface_get(wl_info->window);
	wl_info->display = (void *)ecore_wl_display_get();

	if (wl_info->window == NULL || wl_info->surface == NULL || wl_info->display == NULL) {
		LOGE("something is NULL %p, %p, %p", wl_info->window, wl_info->surface, wl_info->display);
		g_free(wl_info);
		return NULL;
	}

	evas_object_geometry_get(obj, &wl_info->window_x, &wl_info->window_y,
	                         &wl_info->window_width, &wl_info->window_height);

	LOGD("wayland obj %p, window %p, surface %p, display %p, size %d,%d,%dx%d",
	     wl_info->evas_obj, wl_info->window, wl_info->surface, wl_info->display,
	     wl_info->window_x, wl_info->window_y, wl_info->window_width, wl_info->window_height);

	return wl_info;
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
		LOGE("NULL bo");
		return;
	}

	tbm_bo_unmap(*bo);
	tbm_bo_unref(*bo);
	*bo = NULL;

	return;
}

static int _client_wait_for_cb_return(muse_camera_api_e api, callback_cb_info_s *cb_info, int time_out)
{
	int ret = CAMERA_ERROR_NONE;
	gint64 end_time;

	LOGD("Enter api : %d", api);
	g_mutex_lock(&(cb_info->pMutex[api]));

	if (cb_info->activating[api] == 0) {
		end_time = g_get_monotonic_time() + time_out * G_TIME_SPAN_SECOND;
		if (g_cond_wait_until(&(cb_info->pCond[api]), &(cb_info->pMutex[api]), end_time)) {
			LOGD("cb_info->recvApiMsg : %s", cb_info->recvApiMsg);
			if (!muse_camera_msg_get(ret, cb_info->recvApiMsg)) {
				LOGE("Get cb msg failed.");
				ret = CAMERA_ERROR_INVALID_OPERATION;
			} else {
				LOGD("Wait passed, ret : 0x%x", ret);
			}
			if (cb_info->activating[api])
				cb_info->activating[api] = 0;
		} else {
			LOGD("api %d was TIMED OUT!", api);
			ret = CAMERA_ERROR_INVALID_OPERATION;
		}
	} else {
		LOGE("condition is already checked for the api : %d.", api);
		if (!muse_camera_msg_get(ret, cb_info->recvApiMsg)) {
			LOGE("Get cb msg failed.");
			ret = CAMERA_ERROR_INVALID_OPERATION;
		} else {
			LOGD("Already checked condition, Wait passed, ret : 0x%x", ret);
		}
	}
	g_mutex_unlock(&(cb_info->pMutex[api]));
	LOGD("ret : 0x%x", ret);
	return ret;
}

static void _client_user_callback(callback_cb_info_s *cb_info, char *recvMsg, muse_camera_event_e event)
{
	int param1 = 0;
	int param2 = 0;
	int tbm_key = 0;
	tbm_bo bo = NULL;
	tbm_bo_handle bo_handle = {NULL, };

	if (recvMsg == NULL || event >= MUSE_CAMERA_EVENT_TYPE_NUM) {
		LOGE("invalid parameter - msg %p, event %d", recvMsg, event);
		return;
	}

	LOGD("get msg %s, event %d", recvMsg, event);

	if (cb_info->user_cb[event] == NULL) {
		LOGW("user callback for event %d is not set", event);
		return;
	}

	switch (event) {
	case MUSE_CAMERA_EVENT_TYPE_STATE_CHANGE:
		{
			int previous = 0;
			int current = 0;
			int by_policy = 0;

			muse_camera_msg_get(previous, recvMsg);
			muse_camera_msg_get(current, recvMsg);
			muse_camera_msg_get(by_policy, recvMsg);

			LOGD("STATE CHANGE - previous %d, current %d, by_policy %d",
			     previous, current, by_policy);

			((camera_state_changed_cb)cb_info->user_cb[event])((camera_state_e)previous,
			                                                   (camera_state_e)current,
			                                                   (bool)by_policy,
			                                                   cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOCUS_CHANGE:
		{
			int state = 0;

			muse_camera_msg_get(state, recvMsg);

			LOGD("FOCUS state - %d", state);

			((camera_focus_changed_cb)cb_info->user_cb[event])((camera_focus_state_e)state,
			                                                   cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE:
		LOGD("CAPTURE_COMPLETED");
		((camera_capture_completed_cb)cb_info->user_cb[event])(cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_PREVIEW:
		{
			unsigned char *buf_pos = NULL;
			camera_preview_data_s *frame = NULL;
			int total_size = 0;

			muse_camera_msg_get(tbm_key, recvMsg);

			if (tbm_key <= 0) {
				LOGE("invalid key %d", tbm_key);
				break;
			}

			/* import tbm bo and get virtual address */
			if (!_import_tbm_key(cb_info->bufmgr, tbm_key, &bo, &bo_handle)) {
				break;
			}

			buf_pos = (unsigned char *)bo_handle.ptr;

			frame = (camera_preview_data_s *)buf_pos;
			buf_pos += sizeof(camera_preview_data_s);

			switch (frame->num_of_planes) {
			case 1:
				frame->data.single_plane.yuv = buf_pos;
				total_size = frame->data.single_plane.size;
			case 2:
				frame->data.double_plane.y = buf_pos;
				buf_pos += frame->data.double_plane.y_size;
				frame->data.double_plane.uv = buf_pos;
				total_size = frame->data.double_plane.y_size + \
				             frame->data.double_plane.uv_size;
			case 3:
				frame->data.triple_plane.y = buf_pos;
				buf_pos += frame->data.triple_plane.y_size;
				frame->data.triple_plane.u = buf_pos;
				buf_pos += frame->data.triple_plane.u_size;
				frame->data.triple_plane.v = buf_pos;
				total_size = frame->data.triple_plane.y_size + \
				             frame->data.triple_plane.u_size + \
				             frame->data.triple_plane.v_size;
			default:
				break;
			}

			LOGD("PREVIEW_CB - format %d, %dx%d, size %d plane num %d",
			     frame->format, frame->width, frame->height, total_size, frame->num_of_planes);

			((camera_preview_cb)cb_info->user_cb[event])(frame, cb_info->user_data[event]);

			LOGD("PREVIEW_CB retuned");

			/* unmap and unref tbm bo */
			_release_imported_bo(&bo);

			/* return buffer */
			muse_camera_msg_send1_no_return(MUSE_CAMERA_API_RETURN_BUFFER,
			                                cb_info->fd, cb_info,
			                                INT, tbm_key);

			LOGD("return buffer Done");
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW:
		((camera_media_packet_preview_cb)cb_info->user_cb[event])(NULL, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_HDR_PROGRESS:
		{
			int percent = 0;

			muse_camera_msg_get(percent, recvMsg);

			LOGD("HDR progress - %d \%", percent);

			((camera_attr_hdr_progress_cb)cb_info->user_cb[event])(percent, cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_INTERRUPTED:
		{
			int policy = 0;
			int previous = 0;
			int current = 0;

			muse_camera_msg_get(policy, recvMsg);
			muse_camera_msg_get(previous, recvMsg);
			muse_camera_msg_get(current, recvMsg);

			LOGD("INTERRUPTED - policy %d, state previous %d, current %d",
			     policy, previous, current);

			((camera_interrupted_cb)cb_info->user_cb[event])((camera_policy_e)policy,
			                                                 (camera_state_e)previous,
			                                                 (camera_state_e)current,
			                                                 cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FACE_DETECTION:
		{
			int count = 0;
			camera_detected_face_s *faces = NULL;

			muse_camera_msg_get(count, recvMsg);
			muse_camera_msg_get(tbm_key, recvMsg);

			if (count > 0 && tbm_key > 0) {
				LOGD("FACE_DETECTION - count %d, tbm_key %d", count, tbm_key);

				if (!_import_tbm_key(cb_info->bufmgr, tbm_key, &bo, &bo_handle)) {
					break;
				}

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
				                                cb_info->fd, cb_info,
				                                INT, tbm_key);

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

			muse_camera_msg_get(error, recvMsg);
			muse_camera_msg_get(current_state, recvMsg);

			LOGE("ERROR - error 0x%x, current_state %d", error, current_state);

			((camera_error_cb)cb_info->user_cb[event])((camera_error_e)error,
			                                           (camera_state_e)current_state,
			                                           cb_info->user_data[event]);
		}
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_RESOLUTION:
		muse_camera_msg_get(param1, recvMsg);
		muse_camera_msg_get(param2, recvMsg);

		LOGD("SUPPORTED_PREVIEW_RESOLUTION - %d x %d", param1, param2);

		((camera_supported_preview_resolution_cb)cb_info->user_cb[event])(param1, param2, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_RESOLUTION:
		muse_camera_msg_get(param1, recvMsg);
		muse_camera_msg_get(param2, recvMsg);

		LOGD("SUPPORTED_CAPTURE_RESOLUTION - %d x %d", param1, param2);

		((camera_supported_capture_resolution_cb)cb_info->user_cb[event])(param1, param2, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_FORMAT:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_CAPTURE_FORMAT - %d ", param1);

		((camera_supported_capture_format_cb)cb_info->user_cb[event])((camera_pixel_format_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_FORMAT:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_PREVIEW_FORMAT - %d ", param1);

		((camera_supported_preview_format_cb)cb_info->user_cb[event])((camera_pixel_format_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_AF_MODE:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_AF_MODE - %d ", param1);

		((camera_attr_supported_af_mode_cb)cb_info->user_cb[event])((camera_attr_af_mode_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_EXPOSURE_MODE - %d ", param1);

		((camera_attr_supported_exposure_mode_cb)cb_info->user_cb[event])((camera_attr_exposure_mode_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_ISO:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_ISO - %d ", param1);

		((camera_attr_supported_iso_cb)cb_info->user_cb[event])((camera_attr_iso_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_WHITEBALANCE:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_WHITEBALANCE - %d ", param1);

		((camera_attr_supported_whitebalance_cb)cb_info->user_cb[event])((camera_attr_whitebalance_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EFFECT:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_EFFECT - %d ", param1);

		((camera_attr_supported_effect_cb)cb_info->user_cb[event])((camera_attr_effect_mode_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_SCENE_MODE:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_SCENE_MODE - %d ", param1);

		((camera_attr_supported_scene_mode_cb)cb_info->user_cb[event])((camera_attr_scene_mode_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FLASH_MODE:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_FLASH_MODE - %d ", param1);

		((camera_attr_supported_flash_mode_cb)cb_info->user_cb[event])((camera_attr_flash_mode_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_FPS - %d ", param1);

		((camera_attr_supported_fps_cb)cb_info->user_cb[event])((camera_attr_fps_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS_BY_RESOLUTION:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_FPS_BY_RESOLUTION - %d ", param1);

		((camera_attr_supported_fps_cb)cb_info->user_cb[event])((camera_attr_fps_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_FLIP:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_STREAM_FLIP - %d ", param1);

		((camera_attr_supported_stream_flip_cb)cb_info->user_cb[event])((camera_flip_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_ROTATION:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_STREAM_ROTATION - %d ", param1);

		((camera_attr_supported_stream_rotation_cb)cb_info->user_cb[event])((camera_rotation_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_THEATER_MODE:
		muse_camera_msg_get(param1, recvMsg);

		LOGD("SUPPORTED_THEATER_MODE - %d ", param1);

		((camera_attr_supported_theater_mode_cb)cb_info->user_cb[event])((camera_attr_theater_mode_e)param1, cb_info->user_data[event]);
		break;
	case MUSE_CAMERA_EVENT_TYPE_CAPTURE:
		{
			camera_image_data_s *rImage = NULL;
			camera_image_data_s *rPostview = NULL;
			camera_image_data_s *rThumbnail = NULL;
			unsigned char *buf_pos = NULL;
			int is_postview = 0;
			int is_thumbnail = 0;

			muse_camera_msg_get(tbm_key, recvMsg);
			muse_camera_msg_get(is_postview, recvMsg);
			muse_camera_msg_get(is_thumbnail, recvMsg);

			LOGD("camera capture callback came in. key %d, postview %d, thumbnail %d",
			     tbm_key, is_postview, is_thumbnail);

			if (tbm_key <= 0) {
				LOGE("invalid key %d", tbm_key);
				break;
			}

			/* import tbm bo and get virtual address */
			if (!_import_tbm_key(cb_info->bufmgr, tbm_key, &bo, &bo_handle)) {
				break;
			}

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
			                                cb_info->fd,
			                                cb_info,
			                                INT, tbm_key);

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

static void *_event_handler(gpointer data)
{
	camera_event_s *cam_event = NULL;
	callback_cb_info_s *cb_info = (callback_cb_info_s *)data;

	if (cb_info == NULL) {
		LOGE("cb_info NULL");
		return NULL;
	}

	LOGD("start");

	g_mutex_lock(&cb_info->event_mutex);

	while (g_atomic_int_get(&cb_info->event_thread_running)) {
		if (g_queue_is_empty(cb_info->event_queue)) {
			LOGD("signal wait...");
			g_cond_wait(&cb_info->event_cond, &cb_info->event_mutex);
			LOGD("signal received");

			if (g_atomic_int_get(&cb_info->event_thread_running) == 0) {
				LOGD("stop event thread");
				break;
			}
		}

		cam_event = (camera_event_s *)g_queue_pop_head(cb_info->event_queue);

		g_mutex_unlock(&cb_info->event_mutex);

		if (cam_event) {
			_client_user_callback(cam_event->cb_info, cam_event->recvMsg, cam_event->event);
			free(cam_event);
			cam_event = NULL;
		} else {
			LOGW("NULL event info");
		}

		g_mutex_lock(&cb_info->event_mutex);
	}

	/* remove remained event */
	while (!g_queue_is_empty(cb_info->event_queue)) {
		cam_event = (camera_event_s *)g_queue_pop_head(cb_info->event_queue);
		if (cam_event) {
			LOGD("remove event info %p", cam_event);
			free(cam_event);
			cam_event = NULL;
		} else {
			LOGW("NULL event info");
		}
	}

	g_mutex_unlock(&cb_info->event_mutex);

	LOGD("return");

	return NULL;
}

static bool _camera_idle_event_callback(void *data)
{
	callback_cb_info_s *cb_info = NULL;
	camera_idle_event_s *cam_idle_event = (camera_idle_event_s *)data;

	if (cam_idle_event == NULL) {
		LOGE("cam_idle_event is NULL");
		return false;
	}

	/* lock event */
	g_mutex_lock(&cam_idle_event->event_mutex);

	cb_info = cam_idle_event->cb_info;
	if (cb_info == NULL) {
		LOGW("cb_info is NULL. event %d", cam_idle_event->event);
		goto IDLE_EVENT_CALLBACK_DONE;
	}

	/* remove event from list */
	g_mutex_lock(&cb_info->idle_event_mutex);
	if (cb_info->idle_event_list) {
		cb_info->idle_event_list = g_list_remove(cb_info->idle_event_list, (gpointer)cam_idle_event);
	}
	/*LOGD("remove idle event %p, %p", cam_idle_event, cb_info->idle_event_list);*/
	g_mutex_unlock(&cb_info->idle_event_mutex);

	/* user callback */
	_client_user_callback(cam_idle_event->cb_info, cam_idle_event->recvMsg, cam_idle_event->event);

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

static void _camera_remove_idle_event_all(callback_cb_info_s *cb_info)
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

					end_time = g_get_monotonic_time () + G_TIME_SPAN_MILLISECOND * 100;

					if (g_cond_wait_until(&cb_info->idle_event_cond, &cb_info->idle_event_mutex, end_time)) {
						LOGW("signal received");
					} else {
						LOGW("timeout");
					}
				}
			}
		}

		g_list_free(cb_info->idle_event_list);
		cb_info->idle_event_list = NULL;
	}

	g_mutex_unlock(&cb_info->idle_event_mutex);

	return;
}

static void *_client_cb_handler(gpointer data)
{
	int ret = 0;
	int api = 0;
	int num_token = 0;
	int i = 0;
	int str_pos = 0;
	int prev_pos = 0;
	callback_cb_info_s *cb_info = (callback_cb_info_s *)data;
	char *recvMsg = NULL;
	char **parseStr = NULL;

	if (cb_info == NULL) {
		LOGE("cb_info NULL");
		return NULL;
	}

	LOGD("start");

	parseStr = (char **)malloc(sizeof(char *) * CAMERA_PARSE_STRING_SIZE);
	if (parseStr == NULL) {
		LOGE("parseStr malloc failed");
		return NULL;
	}

	for (i = 0 ; i < CAMERA_PARSE_STRING_SIZE ; i++) {
		parseStr[i] = (char *)malloc(sizeof(char) * MUSE_CAMERA_MSG_MAX_LENGTH);
		if (parseStr[i] == NULL) {
			LOGE("parseStr[%d] malloc failed", i);
			goto CB_HANDLER_EXIT;
		}
	}

	recvMsg = cb_info->recvMsg;

	while (g_atomic_int_get(&cb_info->rcv_thread_running)) {
		ret = muse_core_ipc_recv_msg(cb_info->fd, recvMsg);
		if (ret <= 0)
			break;
		recvMsg[ret] = '\0';

		str_pos = 0;
		prev_pos = 0;
		num_token = 0;

		LOGD("recvMSg : %s, length : %d", recvMsg, ret);

		/* Need to split the combined entering msgs.
		    This module supports up to 200 combined msgs. */
		for (str_pos = 0; str_pos < ret; str_pos++) {
			if(recvMsg[str_pos] == '}') {
				memset(parseStr[num_token], 0x0, sizeof(char) * MUSE_CAMERA_MSG_MAX_LENGTH);
				strncpy(parseStr[num_token], recvMsg + prev_pos, str_pos - prev_pos + 1);
				LOGD("splitted msg : [%s], Index : %d", parseStr[num_token], num_token);
				prev_pos = str_pos+1;
				num_token++;
			}
		}

		LOGD("num_token : %d", num_token);

		/* Re-construct to the useful single msg. */
		for (i = 0; i < num_token; i++) {

			if (i >= CAMERA_PARSE_STRING_SIZE)
				break;

			if (muse_camera_msg_get(api, parseStr[i])) {
				if(api < MUSE_CAMERA_API_MAX){
					LOGD("Set Condition - api %d", api);
					g_mutex_lock(&(cb_info->pMutex[api]));

					/* The api msgs should be distinguished from the event msg. */
					memset(cb_info->recvApiMsg, 0, strlen(cb_info->recvApiMsg));
					strcpy(cb_info->recvApiMsg, parseStr[i]);
					LOGD("cb_info->recvApiMsg : [%s]", cb_info->recvApiMsg);
					cb_info->activating[api] = 1;
					g_cond_signal(&(cb_info->pCond[api]));
					g_mutex_unlock(&(cb_info->pMutex[api]));

					if (api == MUSE_CAMERA_API_CREATE) {
						if (muse_camera_msg_get(ret, cb_info->recvApiMsg)) {
							if (ret != CAMERA_ERROR_NONE) {
								g_atomic_int_set(&cb_info->rcv_thread_running, 0);
								LOGE("camera create error 0x%x. close client cb handler", ret);
							}
						} else {
							LOGE("failed to get api return");
						}
					} else if (api == MUSE_CAMERA_API_DESTROY) {
						if (muse_camera_msg_get(ret, cb_info->recvApiMsg)) {
							if (ret == CAMERA_ERROR_NONE) {
								g_atomic_int_set(&cb_info->rcv_thread_running, 0);
								LOGD("camera destroy done. close client cb handler");
							}
						} else {
							LOGE("failed to get api return");
						}
					}
				} else if(api == MUSE_CAMERA_CB_EVENT) {
					int event = -1;
					int class = -1;
					camera_event_s *cam_event = NULL;
					camera_idle_event_s *cam_idle_event = NULL;

					if (!muse_camera_msg_get(event, parseStr[i]) ||
					    !muse_camera_msg_get(class, parseStr[i])) {
						LOGE("failed to get event %d, class %d", event, class);
						continue;
					}

					switch (class) {
					case MUSE_CAMERA_EVENT_CLASS_NORMAL:
						cam_event = (camera_event_s *)malloc(sizeof(camera_event_s));
						if (cam_event) {
							cam_event->event = event;
							cam_event->cb_info = cb_info;
							memcpy(cam_event->recvMsg, recvMsg, sizeof(cam_event->recvMsg));

							LOGD("add event to EVENT QUEUE : %d", event);
							g_mutex_lock(&cb_info->event_mutex);
							g_queue_push_tail(cb_info->event_queue, (gpointer)cam_event);
							g_cond_signal(&cb_info->event_cond);
							g_mutex_unlock(&cb_info->event_mutex);
						} else {
							LOGE("cam_event alloc failed");
						}
						break;
					case MUSE_CAMERA_EVENT_CLASS_IMMEDIATE:
						_client_user_callback(cb_info, recvMsg, event);
						break;
					case MUSE_CAMERA_EVENT_CLASS_MAIN_THREAD:
						cam_idle_event = (camera_idle_event_s *)malloc(sizeof(camera_idle_event_s));
						if (cam_idle_event) {
							cam_idle_event->event = event;
							cam_idle_event->cb_info = cb_info;
							g_mutex_init(&cam_idle_event->event_mutex);
							memcpy(cam_idle_event->recvMsg, recvMsg, sizeof(cam_idle_event->recvMsg));

							LOGD("add event[%d] to IDLE %p", event, cam_idle_event);

							g_mutex_lock(&cb_info->idle_event_mutex);
							cb_info->idle_event_list = g_list_append(cb_info->idle_event_list, (gpointer)cam_idle_event);
							/*LOGD("add idle event %p, %p", cam_idle_event, cb_info->idle_event_list);*/
							g_mutex_unlock(&cb_info->idle_event_mutex);

							g_idle_add_full(G_PRIORITY_DEFAULT,
							                (GSourceFunc)_camera_idle_event_callback,
							                (gpointer)cam_idle_event,
							                NULL);
						} else {
							LOGE("cam_idle_event alloc failed");
						}
						break;
					default:
						LOGE("unknown class %d", class);
						break;
					}
				} else {
					LOGW("unknown api : %d", api);
				}
			}else{
				LOGE("Get Msg Failed");
			}
		}

	}

	LOGD("client cb exit");

CB_HANDLER_EXIT:
	if (parseStr) {
		for (i = 0 ; i < CAMERA_PARSE_STRING_SIZE ; i++) {
			if (parseStr[i]) {
				free(parseStr[i]);
				parseStr[i] = NULL;
			}
		}

		free(parseStr);
		parseStr = NULL;
	}

	return NULL;
}

static callback_cb_info_s *_client_callback_new(gint sockfd)
{
	callback_cb_info_s *cb_info;
	GCond *camera_cond;
	GMutex *camera_mutex;
	gint *camera_activ;
	g_return_val_if_fail(sockfd > 0, NULL);

	cb_info = g_new0(callback_cb_info_s, 1);
	camera_cond = g_new0(GCond, MUSE_CAMERA_API_MAX);
	camera_mutex = g_new0(GMutex, MUSE_CAMERA_API_MAX);
	camera_activ = g_new0(gint, MUSE_CAMERA_API_MAX);

	g_atomic_int_set(&cb_info->rcv_thread_running, 1);
	cb_info->fd = sockfd;
	cb_info->pCond = camera_cond;
	cb_info->pMutex = camera_mutex;
	cb_info->activating = camera_activ;
	cb_info->msg_rcv_thread = g_thread_new("msg_rcv_thread", _client_cb_handler, (gpointer)cb_info);

	g_atomic_int_set(&cb_info->event_thread_running, 1);
	g_mutex_init(&cb_info->event_mutex);
	g_cond_init(&cb_info->event_cond);
	g_mutex_init(&cb_info->idle_event_mutex);
	g_cond_init(&cb_info->idle_event_cond);
	cb_info->event_queue = g_queue_new();
	cb_info->event_thread = g_thread_new("event_thread", _event_handler, (gpointer)cb_info);

	return cb_info;
}

static void _client_callback_destroy(callback_cb_info_s * cb_info)
{
	g_return_if_fail(cb_info != NULL);

	LOGI("MSG receive thread[%p] destroy", cb_info->msg_rcv_thread);

	g_thread_join(cb_info->msg_rcv_thread);
	g_thread_unref(cb_info->msg_rcv_thread);
	cb_info->msg_rcv_thread = NULL;

	LOGD("msg thread removed");

	g_mutex_lock(&cb_info->event_mutex);
	g_atomic_int_set(&cb_info->event_thread_running, 0);
	g_cond_signal(&cb_info->event_cond);
	g_mutex_unlock(&cb_info->event_mutex);

	g_thread_join(cb_info->event_thread);
	g_thread_unref(cb_info->event_thread);
	cb_info->event_thread = NULL;

	g_queue_free(cb_info->event_queue);
	cb_info->event_queue = NULL;
	g_mutex_clear(&cb_info->event_mutex);
	g_cond_clear(&cb_info->event_cond);
	g_mutex_clear(&cb_info->idle_event_mutex);
	g_cond_clear(&cb_info->idle_event_cond);

	LOGD("event thread removed");

	if (cb_info->bufmgr) {
		tbm_bufmgr_deinit(cb_info->bufmgr);
		cb_info->bufmgr = NULL;
	}

	if (cb_info->pCond) {
		g_free(cb_info->pCond);
	}
	if (cb_info->pMutex) {
		g_free(cb_info->pMutex);
	}
	if (cb_info->activating) {
		g_free(cb_info->activating);
	}

	g_free(cb_info);

	return;
}

int camera_create(camera_device_e device, camera_h* camera)
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

	if (camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		ret = CAMERA_ERROR_OUT_OF_MEMORY;
		goto ErrorExit;
	}

	pc->cb_info = _client_callback_new(sock_fd);

	LOGD("cb info : %d", pc->cb_info->fd);

	ret = _client_wait_for_cb_return(api, pc->cb_info, CALLBACK_TIME_OUT);
	if (ret == CAMERA_ERROR_NONE) {
		intptr_t handle = 0;
		muse_camera_msg_get_pointer(handle, pc->cb_info->recvMsg);
		if (handle == 0) {
			LOGE("Receiving Handle Failed!!");
			ret = CAMERA_ERROR_INVALID_OPERATION;
			goto ErrorExit;
		}

		pc->remote_handle = handle;
		pc->cb_info->bufmgr = bufmgr;

		LOGD("camera create 0x%x", pc->remote_handle);
		*camera = (camera_h) pc;
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	muse_camera_api_e api = MUSE_CAMERA_API_DESTROY;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd = pc->cb_info->fd;
	LOGD("ENTER");

	if (pc == NULL) {
		LOGE("pc is already nul!!");
		return CAMERA_ERROR_INVALID_PARAMETER;
	} else if (pc->cb_info == NULL) {
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	if (ret == CAMERA_ERROR_NONE) {
		if (pc->client_handle) {
			mm_camcorder_client_destroy(pc->client_handle);
			pc->client_handle = NULL;
		}
		_camera_remove_idle_event_all(pc->cb_info);
		_client_callback_destroy(pc->cb_info);

		if (pc->wl_info) {
			g_free(pc->wl_info);
			pc->wl_info = NULL;
		}

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
	int prev_state = CAMERA_STATE_NONE;
	char caps[MUSE_CAMERA_MSG_MAX_LENGTH] = {0};

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	LOGD("start");

	sock_fd = pc->cb_info->fd;

	if (pc->client_handle == NULL) {
		LOGW("set display is not called by application. set NONE type internally");
		ret = camera_set_display(camera, CAMERA_DISPLAY_TYPE_NONE, NULL);
		if (ret != CAMERA_ERROR_NONE) {
			LOGE("Internal camera_set_display failed 0x%x", ret);
			return ret;
		}
	}

	muse_camera_msg_send_longtime(api, sock_fd, pc->cb_info, ret);

	if (ret != CAMERA_ERROR_NONE) {
		LOGE("start preview failed 0x%x", ret);
		return ret;
	}

	muse_camera_msg_get(prev_state, pc->cb_info->recvMsg);

	if (prev_state == CAMERA_STATE_CREATED) {
		muse_camera_msg_get_string(caps, pc->cb_info->recvMsg);
		if (caps == NULL) {
			LOGE("failed to get caps string");
			goto _START_PREVIEW_ERROR;
		}

		LOGD("caps : %s", caps);

		ret = mm_camcorder_client_realize(pc->client_handle, caps);
		if (ret != MM_ERROR_NONE) {
			LOGE("client realize failed 0x%x", ret);
			goto _START_PREVIEW_ERROR;
		}
	}

	LOGD("ret : 0x%x", ret);

	return CAMERA_ERROR_NONE;

_START_PREVIEW_ERROR:
	muse_camera_msg_send_longtime(MUSE_CAMERA_API_STOP_PREVIEW, sock_fd, pc->cb_info, ret);

	return CAMERA_ERROR_INVALID_OPERATION;
}

int camera_stop_preview(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	muse_camera_api_e api = MUSE_CAMERA_API_STOP_PREVIEW;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	sock_fd = pc->cb_info->fd;
	LOGD("Enter");
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret != CAMERA_ERROR_NONE) {
		LOGE("stop preview failed 0x%x", ret);
		return ret;
	}

	if (pc->client_handle != NULL) {
		if (mm_camcorder_client_unrealize(pc->client_handle) == MM_ERROR_NONE) {
			LOGD("client unrealize done");
		} else {
			LOGE("client unrealize failed. restart preview...");
			muse_camera_msg_send_longtime(MUSE_CAMERA_API_START_PREVIEW, sock_fd, pc->cb_info, ret);
			return CAMERA_ERROR_INVALID_OPERATION;
		}
	} else {
		LOGW("client handle is NULL");
	}

	LOGD("ret : 0x%x", ret);

	return ret;
}

int camera_start_capture(camera_h camera, camera_capturing_cb capturing_cb, camera_capture_completed_cb completed_cb, void *user_data)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	sock_fd = pc->cb_info->fd;

	if (capturing_cb != NULL) {
		is_capturing_cb = 1;
		pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = capturing_cb;
		pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = user_data;
	}

	if(completed_cb != NULL) {
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SUPPORT_CONTINUOUS_CAPTURE;
	int sock_fd;

	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = capturing_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = user_data;
	pc->cb_info->user_cb_completed[MUSE_CAMERA_EVENT_TYPE_CAPTURE] = completed_cb;

	muse_camera_msg_send2(api, sock_fd, pc->cb_info, ret, INT, count, INT, interval);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_stop_continuous_capture(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}


	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_STOP_CONTINUOUS_CAPTURE;
	LOGD("Enter,  handle :%x", pc->remote_handle);
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SUPPORT_FACE_DETECTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SUPPORT_ZERO_SHUTTER_LAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SUPPORT_MEDIA_PACKET_PREVIEW_CB;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_device_count;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_device_count, pc->cb_info->recvMsg);
		*device_count = get_device_count;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_start_face_detection(camera_h camera, camera_face_detected_cb callback, void * user_data)
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_STOP_FACE_DETECTION;
	LOGD("Enter,  handle :%x", pc->remote_handle);
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_STATE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_state;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_state, pc->cb_info->recvMsg);
		*state = (camera_state_e)get_state;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_start_focusing(camera_h camera, bool continuous)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_START_FOCUSING;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_CANCEL_FOCUSING;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	int set_surface = MM_DISPLAY_SURFACE_X;
	Evas_Object *obj = NULL;
	const char *object_type = NULL;
	char socket_path[MUSE_CAMERA_MSG_MAX_LENGTH] = {0,};
#ifdef HAVE_WAYLAND
	MMCamWaylandInfo *wl_info = NULL;
#endif /* HAVE_WAYLAND */

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x display : 0x%x", pc->remote_handle, display);

	if (type == CAMERA_DISPLAY_TYPE_NONE) {
		set_display_handle = 0;
		set_surface = MM_DISPLAY_SURFACE_NULL;
		LOGD("display type NONE");
	} else {
		obj = (Evas_Object *)display;
		object_type = evas_object_type_get(obj);
		if (object_type) {
			if (type == CAMERA_DISPLAY_TYPE_OVERLAY && !strcmp(object_type, "elm_win")) {
#ifdef HAVE_WAYLAND
				/* set wayland info */
				wl_info = _get_wl_info(obj);
				if (wl_info == NULL) {
					LOGE("failed to get wl_info");
					return CAMERA_ERROR_INVALID_OPERATION;
				}

				set_display_handle = (void *)wl_info;
#else /* HAVE_WAYLAND */
				/* x window overlay surface */
				set_display_handle = (void *)elm_win_xwindow_get(obj);
#endif
				set_surface = MM_DISPLAY_SURFACE_X;
				LOGD("display type OVERLAY : handle %p", set_display_handle);
			} else if (type == CAMERA_DISPLAY_TYPE_EVAS && !strcmp(object_type, "image")) {
				/* evas object surface */
				set_display_handle = (void *)display;
				set_surface = MM_DISPLAY_SURFACE_EVAS;
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

	pc->cli_display_handle = (intptr_t)set_display_handle;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		if (pc->client_handle == NULL) {
			ret = mm_camcorder_client_create(&pc->client_handle);
			if (ret != MM_ERROR_NONE) {
				LOGE("camera client create Failed 0x%x", ret);
				goto _SET_DISPLAY_ERROR;
			}
		}

		muse_camera_msg_get_string(socket_path, pc->cb_info->recvMsg);
		if (socket_path == NULL) {
			LOGE("failed to get socket path");
			goto _SET_DISPLAY_ERROR;
		}

		LOGD("shmsrc socket path : %s", socket_path);

		ret = mm_camcorder_client_set_shm_socket_path(pc->client_handle, socket_path);
		if (ret != MM_ERROR_NONE) {
			LOGE("failed to set socket path 0x%x", ret);
			goto _SET_DISPLAY_ERROR;
		}

		ret = mm_camcorder_set_attributes(pc->client_handle, NULL,
		                                  MMCAM_DISPLAY_SURFACE, set_surface,
		                                  NULL);
		if (ret != MM_ERROR_NONE) {
			LOGE("set display surface failed 0x%x", ret);
			goto _SET_DISPLAY_ERROR;
		}

		if (type != CAMERA_DISPLAY_TYPE_NONE) {
			ret = mm_camcorder_set_attributes(pc->client_handle, NULL,
			                                  MMCAM_DISPLAY_HANDLE, set_display_handle, sizeof(void *),
			                                  NULL);
			if (ret != MM_ERROR_NONE) {
				LOGE("set display handle failed 0x%x", ret);
				goto _SET_DISPLAY_ERROR;
			}
		}

		if (pc->wl_info) {
			g_free(pc->wl_info);
			pc->wl_info = NULL;
		}

		pc->wl_info = wl_info;

		return CAMERA_ERROR_NONE;;
	} else {
		LOGE("set display error - 0x%x");
		return ret;
	}

_SET_DISPLAY_ERROR:
	if (wl_info) {
		g_free(wl_info);
		wl_info = NULL;
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_preview_resolution(camera_h camera,  int width, int height)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_PREVIEW_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_CAPTURE_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	int set_format = (int)format;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_CAPTURE_FORMAT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	int set_format = (int)format;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_PREVIEW_FORMAT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}


	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_PREVIEW_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_width, pc->cb_info->recvMsg);
		muse_camera_msg_get(get_height, pc->cb_info->recvMsg);
		*width = get_width;
		*height = get_height;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_display_rotation(camera_h camera, camera_rotation_e rotation)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_DISPLAY_ROTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_rotation = (int)rotation;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_rotation);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_display_rotation(camera_h camera, camera_rotation_e *rotation)
{
	if( camera == NULL || rotation == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_DISPLAY_ROTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_rotation;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_rotation, pc->cb_info->recvMsg);
		*rotation = (camera_rotation_e)get_rotation;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_display_flip(camera_h camera, camera_flip_e flip)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_DISPLAY_FLIP;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_flip = (int)flip;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_flip);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_display_flip(camera_h camera, camera_flip_e *flip)
{
	if( camera == NULL || flip == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_DISPLAY_FLIP;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_flip;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_flip, pc->cb_info->recvMsg);
		*flip = (camera_flip_e)get_flip;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_display_visible(camera_h camera, bool visible)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_DISPLAY_VISIBLE;
	int set_visible = (int)visible;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_visible);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_is_display_visible(camera_h camera, bool* visible)
{
	if( camera == NULL || visible == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_IS_DISPLAY_VISIBLE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_visible;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_visible, pc->cb_info->recvMsg);
		*visible = (bool)get_visible;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_display_mode(camera_h camera, camera_display_mode_e mode)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	int set_mode = (int)mode;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_DISPLAY_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_display_mode(camera_h camera, camera_display_mode_e* mode)
{
	if( camera == NULL || mode == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_DISPLAY_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_display_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_capture_resolution(camera_h camera, int *width, int *height)
{
	if( camera == NULL || width== NULL || height == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_CAPTURE_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_width, pc->cb_info->recvMsg);
		muse_camera_msg_get(get_height, pc->cb_info->recvMsg);
		*width = get_width;
		*height = get_height;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_capture_format(camera_h camera, camera_pixel_format_e *format)
{
	if( camera == NULL || format == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_CAPTURE_FORMAT;
	int get_format;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_format, pc->cb_info->recvMsg);
		*format = (camera_pixel_format_e)get_format;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_get_preview_format(camera_h camera, camera_pixel_format_e *format)
{
	if( camera == NULL || format == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_PREVIEW_FORMAT;
	int get_format;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_format, pc->cb_info->recvMsg);
		*format = (camera_pixel_format_e)get_format;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_preview_cb(camera_h camera, camera_preview_cb callback, void* user_data)
{
	if (camera == NULL || callback == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_PREVIEW_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_PREVIEW] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_PREVIEW] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_media_packet_preview_cb(camera_h camera, camera_media_packet_preview_cb callback, void* user_data)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (callback == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - callback", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_MEDIA_PACKET_PREVIEW_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_state_changed_cb(camera_h camera, camera_state_changed_cb callback, void* user_data)
{
	if( camera == NULL || callback == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_STATE_CHANGED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_STATE_CHANGED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || callback == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_INTERRUPTED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_INTERRUPTED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_INTERRUPTED] = (void *)NULL;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_INTERRUPTED] = (void *)NULL;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_set_focus_changed_cb(camera_h camera, camera_focus_changed_cb callback, void* user_data)
{
	if( camera == NULL || callback == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOCUS_CHANGED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_FOCUS_CHANGED_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || callback == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_ERROR_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_UNSET_ERROR_CB;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOREACH_SUPPORTED_PREVIEW_RESOLUTION;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOREACH_SUPPORTED_CAPTURE_RESOLUTION;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOREACH_SUPPORTED_CAPTURE_FORMAT;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_SET_FOREACH_SUPPORTED_PREVIEW_FORMAT;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_GET_RECOMMENDED_PREVIEW_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_width, pc->cb_info->recvMsg);
		muse_camera_msg_get(get_height, pc->cb_info->recvMsg);
		*width = get_width;
		*height = get_height;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_lens_orientation(camera_h camera, int *angle)
{
	if( camera == NULL || angle == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_LENS_ORIENTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_angle;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_angle, pc->cb_info->recvMsg);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recvMsg);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_PREVIEW_FPS;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_IMAGE_QUALITY;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || fps == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_PREVIEW_FPS;
	int get_fps;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_fps, pc->cb_info->recvMsg);
		*fps = (camera_attr_fps_e)get_fps;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_image_quality(camera_h camera, int *quality)
{
	if( camera == NULL || quality == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_IMAGE_QUALITY;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_quality;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_quality, pc->cb_info->recvMsg);
		*quality = get_quality;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_zoom(camera_h camera, int zoom)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_ZOOM;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_AF_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_CLEAR_AF_AREA;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (mode < CAMERA_ATTR_EXPOSURE_MODE_OFF || mode > CAMERA_ATTR_EXPOSURE_MODE_CUSTOM) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_EXPOSURE_MODE;
	int set_mode = (int)mode;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_EXPOSURE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_ISO;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_BRIGHTNESS;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (wb < CAMERA_ATTR_WHITE_BALANCE_NONE || wb > CAMERA_ATTR_WHITE_BALANCE_CUSTOM) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_WHITEBALANCE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_EFFECT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_SCENE_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_ENABLE_TAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if( description == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_TAG_IMAGE_DESCRIPTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_TAG_ORIENTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if( software == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_TAG_SOFTWARE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_GEOTAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_REMOVE_GEOTAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_FLASH_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || zoom == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_ZOOM;
	int get_zoom;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_zoom, pc->cb_info->recvMsg);
		*zoom = get_zoom;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_zoom_range(camera_h camera, int *min, int *max)
{
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_ZOOM_RANGE;
	int get_min;
	int get_max;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_min, pc->cb_info->recvMsg);
		muse_camera_msg_get(get_max, pc->cb_info->recvMsg);
		*min = get_min;
		*max = get_max;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_af_mode( camera_h camera,  camera_attr_af_mode_e *mode)
{
	if( camera == NULL || mode == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_AF_MODE;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_af_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_exposure_mode( camera_h camera, camera_attr_exposure_mode_e *mode)
{
	if( camera == NULL|| mode == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_EXPOSURE_MODE;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_exposure_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_get_exposure(camera_h camera, int *value)
{
	if( camera == NULL || value == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_EXPOSURE;
	int get_value;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_value, pc->cb_info->recvMsg);
		*value = get_value;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_exposure_range(camera_h camera, int *min, int *max)
{
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_EXPOSURE_RANGE;
	int get_min;
	int get_max;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_min, pc->cb_info->recvMsg);
		muse_camera_msg_get(get_max, pc->cb_info->recvMsg);
		*min = get_min;
		*max = get_max;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_iso( camera_h camera,  camera_attr_iso_e *iso)
{
	if( camera == NULL || iso == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_ISO;
	int get_iso;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_iso, pc->cb_info->recvMsg);
		*iso = (camera_attr_iso_e)get_iso;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_brightness(camera_h camera,  int *level)
{
	if( camera == NULL || level == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_BRIGHTNESS;
	int get_level;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_level, pc->cb_info->recvMsg);
		*level = get_level;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_brightness_range(camera_h camera, int *min, int *max)
{
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_BRIGHTNESS_RANGE;
	int get_min;
	int get_max;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_min, pc->cb_info->recvMsg);
		muse_camera_msg_get(get_max, pc->cb_info->recvMsg);
		*min = get_min;
		*max = get_max;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_contrast(camera_h camera,  int *level)
{
	if( camera == NULL || level == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_level;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_level, pc->cb_info->recvMsg);
		*level = get_level;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_contrast_range(camera_h camera, int *min , int *max)
{
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_CONTRAST_RANGE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_min;
	int get_max;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_min, pc->cb_info->recvMsg);
		muse_camera_msg_get(get_max, pc->cb_info->recvMsg);
		*min = get_min;
		*max = get_max;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_whitebalance(camera_h camera,  camera_attr_whitebalance_e *wb)
{
	if( camera == NULL || wb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_WHITEBALANCE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_wb;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_wb, pc->cb_info->recvMsg);
		*wb = (camera_attr_whitebalance_e)get_wb;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_effect(camera_h camera, camera_attr_effect_mode_e *effect)
{
	if( camera == NULL || effect == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_EFFECT;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_effect;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_effect, pc->cb_info->recvMsg);
		*effect = (camera_attr_effect_mode_e)get_effect;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_scene_mode(camera_h camera,  camera_attr_scene_mode_e *mode)
{
	if( camera == NULL || mode == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_SCENE_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_scene_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_is_enabled_tag(camera_h camera,  bool *enable)
{
	if( camera == NULL || enable == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_ENABLED_TAG;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_enabled, pc->cb_info->recvMsg);
		*enable = (bool)get_enabled;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_tag_image_description(camera_h camera,  char **description)
{
	if( camera == NULL || description == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_TAG_IMAGE_DESCRIPTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	char get_description[MUSE_CAMERA_MSG_MAX_LENGTH] = {0,};

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get_string(get_description, pc->cb_info->recvMsg);
		*description = strdup(get_description);
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_tag_orientation(camera_h camera, camera_attr_tag_orientation_e *orientation)
{
	if( camera == NULL || orientation == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_TAG_ORIENTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_orientation;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_orientation, pc->cb_info->recvMsg);
		*orientation = (camera_attr_tag_orientation_e)get_orientation;
		LOGD("success, orientation : %d", *orientation);
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_tag_software(camera_h camera, char **software)
{
	if( camera == NULL || software == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_TAG_SOFTWARE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	char get_software[MUSE_CAMERA_MSG_MAX_LENGTH] = {0,};

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get_string(get_software, pc->cb_info->recvMsg);
		*software = strdup(get_software);
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_get_geotag(camera_h camera, double *latitude , double *longitude, double *altitude)
{
	if( camera == NULL || latitude == NULL || longitude == NULL || altitude == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_GEOTAG;
	double get_geotag[3] = {0,};
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int valid = 0;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get_array(get_geotag, pc->cb_info->recvMsg);
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
	if( camera == NULL || mode == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_FLASH_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_flash_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_af_mode( camera_h camera, camera_attr_supported_af_mode_cb foreach_cb , void *user_data)
{
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_AF_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_EXPOSURE_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE] = foreach_cb;
	pc->cb_info->user_data[MUSE_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE] = user_data;

	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_foreach_supported_iso( camera_h camera, camera_attr_supported_iso_cb foreach_cb , void *user_data)
{
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_ISO;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_WHITEBALANCE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_EFFECT;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_SCENE_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_FLASH_MODE;

	LOGD("Enter, handle :%x", pc->remote_handle);

	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_FPS;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_FPS_BY_RESOLUTION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_STREAM_FLIP;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_FOREACH_SUPPORTED_STREAM_ROTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_STREAM_ROTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || rotation == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_STREAM_ROTATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_rotation;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_rotation, pc->cb_info->recvMsg);
		*rotation = (camera_rotation_e)get_rotation;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


int camera_attr_set_stream_flip(camera_h camera , camera_flip_e flip)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_STREAM_FLIP;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL || flip == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_STREAM_FLIP;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_flip;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_flip, pc->cb_info->recvMsg);
		*flip = (camera_flip_e)get_flip;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}

int camera_attr_set_hdr_mode(camera_h camera, camera_attr_hdr_mode_e mode)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_SET_HDR_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x) - handle",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (mode == NULL) {
		LOGE("CAMERA_ERROR_NOT_SUPPORTED(0x%08x) - mode",CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_GET_HDR_MODE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_hdr_mode_e)get_mode;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


bool camera_attr_is_supported_hdr_capture(camera_h camera)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_SUPPORTED_HDR_CAPTURE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);
	LOGD("ret : 0x%x", ret);
	return (bool)ret;
}


int camera_attr_set_hdr_capture_progress_cb(camera_h camera, camera_attr_hdr_progress_cb callback, void* user_data)
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_UNSET_HDR_CAPTURE_PROGRESS_CB;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_ENABLE_ANTI_SHAKE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_enabled, pc->cb_info->recvMsg);
		*enabled = (bool)get_enabled;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


bool camera_attr_is_supported_anti_shake(camera_h camera)
{

	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_SUPPORTED_ANTI_SHAKE;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_ENABLE_VIDEO_STABILIZATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x) - handle",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	if (enabled == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x) - enabled",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_ENABLED_VIDEO_STABILIZATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_enabled, pc->cb_info->recvMsg);
		*enabled = (bool)get_enabled;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


bool camera_attr_is_supported_video_stabilization(camera_h camera)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_SUPPORTED_VIDEO_STABILIZATION;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_ENABLE_AUTO_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send(api, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		muse_camera_msg_get(get_enabled, pc->cb_info->recvMsg);
		*enabled = (bool)get_enabled;
	}
	LOGD("ret : 0x%x", ret);
	return ret;
}


bool camera_attr_is_supported_auto_contrast(camera_h camera)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_IS_SUPPORTED_AUTO_CONTRAST;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	muse_camera_api_e api = MUSE_CAMERA_API_ATTR_DISABLE_SHUTTER_SOUND;
	int sock_fd;
	if (pc->cb_info == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	sock_fd = pc->cb_info->fd;
	int set_disable = (int)disable;

	LOGD("Enter, remote_handle : %x", pc->remote_handle);
	muse_camera_msg_send1(api, sock_fd, pc->cb_info, ret, INT, set_disable);
	LOGD("ret : 0x%x", ret);
	return ret;
}
