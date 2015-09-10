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
#include <mused_camera.h>
#include <mused_camera_msg.h>
#include <mmsvc_core_ipc.h>
#include <camera_private.h>
#include <mmsvc_core.h>
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

//int mmsvc_camera_ipc_import_tbm(mmsvc_camera_transport_info_s *transport_info);
//export int mmsvc_camera_ipc_export_tbm_bo(mmsvc_camera_transport_info_s transport_info);

static void _client_user_callback(callback_cb_info_s * cb_info, mmsvc_camera_event_e event )
{
	char *recvMsg = cb_info->recvMsg;
	int param, param1, param2;
	LOGW("get event %d", event);

	switch (event) {
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_RESOLUTION:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_RESOLUTION:
			mmsvc_camera_msg_get(param1, recvMsg);
			mmsvc_camera_msg_get(param2, recvMsg);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_THEATER_MODE:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_FORMAT:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_FORMAT:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_AF_MODE:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_ISO:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_WHITEBALANCE:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EFFECT:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_SCENE_MODE:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FLASH_MODE:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS_BY_RESOLUTION:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_FLIP:
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_ROTATION:
			mmsvc_camera_msg_get(param, recvMsg);
			break;
		default:
			break;
	}

	switch(event) {
		case MMSVC_CAMERA_EVENT_TYPE_STATE_CHANGE:
		{
			int cb_previous, cb_current, cb_by_policy;
			mmsvc_camera_msg_get(cb_previous, recvMsg);
			mmsvc_camera_msg_get(cb_current, recvMsg);
			mmsvc_camera_msg_get(cb_by_policy, recvMsg);
			((camera_state_changed_cb)cb_info->user_cb[event])((camera_state_e)cb_previous,
													(camera_state_e)cb_current,
													(bool)cb_by_policy,
													cb_info->user_data[event]);
			break;
		}
		case MMSVC_CAMERA_EVENT_TYPE_FOCUS_CHANGE:
		{
			int cb_state;
			mmsvc_camera_msg_get(cb_state, recvMsg);
			((camera_focus_changed_cb)cb_info->user_cb[event])((camera_focus_state_e)cb_state,
													cb_info->user_data[event]);
			break;
		}
		case MMSVC_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE:
			((camera_capture_completed_cb)cb_info->user_cb[event])(cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_PREVIEW:
			((camera_preview_cb)cb_info->user_cb[event])(NULL,
													cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW:
			((camera_media_packet_preview_cb)cb_info->user_cb[event])(NULL,
															cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_HDR_PROGRESS:
		{
			int progress;
			mmsvc_camera_msg_get(progress, recvMsg);
			((camera_attr_hdr_progress_cb)cb_info->user_cb[event])(progress,
															cb_info->user_data[event]);
			break;
		}
		case MMSVC_CAMERA_EVENT_TYPE_INTERRUPTED:
		{
			int cb_policy, cb_previous, cb_current;
			mmsvc_camera_msg_get(cb_policy, recvMsg);
			mmsvc_camera_msg_get(cb_previous, recvMsg);
			mmsvc_camera_msg_get(cb_current, recvMsg);
			((camera_interrupted_cb)cb_info->user_cb[event])((camera_policy_e)cb_policy,
														(camera_state_e)cb_previous,
														(camera_state_e)cb_current,
														cb_info->user_data[event]);
			break;
		}
		case MMSVC_CAMERA_EVENT_TYPE_FACE_DETECTION:
		{
			int count;
			mmsvc_camera_msg_get(count, recvMsg);
			((camera_face_detected_cb)cb_info->user_cb[event])(NULL,
													count,
													cb_info->user_data[event]);
			break;
		}
		case MMSVC_CAMERA_EVENT_TYPE_ERROR:
		{
			int cb_error, cb_current_state;
			mmsvc_camera_msg_get(cb_error, recvMsg);
			mmsvc_camera_msg_get(cb_current_state, recvMsg);
			((camera_error_cb)cb_info->user_cb[event])((camera_error_e)cb_error,
													(camera_state_e)cb_current_state,
													cb_info->user_data[event]);
			break;
		}
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_RESOLUTION:
			((camera_supported_preview_resolution_cb)cb_info->user_cb[event])(param1, param2,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_RESOLUTION:
			((camera_supported_capture_resolution_cb)cb_info->user_cb[event])(param1, param2,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_FORMAT:
			((camera_supported_capture_format_cb)cb_info->user_cb[event])((camera_pixel_format_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_FORMAT:
			((camera_supported_preview_format_cb)cb_info->user_cb[event])((camera_pixel_format_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_AF_MODE:
			((camera_attr_supported_af_mode_cb)cb_info->user_cb[event])((camera_attr_af_mode_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE:
			((camera_attr_supported_exposure_mode_cb)cb_info->user_cb[event])((camera_attr_exposure_mode_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_ISO:
			((camera_attr_supported_iso_cb)cb_info->user_cb[event])((camera_attr_iso_e)param,
															cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_WHITEBALANCE:
			((camera_attr_supported_whitebalance_cb)cb_info->user_cb[event])((camera_attr_whitebalance_e)param,
																		cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EFFECT:
			((camera_attr_supported_effect_cb)cb_info->user_cb[event])((camera_attr_effect_mode_e)param,
																cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_SCENE_MODE:
			((camera_attr_supported_scene_mode_cb)cb_info->user_cb[event])((camera_attr_scene_mode_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FLASH_MODE:
			((camera_attr_supported_flash_mode_cb)cb_info->user_cb[event])((camera_attr_flash_mode_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS:
			((camera_attr_supported_fps_cb)cb_info->user_cb[event])((camera_attr_fps_e)param,
															cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS_BY_RESOLUTION:
			((camera_attr_supported_fps_cb)cb_info->user_cb[event])((camera_attr_fps_e)param,
															cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_FLIP:
			((camera_attr_supported_stream_flip_cb)cb_info->user_cb[event])((camera_flip_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_ROTATION:
			((camera_attr_supported_stream_rotation_cb)cb_info->user_cb[event])((camera_rotation_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_THEATER_MODE:
			((camera_attr_supported_theater_mode_cb)cb_info->user_cb[event])((camera_attr_theater_mode_e)param,
																	cb_info->user_data[event]);
			break;
		case MMSVC_CAMERA_EVENT_TYPE_CAPTURE:
		{
			camera_image_data_s *rImage = NULL;
			camera_image_data_s *rPostview = NULL;
			camera_image_data_s *rThumbnail = NULL;
			unsigned char *buf_pos = NULL;
			mmsvc_camera_transport_info_s transport_info;
			int tKey = 0;
			int is_postview = 0;
			int is_thumbnail = 0;

			LOGW("camera2 capture callback came in.");
			mmsvc_camera_msg_get(tKey, recvMsg);
			mmsvc_camera_msg_get(is_postview, recvMsg);
			mmsvc_camera_msg_get(is_thumbnail, recvMsg);

			if (tKey != 0) {
				transport_info.tbm_key = tKey;
				LOGE("Read key_info INFO : %d", transport_info.tbm_key);

				if (mmsvc_camera_ipc_init_tbm(&transport_info) == FALSE) {
					LOGE("camera_init_tbm ERROR!!");
					break;					
				}

				if(mmsvc_camera_ipc_import_tbm(&transport_info) == FALSE) {
					LOGE("camera_import_tbm ERROR!!");
					mmsvc_camera_unref_tbm(&transport_info);
					break;
				} else {
					if (transport_info.bo_handle.ptr != NULL) {

						buf_pos = (unsigned char *)transport_info.bo_handle.ptr;
						rImage = (camera_image_data_s *)buf_pos;
						LOGE(" !! rImage->size : %d", rImage->size);
						rImage->data = buf_pos + sizeof(camera_image_data_s);
						buf_pos += sizeof(camera_image_data_s) + rImage->size;
						if (is_postview) {
							rPostview = (camera_image_data_s *)buf_pos;
							LOGE(" !! rPostview->size : %d", rPostview->size);
							rPostview->data = buf_pos + sizeof(camera_image_data_s);
							buf_pos += sizeof(camera_image_data_s) + rPostview->size;
						}
						if (is_thumbnail) {
							rThumbnail = (camera_image_data_s *)buf_pos;
							rThumbnail->data = buf_pos + sizeof(camera_image_data_s);
							buf_pos += sizeof(camera_image_data_s) + rThumbnail->size;
						}
					}
				}
			} else {
				LOGE("Get KEY INFO sock msg ERROR!!");
				break;
			}

			LOGW("read image info height: %d, width : %d, size : %d", rImage->height, rImage->width, rImage->size);

			((camera_capturing_cb)cb_info->user_cb[event])(rImage, rPostview, rThumbnail, cb_info->user_data[event]);

			mmsvc_camera_unref_tbm(&transport_info);

			LOGW("finished capture callback");
			break;
		}
		case MMSVC_CAMERA_EVENT_TYPE_VIDEO_FRAME_RENDER_ERROR:
			break;

		default:
			LOGE("Unknonw event");
			break;
	}
}

static void *client_cb_handler(gpointer data)
{
	int ret;
	int api;
	int num_token = 0;
	int i = 0;
	int str_pos = 0;
	int prev_pos = 0;
	callback_cb_info_s *cb_info = data;
	char *recvMsg = cb_info->recvMsg;
	char parseStr[MMSVC_PARSE_STRING_SIZE][MMSVC_MSG_MAX_LENGTH] = {{0,0},};

	while (g_atomic_int_get(&cb_info->running)) {
		ret = mmsvc_core_ipc_recv_msg(cb_info->fd, recvMsg);
		if (ret <= 0)
			break;
		recvMsg[ret] = '\0';

		str_pos = 0;
		prev_pos = 0;
		num_token = 0;
		memset(parseStr, 0, MMSVC_PARSE_STRING_SIZE * MMSVC_MSG_MAX_LENGTH);

		LOGW("recvMSg : %s, length : %d", recvMsg, ret);

		/* Need to split the combined entering msgs. 
 		    This module supports up to 5 combined msgs. */
		for (str_pos = 0; str_pos < ret; str_pos++) {
			if(recvMsg[str_pos] == '}') {
				strncpy(&(parseStr[num_token][0]), recvMsg + prev_pos, str_pos - prev_pos + 1);
				LOGW("splitted msg : %s, Index : %d", &(parseStr[num_token][0]), num_token);
				prev_pos = str_pos+1;
				num_token++;
			}
		}
		LOGW("num_token : %d", num_token);

		/* Re-construct to the useful single msg. */
		for (i = 0; i < num_token; i++) {

			if (i >= MMSVC_PARSE_STRING_SIZE)
				break;

			if (mmsvc_camera_msg_get(api, &(parseStr[i][0]))) {
				if(api < MMSVC_CAMERA_API_MAX){
					LOGW("Set Condition");
					g_mutex_lock(&(cb_info->pMutex[api]));
					/* The api msgs should be distinguished from the event msg. */
					memset(cb_info->recvApiMsg, 0, strlen(cb_info->recvApiMsg));
					strcpy(cb_info->recvApiMsg, &(parseStr[i][0]));
					LOGW("cb_info->recvApiMsg : %s", cb_info->recvApiMsg);
					cb_info->activating[api] = 1;
					g_cond_signal(&(cb_info->pCond[api]));
					g_mutex_unlock(&(cb_info->pMutex[api]));
					//msleep(100);
					if(api == MMSVC_CAMERA_API_DESTROY) {
						g_atomic_int_set(&cb_info->running, 0);
						LOGW("close client cb handler");
					}

				} else if(api == MMSVC_CAMERA_CB_EVENT) {
					int event;
					if (mmsvc_camera_msg_get(event, &(parseStr[i][0]))) {
						LOGW("go callback : %d", event);
						_client_user_callback(cb_info, event);
					}
				}
			}else{
				LOGE("Get Msg Failed");
			}
		}

	}
	LOGW("client cb exit");

	return NULL;
}

static callback_cb_info_s *client_callback_new(gint sockfd)
{
	callback_cb_info_s *cb_info;
	GCond *camera_cond;
	GMutex *camera_mutex;
	gint *camera_activ;
	g_return_val_if_fail(sockfd > 0, NULL);

	cb_info = g_new0(callback_cb_info_s, 1);
	camera_cond = g_new0(GCond, MMSVC_CAMERA_API_MAX);
	camera_mutex = g_new0(GMutex, MMSVC_CAMERA_API_MAX);
	camera_activ = g_new0(gint, MMSVC_CAMERA_API_MAX);

	g_atomic_int_set(&cb_info->running, 1);
	cb_info->fd = sockfd;
	cb_info->pCond = camera_cond;
	cb_info->pMutex = camera_mutex;
	cb_info->activating = camera_activ;
	cb_info->thread =
		g_thread_new("callback_thread", client_cb_handler,
			     (gpointer) cb_info);

	return cb_info;
}

static int client_wait_for_cb_return(mmsvc_camera_api_e api, callback_cb_info_s *cb_info, int time_out)
{
	int ret = CAMERA_ERROR_NONE;
	gint64 end_time;

	LOGW("Enter api : %d", api);
	g_mutex_lock(&(cb_info->pMutex[api]));

	if (cb_info->activating[api] == 0) {
		end_time = g_get_monotonic_time() + time_out * G_TIME_SPAN_SECOND;
		if (g_cond_wait_until(&(cb_info->pCond[api]), &(cb_info->pMutex[api]), end_time)) {
			LOGW("cb_info->recvApiMsg : %s", cb_info->recvApiMsg);
			if (!mmsvc_camera_msg_get(ret, cb_info->recvApiMsg)) {
				LOGE("Get cb msg failed.");
				ret = CAMERA_ERROR_INVALID_OPERATION;
			} else {
				LOGW("Wait passed, ret : 0x%x", ret);
			}
			if (cb_info->activating[api])
				cb_info->activating[api] = 0;
		} else {
			ret = CAMERA_ERROR_INVALID_OPERATION;
		}
	} else {
		LOGE("condition is already checked for the api : %d.", api);
		if (!mmsvc_camera_msg_get(ret, cb_info->recvApiMsg)) {
			LOGE("Get cb msg failed.");
			ret = CAMERA_ERROR_INVALID_OPERATION;
		} else {
			LOGW("Already checked condition, Wait passed, ret : 0x%x", ret);
		}	
	}
	g_mutex_unlock(&(cb_info->pMutex[api]));
	LOGW("ret : 0x%x", ret);
	return ret;
}

static void client_callback_destroy(callback_cb_info_s * cb_info)
{
	g_return_if_fail(cb_info != NULL);

	LOGI("%p Callback destroyed", cb_info->thread);

	g_thread_join(cb_info->thread);
	g_thread_unref(cb_info->thread);

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
}

int camera_create(camera_device_e device, camera_h* camera)
{
	if (camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int sock_fd = -1;
	char *sndMsg;
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = NULL;

	mmsvc_camera_api_e api = MMSVC_CAMERA_API_CREATE;
	mmsvc_api_client_e api_client = MMSVC_CAMERA;
	int device_type = (int)device;

	sock_fd = mmsvc_core_client_new();

	sndMsg = mmsvc_core_msg_json_factory_new(api, "client", api_client,
											      MUSED_TYPE_INT, PARAM_DEVICE_TYPE, (int)device_type,
											      0);
	mmsvc_core_ipc_send_msg(sock_fd, sndMsg);
	LOGW("sock_fd : %d, msg : %s", sock_fd, sndMsg);
	mmsvc_core_msg_json_factory_free(sndMsg);

	pc = g_new0(camera_cli_s, 1);
	if (pc == NULL) {
		return 0;
	}

	pc->cb_info = client_callback_new(sock_fd);
	LOGW("cb info : %d", pc->cb_info->fd);

	ret = client_wait_for_cb_return(api, pc->cb_info, CALLBACK_TIME_OUT);
	LOGW("ret value : 0x%x", ret);
	if (ret == CAMERA_ERROR_NONE) {
		intptr_t handle = 0;

		mmsvc_camera_msg_get_pointer(handle, pc->cb_info->recvMsg);
		if (handle == 0) {
			LOGE("Receiving Handle Failed!!");
			goto ErrorExit;
		} else {
			pc->remote_handle = handle;
		}
		LOGW("camera create 0x%x", pc->remote_handle);
		*camera = (camera_h) pc;
	} else
		goto ErrorExit;

	return ret;

ErrorExit:
	g_free(pc);
	LOGW("ret value : %d", ret);
	return ret;
}

 int camera_destroy(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_NONE;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_DESTROY;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd = pc->cb_info->fd;
	LOGW("ENTER");

	if (pc == NULL) {
		LOGW("pc is already nul!!");
		return CAMERA_ERROR_INVALID_OPERATION;
	} else if (pc->cb_info == NULL) {
		return CAMERA_ERROR_INVALID_OPERATION;
	}

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	if(ret == CAMERA_ERROR_NONE) {
		LOGW("destroy client");

	}
	client_callback_destroy(pc->cb_info);

	if (pc != NULL) {
		g_free(pc);
		pc = NULL;
	}
	LOGW("ret : 0x%x", ret);
	return ret;
}

int camera_start_preview(camera_h camera)
{
	LOGW("start");
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_START_PREVIEW;
	int sock_fd = pc->cb_info->fd;
	char caps[MM_MSG_MAX_LENGTH] = {0};

	LOGW("Enter,  handle :0x%x", pc->remote_handle);
	mmsvc_camera_msg_send_longtime(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("Enter,  ret :0x%x", ret);
	if(ret == CAMERA_ERROR_NONE) {
		LOGW("Enter");
		mmsvc_camera_msg_get_string(caps, pc->cb_info->recvMsg);
		LOGW("caps : %s", caps);
		if (pc->cli_display_handle != 0) {
			LOGW("client's display handle is : 0x%x", pc->cli_display_handle);
			if(strlen(caps) > 0 &&
					mm_camcorder_client_realize(pc->client_handle, caps) != MM_ERROR_NONE)
				ret = CAMERA_ERROR_INVALID_OPERATION;
		} else {
			LOGW("display handle is NULL");
		}
	}
	LOGW("ret : 0x%x", ret);
	return ret;
}

int camera_stop_preview(camera_h camera)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;
	camera_cli_s *pc = (camera_cli_s *)camera;
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_STOP_PREVIEW;
	LOGW("Enter,  handle :%x", pc->remote_handle);

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if(ret == CAMERA_ERROR_NONE) {
		if (pc->cli_display_handle != 0) {
			LOGW("Unrealize client");
			ret = mm_camcorder_client_unrealize(pc->client_handle);
			if (pc->client_handle != NULL) {
				mm_camcorder_client_destroy(pc->client_handle);
			}
		} else {
			LOGW("Client did not realized : Display handle is NULL");
		}
	}
	LOGW("ret : 0x%x", ret);
	return ret;
}

int camera_start_capture(camera_h camera, camera_capturing_cb capturing_cb , camera_capture_completed_cb completed_cb , void *user_data)
{
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_START_CAPTURE;
	int sock_fd = pc->cb_info->fd;
	int is_capturing_cb = 0;
	int is_completed_cb = 0;
	LOGW("Enter, handle :%x", pc->remote_handle);

	if (capturing_cb != NULL) {
		is_capturing_cb = 1;
		pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_CAPTURE] = capturing_cb;
		pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_CAPTURE] = user_data;
	}

	if(completed_cb != NULL) {
		is_completed_cb = 1;
		pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = completed_cb;
		pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = user_data;
	}

	LOGW("is_capturing_cb :%d, is_completed_cb : %d", is_capturing_cb, is_completed_cb);
	mmsvc_camera_msg_send2(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, is_capturing_cb, INT, is_completed_cb);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SUPPORT_CONTINUOUS_CAPTURE;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_START_CONTINUOUS_CAPTURE;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_CAPTURE] = capturing_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_CAPTURE] = user_data;
	pc->cb_info->user_cb_completed[MMSVC_CAMERA_EVENT_TYPE_CAPTURE] = completed_cb;

	mmsvc_camera_msg_send2(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, count, INT, interval);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_STOP_CONTINUOUS_CAPTURE;
	LOGW("Enter,  handle :%x", pc->remote_handle);
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SUPPORT_FACE_DETECTION;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SUPPORT_ZERO_SHUTTER_LAG;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SUPPORT_MEDIA_PACKET_PREVIEW_CB;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_DEVICE_COUNT;
	int sock_fd = pc->cb_info->fd;
	int get_device_count;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_device_count, pc->cb_info->recvMsg);
		*device_count = get_device_count;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_START_FACE_DETECTION;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FACE_DETECTION] = callback;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FACE_DETECTION] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_STOP_FACE_DETECTION;
	LOGW("Enter,  handle :%x", pc->remote_handle);
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_STATE;
	int sock_fd = pc->cb_info->fd;
	int get_state;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_state, pc->cb_info->recvMsg);
		*state = (camera_state_e)get_state;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_START_FOCUSING;
	int sock_fd = pc->cb_info->fd;
	int is_continuous = (int)continuous;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, is_continuous);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_CANCEL_FOCUSING;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
	return ret;
}

int camera_set_display(camera_h camera, camera_display_type_e type, camera_display_h display)
{
	int ret = CAMERA_ERROR_NONE;
	int display_surface;
	void *set_display_handle = NULL;
	int set_surface = MM_DISPLAY_SURFACE_X;
	camera_s *handle = NULL;
	Evas_Object *obj = NULL;
	const char *object_type = NULL;
	char socket_path[MM_MSG_MAX_LENGTH] = {0,};

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (type != CAMERA_DISPLAY_TYPE_NONE && display == NULL) {
		LOGE("display type[%d] is not NONE, but display handle is NULL", type);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int display_type = (int)type;
	camera_cli_s *pc = (camera_cli_s *)camera;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_DISPLAY;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x display : 0x%x", pc->remote_handle, display);

	handle = (camera_s *)camera;
	handle->display_type = type;

	if (type == CAMERA_DISPLAY_TYPE_NONE) {
		set_display_handle = 0;
		set_surface = MM_DISPLAY_SURFACE_NULL;
		LOGW("display type NONE");
	} else {
		obj = (Evas_Object *)display;
		object_type = evas_object_type_get(obj);
		if (object_type) {
			if (type == CAMERA_DISPLAY_TYPE_OVERLAY && !strcmp(object_type, "elm_win")) {
#ifdef HAVE_WAYLAND
				//MMCamWaylandInfo *wl_info = (MMCamWaylandInfo *)g_new0(sizeof(MMCamWaylandInfo), 1);
				MMCamWaylandInfo *wl_info = g_new0(MMCamWaylandInfo, 1);

				if (wl_info == NULL) {
					LOGE("wl_info alloc failed : %d", sizeof(MMCamWaylandInfo));
					return CAMERA_ERROR_OUT_OF_MEMORY;
				}

				wl_info->evas_obj = (void *)obj;
				wl_info->window = (void *)elm_win_wl_window_get(obj);
				wl_info->surface = (void *)ecore_wl_window_surface_get(wl_info->window);
				wl_info->display = (void *)ecore_wl_display_get();

				if (wl_info->window == NULL || wl_info->surface == NULL || wl_info->display == NULL) {
					LOGE("something is NULL %p, %p, %p", wl_info->window, wl_info->surface, wl_info->display);
					//free(wl_info);
					return CAMERA_ERROR_INVALID_OPERATION;
				}

				evas_object_geometry_get(obj, &wl_info->window_x, &wl_info->window_y,
							      &wl_info->window_width, &wl_info->window_height);

				/* set wayland info */
				handle->wl_info = (void *)wl_info;
				pc->wl_info = wl_info;
				set_surface = MM_DISPLAY_SURFACE_X;
				set_display_handle = (void *)wl_info;

				LOGD("wayland obj %p, window %p, surface %p, display %p, size %d,%d,%dx%d",
				     wl_info->evas_obj, wl_info->window, wl_info->surface, wl_info->display,
				     wl_info->window_x, wl_info->window_y, wl_info->window_width, wl_info->window_height);
#else /* HAVE_WAYLAND */
				/* x window overlay surface */
				set_display_handle = (void *)elm_win_xwindow_get(obj);
				set_surface = MM_DISPLAY_SURFACE_X;
				LOGW("display type OVERLAY : handle %p", set_display_handle);
#endif
			} else if (type == CAMERA_DISPLAY_TYPE_EVAS && !strcmp(object_type, "image")) {
				/* evas object surface */
				set_display_handle = (void *)display;
				set_surface = MM_DISPLAY_SURFACE_EVAS;
				LOGW("display type EVAS : handle %p", set_display_handle);
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
	display_surface = (int)set_surface;
	mmsvc_camera_msg_send2(api, pc->remote_handle, sock_fd, pc->cb_info, ret,
							    INT, display_type,
							    INT, display_surface);

	if (ret == CAMERA_ERROR_NONE && type == CAMERA_DISPLAY_TYPE_OVERLAY && !strcmp(object_type, "elm_win")) {
		if (mm_camcorder_client_create(&(pc->client_handle))) {
			LOGE("camera client create Failed");
			return CAMERA_ERROR_INVALID_OPERATION;
		}
		mmsvc_camera_msg_get_string(socket_path, pc->cb_info->recvMsg);
		LOGD("shmsrc stream path : %s", socket_path);
		if(mm_camcorder_client_set_shm_socket_path(pc->client_handle, socket_path)
				!= MM_ERROR_NONE)
			return CAMERA_ERROR_INVALID_OPERATION;
		ret = mm_camcorder_set_attributes(pc->client_handle, NULL,
						  MMCAM_DISPLAY_SURFACE, set_surface,
						  NULL);
		if (ret == MM_ERROR_NONE && type != CAMERA_DISPLAY_TYPE_NONE) {
			ret = mm_camcorder_set_attributes(pc->client_handle, NULL,
							  MMCAM_DISPLAY_HANDLE, pc->cli_display_handle, sizeof(void *),
							  NULL);
			LOGW("ret : 0x%x", ret);
		}
	}
	LOGW("ret : 0x%x", ret);
	return ret;
}

int camera_set_preview_resolution(camera_h camera,  int width, int height)
{
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_NONE;

	camera_cli_s *pc = (camera_cli_s *)camera;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_PREVIEW_RESOLUTION;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send2(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, width, INT, height);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_CAPTURE_RESOLUTION;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send2(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, width, INT, height);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_CAPTURE_FORMAT;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x, capture_format: %d", pc->remote_handle, set_format);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_format);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_PREVIEW_FORMAT;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x, capture_format: %d", pc->remote_handle, set_format);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_format);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_PREVIEW_RESOLUTION;
	int sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_width, pc->cb_info->recvMsg);
		mmsvc_camera_msg_get(get_height, pc->cb_info->recvMsg);
		*width = get_width;
		*height = get_height;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_DISPLAY_ROTATION;
	int sock_fd = pc->cb_info->fd;
	int set_rotation = (int)rotation;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_rotation);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_DISPLAY_ROTATION;
	int sock_fd = pc->cb_info->fd;
	int get_rotation;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_rotation, pc->cb_info->recvMsg);
		*rotation = (camera_rotation_e)get_rotation;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_DISPLAY_FLIP;
	int sock_fd = pc->cb_info->fd;
	int set_flip = (int)flip;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_flip);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_DISPLAY_FLIP;
	int sock_fd = pc->cb_info->fd;
	int get_flip;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_flip, pc->cb_info->recvMsg);
		*flip = (camera_flip_e)get_flip;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_DISPLAY_VISIBLE;
	int set_visible = (int)visible;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_visible);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_IS_DISPLAY_VISIBLE;
	int sock_fd = pc->cb_info->fd;
	int get_visible;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_visible, pc->cb_info->recvMsg);
		*visible = (bool)get_visible;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_DISPLAY_MODE;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_DISPLAY_MODE;
	int sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_display_mode_e)get_mode;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_CAPTURE_RESOLUTION;
	int sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_width, pc->cb_info->recvMsg);
		mmsvc_camera_msg_get(get_height, pc->cb_info->recvMsg);
		*width = get_width;
		*height = get_height;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_CAPTURE_FORMAT;
	int get_format;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_format, pc->cb_info->recvMsg);
		*format = (camera_pixel_format_e)get_format;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_PREVIEW_FORMAT;
	int get_format;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_format, pc->cb_info->recvMsg);
		*format = (camera_pixel_format_e)get_format;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_PREVIEW_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_PREVIEW] = callback;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_PREVIEW] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_UNSET_PREVIEW_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_PREVIEW] = (void *)NULL;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_PREVIEW] = (void *)NULL;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_MEDIA_PACKET_PREVIEW_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = callback;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_UNSET_MEDIA_PACKET_PREVIEW_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void *)NULL;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void *)NULL;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_STATE_CHANGED_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_STATE_CHANGE] = callback;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_STATE_CHANGE] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_UNSET_STATE_CHANGED_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void *)NULL;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void *)NULL;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_INTERRUPTED_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_INTERRUPTED] = callback;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_INTERRUPTED] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_UNSET_INTERRUPTED_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_INTERRUPTED] = (void *)NULL;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_INTERRUPTED] = (void *)NULL;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_FOCUS_CHANGED_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = callback;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_UNSET_FOCUS_CHANGED_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void *)NULL;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void *)NULL;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_ERROR_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_ERROR] = callback;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_ERROR] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_UNSET_ERROR_CB;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_ERROR] = (void *)NULL;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_ERROR] = (void *)NULL;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_FOREACH_SUPPORTED_PREVIEW_RESOLUTION;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_RESOLUTION] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_RESOLUTION] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_FOREACH_SUPPORTED_CAPTURE_RESOLUTION;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_RESOLUTION] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_RESOLUTION] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_FOREACH_SUPPORTED_CAPTURE_FORMAT;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_FORMAT] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_CAPTURE_FORMAT] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_SET_FOREACH_SUPPORTED_PREVIEW_FORMAT;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_FORMAT] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_PREVIEW_FORMAT] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_GET_RECOMMENDED_PREVIEW_RESOLUTION;
	int sock_fd = pc->cb_info->fd;
	int get_width;
	int get_height;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_width, pc->cb_info->recvMsg);
		mmsvc_camera_msg_get(get_height, pc->cb_info->recvMsg);
		*width = get_width;
		*height = get_height;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_LENS_ORIENTATION;
	int sock_fd = pc->cb_info->fd;
	int get_angle;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_angle, pc->cb_info->recvMsg);
		*angle = get_angle;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_THEATER_MODE;
	int sock_fd = pc->cb_info->fd;
	int set_mode = (int)mode;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_THEATER_MODE;
	int sock_fd = pc->cb_info->fd;
	int get_mode;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_theater_mode_e)get_mode;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_THEATER_MODE;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_THEATER_MODE] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_THEATER_MODE] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	LOGW("Finish, return :%x", ret);

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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_PREVIEW_FPS;
	int sock_fd = pc->cb_info->fd;
	int set_fps = (int)fps;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_fps);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_IMAGE_QUALITY;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, quality);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_PREVIEW_FPS;
	int get_fps;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_fps, pc->cb_info->recvMsg);
		*fps = (camera_attr_fps_e)get_fps;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_IMAGE_QUALITY;
	int sock_fd = pc->cb_info->fd;
	int get_quality;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_quality, pc->cb_info->recvMsg);
		*quality = get_quality;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_ZOOM;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, zoom);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_AF_MODE;
	int sock_fd = pc->cb_info->fd;
	int set_mode = (int)mode;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_AF_AREA;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send2(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, x, INT, y);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_CLEAR_AF_AREA;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_EXPOSURE_MODE;
	int set_mode = (int)mode;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_EXPOSURE;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, value);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_ISO;
	int sock_fd = pc->cb_info->fd;
	int set_iso = (int)iso;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_iso);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_BRIGHTNESS;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, level);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_CONTRAST;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, level);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_WHITEBALANCE;
	int sock_fd = pc->cb_info->fd;
	int set_whitebalance = (int)wb;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_whitebalance);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_EFFECT;
	int sock_fd = pc->cb_info->fd;
	int set_effect = (int)effect;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_effect);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_SCENE_MODE;
	int sock_fd = pc->cb_info->fd;
	int set_mode = (int)mode;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_ENABLE_TAG;
	int sock_fd =  pc->cb_info->fd;
	int set_enable = (int)enable;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_enable);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_TAG_IMAGE_DESCRIPTION;
	int sock_fd =  pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, STRING, description);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_TAG_ORIENTATION;
	int sock_fd =  pc->cb_info->fd;
	int set_orientation = (int)orientation;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_orientation);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_TAG_SOFTWARE;
	int sock_fd =  pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, STRING, software);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_GEOTAG;
	int sock_fd =  pc->cb_info->fd;
	double set_geotag[3] = { latitude, longitude, altitude };

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send_array(api, pc->remote_handle, sock_fd, pc->cb_info, ret,
									set_geotag, sizeof(set_geotag), sizeof(double));
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_REMOVE_GEOTAG;
	int sock_fd =  pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_FLASH_MODE;
	int sock_fd =  pc->cb_info->fd;
	int set_mode = (int)mode;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_ZOOM;
	int get_zoom;
	int sock_fd = pc->cb_info->fd;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_zoom, pc->cb_info->recvMsg);
		*zoom = get_zoom;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_ZOOM_RANGE;
	int get_min;
	int get_max;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_min, pc->cb_info->recvMsg);
		mmsvc_camera_msg_get(get_max, pc->cb_info->recvMsg);
		*min = get_min;
		*max = get_max;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_AF_MODE;
	int get_mode;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_af_mode_e)get_mode;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_EXPOSURE_MODE;
	int get_mode;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_exposure_mode_e)get_mode;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_EXPOSURE;
	int get_value;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_value, pc->cb_info->recvMsg);
		*value = get_value;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_EXPOSURE_RANGE;
	int get_min;
	int get_max;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_min, pc->cb_info->recvMsg);
		mmsvc_camera_msg_get(get_max, pc->cb_info->recvMsg);
		*min = get_min;
		*max = get_max;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_ISO;
	int get_iso;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_iso, pc->cb_info->recvMsg);
		*iso = (camera_attr_iso_e)get_iso;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_BRIGHTNESS;
	int get_level;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_level, pc->cb_info->recvMsg);
		*level = get_level;
	}
	LOGW("ret : 0x%x", ret);
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
	int sock_fd = pc->cb_info->fd;
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_BRIGHTNESS_RANGE;
	int get_min;
	int get_max;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_min, pc->cb_info->recvMsg);
		mmsvc_camera_msg_get(get_max, pc->cb_info->recvMsg);
		*min = get_min;
		*max = get_max;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_CONTRAST;
	int sock_fd = pc->cb_info->fd;
	int get_level;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_level, pc->cb_info->recvMsg);
		*level = get_level;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_CONTRAST_RANGE;
	int sock_fd = pc->cb_info->fd;
	int get_min;
	int get_max;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_min, pc->cb_info->recvMsg);
		mmsvc_camera_msg_get(get_max, pc->cb_info->recvMsg);
		*min = get_min;
		*max = get_max;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_WHITEBALANCE;
	int sock_fd = pc->cb_info->fd;
	int get_wb;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_wb, pc->cb_info->recvMsg);
		*wb = (camera_attr_whitebalance_e)get_wb;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_EFFECT;
	int sock_fd = pc->cb_info->fd;
	int get_effect;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_effect, pc->cb_info->recvMsg);
		*effect = (camera_attr_effect_mode_e)get_effect;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_SCENE_MODE;
	int sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_scene_mode_e)get_mode;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_IS_ENABLED_TAG;
	int sock_fd = pc->cb_info->fd;
	int get_enable;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_enable, pc->cb_info->recvMsg);
		*enable = (bool)get_enable;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_TAG_IMAGE_DESCRIPTION;
	int sock_fd = pc->cb_info->fd;
	char get_description[MMSVC_MSG_MAX_LENGTH] = {0,};

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get_string(get_description, pc->cb_info->recvMsg);
		*description = strdup(get_description);
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_TAG_ORIENTATION;
	int sock_fd = pc->cb_info->fd;
	int get_orientation;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_orientation, pc->cb_info->recvMsg);
		*orientation = (camera_attr_tag_orientation_e)get_orientation;
		LOGW("success, orientation : %d", *orientation);
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_TAG_SOFTWARE;
	int sock_fd = pc->cb_info->fd;
	char get_software[MMSVC_MSG_MAX_LENGTH] = {0,};

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get_string(get_software, pc->cb_info->recvMsg);
		*software = strdup(get_software);
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_GEOTAG;
	double get_geotag[3] = {0,};
	int sock_fd = pc->cb_info->fd;
	int valid = 0;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get_array(get_geotag, pc->cb_info->recvMsg);
		*latitude = get_geotag[0];
		*longitude = get_geotag[1];
		*altitude = get_geotag[2];
	} else {
		LOGE("Returned value is not valid : 0x%x", valid);
		*latitude = 0;
		*longitude = 0;
		*altitude = 0;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_FLASH_MODE;
	int sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_flash_mode_e)get_mode;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_AF_MODE;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_AF_MODE] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_AF_MODE] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_EXPOSURE_MODE;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EXPOSURE_MODE] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_ISO;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_ISO] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_ISO] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_WHITEBALANCE;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_WHITEBALANCE] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_WHITEBALANCE] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_EFFECT;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EFFECT] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_EFFECT] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_SCENE_MODE;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_SCENE_MODE] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_SCENE_MODE] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_FLASH_MODE;

	LOGW("Enter, handle :%x", pc->remote_handle);

	int sock_fd = pc->cb_info->fd;
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FLASH_MODE] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FLASH_MODE] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_FPS;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, handle :%x", pc->remote_handle);
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("Enter, handle :%x", pc->remote_handle);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_FPS_BY_RESOLUTION;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, handle :%x", pc->remote_handle);
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS_BY_RESOLUTION] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_FPS_BY_RESOLUTION] = user_data;

	mmsvc_camera_msg_send2(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, width, INT, height);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_STREAM_FLIP;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, handle :%x", pc->remote_handle);
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_FLIP] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_FLIP] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_FOREACH_SUPPORTED_STREAM_ROTATION;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, handle :%x", pc->remote_handle);
	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_ROTATION] = foreach_cb;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_FOREACH_SUPPORTED_STREAM_ROTATION] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_STREAM_ROTATION;
	int sock_fd =  pc->cb_info->fd;
	int set_rotation = (int)rotation;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_rotation);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_STREAM_ROTATION;
	int sock_fd = pc->cb_info->fd;
	int get_rotation;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_rotation, pc->cb_info->recvMsg);
		*rotation = (camera_rotation_e)get_rotation;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_STREAM_FLIP;
	int sock_fd =  pc->cb_info->fd;
	int set_flip = (int)flip;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_flip);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_STREAM_FLIP;
	int sock_fd = pc->cb_info->fd;
	int get_flip;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_flip, pc->cb_info->recvMsg);
		*flip = (camera_flip_e)get_flip;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_HDR_MODE;
	int sock_fd =  pc->cb_info->fd;
	int set_mode = (int)mode;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_mode);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_GET_HDR_MODE;
	int sock_fd = pc->cb_info->fd;
	int get_mode;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_mode, pc->cb_info->recvMsg);
		*mode = (camera_attr_hdr_mode_e)get_mode;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_IS_SUPPORTED_HDR_CAPTURE;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_SET_HDR_CAPTURE_PROGRESS_CB;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, handle :%x", pc->remote_handle);

	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_HDR_PROGRESS] = callback;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_HDR_PROGRESS] = user_data;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_UNSET_HDR_CAPTURE_PROGRESS_CB;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, handle :%x", pc->remote_handle);

	pc->cb_info->user_cb[MMSVC_CAMERA_EVENT_TYPE_HDR_PROGRESS] = (void *)NULL;
	pc->cb_info->user_data[MMSVC_CAMERA_EVENT_TYPE_HDR_PROGRESS] = (void *)NULL;

	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_ENABLE_ANTI_SHAKE;
	int sock_fd =  pc->cb_info->fd;
	int set_enable = (int)enable;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_enable);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_IS_ENABLED_ANTI_SHAKE;
	int sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_enabled, pc->cb_info->recvMsg);
		*enabled = (bool)get_enabled;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_IS_SUPPORTED_ANTI_SHAKE;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_ENABLE_VIDEO_STABILIZATION;
	int sock_fd =  pc->cb_info->fd;
	int set_enable = (int)enable;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_enable);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_IS_ENABLED_VIDEO_STABILIZATION;
	int sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_enabled, pc->cb_info->recvMsg);
		*enabled = (bool)get_enabled;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_IS_SUPPORTED_VIDEO_STABILIZATION;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_ENABLE_AUTO_CONTRAST;
	int sock_fd =  pc->cb_info->fd;
	int set_enable = (int)enable;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_enable);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_IS_ENABLED_AUTO_CONTRAST;
	int sock_fd = pc->cb_info->fd;
	int get_enabled;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);

	if (ret == CAMERA_ERROR_NONE) {
		mmsvc_camera_msg_get(get_enabled, pc->cb_info->recvMsg);
		*enabled = (bool)get_enabled;
	}
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_IS_SUPPORTED_AUTO_CONTRAST;
	int sock_fd = pc->cb_info->fd;
	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send(api, pc->remote_handle, sock_fd, pc->cb_info, ret);
	LOGW("ret : 0x%x", ret);
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
	mmsvc_camera_api_e api = MMSVC_CAMERA_API_ATTR_DISABLE_SHUTTER_SOUND;
	int sock_fd =  pc->cb_info->fd;
	int set_disable = (int)disable;

	LOGW("Enter, remote_handle : %x", pc->remote_handle);
	mmsvc_camera_msg_send1(api, pc->remote_handle, sock_fd, pc->cb_info, ret, INT, set_disable);
	LOGW("ret : 0x%x", ret);
	return ret;
}
