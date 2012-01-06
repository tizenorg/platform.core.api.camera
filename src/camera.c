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
#include <mm_types.h>
#include <math.h>
#include <camera.h>
#include <camera_private.h>
#include <glib.h>
#include <dlog.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_CAMERA"

int _convert_camera_error_code(const char* func, int code){
	int ret = CAMERA_ERROR_NONE;
	char *errorstr = NULL;
	switch(code)
	{
		case MM_ERROR_NONE:
			ret = CAMERA_ERROR_NONE;
			errorstr = "ERROR_NONE";
			break;
		case MM_ERROR_CAMCORDER_INVALID_ARGUMENT :
		case MM_ERROR_COMMON_INVALID_ATTRTYPE :
		case MM_ERROR_COMMON_INVALID_PERMISSION :
		case MM_ERROR_COMMON_OUT_OF_ARRAY :
		case MM_ERROR_COMMON_OUT_OF_RANGE :
		case MM_ERROR_COMMON_ATTR_NOT_EXIST :
			ret = CAMERA_ERROR_INVALID_PARAMETER;
			errorstr = "INVALID_PARAMETER";
			break;
		case MM_ERROR_CAMCORDER_NOT_INITIALIZED :
		case MM_ERROR_CAMCORDER_INVALID_STATE :
			ret = CAMERA_ERROR_INVALID_STATE;
			errorstr = "INVALID_STATE";
			break;

		case MM_ERROR_CAMCORDER_DEVICE :
		case MM_ERROR_CAMCORDER_DEVICE_NOT_FOUND :
		case MM_ERROR_CAMCORDER_DEVICE_BUSY	:
		case MM_ERROR_CAMCORDER_DEVICE_OPEN :
		case MM_ERROR_CAMCORDER_DEVICE_IO :
		case MM_ERROR_CAMCORDER_DEVICE_TIMEOUT	:
		case MM_ERROR_CAMCORDER_DEVICE_REG_TROUBLE :
		case MM_ERROR_CAMCORDER_DEVICE_WRONG_JPEG	 :
		case MM_ERROR_CAMCORDER_DEVICE_LACK_BUFFER :
			ret = CAMERA_ERROR_DEVICE;
			errorstr = "ERROR_DEVICE";
			break;

		case MM_ERROR_CAMCORDER_GST_CORE :
		case MM_ERROR_CAMCORDER_GST_LIBRARY :
		case MM_ERROR_CAMCORDER_GST_RESOURCE :
		case MM_ERROR_CAMCORDER_GST_STREAM :
		case MM_ERROR_CAMCORDER_GST_STATECHANGE :
		case MM_ERROR_CAMCORDER_GST_NEGOTIATION :
		case MM_ERROR_CAMCORDER_GST_LINK :
		case MM_ERROR_CAMCORDER_GST_FLOW_ERROR :
		case MM_ERROR_CAMCORDER_ENCODER :
		case MM_ERROR_CAMCORDER_ENCODER_BUFFER :
		case MM_ERROR_CAMCORDER_ENCODER_WRONG_TYPE :
		case MM_ERROR_CAMCORDER_ENCODER_WORKING :
		case MM_ERROR_CAMCORDER_INTERNAL :
		case MM_ERROR_CAMCORDER_NOT_SUPPORTED :
		case MM_ERROR_CAMCORDER_RESPONSE_TIMEOUT :
		case MM_ERROR_CAMCORDER_CMD_IS_RUNNING :	
		case MM_ERROR_CAMCORDER_DSP_FAIL :
		case MM_ERROR_CAMCORDER_AUDIO_EMPTY :
		case MM_ERROR_CAMCORDER_CREATE_CONFIGURE :
		case MM_ERROR_CAMCORDER_FILE_SIZE_OVER :
		case MM_ERROR_CAMCORDER_DISPLAY_DEVICE_OFF :
		case MM_ERROR_CAMCORDER_INVALID_CONDITION :			
			ret = CAMERA_ERROR_INVALID_OPERATION;
			errorstr = "INVALID_OPERATION";
			break;
				

		case MM_ERROR_CAMCORDER_RESOURCE_CREATION :
		case MM_ERROR_COMMON_OUT_OF_MEMORY:	
			ret = CAMERA_ERROR_OUT_OF_MEMORY;
			errorstr = "OUT_OF_MEMORY";
			break;

		case MM_ERROR_POLICY_BLOCKED:
			ret = CAMERA_ERROR_SOUND_POLICY;
			errorstr = "ERROR_SOUND_POLICY";
			break;

		default:
			ret = CAMERA_ERROR_INVALID_OPERATION;
			errorstr = "INVALID_OPERATION";
		
	}
	
	LOGE( "[%s] %s(0x%08x) : core frameworks error code(0x%08x)",func, errorstr, ret, code);
	
	return ret;
}


gboolean mm_videostream_callback(MMCamcorderVideoStreamDataType * stream, void *user_data){

	if( user_data == NULL || stream == NULL)
		return 0;
		
	camera_s * handle = (camera_s*)user_data;
	if( handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW] )
		((camera_preview_cb)handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW])(stream->data, stream->length, stream->width, stream->height, stream->format, handle->user_data[_CAMERA_EVENT_TYPE_PREVIEW]);
	return 1;
}
gboolean mm_capture_callback(MMCamcorderCaptureDataType *frame, MMCamcorderCaptureDataType *thumbnail, void *user_data){
	if( user_data == NULL || frame == NULL)
		return 0;
	
	camera_s * handle = (camera_s*)user_data;
	if( handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE] )
		((camera_capturing_cb)handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE])(frame->data, frame->length, frame->width, frame->height, frame->format, handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE]);

	return 1;
}

camera_state_e _camera_state_convert(MMCamcorderStateType mm_state)
{
	camera_state_e state = CAMERA_STATE_NONE;
	
	switch( mm_state ){
		case MM_CAMCORDER_STATE_NONE:				
			state = CAMERA_STATE_NONE;
			break;
		case MM_CAMCORDER_STATE_NULL:
			state = CAMERA_STATE_CREATED;
			break;
		case MM_CAMCORDER_STATE_READY:
			state = CAMERA_STATE_CREATED;
			break;
		case MM_CAMCORDER_STATE_PREPARE:
			state = CAMERA_STATE_PREVIEW;
			break;
		case MM_CAMCORDER_STATE_CAPTURING:
			state = CAMERA_STATE_CAPTURING;
			break;
		case MM_CAMCORDER_STATE_RECORDING:
			state = CAMERA_STATE_PREVIEW;
			break;
		case MM_CAMCORDER_STATE_PAUSED:
			state = CAMERA_STATE_PREVIEW;
			break;
		default:
			state = CAMERA_STATE_NONE;
			break;
	}

	return state;
	
}


int mm_message_callback(int message, void *param, void *user_data){
	if( user_data == NULL || param == NULL )
		return 0;
	
	
	camera_s * handle = (camera_s*)user_data;
	MMMessageParamType *m = (MMMessageParamType*)param;
	camera_state_e previous_state; 


	switch(message){
		case MM_MESSAGE_CAMCORDER_STATE_CHANGED:
		case MM_MESSAGE_CAMCORDER_STATE_CHANGED_BY_ASM:
			if( m->state.previous < MM_CAMCORDER_STATE_NONE ||  m->state.previous > MM_CAMCORDER_STATE_PAUSED || m->state.code != 0 ){
				LOGI( "Invalid state changed message");
				break;
			}
			
			previous_state = handle->state;
			handle->state = _camera_state_convert(m->state.current );
			
			if( previous_state != handle->state && handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] ){
				((camera_state_changed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE])(previous_state, handle->state, message == MM_MESSAGE_CAMCORDER_STATE_CHANGED ? 0 : 1 , handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE]);
			}

			// should change intermediate state MM_CAMCORDER_STATE_READY is not valid in capi , change to NULL state
			if( message == MM_MESSAGE_CAMCORDER_STATE_CHANGED_BY_ASM ){
				if( m->state.previous == MM_CAMCORDER_STATE_PREPARE && m->state.current == MM_CAMCORDER_STATE_READY ){
					mm_camcorder_unrealize(handle->mm_handle);
				}
			}
			
			break;
		case MM_MESSAGE_CAMCORDER_FOCUS_CHANGED :
			if( handle->user_cb[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] ){
				((camera_focus_changed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_FOCUS_CHANGE])( m->code, handle->user_data[_CAMERA_EVENT_TYPE_FOCUS_CHANGE]);
			}
			break;
		case MM_MESSAGE_CAMCORDER_CAPTURED:
		{
			MMCamRecordingReport *report = (MMCamRecordingReport *)m ->data;
			int mode;
			 mm_camcorder_get_attributes(handle->mm_handle ,NULL,MMCAM_MODE, &mode, NULL);
			 if( mode == MM_CAMCORDER_MODE_IMAGE){
				//pseudo state change
				previous_state = handle->state ;
				handle->state = CAMERA_STATE_CAPTURED;
				if( previous_state != handle->state && handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] ){
					((camera_state_changed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE])(previous_state, handle->state,  0 , handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE]);
				}
				
				if( handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] ){
					((camera_capture_completed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE])(handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE]);
				}
			}else{
				if( report != NULL && report->recording_filename ){
					free(report->recording_filename );
					report->recording_filename = NULL;
				}
				if( report ){
					free(report);
					report = NULL;
				}			
			}
			break;
		}
		case MM_MESSAGE_CAMCORDER_ERROR: 
		{
			int errorcode = m->code;
			errorcode = _convert_camera_error_code("NOTIFY", errorcode);
			if( handle->user_cb[_CAMERA_EVENT_TYPE_ERROR] )
				((camera_error_cb)handle->user_cb[_CAMERA_EVENT_TYPE_ERROR])(errorcode, handle->state , handle->user_data[_CAMERA_EVENT_TYPE_ERROR]);
			
			break;
		}
			
			
	}
	
	return 1;
}

int camera_create( camera_device_e device, camera_h* camera){

	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
		
	
 	int ret;
	MMCamPreset info;
	int preview_format;
	int rotation;
	
	if( device == CAMERA_DEVICE_CAMERA1 )
		info.videodev_type= MM_VIDEO_DEVICE_CAMERA1;
	else
		info.videodev_type= MM_VIDEO_DEVICE_CAMERA0;
	
	camera_s* handle = (camera_s*)malloc( sizeof(camera_s) );
	if(handle==NULL){
		LOGE("[%s] malloc fail",__func__);
		return CAMERA_ERROR_OUT_OF_MEMORY;
	}
	memset(handle, 0 , sizeof(camera_s));

	ret = mm_camcorder_create(&handle->mm_handle, &info);
	if( ret != MM_ERROR_NONE){
		free(handle);
		return _convert_camera_error_code(__func__,ret);
	}

	preview_format = MM_PIXEL_FORMAT_YUYV;
	rotation = MM_DISPLAY_ROTATION_NONE;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_RECOMMEND_PREVIEW_FORMAT_FOR_CAPTURE, &preview_format, MMCAM_RECOMMEND_DISPLAY_ROTATION, &rotation, NULL);

	
	char *error;
	ret = mm_camcorder_set_attributes(handle->mm_handle, &error, 
																MMCAM_MODE , MM_CAMCORDER_MODE_IMAGE, 
																MMCAM_CAMERA_FORMAT,  preview_format,
																MMCAM_IMAGE_ENCODER , MM_IMAGE_CODEC_JPEG, 
																MMCAM_CAPTURE_FORMAT,  MM_PIXEL_FORMAT_ENCODED ,
																MMCAM_DISPLAY_SURFACE, MM_DISPLAY_SURFACE_NULL, 
																MMCAM_DISPLAY_ROTATION, rotation, 
																MMCAM_CAPTURE_COUNT, 1, 
																(void*)NULL);

	handle->display_type = CAMERA_DISPLAY_TYPE_NONE;
	
	if( ret != MM_ERROR_NONE){
		LOGE("[%s] mm_camcorder_set_attributes fail(%x, %s)",__func__, ret, error);
		mm_camcorder_destroy(handle->mm_handle);
		free(error);
		free(handle);
		return _convert_camera_error_code(__func__, ret);
	}	

	handle->state = CAMERA_STATE_CREATED;
	mm_camcorder_set_message_callback(handle->mm_handle, mm_message_callback, (void*)handle);
		
		
	if( ret == MM_ERROR_NONE)
		*camera = (camera_h)handle;

	return _convert_camera_error_code(__func__, ret);
	
}

 int camera_destroy(camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
 	int ret;
	camera_s *handle = (camera_s*)camera;
	
	ret = mm_camcorder_destroy(handle->mm_handle);
	
	if( ret == MM_ERROR_NONE)
		free(handle);

	return _convert_camera_error_code(__func__, ret);

}

int camera_start_preview(camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	
 	int ret;
	camera_s *handle = (camera_s*)camera;

	if( handle->state == CAMERA_STATE_CAPTURED )
	{
		ret = mm_camcorder_capture_stop(handle->mm_handle);
		return _convert_camera_error_code(__func__, ret);
	}

	

	//for receving MM_MESSAGE_CAMCORDER_CAPTURED evnet must be seted capture callback
	mm_camcorder_set_video_capture_callback( handle->mm_handle, (mm_camcorder_video_capture_callback)mm_capture_callback, (void*)handle);

	if( handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW] )
		mm_camcorder_set_video_stream_callback( handle->mm_handle, (mm_camcorder_video_stream_callback)mm_videostream_callback, (void*)handle);
	else
		mm_camcorder_set_video_stream_callback( handle->mm_handle, (mm_camcorder_video_stream_callback)NULL, (void*)NULL);

	MMCamcorderStateType state ;
	mm_camcorder_get_state(handle->mm_handle, &state);	
	if( state != MM_CAMCORDER_STATE_READY){
		ret = mm_camcorder_realize(handle->mm_handle);	
		if( ret != MM_ERROR_NONE )
			return _convert_camera_error_code(__func__, ret);
	}
	
	ret = mm_camcorder_start(handle->mm_handle);

	//start fail.
	if( ret != MM_ERROR_NONE &&  state != MM_CAMCORDER_STATE_READY){
		mm_camcorder_unrealize(handle->mm_handle);
	}
	
	return _convert_camera_error_code(__func__, ret);
}

int camera_stop_preview(camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	
 	int ret;
	camera_s *handle = (camera_s*)camera;
	MMCamcorderStateType state ;
	mm_camcorder_get_state(handle->mm_handle, &state);	

	if( state == MM_CAMCORDER_STATE_PREPARE ){
		ret = mm_camcorder_stop(handle->mm_handle);	
		if( ret != MM_ERROR_NONE)
			return _convert_camera_error_code(__func__, ret);	
	}
	
	ret = mm_camcorder_unrealize(handle->mm_handle);
	return _convert_camera_error_code(__func__, ret);	
}

int camera_start_capture(camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	return mm_camcorder_capture_start(((camera_s*)camera)->mm_handle);
}


int camera_get_state(camera_h camera, camera_state_e * state){
	if( camera == NULL || state == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s *handle = (camera_s*)camera;
	camera_state_e capi_state;
	MMCamcorderStateType mmstate ;
	mm_camcorder_get_state(handle->mm_handle, &mmstate);	

	capi_state = _camera_state_convert(mmstate);

	if( handle->state == CAMERA_STATE_CAPTURED && mmstate == MM_CAMCORDER_STATE_CAPTURING )
		capi_state = CAMERA_STATE_CAPTURED;

	*state = capi_state;
	
	return CAMERA_ERROR_NONE;
	
}

int camera_start_focusing( camera_h camera ){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	return _convert_camera_error_code(__func__, mm_camcorder_start_focusing(((camera_s*)camera)->mm_handle));
}
int camera_cancel_focusing( camera_h camera ){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	return _convert_camera_error_code(__func__, mm_camcorder_stop_focusing(((camera_s*)camera)->mm_handle));	
}
int camera_set_display(camera_h camera, camera_display_type_e type, camera_display_h display){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	handle->display_handle = display;
	handle->display_type = type;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL,
		MMCAM_DISPLAY_DEVICE, MM_DISPLAY_DEVICE_MAINLCD,
		MMCAM_DISPLAY_SURFACE  ,type,  
		MMCAM_DISPLAY_HANDLE  , type == CAMERA_DISPLAY_TYPE_X11 ? &handle->display_handle : display , sizeof(display) , 
		NULL);
	return _convert_camera_error_code(__func__, ret);
}

int camera_set_preview_resolution(camera_h camera,  int width, int height){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_WIDTH  , width ,MMCAM_CAMERA_HEIGHT ,height,  NULL);
	return _convert_camera_error_code(__func__, ret);
	
}
int camera_set_x11_display_rotation(camera_h camera,  camera_display_rotation_e rotation){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( rotation < CAMERA_DISPLAY_ROTATION_NONE || rotation > CAMERA_DISPLAY_ROTATION_270 )
		return CAMERA_ERROR_INVALID_PARAMETER;
	
	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_ROTATION , rotation, NULL);
	return _convert_camera_error_code(__func__, ret);
}

int camera_set_capture_resolution(camera_h camera,  int width, int height){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAPTURE_WIDTH, width  ,MMCAM_CAPTURE_HEIGHT , height, NULL);
	return _convert_camera_error_code(__func__, ret);
}

int camera_set_capture_format(camera_h camera, camera_pixel_format_e format){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAPTURE_FORMAT, format , NULL);
	return _convert_camera_error_code(__func__, ret);	
}

int camera_set_preview_format(camera_h camera, camera_pixel_format_e format){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FORMAT, format , NULL);
	return _convert_camera_error_code(__func__, ret);	
}

int camera_get_preview_resolution(camera_h camera,  int *width, int *height){
	if( camera == NULL || width == NULL || height == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_WIDTH , width,MMCAM_CAMERA_HEIGHT, height,  NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_get_x11_display_rotation( camera_h camera,  camera_display_rotation_e *rotation){
	if( camera == NULL || rotation == NULL ){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_ROTATION , rotation, NULL);
	return _convert_camera_error_code(__func__, ret);
}

int camera_set_x11_display_visible(camera_h camera, bool visible){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_VISIBLE , visible, NULL);
	return _convert_camera_error_code(__func__, ret);	
}

int camera_is_x11_display_visible(camera_h camera, bool* visible){
	if( camera == NULL || visible == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	int result;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_VISIBLE , &result, NULL);
	if( ret == 0)
		*visible = result;
	return _convert_camera_error_code(__func__, ret);	
}

int camera_set_x11_display_mode( camera_h camera , camera_display_mode_e ratio){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( ratio < CAMERA_DISPLAY_MODE_LETTER_BOX || ratio > CAMERA_DISPLAY_MODE_CROPPED_FULL )
		return CAMERA_ERROR_INVALID_PARAMETER;
	
	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_GEOMETRY_METHOD , ratio, NULL);
	return _convert_camera_error_code(__func__, ret);		
}

int camera_get_x11_display_mode( camera_h camera , camera_display_mode_e* ratio){
	if( camera == NULL || ratio == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_GEOMETRY_METHOD , ratio, NULL);
	return _convert_camera_error_code(__func__, ret);		
}


int camera_get_capture_resolution(camera_h camera, int *width, int *height){
	if( camera == NULL || width== NULL || height == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAPTURE_WIDTH, width  ,MMCAM_CAPTURE_HEIGHT , height, NULL);
	return _convert_camera_error_code(__func__, ret);
}

int camera_get_capture_format(camera_h camera, camera_pixel_format_e *format){
	if( camera == NULL || format == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAPTURE_FORMAT, format , NULL);
	return _convert_camera_error_code(__func__, ret);	
}

int camera_get_preview_format(camera_h camera, camera_pixel_format_e *format){
	if( camera == NULL || format == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FORMAT, format , NULL);
	return _convert_camera_error_code(__func__, ret);	
}

int camera_set_preview_cb( camera_h camera, camera_preview_cb callback, void* user_data ){
	if( camera == NULL || callback == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_PREVIEW] = (void*)user_data;
	return CAMERA_ERROR_NONE;	
}
int camera_unset_preview_cb( camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_PREVIEW] = (void*)NULL;
	return CAMERA_ERROR_NONE;		
}
int camera_set_capturing_cb( camera_h camera, camera_capturing_cb callback, void* user_data ){
		
	if( camera == NULL || callback == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE] = (void*)user_data;
	return CAMERA_ERROR_NONE;			
}
int camera_unset_capturing_cb( camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE] = (void*)NULL;
	return CAMERA_ERROR_NONE;				
}
int camera_set_state_changed_cb(camera_h camera, camera_state_changed_cb callback, void* user_data){
	if( camera == NULL || callback == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void*)user_data;
	return CAMERA_ERROR_NONE;				
}
int camera_unset_state_changed_cb(camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void*)NULL;
	return CAMERA_ERROR_NONE;					
}

int camera_set_focus_changed_cb(camera_h camera, camera_focus_changed_cb callback, void* user_data){
	if( camera == NULL || callback == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void*)user_data;
	return CAMERA_ERROR_NONE;
}
int camera_unset_focus_changed_cb(camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void*)NULL;
	return CAMERA_ERROR_NONE;	
}

int camera_set_capture_completed_cb(camera_h camera, camera_capture_completed_cb callback, void* user_data){
		
	if( camera == NULL || callback == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = (void*)user_data;
	return CAMERA_ERROR_NONE;
}

int camera_unset_capture_completed_cb(camera_h camera){
		
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = (void*)NULL;
	return CAMERA_ERROR_NONE;	
}

int camera_set_error_cb(camera_h camera, camera_error_cb callback, void *user_data){
	if( camera == NULL || callback == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_ERROR] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_ERROR] = (void*)user_data;
	return CAMERA_ERROR_NONE;
}

int camera_unset_error_cb(camera_h camera){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_ERROR] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_ERROR] = (void*)NULL;
	return CAMERA_ERROR_NONE;		
}

int camera_foreach_supported_preview_resolution(camera_h camera, camera_supported_preview_resolution_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo preview_width;
	MMCamAttrsInfo preview_height;	
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_WIDTH , &preview_width);
	ret |= mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_HEIGHT , &preview_height);
		
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	for( i=0 ; i < preview_width.int_array.count ; i++)
	{
		if ( foreach_cb(preview_width.int_array.array[i], preview_height.int_array.array[i],user_data) < 0 )
			break;
	}
	return CAMERA_ERROR_NONE;
	
}
int camera_foreach_supported_capture_resolution(camera_h camera, camera_supported_capture_resolution_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo capture_width;
	MMCamAttrsInfo capture_height;	
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAPTURE_WIDTH , &capture_width);
	ret |= mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAPTURE_HEIGHT , &capture_height);
		
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	for( i=0 ; i < capture_width.int_array.count ; i++)
	{
		if ( !foreach_cb(capture_width.int_array.array[i], capture_height.int_array.array[i],user_data) )
			break;
	}
	return CAMERA_ERROR_NONE;
}

int camera_foreach_supported_capture_format(camera_h camera, camera_supported_capture_format_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo format;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAPTURE_FORMAT , &format);

	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);	
	
	int i;
	for( i=0 ; i < format.int_array.count ; i++)
	{
		if ( !foreach_cb(format.int_array.array[i], user_data) )
			break;
	}	
	return CAMERA_ERROR_NONE;

}


int camera_foreach_supported_preview_format(camera_h camera, camera_supported_preview_format_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo format;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_FORMAT , &format);

	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);	
	
	int i;
	for( i=0 ; i < format.int_array.count ; i++)
	{
		if ( !foreach_cb(format.int_array.array[i], user_data) )
			break;
	}	
	return CAMERA_ERROR_NONE;

}


int camera_attr_get_lens_orientation(camera_h camera, int *angle) {
	if( camera == NULL || angle == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	int rotation;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_RECOMMEND_DISPLAY_ROTATION, &rotation , NULL);

	switch( rotation ) {
		case MM_DISPLAY_ROTATION_NONE:
			*angle = 0;
			break;
		case MM_DISPLAY_ROTATION_90:
			*angle = 270;
			break;
		case MM_DISPLAY_ROTATION_180:
			*angle = 180;
			break;
		case MM_DISPLAY_ROTATION_270:
			*angle = 90;
			break;
	}	
	
	return _convert_camera_error_code(__func__, ret);	
}



int camera_attr_set_preview_fps(camera_h camera,  camera_attr_fps_e fps){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	
	if( fps == CAMERA_ATTR_FPS_AUTO )
		ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FPS_AUTO  , 1, MMCAM_CAMERA_FPS, CAMERA_ATTR_FPS_60 , NULL);	
	else
		ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FPS_AUTO  , 0, MMCAM_CAMERA_FPS  , fps, NULL);
	
	return _convert_camera_error_code(__func__, ret);
	
}


int camera_attr_set_image_quality(camera_h camera,  int quality){
		
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_IMAGE_ENCODER_QUALITY , quality, NULL);
	return _convert_camera_error_code(__func__, ret);

}
int camera_attr_get_preview_fps(camera_h camera,  camera_attr_fps_e *fps){
	if( camera == NULL || fps == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	int mm_fps;
	int is_auto;
	camera_s * handle = (camera_s*)camera;
	
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FPS , &mm_fps, MMCAM_CAMERA_FPS_AUTO , &is_auto, NULL);
	if( is_auto )
		*fps = CAMERA_ATTR_FPS_AUTO;
	else
		*fps = mm_fps;
	
	return _convert_camera_error_code(__func__, ret);
	
}
int camera_attr_get_image_quality(camera_h camera,  int *quality){
	if( camera == NULL || quality == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_IMAGE_ENCODER_QUALITY   , quality, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}


int camera_attr_set_zoom(camera_h camera,  int zoom){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_DIGITAL_ZOOM  , zoom, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}
int camera_attr_set_af_mode(camera_h camera,  camera_attr_af_mode_e mode){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret = CAMERA_ERROR_INVALID_PARAMETER;
	camera_s * handle = (camera_s*)camera;

	switch(mode){
		case CAMERA_ATTR_AF_NONE:
			ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FOCUS_MODE     , MM_CAMCORDER_FOCUS_MODE_NONE ,
																														MMCAM_CAMERA_AF_SCAN_RANGE  , MM_CAMCORDER_AUTO_FOCUS_NORMAL , NULL);			
			break; 
		case CAMERA_ATTR_AF_NORMAL:
			ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FOCUS_MODE  , MM_CAMCORDER_FOCUS_MODE_AUTO , 
																														MMCAM_CAMERA_AF_SCAN_RANGE  , MM_CAMCORDER_AUTO_FOCUS_NORMAL , NULL);
			break;
		case CAMERA_ATTR_AF_MACRO:
			ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FOCUS_MODE  , MM_CAMCORDER_FOCUS_MODE_AUTO , 
																														MMCAM_CAMERA_AF_SCAN_RANGE  , MM_CAMCORDER_AUTO_FOCUS_MACRO  , NULL);						
			break;
		case CAMERA_ATTR_AF_FULL:
			ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FOCUS_MODE  , MM_CAMCORDER_FOCUS_MODE_AUTO , 
																														MMCAM_CAMERA_AF_SCAN_RANGE  , MM_CAMCORDER_AUTO_FOCUS_FULL  , NULL);						
			break;
			
		default:
			return ret;
	}
	
	return _convert_camera_error_code(__func__, ret);
}

int camera_attr_set_exposure_mode(camera_h camera,  camera_attr_exposure_mode_e mode){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int maptable[] = {MM_CAMCORDER_AUTO_EXPOSURE_OFF, //CAMCORDER_EXPOSURE_MODE_OFF 
									MM_CAMCORDER_AUTO_EXPOSURE_ALL, //CAMCORDER_EXPOSURE_MODE_ALL
									MM_CAMCORDER_AUTO_EXPOSURE_CENTER_1, //CAMCORDER_EXPOSURE_MODE_CENTER
									MM_CAMCORDER_AUTO_EXPOSURE_SPOT_1, //CAMCORDER_EXPOSURE_MODE_SPOT
									MM_CAMCORDER_AUTO_EXPOSURE_CUSTOM_1,//CAMCORDER_EXPOSURE_MODE_CUSTOM
		};
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_EXPOSURE_MODE  , maptable[abs(mode%5)], NULL);
	return _convert_camera_error_code(__func__, ret);

}

int camera_attr_set_exposure(camera_h camera, int value){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;

	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_EXPOSURE_VALUE  , value, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_iso(camera_h camera,  camera_attr_iso_e iso){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_ISO  , iso, NULL);
	return _convert_camera_error_code(__func__, ret);	
}
int camera_attr_set_brightness(camera_h camera,  int level){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
		
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_BRIGHTNESS  , level, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_contrast(camera_h camera,  int level){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_CONTRAST  , level, NULL);

	return _convert_camera_error_code(__func__, ret);
	
}
int camera_attr_set_whitebalance(camera_h camera,  camera_attr_whitebalance_e wb){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_WB  , wb, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_effect(camera_h camera, camera_attr_effect_mode_e effect){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int maptable[] = {
		MM_CAMCORDER_COLOR_TONE_NONE, // CAMCORDER_EFFECT_NONE = 0,			/**< None */
		MM_CAMCORDER_COLOR_TONE_MONO, //CAMCORDER_EFFECT_MONO,			/**< Mono */
		MM_CAMCORDER_COLOR_TONE_SEPIA, //CAMCORDER_EFFECT_SEPIA,			/**< Sepia */
		MM_CAMCORDER_COLOR_TONE_NEGATIVE, //CAMCORDER_EFFECT_NEGATIVE,		/**< Negative */
		MM_CAMCORDER_COLOR_TONE_BLUE, //CAMCORDER_EFFECT_BLUE,			/**< Blue */
		MM_CAMCORDER_COLOR_TONE_GREEN, //CAMCORDER_EFFECT_GREEN,			/**< Green */
		MM_CAMCORDER_COLOR_TONE_AQUA, //CAMCORDER_EFFECT_AQUA,			/**< Aqua */
		MM_CAMCORDER_COLOR_TONE_VIOLET, //CAMCORDER_EFFECT_VIOLET,			/**< Violet */
		MM_CAMCORDER_COLOR_TONE_ORANGE, //CAMCORDER_EFFECT_ORANGE,			/**< Orange */
		MM_CAMCORDER_COLOR_TONE_GRAY, //CAMCORDER_EFFECT_GRAY,			/**< Gray */
		MM_CAMCORDER_COLOR_TONE_RED, //CAMCORDER_EFFECT_RED,			/**< Red */
		MM_CAMCORDER_COLOR_TONE_ANTIQUE, //CAMCORDER_EFFECT_ANTIQUE,		/**< Antique */
		MM_CAMCORDER_COLOR_TONE_WARM, //CAMCORDER_EFFECT_WARM,			/**< Warm */
		MM_CAMCORDER_COLOR_TONE_PINK, //CAMCORDER_EFFECT_PINK,		 	/**< Pink */
		MM_CAMCORDER_COLOR_TONE_YELLOW, //CAMCORDER_EFFECT_YELLOW,			/**< Yellow */
		MM_CAMCORDER_COLOR_TONE_PURPLE, //CAMCORDER_EFFECT_PURPLE,			/**< Purple */
		MM_CAMCORDER_COLOR_TONE_EMBOSS, //CAMCORDER_EFFECT_EMBOSS,			/**< Emboss */
		MM_CAMCORDER_COLOR_TONE_OUTLINE, //CAMCORDER_EFFECT_OUTLINE,		/**< Outline */
		MM_CAMCORDER_COLOR_TONE_SOLARIZATION_1, //CAMCORDER_EFFECT_SOLARIZATION,	/**< Solarization1 */
		MM_CAMCORDER_COLOR_TONE_SKETCH_1, //CAMCORDER_EFFECT_SKETCH,		/**< Sketch1 */	
	};
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_COLOR_TONE , maptable[abs(effect%20)], NULL);
	return _convert_camera_error_code(__func__, ret);
}
int camera_attr_set_scene_mode(camera_h camera,  camera_attr_scene_mode_e mode){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_SCENE_MODE  , mode, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}


int camera_attr_enable_tag(camera_h camera,  bool enable){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_TAG_ENABLE  , enable, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_tag_image_description(camera_h camera,  const char *description){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_TAG_IMAGE_DESCRIPTION    , description, strlen(description), NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_tag_orientation(camera_h camera,  camera_attr_tag_orientation_e orientation){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_TAG_ORIENTATION  , orientation, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_tag_software(camera_h camera,  const char *software){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_TAG_SOFTWARE   , software, strlen(software), NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_tag_latitude(camera_h camera,  double latitude){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_TAG_LATITUDE  , latitude, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_tag_longitude(camera_h camera,  double longtitude){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_TAG_LONGITUDE  , longtitude, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_set_tag_altitude(camera_h camera,  double altitude){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_TAG_ALTITUDE  , altitude, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}
int camera_attr_set_flash_mode(camera_h camera,  camera_attr_flash_mode_e mode){
	if( camera == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_STROBE_MODE  , mode, NULL);
	return _convert_camera_error_code(__func__, ret);	
}


int camera_attr_get_zoom(camera_h camera,  int *zoom){
	if( camera == NULL || zoom == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_DIGITAL_ZOOM , zoom, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_zoom_range(camera_h camera , int *min , int *max){
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo ainfo;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_DIGITAL_ZOOM, &ainfo);
	if( min )
		*min = ainfo.int_range.min;
	if( max )
		*max = ainfo.int_range.max;
	
	return _convert_camera_error_code(__func__, ret);	
}


int camera_attr_get_af_mode( camera_h camera,  camera_attr_af_mode_e *mode){
	if( camera == NULL || mode == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	int focus_mode;
	int af_range;
	int detect_mode;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FOCUS_MODE , &focus_mode, MMCAM_CAMERA_AF_SCAN_RANGE , &af_range, MMCAM_DETECT_MODE , &detect_mode, NULL);
	if( ret == CAMERA_ERROR_NONE){
		switch( focus_mode ){
			case MM_CAMCORDER_FOCUS_MODE_NONE :
			case MM_CAMCORDER_FOCUS_MODE_PAN :
			case MM_CAMCORDER_FOCUS_MODE_MANUAL :
				*mode = CAMERA_ATTR_AF_NONE;
				break;
			case MM_CAMCORDER_FOCUS_MODE_AUTO:
				switch ( af_range ){
					case MM_CAMCORDER_AUTO_FOCUS_NONE :
						*mode = CAMERA_ATTR_AF_NONE;
						break;
					case MM_CAMCORDER_AUTO_FOCUS_NORMAL:
						*mode = CAMERA_ATTR_AF_NORMAL;
						
						break;
					case MM_CAMCORDER_AUTO_FOCUS_MACRO:
						*mode = CAMERA_ATTR_AF_MACRO;
						break;
					case MM_CAMCORDER_AUTO_FOCUS_FULL:
						*mode = CAMERA_ATTR_AF_FULL;
						break;
				}
				break;
			case MM_CAMCORDER_FOCUS_MODE_TOUCH_AUTO:
				*mode = CAMERA_ATTR_AF_NORMAL;
				break;
		}
		
	}
	return _convert_camera_error_code(__func__, ret);	
}

int camera_attr_get_exposure_mode( camera_h camera,  camera_attr_exposure_mode_e *mode){
	if( camera == NULL|| mode == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int maptable[] = {
			CAMERA_ATTR_EXPOSURE_MODE_OFF,  //MM_CAMCORDER_AUTO_EXPOSURE_OFF
			CAMERA_ATTR_EXPOSURE_MODE_ALL,  //MM_CAMCORDER_AUTO_EXPOSURE_ALL
			CAMERA_ATTR_EXPOSURE_MODE_CENTER, //MM_CAMCORDER_AUTO_EXPOSURE_CENTER_1
			CAMERA_ATTR_EXPOSURE_MODE_CENTER, //MM_CAMCORDER_AUTO_EXPOSURE_CENTER_2
			CAMERA_ATTR_EXPOSURE_MODE_CENTER, //MM_CAMCORDER_AUTO_EXPOSURE_CENTER_3
			CAMERA_ATTR_EXPOSURE_MODE_SPOT, //MM_CAMCORDER_AUTO_EXPOSURE_SPOT_1
			CAMERA_ATTR_EXPOSURE_MODE_SPOT, //MM_CAMCORDER_AUTO_EXPOSURE_SPOT_2
			CAMERA_ATTR_EXPOSURE_MODE_CUSTOM,//MM_CAMCORDER_AUTO_EXPOSURE_CUSTOM_1
			CAMERA_ATTR_EXPOSURE_MODE_CUSTOM //MM_CAMCORDER_AUTO_EXPOSURE_CUSTOM_2
		};
	int ret;
	int exposure_mode;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_EXPOSURE_MODE , &exposure_mode, NULL);
	if( ret == CAMERA_ERROR_NONE ){
		*mode = maptable[abs(exposure_mode%9)];
	}
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_exposure(camera_h camera, int *value){
	if( camera == NULL || value == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_EXPOSURE_VALUE , value, NULL);
	return _convert_camera_error_code(__func__, ret);	
}


int camera_attr_get_exposure_range(camera_h camera, int *min, int *max){
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo ainfo;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_EXPOSURE_VALUE, &ainfo);
	if( min )
		*min = ainfo.int_range.min;
	if( max )
		*max = ainfo.int_range.max;
	
	return _convert_camera_error_code(__func__, ret);
}


int camera_attr_get_iso( camera_h camera,  camera_attr_iso_e *iso){
	if( camera == NULL || iso == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_ISO , iso, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_brightness(camera_h camera,  int *level){
	if( camera == NULL || level == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_BRIGHTNESS , level, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_brightness_range(camera_h camera, int *min, int *max){
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo ainfo;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_FILTER_BRIGHTNESS, &ainfo);
	if( min )
		*min = ainfo.int_range.min;
	if( max )
		*max = ainfo.int_range.max;
	
	return _convert_camera_error_code(__func__, ret);	
}


int camera_attr_get_contrast(camera_h camera,  int *level){
	if( camera == NULL || level == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_CONTRAST , level, NULL);
	return _convert_camera_error_code(__func__, ret);
}

int camera_attr_get_contrast_range(camera_h camera, int *min , int *max){
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo ainfo;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_FILTER_CONTRAST, &ainfo);
	if( min )
		*min = ainfo.int_range.min;
	if( max )
		*max = ainfo.int_range.max;
	
	return _convert_camera_error_code(__func__, ret);		
}


int camera_attr_get_whitebalance(camera_h camera,  camera_attr_whitebalance_e *wb){
	if( camera == NULL || wb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_WB , wb, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_effect(camera_h camera, camera_attr_effect_mode_e *effect){
		
	if( camera == NULL || effect == NULL ){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	int tone;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_COLOR_TONE , &tone, NULL);

	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);

	switch(tone){
		case MM_CAMCORDER_COLOR_TONE_NONE:
		case MM_CAMCORDER_COLOR_TONE_MONO:
		case MM_CAMCORDER_COLOR_TONE_SEPIA:
		case MM_CAMCORDER_COLOR_TONE_NEGATIVE:
		case MM_CAMCORDER_COLOR_TONE_BLUE:
		case MM_CAMCORDER_COLOR_TONE_GREEN:
		case MM_CAMCORDER_COLOR_TONE_AQUA:
		case MM_CAMCORDER_COLOR_TONE_VIOLET:
		case MM_CAMCORDER_COLOR_TONE_ORANGE:
		case MM_CAMCORDER_COLOR_TONE_GRAY:
		case MM_CAMCORDER_COLOR_TONE_RED:
		case MM_CAMCORDER_COLOR_TONE_ANTIQUE:
		case MM_CAMCORDER_COLOR_TONE_WARM:
		case MM_CAMCORDER_COLOR_TONE_PINK:
		case MM_CAMCORDER_COLOR_TONE_YELLOW:
		case MM_CAMCORDER_COLOR_TONE_PURPLE:
		case MM_CAMCORDER_COLOR_TONE_EMBOSS:
		case MM_CAMCORDER_COLOR_TONE_OUTLINE:
		case MM_CAMCORDER_COLOR_TONE_SOLARIZATION_1:
			*effect = tone;
			break;			
		case MM_CAMCORDER_COLOR_TONE_SOLARIZATION_2:
		case MM_CAMCORDER_COLOR_TONE_SOLARIZATION_3:
		case MM_CAMCORDER_COLOR_TONE_SOLARIZATION_4:
			*effect = CAMERA_ATTR_EFFECT_SOLARIZATION;
			break;
		case MM_CAMCORDER_COLOR_TONE_SKETCH_1:
		case MM_CAMCORDER_COLOR_TONE_SKETCH_2:
		case MM_CAMCORDER_COLOR_TONE_SKETCH_3:
		case MM_CAMCORDER_COLOR_TONE_SKETCH_4:
			*effect = CAMERA_ATTR_EFFECT_SKETCH;
			break;
	}

	return _convert_camera_error_code(__func__, ret);	
}

int camera_attr_get_scene_mode(camera_h camera,  camera_attr_scene_mode_e *mode){
		
	if( camera == NULL || mode == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_FILTER_SCENE_MODE , mode, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}


int camera_attr_is_enabled_tag(camera_h camera,  bool *enable){
	if( camera == NULL || enable == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_TAG_ENABLE , enable, NULL);

	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_tag_image_description(camera_h camera,  char **description){
	if( camera == NULL || description == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	char *ndescription = NULL;
	int desc_size;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_TAG_IMAGE_DESCRIPTION , &ndescription, &desc_size, NULL);
	if( ret == CAMERA_ERROR_NONE ){
		if( ndescription != NULL )
			*description = strdup(ndescription);
		else
			*description = strdup("");
	}
	
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_tag_orientation(camera_h camera,  camera_attr_tag_orientation_e *orientation){
		
	if( camera == NULL || orientation == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_TAG_ORIENTATION , orientation, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}
int camera_attr_get_tag_software(camera_h camera,  char **software){
	if( camera == NULL || software == NULL ){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	char *soft;
	int soft_size;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_TAG_SOFTWARE , &soft, &soft_size, NULL);
	if( ret == CAMERA_ERROR_NONE ){
		if( soft != NULL )
			*software = strdup(soft);
		else
			*software = strdup("");
	}
	
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_tag_latitude(camera_h camera,  double *latitude){
	if( camera == NULL || latitude == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_TAG_LATITUDE , latitude, NULL);
	return _convert_camera_error_code(__func__, ret);
	
}

int camera_attr_get_tag_longitude(camera_h camera,  double *longtitude){
	if( camera == NULL || longtitude == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_TAG_LONGITUDE  , longtitude, NULL);
	return _convert_camera_error_code(__func__, ret);
}

int camera_attr_get_tag_altitude(camera_h camera,  double *altitude){
	if( camera == NULL || altitude == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_TAG_ALTITUDE  , altitude, NULL);
	return _convert_camera_error_code(__func__, ret);
}
int camera_attr_get_flash_mode(camera_h camera,  camera_attr_flash_mode_e *mode){
	if( camera == NULL || mode == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_STROBE_MODE , mode, NULL);
	return _convert_camera_error_code(__func__, ret);
}

int camera_attr_foreach_supported_af_mode( camera_h camera, camera_attr_supported_af_mode_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	
	int ret;
	int i;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo af_range;
	MMCamAttrsInfo focus_mode;	
	
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_AF_SCAN_RANGE , &af_range);
	ret |= mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_FOCUS_MODE , &focus_mode);	
	
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);

	for( i=0 ; i < af_range.int_array.count ; i++)
	{
		if ( !foreach_cb(af_range.int_array.array[i],user_data) )
			goto ENDCALLBACK;
	}

	
	ENDCALLBACK:

	return CAMERA_ERROR_NONE;
	
}

int camera_attr_foreach_supported_exposure_mode(camera_h camera, camera_attr_supported_exposure_mode_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int maptable[] = {
			CAMERA_ATTR_EXPOSURE_MODE_OFF,  //MM_CAMCORDER_AUTO_EXPOSURE_OFF
			CAMERA_ATTR_EXPOSURE_MODE_ALL,  //MM_CAMCORDER_AUTO_EXPOSURE_ALL
			CAMERA_ATTR_EXPOSURE_MODE_CENTER, //MM_CAMCORDER_AUTO_EXPOSURE_CENTER_1
			-1, //MM_CAMCORDER_AUTO_EXPOSURE_CENTER_2
			-1, //MM_CAMCORDER_AUTO_EXPOSURE_CENTER_3
			CAMERA_ATTR_EXPOSURE_MODE_SPOT, //MM_CAMCORDER_AUTO_EXPOSURE_SPOT_1
			-1, //MM_CAMCORDER_AUTO_EXPOSURE_SPOT_2
			CAMERA_ATTR_EXPOSURE_MODE_CUSTOM,//MM_CAMCORDER_AUTO_EXPOSURE_CUSTOM_1
			-1//MM_CAMCORDER_AUTO_EXPOSURE_CUSTOM_2
		};
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_EXPOSURE_MODE , &info);
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	for( i=0 ; i < info.int_array.count ; i++)
	{
		if( maptable[info.int_array.array[i]] != -1){
			if ( !foreach_cb(maptable[info.int_array.array[i]],user_data) )
				break;
		}
	}
	return CAMERA_ERROR_NONE;
	
}
int camera_attr_foreach_supported_iso( camera_h camera, camera_attr_supported_iso_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_ISO , &info);
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	for( i=0 ; i < info.int_array.count ; i++)
	{
		if ( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}
	return CAMERA_ERROR_NONE;	
	
}

int camera_attr_foreach_supported_whitebalance(camera_h camera, camera_attr_supported_whitebalance_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_FILTER_WB , &info);
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	for( i=0 ; i < info.int_array.count ; i++)
	{
		if ( !foreach_cb(info.int_array.array[i],user_data)  )
			break;
	}
	return CAMERA_ERROR_NONE;	
	
}
int camera_attr_foreach_supported_effect(camera_h camera, camera_attr_supported_effect_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}		
	int maptable[] = {
		CAMERA_ATTR_EFFECT_NONE, //MM_CAMCORDER_COLOR_TONE_NONE
		CAMERA_ATTR_EFFECT_MONO, //MM_CAMCORDER_COLOR_TONE_MONO, 
		CAMERA_ATTR_EFFECT_SEPIA, //MM_CAMCORDER_COLOR_TONE_SEPIA, 	/**< Sepia */
		CAMERA_ATTR_EFFECT_NEGATIVE, //MM_CAMCORDER_COLOR_TONE_NEGATIVE, //,		/**< Negative */
		CAMERA_ATTR_EFFECT_BLUE, //MM_CAMCORDER_COLOR_TONE_BLUE, /**< Blue */
		CAMERA_ATTR_EFFECT_GREEN, //MM_CAMCORDER_COLOR_TONE_GREEN,		/**< Green */
		CAMERA_ATTR_EFFECT_AQUA, //MM_CAMCORDER_COLOR_TONE_AQUA, 	/**< Aqua */
		CAMERA_ATTR_EFFECT_VIOLET, //MM_CAMCORDER_COLOR_TONE_VIOLET, /**< Violet */
		CAMERA_ATTR_EFFECT_ORANGE, //MM_CAMCORDER_COLOR_TONE_ORANGE, //,			/**< Orange */
		CAMERA_ATTR_EFFECT_GRAY, //MM_CAMCORDER_COLOR_TONE_GRAY, //,			/**< Gray */
		CAMERA_ATTR_EFFECT_RED, //MM_CAMCORDER_COLOR_TONE_RED, //,			/**< Red */
		CAMERA_ATTR_EFFECT_ANTIQUE, //MM_CAMCORDER_COLOR_TONE_ANTIQUE 	/**< Antique */
		CAMERA_ATTR_EFFECT_WARM, //MM_CAMCORDER_COLOR_TONE_WARM, //,			/**< Warm */
		CAMERA_ATTR_EFFECT_PINK, //MM_CAMCORDER_COLOR_TONE_PINK, 	/**< Pink */
		CAMERA_ATTR_EFFECT_YELLOW, //MM_CAMCORDER_COLOR_TONE_YELLOW, 		/**< Yellow */
		CAMERA_ATTR_EFFECT_PURPLE, //MM_CAMCORDER_COLOR_TONE_PURPLE, 	/**< Purple */
		CAMERA_ATTR_EFFECT_EMBOSS, //MM_CAMCORDER_COLOR_TONE_EMBOSS,,			/**< Emboss */
		CAMERA_ATTR_EFFECT_OUTLINE, //MM_CAMCORDER_COLOR_TONE_OUTLINE, //,		/**< Outline */
		CAMERA_ATTR_EFFECT_SOLARIZATION, //MM_CAMCORDER_COLOR_TONE_SOLARIZATION_1, //,	/**< Solarization1 */
		-1, //MM_CAMCORDER_COLOR_TONE_SOLARIZATION_2
		-1 , //MM_CAMCORDER_COLOR_TONE_SOLARIZATION_3
		-1, //MM_CAMCORDER_COLOR_TONE_SOLARIZATION_4
		CAMERA_ATTR_EFFECT_SKETCH ,  //	MM_CAMCORDER_COLOR_TONE_SKETCH_1,/**< Sketch1 */	
		-1, //MM_CAMCORDER_COLOR_TONE_SKETCH_2
		-1, //MM_CAMCORDER_COLOR_TONE_SKETCH_3
		-1 //MM_CAMCORDER_COLOR_TONE_SKETCH_4
	};

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_FILTER_COLOR_TONE , &info);
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	for( i=0 ; i < info.int_array.count ; i++)
	{
		if( maptable[info.int_array.array[i]] != -1){
			if ( !foreach_cb(maptable[info.int_array.array[i]],user_data) )
				break;
		}
	}
	return CAMERA_ERROR_NONE;	
	
}
int camera_attr_foreach_supported_scene_mode(camera_h camera, camera_attr_supported_scene_mode_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_FILTER_SCENE_MODE  , &info);
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	for( i=0 ; i < info.int_array.count ; i++)
	{
		if ( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}
	return CAMERA_ERROR_NONE;	

}

int camera_attr_foreach_supported_flash_mode(camera_h camera, camera_attr_supported_flash_mode_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_STROBE_MODE  , &info);
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	for( i=0 ; i < info.int_array.count ; i++)
	{
		if ( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}
	return CAMERA_ERROR_NONE;	
	
}
int camera_attr_foreach_supported_fps(camera_h camera, camera_attr_supported_fps_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE( "[%s] INVALID_PARAMETER(0x%08x)",__func__,CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}	
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_FPS , &info);
	if( ret != CAMERA_ERROR_NONE )
		return _convert_camera_error_code(__func__, ret);
	
	int i;
	//if (foreach_cb(CAMERA_ATTR_FPS_AUTO, user_data) < 0 )
	//	return CAMERA_ERROR_NONE;
	for( i=0 ; i < info.int_array.count ; i++)
	{
		if ( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}
	return CAMERA_ERROR_NONE;

	
}



