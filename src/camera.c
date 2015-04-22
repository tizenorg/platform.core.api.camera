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
#include <audio-session-manager-types.h>
#include <mm_camcorder.h>
#include <mm_types.h>
#include <math.h>
#include <camera.h>
#include <camera_private.h>
#include <glib.h>
#include <dlog.h>
#include <gst/gst.h>
#include <tbm_bufmgr.h>
#include <tbm_surface_internal.h>
#include <Evas.h>
#include <Ecore.h>
#include <Elementary.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TIZEN_N_CAMERA"

static gboolean __mm_videostream_callback(MMCamcorderVideoStreamDataType * stream, void *user_data);
static gboolean __mm_capture_callback(MMCamcorderCaptureDataType *frame, MMCamcorderCaptureDataType *thumbnail, void *user_data);

void _camera_remove_cb_message(camera_s *handle){
	int ret = 0;
	GList *list = NULL;
	camera_cb_data *cb_data = NULL;

	if( handle == NULL ){
		LOGE("handle is NULL");
		return;
	}

	LOGI("start");

	g_mutex_lock(&handle->idle_cb_lock);

	if( handle->cb_data_list ){
		list = handle->cb_data_list;

		while( list ){
			cb_data = list->data;
			list =  g_list_next(list);

			if( !cb_data ){
				LOGW("cb_data is NULL");
			} else {
				ret = g_idle_remove_by_data (cb_data);
				LOGW("Remove cb_data[%p]. ret[%d]", cb_data, ret);

				handle->cb_data_list = g_list_remove(handle->cb_data_list, cb_data);
				free(cb_data);
				cb_data = NULL;
			}
		}

		g_list_free(handle->cb_data_list);
		handle->cb_data_list = NULL;
	} else {
		LOGW("There is no remained callback");
	}

	g_mutex_unlock(&handle->idle_cb_lock);

	LOGI("done");

	return;
}


int __convert_camera_error_code(const char* func, int code){
	int ret = CAMERA_ERROR_NONE;
	const char *errorstr = NULL;

	switch( code ){
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

	if( code != MM_ERROR_NONE ){
		LOGE("%s(0x%08x) : core frameworks error code(0x%08x)", errorstr, ret, code);
	}

	return ret;
}


static gboolean __mm_videostream_callback(MMCamcorderVideoStreamDataType * stream, void *user_data){
	if( user_data == NULL || stream == NULL)
		return 0;

	camera_s * handle = (camera_s*)user_data;
	if( handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW] ){
		camera_preview_data_s frame;
		frame.format = stream->format;
		if( frame.format  == (camera_pixel_format_e)MM_PIXEL_FORMAT_ITLV_JPEG_UYVY )
			frame.format  = MM_PIXEL_FORMAT_UYVY;
		frame.width = stream->width;
		frame.height = stream->height;
		frame.timestamp = stream->timestamp;
		frame.num_of_planes = stream->num_planes;
		switch( stream->data_type ){
			case MM_CAM_STREAM_DATA_YUV420 :
				frame.data.single_plane.yuv = stream->data.yuv420.yuv;
				frame.data.single_plane.size = stream->data.yuv420.length_yuv;
				break;
			case MM_CAM_STREAM_DATA_YUV422:
				frame.data.single_plane.yuv = stream->data.yuv422.yuv;
				frame.data.single_plane.size = stream->data.yuv422.length_yuv;
				break;
			case MM_CAM_STREAM_DATA_YUV420SP:
				frame.data.double_plane.y = stream->data.yuv420sp.y;
				frame.data.double_plane.uv = stream->data.yuv420sp.uv;
				frame.data.double_plane.y_size = stream->data.yuv420sp.length_y;
				frame.data.double_plane.uv_size = stream->data.yuv420sp.length_uv;
				break;
			case MM_CAM_STREAM_DATA_YUV420P:
				frame.data.triple_plane.y = stream->data.yuv420p.y;
				frame.data.triple_plane.u = stream->data.yuv420p.u;
				frame.data.triple_plane.v = stream->data.yuv420p.v;
				frame.data.triple_plane.y_size = stream->data.yuv420p.length_y;
				frame.data.triple_plane.u_size = stream->data.yuv420p.length_u;
				frame.data.triple_plane.v_size = stream->data.yuv420p.length_v;
				break;
			case MM_CAM_STREAM_DATA_YUV422P:
				frame.data.triple_plane.y = stream->data.yuv422p.y;
				frame.data.triple_plane.u = stream->data.yuv422p.u;
				frame.data.triple_plane.v = stream->data.yuv422p.v;
				frame.data.triple_plane.y_size = stream->data.yuv422p.length_y;
				frame.data.triple_plane.u_size = stream->data.yuv422p.length_u;
				frame.data.triple_plane.v_size = stream->data.yuv422p.length_v;
				break;
			default :
				break;
		}
		((camera_preview_cb)handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW])(&frame, handle->user_data[_CAMERA_EVENT_TYPE_PREVIEW]);
	}

	if( handle->user_cb[_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] ){
		media_packet_h pkt = NULL;
		tbm_surface_h tsurf = NULL;
		uint32_t bo_format = 0;
		int i;
		int bo_num;
		int ret = 0;
		media_format_mimetype_e mimetype = MEDIA_FORMAT_NV12;
		bool make_pkt_fmt = false;

		/* create tbm surface */
		for( i = 0, bo_num = 0 ; i < BUFFER_MAX_PLANE_NUM ; i++ ){
			if( stream->bo[i] ){
				bo_num++;
			}
		}

		/* get tbm surface format */
		ret = _camera_get_tbm_surface_format(stream->format, &bo_format);
		ret |= _camera_get_media_packet_mimetype(stream->format, &mimetype);

		if( bo_num > 0 && ret == CAMERA_ERROR_NONE ){
			tsurf = tbm_surface_internal_create_with_bos(stream->width, stream->height, bo_format, (tbm_bo *)stream->bo, bo_num);
			/*LOGD("tbm surface %p", tsurf);*/
		}

		if( tsurf ){
			/* check media packet format */
			if( handle->pkt_fmt ){
				int pkt_fmt_width = 0;
				int pkt_fmt_height = 0;
				media_format_mimetype_e pkt_fmt_mimetype = MEDIA_FORMAT_NV12;

				media_format_get_video_info(handle->pkt_fmt, &pkt_fmt_mimetype, &pkt_fmt_width, &pkt_fmt_height, NULL, NULL);
				if( pkt_fmt_mimetype != mimetype ||
				    pkt_fmt_width != stream->width ||
				    pkt_fmt_height != stream->height ){
					LOGW("different format. current 0x%x, %dx%d, new 0x%x, %dx%d",
					     pkt_fmt_mimetype, pkt_fmt_width, pkt_fmt_height, mimetype, stream->width, stream->height);
					media_format_unref(handle->pkt_fmt);
					handle->pkt_fmt = NULL;
					make_pkt_fmt = true;
				}
			} else {
				make_pkt_fmt = true;
			}

			/* create packet format */
			if( make_pkt_fmt ){
				LOGW("make new pkt_fmt - mimetype 0x%x, %dx%d", mimetype, stream->width, stream->height);
				ret = media_format_create(&handle->pkt_fmt);
				if (ret == MEDIA_FORMAT_ERROR_NONE) {
					ret = media_format_set_video_mime(handle->pkt_fmt, mimetype);
					ret |= media_format_set_video_width(handle->pkt_fmt, stream->width);
					ret |= media_format_set_video_height(handle->pkt_fmt, stream->height);
					LOGW("media_format_set_video_mime,width,height ret : 0x%x", ret);
				} else {
					LOGW("media_format_create failed");
				}
			}

			/* create media packet */
			ret = media_packet_create_from_tbm_surface(handle->pkt_fmt, tsurf, (media_packet_finalize_cb)_camera_media_packet_finalize, (void *)handle, &pkt);
			if( ret != MEDIA_PACKET_ERROR_NONE ){
				LOGE("media_packet_create_from_tbm_surface failed");

				tbm_surface_destroy(tsurf);
				tsurf = NULL;
			}
		} else {
			LOGE("failed to create tbm surface %dx%d, format %d, bo_num %d", stream->width, stream->height, stream->format, bo_num);
		}

		if( pkt ){
			/*LOGD("media packet %p, internal buffer %p", pkt, stream->internal_buffer);*/

			/* set internal buffer */
			ret = media_packet_set_extra(pkt, stream->internal_buffer);
			if( ret != MEDIA_PACKET_ERROR_NONE ){
				LOGE("media_packet_set_extra failed");

				media_packet_destroy(pkt);
				pkt = NULL;
			} else {
				/* set timestamp : msec -> nsec */
				if( media_packet_set_pts(pkt, (uint64_t)(stream->timestamp) * 1000000) != MEDIA_PACKET_ERROR_NONE ){
					LOGW("media_packet_set_pts failed");
				}

				/* increase ref count of gst buffer */
				gst_buffer_ref((GstBuffer *)stream->internal_buffer);

				/* call media packet callback */
				((camera_media_packet_preview_cb)handle->user_cb[_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW])(pkt, handle->user_data[_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW]);
			}
		}
	}

	return 1;
}

static gboolean __mm_capture_callback(MMCamcorderCaptureDataType *frame, MMCamcorderCaptureDataType *thumbnail, void *user_data){
	if( user_data == NULL || frame == NULL)
		return 0;

	camera_s * handle = (camera_s*)user_data;
	handle->current_capture_count++;
	if( handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE] ){
		MMCamcorderCaptureDataType *scrnl = NULL;
		int size = 0;
		camera_image_data_s image = { NULL, 0, 0, 0, 0, NULL, 0 };
		camera_image_data_s thumb = { NULL, 0, 0, 0, 0, NULL, 0 };
		camera_image_data_s postview = { NULL, 0, 0, 0, 0, NULL, 0 };
		if( frame ){
			int ret;
			unsigned char *exif;
			int exif_size;
			image.data = frame->data;
			image.size = frame->length;
			image.width = frame->width;
			image.height = frame->height;
			image.format = frame->format;
			ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, "captured-exif-raw-data", &exif, &exif_size, NULL);
			if( ret == MM_ERROR_NONE ){
				image.exif = exif;
				image.exif_size = exif_size;
			}
		}

		if( thumbnail ){
			thumb.data = thumbnail->data;
			thumb.size = thumbnail->length;
			thumb.width = thumbnail->width;
			thumb.height = thumbnail->height;
			thumb.format = thumbnail->format;
		}
		mm_camcorder_get_attributes( handle->mm_handle, NULL, "captured-screennail", &scrnl, &size,NULL );
		if( scrnl ){
			postview.data = scrnl->data;
			postview.size = scrnl->length;
			postview.width = scrnl->width;
			postview.height = scrnl->height;
			postview.format = scrnl->format;
		}

		((camera_capturing_cb)handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE])(&image, scrnl ? &postview : NULL, thumbnail ? &thumb : NULL, handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE]);
	}
	// update captured state
	if( handle->capture_count == 1 && handle->hdr_keep_mode ){
		if( handle->current_capture_count == 2 ){
			handle->is_capture_completed = true;
		}
	} else if( handle->capture_count == handle->current_capture_count || handle->is_continuous_shot_break){
		handle->is_capture_completed = true;
	}

	return 1;
}

static camera_state_e __camera_state_convert(MMCamcorderStateType mm_state){
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


static int __mm_camera_message_callback(int message, void *param, void *user_data){
	if( user_data == NULL || param == NULL )
		return 0;

	camera_s * handle = (camera_s*)user_data;

	if( handle->relay_message_callback )
		handle->relay_message_callback(message, param, handle->relay_user_data);

	MMMessageParamType *m = (MMMessageParamType*)param;
	camera_state_e previous_state;


	switch(message){
		case MM_MESSAGE_CAMCORDER_STATE_CHANGED:
		case MM_MESSAGE_CAMCORDER_STATE_CHANGED_BY_ASM:
		case MM_MESSAGE_CAMCORDER_STATE_CHANGED_BY_SECURITY:
			if( message == MM_MESSAGE_CAMCORDER_STATE_CHANGED && (m->state.previous < MM_CAMCORDER_STATE_NONE ||  m->state.previous > MM_CAMCORDER_STATE_PAUSED || m->state.code != 0) ){
				LOGI( "Invalid state changed message");
				break;
			}

			previous_state = handle->state;
			handle->state = __camera_state_convert(m->state.current );
			camera_policy_e policy = CAMERA_POLICY_NONE;
			if (message == MM_MESSAGE_CAMCORDER_STATE_CHANGED_BY_ASM) {
				switch (m->state.code) {
				case ASM_EVENT_SOURCE_CALL_START:
					policy = CAMERA_POLICY_SOUND_BY_CALL;
					LOGW("CAMERA_POLICY_SOUND_BY_CALL");
					break;
				case ASM_EVENT_SOURCE_ALARM_START:
				case ASM_EVENT_SOURCE_ALARM_END:
					policy = CAMERA_POLICY_SOUND_BY_ALARM;
					LOGW("CAMERA_POLICY_SOUND_BY_ALARM");
					break;
				default:
					policy = CAMERA_POLICY_SOUND;
					LOGW("CAMERA_POLICY_SOUND");
					break;
				}
			} else if (message == MM_MESSAGE_CAMCORDER_STATE_CHANGED_BY_SECURITY) {
				policy = CAMERA_POLICY_SECURITY;
				LOGW("CAMERA_POLICY_SECURITY");
			}

			if( previous_state != handle->state && handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] ){
				((camera_state_changed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE])(previous_state, handle->state, policy, handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE]);
			}

			// should change intermediate state MM_CAMCORDER_STATE_READY is not valid in capi , change to NULL state
			if( policy != CAMERA_POLICY_NONE &&
			    m->state.current == MM_CAMCORDER_STATE_NULL ){
				if( handle->user_cb[_CAMERA_EVENT_TYPE_INTERRUPTED]){
					((camera_interrupted_cb)handle->user_cb[_CAMERA_EVENT_TYPE_INTERRUPTED])(policy, previous_state, handle->state, handle->user_data[_CAMERA_EVENT_TYPE_INTERRUPTED]);
				} else {
					LOGW("_CAMERA_EVENT_TYPE_INTERRUPTED cb is NULL");
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
			handle->current_capture_complete_count = m->code;
			if( handle->capture_count == 1 || m->code == handle->capture_count ||(handle->is_continuous_shot_break && handle->state == CAMERA_STATE_CAPTURING) ){
				//pseudo state change
				previous_state = handle->state ;
				handle->state = CAMERA_STATE_CAPTURED;
				if( previous_state != handle->state && handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] ){
					((camera_state_changed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE])(previous_state, handle->state,  0 , handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE]);
				}
				if( handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] ){
					((camera_capture_completed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE])(handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE]);
				}
			}
			break;
		}
		case MM_MESSAGE_CAMCORDER_VIDEO_CAPTURED:
		case MM_MESSAGE_CAMCORDER_AUDIO_CAPTURED:
		{
			MMCamRecordingReport *report = (MMCamRecordingReport *)m ->data;
			if( report != NULL && report->recording_filename ){
				free(report->recording_filename );
				report->recording_filename = NULL;
			}
			if( report ){
				free(report);
				report = NULL;
			}
			break;
		}
		case MM_MESSAGE_CAMCORDER_VIDEO_SNAPSHOT_CAPTURED:
		{
			if( handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] ){
				((camera_capture_completed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE])(handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE]);
			}
			break;
		}
		case MM_MESSAGE_CAMCORDER_ERROR:
		{
			int errorcode = m->code;
			int camera_error = 0;
			switch( errorcode ){
				case MM_ERROR_CAMCORDER_DEVICE :
				case MM_ERROR_CAMCORDER_DEVICE_TIMEOUT:
				case MM_ERROR_CAMCORDER_DEVICE_WRONG_JPEG:
					camera_error = CAMERA_ERROR_DEVICE;
					break;
				case MM_ERROR_CAMCORDER_GST_CORE:
				case MM_ERROR_CAMCORDER_GST_LIBRARY:
				case MM_ERROR_CAMCORDER_GST_RESOURCE:
				case MM_ERROR_CAMCORDER_GST_STREAM:
				case MM_ERROR_CAMCORDER_GST_NEGOTIATION:
				case MM_ERROR_CAMCORDER_GST_FLOW_ERROR:
				case MM_ERROR_CAMCORDER_ENCODER:
				case MM_ERROR_CAMCORDER_ENCODER_BUFFER:
				case MM_ERROR_CAMCORDER_ENCODER_WORKING:
				case MM_ERROR_CAMCORDER_MNOTE_CREATION:
				case MM_ERROR_CAMCORDER_MNOTE_ADD_ENTRY:
				case MM_ERROR_CAMCORDER_INTERNAL:
				case MM_ERROR_FILE_NOT_FOUND:
				case MM_ERROR_FILE_READ:
					camera_error = CAMERA_ERROR_INVALID_OPERATION;
					break;
				case MM_ERROR_CAMCORDER_LOW_MEMORY:
				case MM_ERROR_CAMCORDER_MNOTE_MALLOC:
					camera_error = CAMERA_ERROR_OUT_OF_MEMORY;
					break;
				case MM_ERROR_CAMCORDER_DEVICE_REG_TROUBLE:
					camera_error = CAMERA_ERROR_ESD;
					break;
				default :
					camera_error = CAMERA_ERROR_INVALID_OPERATION;
					break;
			}

			/* set capture completed flag as true to release camera handle */
			handle->is_capture_completed = true;

			if( camera_error != 0 && handle->user_cb[_CAMERA_EVENT_TYPE_ERROR] )
				((camera_error_cb)handle->user_cb[_CAMERA_EVENT_TYPE_ERROR])(camera_error, handle->state , handle->user_data[_CAMERA_EVENT_TYPE_ERROR]);

			break;
		}
		case MM_MESSAGE_CAMCORDER_HDR_PROGRESS:
		{
			int percent = m->code;
			if( handle->user_cb[_CAMERA_EVENT_TYPE_HDR_PROGRESS] )
				((camera_attr_hdr_progress_cb)handle->user_cb[_CAMERA_EVENT_TYPE_HDR_PROGRESS])(percent, handle->user_data[_CAMERA_EVENT_TYPE_HDR_PROGRESS]);
			break;
		}
		case MM_MESSAGE_CAMCORDER_FACE_DETECT_INFO:
		{
			MMCamFaceDetectInfo *cam_fd_info = (MMCamFaceDetectInfo *)(m->data);
			if ( cam_fd_info ) {
				camera_detected_face_s faces[cam_fd_info->num_of_faces];
				handle->num_of_faces = cam_fd_info->num_of_faces > MAX_DETECTED_FACE ? MAX_DETECTED_FACE : cam_fd_info->num_of_faces;
				int i;
				for(i=0; i < handle->num_of_faces ; i++){
					faces[i].id = cam_fd_info->face_info[i].id;
					faces[i].score = cam_fd_info->face_info[i].score;
					faces[i].x = cam_fd_info->face_info[i].rect.x;
					faces[i].y = cam_fd_info->face_info[i].rect.y;
					faces[i].width = cam_fd_info->face_info[i].rect.width;
					faces[i].height = cam_fd_info->face_info[i].rect.height;
					handle->faceinfo[i] = faces[i];	//cache face coordinate
				}
				if( handle->user_cb[_CAMERA_EVENT_TYPE_FACE_DETECTION] )
					((camera_face_detected_cb)handle->user_cb[_CAMERA_EVENT_TYPE_FACE_DETECTION])(faces, handle->num_of_faces, handle->user_data[_CAMERA_EVENT_TYPE_FACE_DETECTION]);
			}else{
				handle->num_of_faces = 0;
			}
			break;
		}
		default:
			break;
	}

	return 1;
}

static int __capture_completed_event_cb(void *data){
	camera_s *handle = (camera_s*)data;
	if( handle->current_capture_count > 0 && handle->current_capture_count == handle->current_capture_complete_count && handle->state == CAMERA_STATE_CAPTURING ){
		//pseudo state change
		camera_state_e previous_state = handle->state;
		handle->state = CAMERA_STATE_CAPTURED;
		if( previous_state != handle->state && handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] ){
			((camera_state_changed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE])(previous_state, handle->state,  0 , handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE]);
		}
		if( handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] ){
			((camera_capture_completed_cb)handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE])(handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE]);
		}
	}
	return false;
}

int camera_create(camera_device_e device, camera_h* camera){

	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	MMCamPreset info;
	int preview_format;
	int rotation;

	LOGW("device name = [%d]",device);

	info.videodev_type = device;

	camera_s* handle = (camera_s*)malloc( sizeof(camera_s) );
	if( handle==NULL ){
		LOGE("malloc fail");
		return CAMERA_ERROR_OUT_OF_MEMORY;
	}
	memset(handle, 0 , sizeof(camera_s));

	ret = mm_camcorder_create(&handle->mm_handle, &info);
	if( ret != MM_ERROR_NONE ){
		free(handle);
		return __convert_camera_error_code(__func__,ret);
	}

	preview_format = MM_PIXEL_FORMAT_YUYV;
	rotation = MM_DISPLAY_ROTATION_NONE;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_RECOMMEND_PREVIEW_FORMAT_FOR_CAPTURE, &preview_format,
					  MMCAM_RECOMMEND_DISPLAY_ROTATION, &rotation,
					  MMCAM_CAPTURE_WIDTH, &handle->capture_width,
					  MMCAM_CAPTURE_HEIGHT, &handle->capture_height,
					  NULL);

	char *error;
	ret = mm_camcorder_set_attributes(handle->mm_handle, &error,
					  MMCAM_MODE , MM_CAMCORDER_MODE_VIDEO_CAPTURE,
					  MMCAM_CAMERA_FORMAT,  preview_format,
					  MMCAM_IMAGE_ENCODER , MM_IMAGE_CODEC_JPEG,
					  MMCAM_CAPTURE_FORMAT,  MM_PIXEL_FORMAT_ENCODED,
					  MMCAM_DISPLAY_SURFACE, MM_DISPLAY_SURFACE_NULL,
					  MMCAM_DISPLAY_ROTATION, rotation,
					  MMCAM_CAPTURE_COUNT, 1,
					  (void*)NULL);

	handle->display_type = CAMERA_DISPLAY_TYPE_NONE;

	if( ret != MM_ERROR_NONE ){
		LOGE("mm_camcorder_set_attributes fail(%x, %s)", ret, error);
		mm_camcorder_destroy(handle->mm_handle);
		free(error);
		free(handle);
		return __convert_camera_error_code(__func__, ret);
	}

	handle->state = CAMERA_STATE_CREATED;
	handle->relay_message_callback = NULL;
	handle->relay_user_data = NULL;
	handle->capture_resolution_modified = false;
	handle->hdr_keep_mode = false;
	handle->focus_area_valid = false;
	handle->is_used_in_recorder = false;
	handle->on_continuous_focusing = false;
	handle->cached_focus_mode = -1;
	g_mutex_init(&handle->idle_cb_lock);
	mm_camcorder_set_message_callback(handle->mm_handle, __mm_camera_message_callback, (void*)handle);

	LOGW("camera handle %p", handle);

	*camera = (camera_h)handle;
	return __convert_camera_error_code(__func__, ret);
}

 int camera_destroy(camera_h camera)
{
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s *handle = (camera_s*)camera;
	if( handle->is_used_in_recorder ){
		LOGE("camera is using in another recorder.");
		return CAMERA_ERROR_INVALID_OPERATION;
	}

	LOGW("camera handle %p", handle);

	ret = mm_camcorder_destroy(handle->mm_handle);

	if( handle->pkt_fmt ){
		media_format_unref(handle->pkt_fmt);
		handle->pkt_fmt = NULL;
	}

	if( ret == MM_ERROR_NONE ){
		_camera_remove_cb_message(handle);
		g_mutex_clear(&handle->idle_cb_lock);
#ifdef HAVE_WAYLAND
		if (handle->wl_info) {
			free(handle->wl_info);
			handle->wl_info = NULL;
		}
#endif /* HAVE_WAYLAND */
		free(handle);
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_start_preview(camera_h camera){
	LOGW("start");
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s *handle = (camera_s*)camera;
	camera_state_e capi_state;
	camera_get_state(camera, &capi_state);

	if( capi_state == CAMERA_STATE_CAPTURED ){
		ret = mm_camcorder_capture_stop(handle->mm_handle);
		return __convert_camera_error_code(__func__, ret);
	}

	/*for receving MM_MESSAGE_CAMCORDER_CAPTURED evnet must be seted capture callback*/
	mm_camcorder_set_video_capture_callback( handle->mm_handle, (mm_camcorder_video_capture_callback)__mm_capture_callback, (void*)handle);

	MMCamcorderStateType state;
	mm_camcorder_get_state(handle->mm_handle, &state);
	if( state != MM_CAMCORDER_STATE_READY ){
		ret = mm_camcorder_realize(handle->mm_handle);
		if( ret != MM_ERROR_NONE ){
			return __convert_camera_error_code(__func__, ret);
		}
	}

	ret = mm_camcorder_start(handle->mm_handle);
	if( ret != MM_ERROR_NONE ){
		/*start fail */
		mm_camcorder_unrealize(handle->mm_handle);
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_stop_preview(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s *handle = (camera_s*)camera;
	MMCamcorderStateType state ;
	mm_camcorder_get_state(handle->mm_handle, &state);

	if( state == MM_CAMCORDER_STATE_PREPARE ){
		ret = mm_camcorder_stop(handle->mm_handle);
		if( ret != MM_ERROR_NONE ){
			return __convert_camera_error_code(__func__, ret);
		}
	}

	camera_stop_face_detection(camera);

	ret = mm_camcorder_unrealize(handle->mm_handle);

	return __convert_camera_error_code(__func__, ret);
}

int camera_start_capture(camera_h camera, camera_capturing_cb capturing_cb , camera_capture_completed_cb completed_cb , void *user_data){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	int ret;
	MMCamcorderStateType state;
	mm_camcorder_get_state(handle->mm_handle, &state);
	if( state != MM_CAMCORDER_STATE_PREPARE &&
	    state != MM_CAMCORDER_STATE_RECORDING &&
	    state != MM_CAMCORDER_STATE_PAUSED ){
		LOGE("INVALID_STATE(0x%08x)",CAMERA_ERROR_INVALID_STATE);
		return CAMERA_ERROR_INVALID_STATE;
	}

	if( handle->capture_resolution_modified ){
		mm_camcorder_set_attributes(handle->mm_handle, NULL,
					    MMCAM_CAPTURE_WIDTH, handle->capture_width,
					    MMCAM_CAPTURE_HEIGHT, handle->capture_height,
					    NULL);
		handle->capture_resolution_modified = false;
	}
	mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_CAPTURE_COUNT , 1,NULL);

	handle->capture_count = 1;
	handle->is_continuous_shot_break = false;
	handle->current_capture_count = 0;
	handle->current_capture_complete_count = 0;
	handle->is_capture_completed = false;

	handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE] = (void*)capturing_cb;
	handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE] = (void*)user_data;
	handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = (void*)completed_cb;
	handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = (void*)user_data;
	ret = mm_camcorder_capture_start(handle->mm_handle);
	if( ret != 0 ){
		handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE] = NULL;
		handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE] = NULL;
		handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = NULL;
		handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = NULL;
	}

	return __convert_camera_error_code(__func__, ret);
}

bool camera_is_supported_continuous_capture(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int ret = MM_ERROR_NONE;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAPTURE_COUNT , &info);
	set_last_result(__convert_camera_error_code(__func__, ret));
	if( ret != MM_ERROR_NONE ){
		return false;
	}
	if( info.int_range.max > 1){
		return true;
	} else {
		return false;
	}
}

int camera_start_continuous_capture(camera_h camera, int count, int interval, camera_capturing_cb capturing_cb, camera_capture_completed_cb completed_cb , void *user_data){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_is_supported_continuous_capture(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	if( count < 2 || interval < 0 ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;

	MMCamcorderStateType state;
	mm_camcorder_get_state(handle->mm_handle, &state);
	if( state != MM_CAMCORDER_STATE_PREPARE ){
		LOGE("INVALID_STATE(0x%08x)",CAMERA_ERROR_INVALID_STATE);
		return CAMERA_ERROR_INVALID_STATE;
	}

	int supported_zsl = FALSE;

	int ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					      MMCAM_CAPTURE_COUNT, count,
					      MMCAM_CAPTURE_INTERVAL, interval,
					      NULL);
	if( ret != 0 ){
		LOGE("(%x) error set continuous shot attribute", ret);
		return __convert_camera_error_code(__func__, ret);
	}

	handle->capture_count = count;
	handle->is_continuous_shot_break = false;
	handle->current_capture_count = 0;
	handle->current_capture_complete_count = 0;
	handle->is_capture_completed = false;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_SUPPORT_ZSL_CAPTURE, &supported_zsl, NULL);
	if( ret != 0 ){
		LOGE("(%x) error get continuous shot attribute", ret);
	}

	if( !supported_zsl ){
		int preview_width;
		int preview_height;
		int capture_width;
		int capture_height;

		mm_camcorder_get_attributes(handle->mm_handle, NULL,
					    MMCAM_CAMERA_WIDTH, &preview_width,
					    MMCAM_CAMERA_HEIGHT, &preview_height,
					    MMCAM_CAPTURE_WIDTH, &capture_width,
					    MMCAM_CAPTURE_HEIGHT, &capture_height,
					    NULL);
		if( preview_width != capture_width || preview_height != capture_height ){
			mm_camcorder_set_attributes(handle->mm_handle, NULL,
						    MMCAM_CAPTURE_WIDTH, preview_width,
						    MMCAM_CAPTURE_HEIGHT, preview_height,
						    NULL);
			handle->capture_resolution_modified = true;
		}
	}

	handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE] = (void*)capturing_cb;
	handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE] = (void*)user_data;
	handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = (void*)completed_cb;
	handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = (void*)user_data;

	ret = mm_camcorder_capture_start(handle->mm_handle);
	if( ret != 0 ){
		handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE] = NULL;
		handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE] = NULL;
		handle->user_cb[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = NULL;
		handle->user_data[_CAMERA_EVENT_TYPE_CAPTURE_COMPLETE] = NULL;
	}

	return __convert_camera_error_code(__func__,ret);

}

int camera_stop_continuous_capture(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_is_supported_continuous_capture(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	camera_s *handle = (camera_s*)camera;

	int ret;
	camera_state_e state;
	camera_get_state(camera, &state);

	if( state != CAMERA_STATE_CAPTURING && handle->capture_count > 1 ){
		LOGE("INVALID_STATE(0x%08x)",CAMERA_ERROR_INVALID_STATE);
		return CAMERA_ERROR_INVALID_STATE;
	}

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL, "capture-break-cont-shot", 1, NULL);
	if( ret == MM_ERROR_NONE ){
		handle->is_continuous_shot_break = true;
		if( handle->current_capture_count > 0 ){
			handle->is_capture_completed = true;
		}
		g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, __capture_completed_event_cb, handle, NULL);
	}

	return __convert_camera_error_code(__func__,ret);
}

bool camera_is_supported_face_detection(camera_h camera){
	if (camera == NULL) {
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int i = 0;
	int ret = MM_ERROR_NONE;
	camera_s *handle = (camera_s *)camera;
	MMCamAttrsInfo info;

	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_DETECT_MODE , &info);
	set_last_result(__convert_camera_error_code(__func__, ret));
	if( ret != MM_ERROR_NONE ){
		LOGE("MMCAM_DETECT_MODE get attr info failed");
		return false;
	}

	if( info.validity_type == MM_CAM_ATTRS_VALID_TYPE_INT_ARRAY ){
		for( i = 0 ; i < info.int_array.count ; i++ ){
			if( info.int_array.array[i] == MM_CAMCORDER_DETECT_MODE_ON ){
				LOGD("face detection supported");
				return true;
			}
		}
	}

	LOGD("face detection NOT supported");

	return false;
}

bool camera_is_supported_zero_shutter_lag(camera_h camera){
	int ret = MM_ERROR_NONE;
	int supported_zsl = false;
	camera_s *handle = (camera_s *)camera;

	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_SUPPORT_ZSL_CAPTURE, &supported_zsl, NULL);
	set_last_result(__convert_camera_error_code(__func__, ret));
	if( ret != MM_ERROR_NONE ){
		LOGE("MMCAM_SUPPORT_ZSL_CAPTURE get failed");
		return false;
	}

	LOGD("support zero shutter lag : %d", supported_zsl);

	return supported_zsl;
}

bool camera_is_supported_media_packet_preview_cb(camera_h camera){
	int ret = MM_ERROR_NONE;
	int supported = false;
	camera_s *handle = (camera_s *)camera;

	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_SUPPORT_MEDIA_PACKET_PREVIEW_CB, &supported,
					  NULL);
	set_last_result(__convert_camera_error_code(__func__, ret));
	if( ret != MM_ERROR_NONE ){
		LOGE("MMCAM_SUPPORT_MEDIA_PACKET_PREVIEW_CB get failed");
		return false;
	}

	LOGD("support media packet preview callback : %d", supported);

	return supported;
}

int camera_get_device_count(camera_h camera, int *device_count){
	int ret = MM_ERROR_NONE;
	camera_s *handle = (camera_s *)camera;

	if( camera == NULL || device_count == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_DEVICE_COUNT, device_count,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_start_face_detection(camera_h camera, camera_face_detected_cb callback, void * user_data){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_is_supported_face_detection(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	camera_s * handle = (camera_s*)camera;
	camera_state_e state = CAMERA_STATE_NONE;
	int ret;
	camera_get_state(camera, &state);
	if( state != CAMERA_STATE_PREVIEW ){
		LOGE("INVALID_STATE(0x%08x)",CAMERA_ERROR_INVALID_STATE);
		return CAMERA_ERROR_INVALID_STATE;
	}

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_DETECT_MODE, MM_CAMCORDER_DETECT_MODE_ON,
					  NULL);
	if( ret == MM_ERROR_NONE ){
		handle->user_cb[_CAMERA_EVENT_TYPE_FACE_DETECTION] = (void*)callback;
		handle->user_data[_CAMERA_EVENT_TYPE_FACE_DETECTION] = (void*)user_data;
		handle->num_of_faces = 0;
	}

	return __convert_camera_error_code(__func__,ret);
}

int camera_stop_face_detection(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s * handle = (camera_s*)camera;
	int ret;

	if( camera_is_supported_face_detection(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_DETECT_MODE, MM_CAMCORDER_DETECT_MODE_OFF, NULL);
	handle->user_cb[_CAMERA_EVENT_TYPE_FACE_DETECTION] = NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_FACE_DETECTION] = NULL;
	handle->num_of_faces = 0;
	return __convert_camera_error_code(__func__,ret);
}

int camera_get_state(camera_h camera, camera_state_e * state){
	if( camera == NULL || state == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s *handle = (camera_s*)camera;
	camera_state_e capi_state;
	MMCamcorderStateType mmstate ;
	mm_camcorder_get_state(handle->mm_handle, &mmstate);

	capi_state = __camera_state_convert(mmstate);

	if( (handle->state == CAMERA_STATE_CAPTURED || handle->is_capture_completed) && (handle->current_capture_count > 0 || handle->is_capture_completed) && mmstate == MM_CAMCORDER_STATE_CAPTURING ){
		capi_state = CAMERA_STATE_CAPTURED;
	}

	*state = capi_state;
	return CAMERA_ERROR_NONE;
}

int camera_start_focusing( camera_h camera, bool continuous ){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s *handle = (camera_s*)camera;

	if( handle->cached_focus_mode != -1 ){
		LOGD("apply cached focus mode %d", handle->cached_focus_mode);
		mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_AF_SCAN_RANGE  , handle->cached_focus_mode, NULL);
		handle->cached_focus_mode = -1;
	}

	if( continuous )
		return __camera_start_continuous_focusing(camera);
	else{
		mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_FOCUS_MODE, handle->focus_area_valid ? MM_CAMCORDER_FOCUS_MODE_TOUCH_AUTO : MM_CAMCORDER_FOCUS_MODE_AUTO, NULL);
		return __convert_camera_error_code(__func__, mm_camcorder_start_focusing(((camera_s*)camera)->mm_handle));
	}
}

int __camera_start_continuous_focusing(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s *handle = (camera_s*)camera;
	int ret;
	int mode;
	handle->on_continuous_focusing = true;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FOCUS_MODE , &mode, NULL);

	if( mode == MM_CAMCORDER_FOCUS_MODE_CONTINUOUS )
		ret = mm_camcorder_start_focusing(handle->mm_handle);
	else
		ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FOCUS_MODE, MM_CAMCORDER_FOCUS_MODE_CONTINUOUS, NULL);
	return __convert_camera_error_code(__func__, ret);
}

int camera_cancel_focusing(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s *handle = (camera_s*)camera;
	handle->on_continuous_focusing = false;
	return __convert_camera_error_code(__func__, mm_camcorder_stop_focusing(handle->mm_handle));
}

int camera_set_display(camera_h camera, camera_display_type_e type, camera_display_h display)
{
	int ret = MM_ERROR_NONE;
	int set_surface = MM_DISPLAY_SURFACE_X;
	void *set_handle = NULL;
	camera_s *handle = NULL;

	Evas_Object *obj = NULL;
	const char *object_type = NULL;

	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( type != CAMERA_DISPLAY_TYPE_NONE && display == NULL ){
		LOGE("display type[%d] is not NONE, but display handle is NULL", type);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	handle = (camera_s *)camera;
	handle->display_type = type;

	if( type == CAMERA_DISPLAY_TYPE_NONE ){
		/* NULL surface */
		set_surface = MM_DISPLAY_SURFACE_NULL;
		handle->display_handle = 0;

		LOGD("display type NONE");
	} else {
		obj = (Evas_Object *)display;
		object_type = evas_object_type_get(obj);
		if( object_type ){
			if( type == CAMERA_DISPLAY_TYPE_OVERLAY && !strcmp(object_type, "elm_win") ){
#ifdef HAVE_WAYLAND
				MMCamWaylandInfo *wl_info = (MMCamWaylandInfo *)malloc(sizeof(MMCamWaylandInfo));

				if (wl_info == NULL) {
					LOGE("wl_info alloc failed : %d", sizeof(MMCamWaylandInfo));
					return __convert_camera_error_code(__func__, MM_ERROR_CAMCORDER_LOW_MEMORY);
				}

				memset(wl_info, 0x0, sizeof(MMCamWaylandInfo));

				wl_info->evas_obj = (void *)obj;
				wl_info->window = (void *)elm_win_wl_window_get(obj);
				wl_info->surface = (void *)ecore_wl_window_surface_get(wl_info->window);
				wl_info->display = (void *)ecore_wl_display_get();

				if (wl_info->window == NULL || wl_info->surface == NULL || wl_info->display == NULL) {
					LOGE("something is NULL %p, %p, %p", wl_info->window, wl_info->surface, wl_info->display);
					free(wl_info);
					return __convert_camera_error_code(__func__, MM_ERROR_CAMCORDER_INTERNAL);
				}

				evas_object_geometry_get(obj, &wl_info->window_x, &wl_info->window_y,
							      &wl_info->window_width, &wl_info->window_height);

				if (handle->wl_info) {
					free(handle->wl_info);
					handle->wl_info = NULL;
				}

				/* set wayland info */
				handle->wl_info = (void *)wl_info;
				set_surface = MM_DISPLAY_SURFACE_X;
				set_handle = (void *)wl_info;

				LOGD("wayland obj %p, window %p, surface %p, display %p, size %d,%d,%dx%d",
				     wl_info->evas_obj, wl_info->window, wl_info->surface, wl_info->display,
				     wl_info->window_x, wl_info->window_y, wl_info->window_width, wl_info->window_height);
#else /* HAVE_WAYLAND */
				/* x window overlay surface */
				handle->display_handle = (void *)elm_win_xwindow_get(obj);
				set_surface = MM_DISPLAY_SURFACE_X;
				set_handle = &(handle->display_handle);

				LOGD("display type OVERLAY : handle %p, %d", set_handle, (int)handle->display_handle);
#endif /* HAVE_WAYLAND */
			} else if( type == CAMERA_DISPLAY_TYPE_EVAS && !strcmp(object_type, "image") ) {
				/* evas object surface */
				handle->display_handle = display;
				set_surface = MM_DISPLAY_SURFACE_EVAS;
				set_handle = display;

				LOGD("display type EVAS : handle %p", set_handle);
			} else {
				LOGE("unknown evas object [%p,%s] or type [%d] mismatch", obj, object_type, type);
				return CAMERA_ERROR_INVALID_PARAMETER;
			}
		} else {
			LOGE("failed to get evas object type from %p", obj);
			return CAMERA_ERROR_INVALID_PARAMETER;
		}
	}

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_DEVICE, MM_DISPLAY_DEVICE_MAINLCD,
					  MMCAM_DISPLAY_SURFACE, set_surface,
					  NULL);

	if( ret == MM_ERROR_NONE && type != CAMERA_DISPLAY_TYPE_NONE ){
		ret = mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_DISPLAY_HANDLE, set_handle, sizeof(void *), NULL);
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_preview_resolution(camera_h camera,  int width, int height){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_WIDTH  , width ,MMCAM_CAMERA_HEIGHT ,height,  NULL);
	return __convert_camera_error_code(__func__, ret);
}

int camera_set_capture_resolution(camera_h camera,  int width, int height){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAPTURE_WIDTH, width  ,MMCAM_CAPTURE_HEIGHT , height, NULL);

	if( ret == MM_ERROR_NONE ){
		handle->capture_width = width;
		handle->capture_height = height;
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_capture_format(camera_h camera, camera_pixel_format_e format){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAPTURE_FORMAT, format , NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_preview_format(camera_h camera, camera_pixel_format_e format){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	if( format == CAMERA_PIXEL_FORMAT_UYVY ){
		bool supported_ITLV_UYVY = false;
		MMCamAttrsInfo supported_format;
		ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_FORMAT , &supported_format);
		int i;
		for( i=0 ; i < supported_format.int_array.count ; i++ ){
			if( supported_format.int_array.array[i] == MM_PIXEL_FORMAT_ITLV_JPEG_UYVY )
				supported_ITLV_UYVY = true;
		}
		ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
						  MMCAM_CAMERA_FORMAT, supported_ITLV_UYVY ? MM_PIXEL_FORMAT_ITLV_JPEG_UYVY : MM_PIXEL_FORMAT_UYVY,
						  NULL);
	} else {
		ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_FORMAT, format , NULL);
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_get_preview_resolution(camera_h camera,  int *width, int *height){
	if( camera == NULL || width == NULL || height == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_WIDTH, width,
					  MMCAM_CAMERA_HEIGHT, height,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_display_rotation(camera_h camera, camera_rotation_e rotation){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( rotation > CAMERA_ROTATION_270 )
		return CAMERA_ERROR_INVALID_PARAMETER;

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_ROTATION, rotation,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_get_display_rotation( camera_h camera,  camera_rotation_e *rotation){
	if( camera == NULL || rotation == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_ROTATION , rotation, NULL);
	return __convert_camera_error_code(__func__, ret);
}

int camera_set_display_flip(camera_h camera, camera_flip_e flip){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( flip > CAMERA_FLIP_BOTH )
		return CAMERA_ERROR_INVALID_PARAMETER;

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_FLIP, flip,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_get_display_flip(camera_h camera, camera_flip_e *flip){
	if( camera == NULL || flip == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_FLIP, flip,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_display_visible(camera_h camera, bool visible){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_VISIBLE, visible,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_is_display_visible(camera_h camera, bool* visible){
	if( camera == NULL || visible == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	int result;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_VISIBLE, &result,
					  NULL);

	if( ret == MM_ERROR_NONE)
		*visible = result;

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_display_mode(camera_h camera, camera_display_mode_e mode){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( mode > CAMERA_DISPLAY_MODE_CROPPED_FULL )
		return CAMERA_ERROR_INVALID_PARAMETER;

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_GEOMETRY_METHOD, mode,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_get_display_mode(camera_h camera, camera_display_mode_e* mode){
	if( camera == NULL || mode == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_DISPLAY_GEOMETRY_METHOD, mode,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_get_capture_resolution(camera_h camera, int *width, int *height){
	if( camera == NULL || width== NULL || height == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	*width = handle->capture_width;
	*height = handle->capture_height;

	return CAMERA_ERROR_NONE;
}

int camera_get_capture_format(camera_h camera, camera_pixel_format_e *format){
	if( camera == NULL || format == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_CAPTURE_FORMAT, format,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_get_preview_format(camera_h camera, camera_pixel_format_e *format){
	if( camera == NULL || format == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_FORMAT, format,
					  NULL);

	if( (MMPixelFormatType)*format == MM_PIXEL_FORMAT_ITLV_JPEG_UYVY )
		*format = CAMERA_PIXEL_FORMAT_UYVY;

	return __convert_camera_error_code(__func__, ret);
}

int camera_set_preview_cb(camera_h camera, camera_preview_cb callback, void* user_data){
	if( camera == NULL || callback == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s *handle = (camera_s *)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_PREVIEW] = (void*)user_data;

	mm_camcorder_set_video_stream_callback(handle->mm_handle,
					       (mm_camcorder_video_stream_callback)__mm_videostream_callback,
					       (void *)handle);

	return CAMERA_ERROR_NONE;
}

int camera_unset_preview_cb( camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s *handle = (camera_s *)camera;

	if( handle->user_cb[_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] == NULL ){
		mm_camcorder_set_video_stream_callback(handle->mm_handle,
						       (mm_camcorder_video_stream_callback)NULL,
						       (void *)NULL);
	}

	handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW] = (void *)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_PREVIEW] = (void *)NULL;

	return CAMERA_ERROR_NONE;
}

int camera_set_media_packet_preview_cb(camera_h camera, camera_media_packet_preview_cb callback, void* user_data){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_is_supported_media_packet_preview_cb(camera) == false ){
		LOGE("NOT SUPPORTED");
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	if( callback == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - callback", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s *handle = (camera_s *)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void *)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void *)user_data;

	mm_camcorder_set_video_stream_callback(handle->mm_handle,
					       (mm_camcorder_video_stream_callback)__mm_videostream_callback,
					       (void *)handle);

	return CAMERA_ERROR_NONE;
}

int camera_unset_media_packet_preview_cb(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_is_supported_media_packet_preview_cb(camera) == false ){
		LOGE("NOT SUPPORTED");
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	camera_s *handle = (camera_s *)camera;

	if( handle->user_cb[_CAMERA_EVENT_TYPE_PREVIEW] == NULL ){
		mm_camcorder_set_video_stream_callback(handle->mm_handle,
						       (mm_camcorder_video_stream_callback)NULL,
						       (void*)NULL);
	}

	handle->user_cb[_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_MEDIA_PACKET_PREVIEW] = (void*)NULL;

	return CAMERA_ERROR_NONE;
}

int camera_set_state_changed_cb(camera_h camera, camera_state_changed_cb callback, void* user_data){
	if( camera == NULL || callback == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void*)user_data;

	return CAMERA_ERROR_NONE;
}
int camera_unset_state_changed_cb(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_STATE_CHANGE] = (void*)NULL;

	return CAMERA_ERROR_NONE;
}

int camera_set_interrupted_cb(camera_h camera, camera_interrupted_cb callback, void *user_data){
	if( camera == NULL || callback == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_INTERRUPTED] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_INTERRUPTED] = (void*)user_data;

	return CAMERA_ERROR_NONE;
}

int camera_unset_interrupted_cb(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_INTERRUPTED] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_INTERRUPTED] = (void*)NULL;

	return CAMERA_ERROR_NONE;
}

int camera_set_focus_changed_cb(camera_h camera, camera_focus_changed_cb callback, void* user_data){
	if( camera == NULL || callback == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void*)user_data;

	return CAMERA_ERROR_NONE;
}
int camera_unset_focus_changed_cb(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_FOCUS_CHANGE] = (void*)NULL;

	return CAMERA_ERROR_NONE;
}

int camera_set_error_cb(camera_h camera, camera_error_cb callback, void *user_data){
	if( camera == NULL || callback == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_ERROR] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_ERROR] = (void*)user_data;

	return CAMERA_ERROR_NONE;
}

int camera_unset_error_cb(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_ERROR] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_ERROR] = (void*)NULL;

	return CAMERA_ERROR_NONE;
}

int camera_foreach_supported_preview_resolution(camera_h camera, camera_supported_preview_resolution_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo preview_width;
	MMCamAttrsInfo preview_height;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_WIDTH , &preview_width);
	ret |= mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_HEIGHT , &preview_height);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < preview_width.int_array.count ; i++ ){
		if( !foreach_cb(preview_width.int_array.array[i], preview_height.int_array.array[i],user_data) )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_foreach_supported_capture_resolution(camera_h camera, camera_supported_capture_resolution_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo capture_width;
	MMCamAttrsInfo capture_height;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAPTURE_WIDTH , &capture_width);
	ret |= mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAPTURE_HEIGHT , &capture_height);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

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
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo format;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAPTURE_FORMAT , &format);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < format.int_array.count ; i++ ){
		if( format.int_array.array[i] != MM_PIXEL_FORMAT_ITLV_JPEG_UYVY )
			if ( !foreach_cb(format.int_array.array[i], user_data) )
				break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_foreach_supported_preview_format(camera_h camera, camera_supported_preview_format_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo format;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_FORMAT , &format);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < format.int_array.count ; i++ ){
		if( format.int_array.array[i] != MM_PIXEL_FORMAT_ITLV_JPEG_UYVY /* || format.int_array.array[i] != MM_PIXEL_FORMAT_ITLV_JPEG_NV12 */)
			if ( !foreach_cb(format.int_array.array[i], user_data) )
				break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_get_recommended_preview_resolution(camera_h camera, int *width, int *height){
	if( camera == NULL || width == NULL || height == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	enum MMCamcorderPreviewType wide;
	int capture_w, capture_h;
	double ratio;
	int ret;
	camera_s * handle = (camera_s*)camera;

	camera_get_capture_resolution(camera, &capture_w, &capture_h);
	ratio = (double)capture_w/(double)capture_h;
	if( ratio > 1.5 ){
		wide = MM_CAMCORDER_PREVIEW_TYPE_WIDE;
	} else if( ratio == 1.0 ){
		wide = MM_CAMCORDER_PREVIEW_TYPE_SQUARE;
	} else {
		wide = MM_CAMCORDER_PREVIEW_TYPE_NORMAL;
	}

	MMCamAttrsInfo width_info, height_info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_RECOMMEND_CAMERA_WIDTH , &width_info);
	ret |= mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_RECOMMEND_CAMERA_HEIGHT, &height_info);

	if( ret != 0 )
		return __convert_camera_error_code(__func__, ret);

	if( width && (unsigned int)width_info.int_array.count > wide ){
		*width = width_info.int_array.array[wide];
	} else {
		LOGE("there is no width value for resolution %dx%d type %d", capture_w, capture_h, wide);
		return CAMERA_ERROR_INVALID_OPERATION;
	}

	if( height && (unsigned int)height_info.int_array.count > wide ){
		*height = height_info.int_array.array[wide];
	} else {
		LOGE("there is no height value for resolution %dx%d type %d", capture_w, capture_h, wide);
		return CAMERA_ERROR_INVALID_OPERATION;
	}

	LOGI("recommend resolution %dx%d, type %d", *width, *height, wide);

	return CAMERA_ERROR_NONE;
}

int camera_attr_get_lens_orientation(camera_h camera, int *angle){
	if( camera == NULL || angle == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	int rotation;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_RECOMMEND_DISPLAY_ROTATION, &rotation , NULL);

	switch( rotation ){
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
		default :
			*angle = 0;
			break;
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_theater_mode(camera_h camera, camera_attr_theater_mode_e mode){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_MODE, mode, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_theater_mode(camera_h camera, camera_attr_theater_mode_e *mode){
	if( camera == NULL || mode == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_DISPLAY_MODE , mode, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_foreach_supported_theater_mode(camera_h camera, camera_attr_supported_theater_mode_cb foreach_cb, void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_DISPLAY_MODE , &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for (i=0 ; i < info.int_array.count ; i++ ){
		if ( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_set_preview_fps(camera_h camera,  camera_attr_fps_e fps){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	if( fps == CAMERA_ATTR_FPS_AUTO ){
		ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
		                                  MMCAM_CAMERA_FPS_AUTO, 1,
		                                  NULL);
	} else {
		ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
		                                  MMCAM_CAMERA_FPS_AUTO, 0,
		                                  MMCAM_CAMERA_FPS, fps,
		                                  NULL);
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_image_quality(camera_h camera,  int quality){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_IMAGE_ENCODER_QUALITY , quality, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_preview_fps(camera_h camera,  camera_attr_fps_e *fps){
	if( camera == NULL || fps == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_image_quality(camera_h camera,  int *quality){
	if( camera == NULL || quality == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_IMAGE_ENCODER_QUALITY   , quality, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_zoom(camera_h camera,  int zoom){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_DIGITAL_ZOOM  , zoom, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_af_mode(camera_h camera,  camera_attr_af_mode_e mode){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_INVALID_PARAMETER;
	camera_s * handle = (camera_s*)camera;

	int focus_mode;
	bool should_change_focus_mode = false;
	mm_camcorder_get_attributes(handle->mm_handle, NULL,
				    MMCAM_CAMERA_FOCUS_MODE, &focus_mode,
				    NULL);

	if( focus_mode != MM_CAMCORDER_FOCUS_MODE_TOUCH_AUTO && focus_mode != MM_CAMCORDER_FOCUS_MODE_CONTINUOUS && focus_mode != MM_CAMCORDER_FOCUS_MODE_AUTO )
		should_change_focus_mode = true;

	if( mode != CAMERA_ATTR_AF_NONE && focus_mode == MM_CAMCORDER_FOCUS_MODE_CONTINUOUS && !handle->on_continuous_focusing ){
		handle->cached_focus_mode = mode;
		LOGD("af mode will be set actually start focusing");
		return __convert_camera_error_code(__func__, 0);
	} else {
		handle->cached_focus_mode = -1;
	}

	if( mode != CAMERA_ATTR_AF_NONE && should_change_focus_mode ){
		mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_FOCUS_MODE, MM_CAMCORDER_FOCUS_MODE_AUTO, NULL);
	}

	switch( mode ){
		case CAMERA_ATTR_AF_NONE:
			ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
							  MMCAM_CAMERA_FOCUS_MODE, MM_CAMCORDER_FOCUS_MODE_NONE,
							  MMCAM_CAMERA_AF_SCAN_RANGE, MM_CAMCORDER_AUTO_FOCUS_NONE,
							  NULL);
			break;
		case CAMERA_ATTR_AF_NORMAL:
			ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
							  MMCAM_CAMERA_AF_SCAN_RANGE, MM_CAMCORDER_AUTO_FOCUS_NORMAL,
							  NULL);
			break;
		case CAMERA_ATTR_AF_MACRO:
			ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
							  MMCAM_CAMERA_AF_SCAN_RANGE, MM_CAMCORDER_AUTO_FOCUS_MACRO,
							  NULL);
			break;
		case CAMERA_ATTR_AF_FULL:
			ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
							  MMCAM_CAMERA_AF_SCAN_RANGE, MM_CAMCORDER_AUTO_FOCUS_FULL,
							  NULL);
			break;
		default:
			return ret;
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_af_area(camera_h camera, int x, int y){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret = CAMERA_ERROR_INVALID_PARAMETER;
	camera_s * handle = (camera_s*)camera;
	camera_attr_af_mode_e mode;
	camera_attr_get_af_mode(camera, &mode);

	if( mode == CAMERA_ATTR_AF_NONE ){
		LOGE("INVALID_OPERATION(0x%08x) AF mode is CAMERA_ATTR_AF_NONE",CAMERA_ERROR_INVALID_OPERATION);
		return CAMERA_ERROR_INVALID_OPERATION;
	}

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_AF_TOUCH_X, x,
					  MMCAM_CAMERA_AF_TOUCH_Y, y,
					  NULL);

	if( ret == MM_ERROR_NONE )
		handle->focus_area_valid = true;

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_clear_af_area(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->focus_area_valid = false;

	return CAMERA_ERROR_NONE;
}

int camera_attr_set_exposure_mode(camera_h camera,  camera_attr_exposure_mode_e mode){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_EXPOSURE_MODE, maptable[abs(mode%5)],
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_exposure(camera_h camera, int value){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_EXPOSURE_VALUE, value,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_iso(camera_h camera,  camera_attr_iso_e iso){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_ISO, iso,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_brightness(camera_h camera, int level){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_BRIGHTNESS, level,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_contrast(camera_h camera, int level){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_CONTRAST, level,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_whitebalance(camera_h camera, camera_attr_whitebalance_e wb){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_WB, wb,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_effect(camera_h camera, camera_attr_effect_mode_e effect){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_COLOR_TONE, effect,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_scene_mode(camera_h camera,  camera_attr_scene_mode_e mode){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_SCENE_MODE, mode,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_enable_tag(camera_h camera,  bool enable){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_ENABLE, enable,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_tag_image_description(camera_h camera,  const char *description){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_IMAGE_DESCRIPTION, description, strlen(description),
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_tag_orientation(camera_h camera,  camera_attr_tag_orientation_e orientation){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_ORIENTATION, orientation,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_tag_software(camera_h camera,  const char *software){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_SOFTWARE, software, strlen(software),
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_geotag(camera_h camera, double latitude , double longitude, double altitude){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_GPS_ENABLE, 1,
					  MMCAM_TAG_LATITUDE, latitude,
					  MMCAM_TAG_LONGITUDE, longitude,
					  MMCAM_TAG_ALTITUDE, altitude,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_remove_geotag(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_GPS_ENABLE, 0,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_flash_mode(camera_h camera, camera_attr_flash_mode_e mode){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_STROBE_MODE, mode,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}


int camera_attr_get_zoom(camera_h camera, int *zoom){
	if( camera == NULL || zoom == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_DIGITAL_ZOOM, zoom,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_zoom_range(camera_h camera, int *min, int *max){
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_af_mode( camera_h camera,  camera_attr_af_mode_e *mode){
	if( camera == NULL || mode == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	int focus_mode;
	int af_range;
	int detect_mode;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_FOCUS_MODE, &focus_mode,
					  MMCAM_CAMERA_AF_SCAN_RANGE, &af_range,
					  MMCAM_DETECT_MODE, &detect_mode,
					  NULL);

	if( ret == CAMERA_ERROR_NONE ){
		switch( focus_mode ){
		case MM_CAMCORDER_FOCUS_MODE_NONE :
		case MM_CAMCORDER_FOCUS_MODE_PAN :
		case MM_CAMCORDER_FOCUS_MODE_MANUAL :
			*mode = CAMERA_ATTR_AF_NONE;
			break;
		case MM_CAMCORDER_FOCUS_MODE_AUTO:
		case MM_CAMCORDER_FOCUS_MODE_TOUCH_AUTO:
		case MM_CAMCORDER_FOCUS_MODE_CONTINUOUS:
			switch ( af_range ){
			case MM_CAMCORDER_AUTO_FOCUS_NONE :
				*mode = CAMERA_ATTR_AF_NORMAL;
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
			default :
				*mode = CAMERA_ATTR_AF_NORMAL;
				break;
			}
			break;
		default :
			*mode = CAMERA_ATTR_AF_NONE;
			break;
		}
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_exposure_mode( camera_h camera,  camera_attr_exposure_mode_e *mode){
	if( camera == NULL|| mode == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_EXPOSURE_MODE, &exposure_mode, NULL);
	if( ret == CAMERA_ERROR_NONE ){
		*mode = maptable[abs(exposure_mode%9)];
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_exposure(camera_h camera, int *value){
	if( camera == NULL || value == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_EXPOSURE_VALUE, value, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_exposure_range(camera_h camera, int *min, int *max){
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_iso( camera_h camera,  camera_attr_iso_e *iso){
	if( camera == NULL || iso == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_ISO, iso,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_brightness(camera_h camera,  int *level){
	if( camera == NULL || level == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_BRIGHTNESS, level,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_brightness_range(camera_h camera, int *min, int *max){
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_contrast(camera_h camera,  int *level){
	if( camera == NULL || level == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_CONTRAST, level,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_contrast_range(camera_h camera, int *min , int *max){
	if( camera == NULL || min == NULL || max == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_whitebalance(camera_h camera,  camera_attr_whitebalance_e *wb){
	if( camera == NULL || wb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_WB, wb,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_effect(camera_h camera, camera_attr_effect_mode_e *effect){
	if( camera == NULL || effect == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	int tone;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_COLOR_TONE, &tone,
					  NULL);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	*effect = (camera_attr_effect_mode_e)tone;

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_scene_mode(camera_h camera,  camera_attr_scene_mode_e *mode){
	if( camera == NULL || mode == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_FILTER_SCENE_MODE, mode,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_is_enabled_tag(camera_h camera,  bool *enable){
	if( camera == NULL || enable == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_ENABLE, enable,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_tag_image_description(camera_h camera,  char **description){
	if( camera == NULL || description == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	char *ndescription = NULL;
	int desc_size;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_TAG_IMAGE_DESCRIPTION, &ndescription, &desc_size, NULL);

	if( ret == CAMERA_ERROR_NONE ){
		if( ndescription != NULL )
			*description = strdup(ndescription);
		else
			*description = strdup("");
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_tag_orientation(camera_h camera,  camera_attr_tag_orientation_e *orientation){
	if( camera == NULL || orientation == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_ORIENTATION, orientation,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_tag_software(camera_h camera,  char **software){
	if( camera == NULL || software == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	char *soft;
	int soft_size;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_TAG_SOFTWARE, &soft, &soft_size, NULL);

	if( ret == CAMERA_ERROR_NONE ){
		if( soft != NULL )
			*software = strdup(soft);
		else
			*software = strdup("");
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_geotag(camera_h camera, double *latitude , double *longitude, double *altitude){
	if( camera == NULL || latitude == NULL || longitude == NULL || altitude == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_TAG_LATITUDE, latitude,
					  MMCAM_TAG_LONGITUDE, longitude,
					  MMCAM_TAG_ALTITUDE, altitude,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_flash_mode(camera_h camera,  camera_attr_flash_mode_e *mode){
	if( camera == NULL || mode == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL,
					  MMCAM_STROBE_MODE, mode,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_foreach_supported_af_mode( camera_h camera, camera_attr_supported_af_mode_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	int i;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo af_range;
	MMCamAttrsInfo focus_mode;

	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_AF_SCAN_RANGE, &af_range);
	ret |= mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_FOCUS_MODE, &focus_mode);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	for( i=0 ; i < af_range.int_array.count ; i++ )	{
		if( !foreach_cb(af_range.int_array.array[i],user_data) )
			goto ENDCALLBACK;
	}

	ENDCALLBACK:

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_exposure_mode(camera_h camera, camera_attr_supported_exposure_mode_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
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
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++ ){
		if( maptable[info.int_array.array[i]] != -1 ){
			if ( !foreach_cb(maptable[info.int_array.array[i]],user_data) )
				break;
		}
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_iso( camera_h camera, camera_attr_supported_iso_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_ISO , &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++ ){
		if( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_whitebalance(camera_h camera, camera_attr_supported_whitebalance_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_FILTER_WB, &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++ ){
		if ( !foreach_cb(info.int_array.array[i],user_data)  )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_effect(camera_h camera, camera_attr_supported_effect_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_FILTER_COLOR_TONE , &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++){
		int effect = info.int_array.array[i];
		if( !foreach_cb(effect,user_data) )
				break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_scene_mode(camera_h camera, camera_attr_supported_scene_mode_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_FILTER_SCENE_MODE  , &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++){
		if ( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_flash_mode(camera_h camera, camera_attr_supported_flash_mode_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_STROBE_MODE  , &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++){
		if( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_fps(camera_h camera, camera_attr_supported_fps_cb foreach_cb , void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_FPS , &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++){
		if( !foreach_cb(info.int_array.array[i],user_data) )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_stream_flip(camera_h camera, camera_attr_supported_stream_flip_cb foreach_cb, void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_FLIP , &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++){
		if( !foreach_cb(info.int_array.array[i], user_data) )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_foreach_supported_stream_rotation(camera_h camera, camera_attr_supported_stream_rotation_cb foreach_cb, void *user_data){
	if( camera == NULL || foreach_cb == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_ROTATION , &info);

	if( ret != CAMERA_ERROR_NONE )
		return __convert_camera_error_code(__func__, ret);

	int i;
	for( i=0 ; i < info.int_array.count ; i++ ){
		if( !foreach_cb(info.int_array.array[i], user_data) )
			break;
	}

	return CAMERA_ERROR_NONE;
}

int camera_attr_set_stream_rotation(camera_h camera , camera_rotation_e rotation){
	if( camera == NULL){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( rotation > CAMERA_ROTATION_270 )
		return CAMERA_ERROR_INVALID_PARAMETER;

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_ROTATION, rotation, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_stream_rotation(camera_h camera , camera_rotation_e *rotation){
	if( camera == NULL || rotation == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;

	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_ROTATION, rotation, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_set_stream_flip(camera_h camera , camera_flip_e flip){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( flip > CAMERA_FLIP_BOTH )
		return CAMERA_ERROR_INVALID_PARAMETER;

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,
					  MMCAM_CAMERA_FLIP, flip,
					  NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_stream_flip(camera_h camera , camera_flip_e *flip){
	if( camera == NULL || flip == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_FLIP, flip, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int _camera_set_use(camera_h camera, bool used){
	camera_s * handle = (camera_s*)camera;
	handle->is_used_in_recorder = used;
	return CAMERA_ERROR_NONE;
}

bool _camera_is_used(camera_h camera){
	camera_s * handle = (camera_s*)camera;
	return handle->is_used_in_recorder;
}

int _camera_get_mm_handle(camera_h camera , MMHandleType *handle){
	if( camera == NULL || handle == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}
	camera_s *camera_handle = (camera_s*)camera;
	*handle =  camera_handle->mm_handle;

	return CAMERA_ERROR_NONE;
}

int _camera_set_relay_mm_message_callback(camera_h camera, MMMessageCallback callback, void *user_data){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s *handle = (camera_s*)camera;
	handle->relay_message_callback = callback;
	handle->relay_user_data = user_data;

	return CAMERA_ERROR_NONE;
}

int _camera_get_tbm_surface_format(int in_format, uint32_t *out_format){
	if( in_format <= MM_PIXEL_FORMAT_INVALID ||
	    in_format >= MM_PIXEL_FORMAT_NUM ||
	    out_format == NULL ){
		LOGE("INVALID_PARAMETER : in_format %d, out_format ptr %p", in_format, out_format);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	switch( in_format ){
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

int _camera_get_media_packet_mimetype(int in_format, media_format_mimetype_e *mimetype){
	if( in_format <= MM_PIXEL_FORMAT_INVALID ||
	    in_format >= MM_PIXEL_FORMAT_NUM ||
	    mimetype == NULL ){
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

int _camera_media_packet_finalize(media_packet_h pkt, int error_code, void *user_data){
	int ret = 0;
	void *internal_buffer = NULL;
	tbm_surface_h tsurf = NULL;

	if( pkt == NULL || user_data == NULL ){
		LOGE("invalid parameter buffer %p, user_data %p", pkt, user_data);
		return MEDIA_PACKET_FINALIZE;
	}

	ret = media_packet_get_extra(pkt, &internal_buffer);

	if( ret != MEDIA_PACKET_ERROR_NONE ){
		LOGE("media_packet_get_extra failed 0x%x", ret);
		return MEDIA_PACKET_FINALIZE;
	}

	/*LOGD("pointer gst buffer %p, ret 0x%x", internal_buffer, ret);*/

	if( internal_buffer ){
		gst_buffer_unref((GstBuffer *)internal_buffer);
		internal_buffer = NULL;
	}

	ret = media_packet_get_tbm_surface(pkt, &tsurf);
	if( ret != MEDIA_PACKET_ERROR_NONE ){
		LOGE("media_packet_get_tbm_surface failed 0x%x", ret);
		return MEDIA_PACKET_FINALIZE;
	}

	if( tsurf ){
		tbm_surface_destroy(tsurf);
		tsurf = NULL;
	}

	return MEDIA_PACKET_FINALIZE;
}

int camera_attr_set_hdr_mode(camera_h camera, camera_attr_hdr_mode_e mode){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_hdr_capture(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_HDR_CAPTURE, mode, NULL);

	if( ret == MM_ERROR_NONE ){
		if( mode == CAMERA_ATTR_HDR_MODE_KEEP_ORIGINAL )
			handle->hdr_keep_mode = true;
		else
			handle->hdr_keep_mode = false;
	}

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_get_hdr_mode(camera_h camera, camera_attr_hdr_mode_e *mode){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - handle",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if(  camera_attr_is_supported_hdr_capture(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	if( mode == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - mode",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	int result;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_HDR_CAPTURE, &result, NULL);

	if( ret == MM_ERROR_NONE ){
		*mode = result;
	}

	return __convert_camera_error_code(__func__, ret);
}

bool camera_attr_is_supported_hdr_capture(camera_h camera){
	if( camera == NULL  ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}
	int ret;
	int i;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo hdr_info;

	ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_HDR_CAPTURE, &hdr_info);
	set_last_result(__convert_camera_error_code(__func__, ret));
	if( ret != MM_ERROR_NONE ){
		LOGE("MMCAM_CAMERA_HDR_CAPTURE get attr info failed");
		return false;
	}

	for( i = 0; i < hdr_info.int_array.count ; i++ ){
		if( hdr_info.int_array.array[i] >= MM_CAMCORDER_HDR_ON  ){
			LOGD("HDR capture supported");
			return true;
		}
	}

	LOGD("HDR capture NOT supported");

	return false;
}

int camera_attr_set_hdr_capture_progress_cb(camera_h camera, camera_attr_hdr_progress_cb callback, void* user_data){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_hdr_capture(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	if( callback == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - callback", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_HDR_PROGRESS] = (void*)callback;
	handle->user_data[_CAMERA_EVENT_TYPE_HDR_PROGRESS] = (void*)user_data;
	return CAMERA_ERROR_NONE;
}

int camera_attr_unset_hdr_capture_progress_cb(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_hdr_capture(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	camera_s * handle = (camera_s*)camera;
	handle->user_cb[_CAMERA_EVENT_TYPE_HDR_PROGRESS] = (void*)NULL;
	handle->user_data[_CAMERA_EVENT_TYPE_HDR_PROGRESS] = (void*)NULL;

	return CAMERA_ERROR_NONE;
}

int camera_attr_enable_anti_shake(camera_h camera, bool enable){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_anti_shake(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	int ret;
	int mode = MM_CAMCORDER_AHS_OFF;

	if( enable )
		mode = MM_CAMCORDER_AHS_ON;

	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_ANTI_HANDSHAKE, mode, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_is_enabled_anti_shake(camera_h camera , bool *enabled){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_anti_shake(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	if( enabled == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - enabled", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	int mode = MM_CAMCORDER_AHS_OFF;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_ANTI_HANDSHAKE, &mode, NULL);

	if( ret == MM_ERROR_NONE )
		*enabled = mode;

	return __convert_camera_error_code(__func__, ret);
}

bool camera_attr_is_supported_anti_shake(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int i;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo ash_info;
	int ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_ANTI_HANDSHAKE , &ash_info);
	set_last_result(__convert_camera_error_code(__func__, ret));

	for( i=0 ; i < ash_info.int_array.count ; i++ ){
		if( ash_info.int_array.array[i] == MM_CAMCORDER_AHS_ON )
			return true;
	}

	return false;
}

int camera_attr_enable_video_stabilization(camera_h camera, bool enable){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_video_stabilization(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	int ret;
	int mode = MM_CAMCORDER_VIDEO_STABILIZATION_OFF;

	if( enable )
		mode = MM_CAMCORDER_VIDEO_STABILIZATION_ON;

	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_VIDEO_STABILIZATION, mode, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_is_enabled_video_stabilization(camera_h camera, bool *enabled){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - handle",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_video_stabilization(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	if( enabled == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - enabled", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	int mode = MM_CAMCORDER_VIDEO_STABILIZATION_OFF;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle, NULL, MMCAM_CAMERA_VIDEO_STABILIZATION, &mode, NULL);

	if( ret == MM_ERROR_NONE )
		*enabled = (mode == MM_CAMCORDER_VIDEO_STABILIZATION_ON);

	return __convert_camera_error_code(__func__, ret);
}

bool camera_attr_is_supported_video_stabilization(camera_h camera){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int i;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo vs_info;
	int ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_VIDEO_STABILIZATION , &vs_info);
	set_last_result(__convert_camera_error_code(__func__, ret));

	for( i=0 ; i < vs_info.int_array.count ; i++ ){
		if( vs_info.int_array.array[i] == MM_CAMCORDER_VIDEO_STABILIZATION_ON )
			return true;
	}

	return false;
}

int camera_attr_enable_auto_contrast(camera_h camera, bool enable){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_auto_contrast(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	int ret;
	int mode = MM_CAMCORDER_WDR_OFF;

	if( enable )
		mode = MM_CAMCORDER_WDR_ON;

	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL,  MMCAM_CAMERA_WDR, mode, NULL);

	return __convert_camera_error_code(__func__, ret);
}

int camera_attr_is_enabled_auto_contrast(camera_h camera, bool *enabled){
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - handle", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	if( camera_attr_is_supported_auto_contrast(camera) == false ){
		LOGE("NOT_SUPPORTED(0x%08x)", CAMERA_ERROR_NOT_SUPPORTED);
		return CAMERA_ERROR_NOT_SUPPORTED;
	}

	if( enabled == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x) - enabled", CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	int mode = MM_CAMCORDER_WDR_OFF;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_get_attributes(handle->mm_handle ,NULL, MMCAM_CAMERA_WDR , &mode, NULL);

	if( ret == MM_ERROR_NONE )
		*enabled = mode;

	return __convert_camera_error_code(__func__, ret);
}

bool camera_attr_is_supported_auto_contrast(camera_h camera) {
	if( camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return false;
	}

	int i;
	camera_s * handle = (camera_s*)camera;
	MMCamAttrsInfo info;
	int ret = mm_camcorder_get_attribute_info(handle->mm_handle, MMCAM_CAMERA_WDR, &info);
	set_last_result(__convert_camera_error_code(__func__, ret));

	for( i=0 ; i < info.int_array.count ; i++ )	{
		if( info.int_array.array[i] == MM_CAMCORDER_WDR_ON )
			return true;
	}

	return false;
}

int camera_attr_disable_shutter_sound(camera_h camera, bool disable){
	if (camera == NULL ){
		LOGE("INVALID_PARAMETER(0x%08x)",CAMERA_ERROR_INVALID_PARAMETER);
		return CAMERA_ERROR_INVALID_PARAMETER;
	}

	int ret;
	camera_s * handle = (camera_s*)camera;
	ret = mm_camcorder_set_attributes(handle->mm_handle, NULL, "capture-sound-enable", !disable, NULL);

	if( ret != 0 ){
		LOGE("CAMERA_ERROR_INVALID_OPERATION : not permitted disable shutter sound");
		return CAMERA_ERROR_INVALID_OPERATION;
	}

	return CAMERA_ERROR_NONE;
}
