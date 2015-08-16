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
#include <mm_camcorder.h>
#include <mm_camcorder_mused.h>
#include <mm_types.h>
#include <camera.h>
#include <camera_internal.h>
#include <camera_private.h>
#include <glib.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_CAMERA"


int camera_set_x11_display_rotation(camera_h camera, camera_rotation_e rotation)
{
	return camera_set_display_rotation(camera, rotation);
}


int camera_get_x11_display_rotation(camera_h camera, camera_rotation_e *rotation)
{
	return camera_get_display_rotation(camera, rotation);
}


int camera_set_x11_display_flip(camera_h camera, camera_flip_e flip)
{
	return camera_set_display_flip(camera, flip);
}


int camera_get_x11_display_flip(camera_h camera, camera_flip_e *flip)
{
	return camera_get_display_flip(camera, flip);
}


int camera_set_x11_display_visible(camera_h camera, bool visible)
{
	return camera_set_display_visible(camera, visible);
}


int camera_is_x11_display_visible(camera_h camera, bool *visible)
{
	return camera_is_display_visible(camera, visible);
}


int camera_set_x11_display_mode(camera_h camera, camera_display_mode_e mode)
{
	return camera_set_display_mode(camera, mode);
}


int camera_get_x11_display_mode(camera_h camera, camera_display_mode_e *mode)
{
	return camera_get_display_mode(camera, mode);
}


int camera_set_x11_display_pixmap(camera_h camera, camera_x11_pixmap_updated_cb callback, void *user_data)
{
	int ret = MM_ERROR_NONE;
	camera_s *handle = (camera_s *)camera;
	camera_state_e capi_state = CAMERA_STATE_NONE;

	if (handle == NULL || callback == NULL) {
		LOGE("INVALID_PARAMETER(handle:%p,callback:%p,user_data:%p)", handle, callback, user_data);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_get_state(camera, &capi_state);
	if (capi_state > CAMERA_STATE_CREATED) {
		LOGE("INVALID STATE(state:%d)", capi_state);
		return CAMERA_ERROR_INVALID_STATE;
	}

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_SURFACE, MM_DISPLAY_SURFACE_X_EXT,
					  MMCAM_DISPLAY_HANDLE, callback, sizeof(void *),
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_mused_display(camera_h camera, camera_display_type_e type, void *display_handle)
{
	int ret = MM_ERROR_NONE;
	int set_surface = MM_DISPLAY_SURFACE_X;
	camera_s *handle = NULL;
	void *set_handle = NULL;

	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if (type != CAMERA_DISPLAY_TYPE_NONE && display_handle == NULL) {
		LOGE("display type[%d] is not NONE, but display handle is NULL", type);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	handle = (camera_s *)camera;
	handle->display_type = type;
	handle->display_handle = display_handle;

	switch(type) {
		case CAMERA_DISPLAY_TYPE_OVERLAY:
			set_surface = MM_DISPLAY_SURFACE_X;
			set_handle = &(handle->display_handle);
			LOGD("display type OVERLAY : handle %p", (int)handle->display_handle);
			break;
		case CAMERA_DISPLAY_TYPE_EVAS:
			set_surface = MM_DISPLAY_SURFACE_EVAS;
			set_handle = display_handle;
			break;
		case CAMERA_DISPLAY_TYPE_NONE:
		default:
			set_surface = MM_DISPLAY_SURFACE_NULL;
			handle->display_handle = NULL;
			break;
	}

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_DEVICE, MM_DISPLAY_DEVICE_MAINLCD,
					  MMCAM_DISPLAY_SURFACE, set_surface,
					  NULL);

	if (ret == MM_ERROR_NONE && type != CAMERA_DISPLAY_TYPE_NONE) {
		ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
						  MMCAM_DISPLAY_HANDLE, set_handle, sizeof(void *),
						  NULL);
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_get_video_caps(camera_h camera, char **caps)
{
	int ret;
	camera_s *handle = (camera_s *)camera;

	ret = mm_camcorder_mused_get_video_caps(handle->mm_handle, caps);
	if(ret != MM_ERROR_NONE) {
		return __convert_camera_error_code(__func__, ret);
	}

	return CAMERA_ERROR_NONE;
}

int camera_set_shm_socket_path_for_mused(camera_h camera, char *socket_path)
{
	int ret;
	camera_s *handle = (camera_s *)camera;

	LOGE("var : %s", socket_path);
	mm_camcorder_set_attributes(handle->mm_handle, NULL,
				    MMCAM_DISPLAY_SHM_SOCKET_PATH, socket_path, strlen(socket_path),
				    NULL);

	if (ret != MM_ERROR_NONE) {
		LOGE("error set shm socket path attribute 0x%x", ret);
		return __convert_camera_error_code(__func__, ret);
	}

	return CAMERA_ERROR_NONE;
}


