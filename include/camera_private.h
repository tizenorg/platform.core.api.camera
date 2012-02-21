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
#include <mm_camcorder.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	_CAMERA_EVENT_TYPE_STATE_CHANGE,
	_CAMERA_EVENT_TYPE_FOCUS_CHANGE,	
	_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE,
	_CAMERA_EVENT_TYPE_PREVIEW,
	_CAMERA_EVENT_TYPE_CAPTURE,	
	_CAMERA_EVENT_TYPE_ERROR,		
	_CAMERA_EVENT_TYPE_NUM
}_camera_event_e;

typedef struct _camera_s{
	MMHandleType mm_handle;
	
	void* user_cb[_CAMERA_EVENT_TYPE_NUM];
	void* user_data[_CAMERA_EVENT_TYPE_NUM];
	void* display_handle;
	camera_display_type_e display_type;
	int state;
	
} camera_s;

typedef enum {
	CAMERA_MODE_IMAGE = MM_CAMCORDER_MODE_IMAGE,				/**< Still image capture mode */
	CAMERA_MODE_VIDEO = MM_CAMCORDER_MODE_VIDEO				/**< Video recording mode */
} camera_mode_e;



int __mm_camera_message_callback(int message, void *param, void *user_data);

#ifdef __cplusplus
}
#endif

#endif //__TIZEN_MULTIMEDIA_CAMERA_PRIVATE_H__

