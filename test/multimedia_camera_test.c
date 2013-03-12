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


#include <Elementary.h>
#include <glib.h>
#include <Ecore.h>
#include <Ecore_X.h>

#include <stdio.h>
#include <camera.h>

#include <assert.h>
#include <pthread.h>

typedef struct{
	Evas_Object* win;
	
}appdata;


Evas_Object* mEvasWindow;
Ecore_X_Window preview_win;
Evas_Object* img;

void state_cb(camera_state_e previous , camera_state_e current , int by_asm, const void *user_data){
	char *state_table[] ={
			"CAMERA_STATE_NONE",				/**< camera is not created yet */
			"CAMERA_STATE_CREATED",				/**< camera is created, but not initialized yet */
			"CAMERA_STATE_PREVIEW",				/**< camera is prepared to capture (Preview) */
			"CAMERA_STATE_CAPTURING",			/**< While capturing*/
			"CAMERA_STATE_CAPTURED",			/**< camera is now recording */
			"CAMERA_STATE_NUM",					/**< Number of camera states */
		};
	printf("%s\n", state_table[current]);
}


void capturing_cb(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data)
{
	char * filepath = (char*)user_data;
	FILE* f = fopen(filepath, "w+");
	bool ret;
	if(f!=NULL && image !=NULL)
	{
		fwrite(image->data,1,  image->size, f);
		printf("capture(%s) %dx%d, buffer_size=%d\n", filepath, image->width, image->height, image->size);
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}
	fclose(f);

}

int capture_complete(void *user_data){
	camera_h cam = (camera_h)user_data;
	
	printf("capture_complete!!\n");

	camera_start_preview(cam);
	
	return 1;	
}

int stillshot_test(){
	camera_h camera;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	camera_attr_set_image_quality(camera, 100);
	camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);
	camera_attr_set_tag_orientation(camera,6);
	//camera_attr_set_tag_orientation(camera,CAMERA_ATTR_TAG_ORT_0R_VT_0C_VR);	
	//camera_attr_enable_tag(camera, true);
	camera_set_capture_format(camera, CAMERA_PIXEL_FORMAT_JPEG);
	
	camera_start_preview(camera);
	camera_start_focusing(camera, false);

	sleep(1);
	camera_start_capture(camera, capturing_cb, NULL, "/mnt/nfs/test.jpg");
	sleep(1);
	camera_start_preview(camera);
	camera_stop_preview(camera);
	camera_destroy(camera);
	return 0;
}

bool g_preview_fps_pass;
bool _preview_fps_cb(camera_attr_fps_e fps, void *user_data){
	int ret;
	camera_attr_fps_e get_fps;
	camera_h camera = (camera_h) user_data;
	ret = camera_attr_set_preview_fps(camera, fps);
	printf("-set preview fps %d\tret=%x\n", fps, ret);	
	ret = camera_attr_get_preview_fps(camera, &get_fps);
	printf("-get preview fps %d\tret=%x", get_fps, ret);	
	
	if(get_fps == fps)
		printf("\t\t\tpass\n");
	else{
		printf("\t\t\tfail\n");
		g_preview_fps_pass = false;
		return false;
	}		
	return true;
}


int preview_fps_test(camera_h camera)
{
	g_preview_fps_pass = true;
	printf("------------- PREVIEW FPS TEST -------------\n");
	camera_attr_foreach_supported_fps(camera, _preview_fps_cb,(void*)camera);
	printf("--------------------------------------------\n");	
	if( g_preview_fps_pass ){
		printf("PREVIEW FPS TEST PASS\n\n");
		return 0;
	}else{
		printf("PREVIEW FPS TEST FAIL\n\n");
		return -1;
	}
}

int image_quality_test(camera_h camera){
	int ret1;	
	int ret2;
	int i;
	printf("------------- IMAGE QUALITY TEST -------------\n");
	for( i =-10; i <= 110 ; i+=10){
		int quality;
		ret1 = camera_attr_set_image_quality(camera,i);
		printf("-set image quality %d\tret=%x\n",i,ret1);
		ret2 = camera_attr_get_image_quality(camera,&quality);
		printf("-get image quality %d\tret=%x",quality,ret2);
		
		if( i >=0 && i <= 100){
			if( quality == i ){
				printf("\t\t\tpass\n");
			}else
			{
				printf("\t\t\tfail\n");			
				return -1;
			}
		}else{	//out of bound error
			if( ret1 == 0){
				printf("\t\t\tfail\n");
				return -1;
			}else{
				printf("\t\t\tpass\n");
			}
		}
		
	}
	printf("--------------------------------------------\n");	
	printf("IMAGE QUALITY TEST PASS\n\n");
	
	return 0;
}

int zoom_test(camera_h camera){
	int ret1 ;
	int ret2 ;	
	int i;
	int min, max;
	printf("------------- ZOOM TEST -------------\n");
	camera_attr_get_zoom_range(camera, &min, &max);
	if(max == -1 )
		return 0;
	for( i = min ; i <= max; i+=5 ){
		int zoom;
		ret1 = camera_attr_set_zoom(camera, i);
		printf("-set zoom %d\tret=%x\n",i, ret1);
		ret2 = camera_attr_get_zoom(camera,&zoom);
		printf("-get zoom %d\tret=%x",zoom, ret2);

		if( i >=min && i <= max ){
			if( i == zoom )
				printf("\t\t\tpass\n");
			else{
				printf("\t\t\tfail\n");
				return -1;
			}	
		}else{
			if( ret1 == 0 ){
				printf("\t\t\tfail\n");
				return -1;
			}else{
				printf("\t\t\tpass\n");
			}
		}
	}
	printf("--------------------------------------------\n");	
	printf("ZOOM TEST PASS\n\n");

	camera_attr_set_zoom(camera, 10);	
	return 0;
}

bool g_af_test_pass ;
bool _af_mode_test_cb(camera_attr_af_mode_e mode, void *user_data){
	camera_h camera = (camera_h) user_data;
	int ret;
	camera_attr_af_mode_e get_mode;
	ret= camera_attr_set_af_mode(camera, mode);
	printf("-set af mode %d\tret=%x\n", mode, ret);
	ret= camera_attr_get_af_mode(camera, &get_mode);
	printf("-get af mode %d\tret=%x", get_mode, ret);	
	if( mode != get_mode ){
		printf("\t\t\tFAIL\n");
		g_af_test_pass= false;
		return false;
	}else
		printf("\t\t\tPASS\n");
	return true;
}

int af_mode_test(camera_h camera){
	g_af_test_pass = true;
	camera_attr_foreach_supported_af_mode(camera, _af_mode_test_cb, camera);
	return g_af_test_pass ? 0 : -1;
}

bool g_exposure_mode_pass;
bool _exposure_mode_test_cb(camera_attr_exposure_mode_e mode, void *user_data){
	camera_h camera = (camera_h) user_data;
	int ret;
	camera_attr_exposure_mode_e get_mode;
	
	ret = camera_attr_set_exposure_mode(camera, mode);
	printf("-set exposure mode %d\tret=%x\n", mode,ret);
	ret = camera_attr_get_exposure_mode(camera,&get_mode);
	printf("-get exposure mode %d\tret=%x\n", get_mode,ret);		
	if( get_mode != mode ){
		printf("\t\t\tFAIL\n");
		g_exposure_mode_pass = false;
		return false;
	}else
		printf("\t\t\tPASS\n");
	return true;
}

int exposure_mode_test(camera_h camera){
	g_exposure_mode_pass = true;
	camera_attr_foreach_supported_exposure_mode(camera,_exposure_mode_test_cb, camera);
	camera_attr_set_exposure_mode(camera, CAMERA_ATTR_EXPOSURE_MODE_ALL);
	return g_exposure_mode_pass ? 0 : -1;
}

int exposure_test(camera_h camera){
	int i;
	int ret1, ret2;
	int default_value;
	int min,max;
	ret1 = camera_attr_get_exposure(camera, &default_value );	
	camera_attr_get_exposure_range(camera, &min, &max);
	printf("exposure range %d~%d\n", min, max);
	if(max == -1 )
		return 0;
	for( i = 1; i < 13 ; i++ ){
		int value;
		ret1 = camera_attr_set_exposure(camera, i );
		printf("-set exposure %d\tret=%x\n",i,ret1);
		ret2 = camera_attr_get_exposure(camera, &value);
		printf("-get exposure %d\tret=%x\n",value,ret2);
		if( i >= min && i <= max ){
			if( value != i)
				return -1;
		}else{ // out of bound error
			if( ret1 == 0 )
				return -1;
		}
	}	
	ret1 = camera_attr_set_exposure(camera, default_value );	
	return 0;
}

bool g_iso_test_pass ;
bool _iso_test_cb(camera_attr_iso_e iso, void *user_data){
	camera_h camera = (camera_h) user_data;
	int ret;
	camera_attr_iso_e get_iso;
	ret = camera_attr_set_iso(camera, iso);
	printf("-set iso %d\tret=%x\n", iso, ret);
	ret = camera_attr_get_iso(camera,&get_iso);
	printf("-get iso %d\tret=%x\n", get_iso, ret);	
	if( get_iso != iso ){
		g_iso_test_pass = false;
		return false;
	}
	return true;
}

int iso_test(camera_h camera){
	g_iso_test_pass = true;
	camera_attr_foreach_supported_iso(camera,_iso_test_cb, camera);
	return g_iso_test_pass ? 0 : -1;
}

int brightness_test(camera_h camera){
	int i;
	int ret1,ret2;
	int default_value;
	int min,max;
	ret1 = camera_attr_get_brightness(camera, &default_value );	
	camera_attr_get_brightness_range(camera, &min, &max);
	if(max == -1 )
		return 0;
	for( i = 1; i < 13 ; i++ ){
		int value;
		ret1 = camera_attr_set_brightness(camera, i );
		printf("-set brightness %d\tret=%x\n",i,ret1);
		ret2 = camera_attr_get_brightness(camera, &value);
		printf("-get brightness %d\tret=%x\n",value,ret2);

		if( i >= min && i <= max ){
			if( value != i)
				return -1;
		}else{ // out of bound error
			if( ret1 == 0 )
				return -1;
		}
		
	}	
	ret1 = camera_attr_set_brightness(camera, default_value );	
	return 0;
	
}

int contrast_test(camera_h camera){
	int i;
	int ret1,ret2;
	int default_value;
	int min,max;
	ret1 = camera_attr_get_contrast (camera, &default_value );	
	camera_attr_get_contrast_range(camera, &min, &max);
	if(max == -1 )
		return 0;
	for( i = 1; i < 13 ; i++ ){
		int value;
		ret1 = camera_attr_set_contrast (camera, i );
		printf("-set contrast %d\tret=%x\n",i,ret1);
		ret2 = camera_attr_get_contrast (camera, &value);
		printf("-get contrast %d\tret=%x\n",value,ret2);

		if( i >= min && i <= max ){
			if( value != i)
				return -1;
		}else{ // out of bound error
			if( ret1 == 0 )
				return -1;
		}		
	}	
	ret1 = camera_attr_set_contrast (camera, default_value );	
	return 0;	
}

bool _whitebalance_test_cb(camera_attr_whitebalance_e wb, void *user_data){
	camera_h camera = (camera_h) user_data;
	int ret;
	ret = camera_attr_set_whitebalance(camera, wb);
	printf("-set whitebalance %d\tret=%x\n", wb,ret);
	ret = camera_attr_get_whitebalance(camera,&wb);
	printf("-get whitebalance %d\tret=%x\n", wb,ret);		
	return true;	
}


int whitebalance_test(camera_h camera){
	camera_attr_foreach_supported_whitebalance(camera, _whitebalance_test_cb ,camera);
	return 0;
}

bool _effect_test_cb(camera_attr_effect_mode_e effect, void *user_data){
	camera_h camera = (camera_h) user_data;
	int ret;
	ret = camera_attr_set_effect(camera, effect);
	printf("-set effect %d\tret=%x\n", effect,ret);
	ret = camera_attr_get_effect(camera,&effect);
	printf("-get effect %d\tret=%x\n", effect,ret);		
	return true;
}


int effect_test(camera_h camera){
	camera_attr_foreach_supported_effect(camera, _effect_test_cb, camera);
	return 0;
}


bool _scene_mode_test_cb (camera_attr_scene_mode_e mode, void *user_data){
	camera_h camera = (camera_h) user_data;
	int ret;
	ret = camera_attr_set_scene_mode(camera, mode);
	printf("-set scene %d\tret=%x\n", mode,ret);
	ret = camera_attr_get_scene_mode(camera,&mode);
	printf("-get scene %d\tret=%x\n", mode,ret);		
	return true;
}

int scene_mode_test(camera_h camera){
	camera_attr_foreach_supported_scene_mode(camera, _scene_mode_test_cb, camera);
	return 0;
}

int tag_enable_test(camera_h camera){
	int ret;
	bool enable;
	ret = camera_attr_enable_tag(camera, true);
	printf("-set enable tag true\tret=%x\n",ret);
	ret = camera_attr_is_enabled_tag(camera, &enable);
	printf("-get enable tag %d\tret=%x\n",enable, ret);
	return 0;
}

int tag_orientation_test(camera_h camera){
	int ret;
	camera_attr_tag_orientation_e orientation;
	
	ret = camera_attr_set_tag_orientation(camera, 1);
	printf("-set tag orientation %d\tret=%x\n",1 ,ret);
	ret= camera_attr_get_tag_orientation(camera, &orientation);
	printf("-get tag orientation %d\tret=%x\n",orientation,ret);	

	
	ret |= camera_attr_set_tag_orientation(camera, 2 );
	printf("-set tag orientation %d\tret=%x\n",2 ,ret);
	ret |= camera_attr_get_tag_orientation(camera, &orientation);
	printf("-get tag orientation %d\tret=%x\n",orientation,ret);		

	ret |= camera_attr_set_tag_orientation(camera, 3 );
	printf("-set tag orientation %d\tret=%x\n",3 ,ret);
	ret |= camera_attr_get_tag_orientation(camera, &orientation);
	printf("-get tag orientation %d\tret=%x\n",orientation,ret);		

	ret |= camera_attr_set_tag_orientation(camera, 4 );
	printf("-set tag orientation %d\tret=%x\n",4 ,ret);
	ret |= camera_attr_get_tag_orientation(camera, &orientation);
	printf("-get tag orientation %d\tret=%x\n",orientation,ret);		

	ret |= camera_attr_set_tag_orientation(camera, 5  );
	printf("-set tag orientation %d\tret=%x\n",5  ,ret);
	ret |= camera_attr_get_tag_orientation(camera, &orientation);
	printf("-get tag orientation %d\tret=%x\n",orientation,ret);		

	ret |= camera_attr_set_tag_orientation(camera, 6 );
	printf("-set tag orientation %d\tret=%x\n",6 ,ret);
	ret |= camera_attr_get_tag_orientation(camera, &orientation);
	printf("-get tag orientation %d\tret=%x\n",orientation,ret);		

	ret |= camera_attr_set_tag_orientation(camera, 7  );
	printf("-set tag orientation %d\tret=%x\n",7  ,ret);
	ret |= camera_attr_get_tag_orientation(camera, &orientation);
	printf("-get tag orientation %d\tret=%x\n",orientation,ret);		

	ret |= camera_attr_set_tag_orientation(camera, 8  );
	printf("-set tag orientation %d\tret=%x\n",8  ,ret);
	ret |= camera_attr_get_tag_orientation(camera, &orientation);
	printf("-get tag orientation %d\tret=%x\n",orientation,ret);		

	return ret == 0 ? 0 : -1;
	
}

int tag_image_description_test(camera_h camera){
	char *description;
	int ret;
	ret = camera_attr_set_tag_image_description(camera, "hello capi");
	printf("-set tag image description \"hello capi\"\tret=%x\n", ret);
	ret = camera_attr_get_tag_image_description(camera, &description);
	printf("-get tag image description \"%s\"\tret=%x\n", description, ret);
	free(description);
	ret = camera_attr_set_tag_image_description(camera, "12345678901234567890");
	printf("-set tag image description \"12345678901234567890\"\tret=%x\n", ret);
	ret = camera_attr_get_tag_image_description(camera, &description);
	printf("-get tag image description \"%s\"\tret=%x\n", description, ret);
	free(description);	
	return 0;
}

int tag_software_test(camera_h camera){
	char *buffer;
	int ret;
	ret = camera_attr_set_tag_software(camera, "hello capi");
	printf("-set tag software \"hello capi\"\tret=%x\n", ret);
	ret = camera_attr_get_tag_software(camera, &buffer);
	printf("-get tag software  \"%s\"\tret=%x\n", buffer, ret);
	free(buffer);

	ret = camera_attr_set_tag_software(camera, "12345678901234567890");
	printf("-set tag software \"12345678901234567890\"\tret=%x\n", ret);
	ret = camera_attr_get_tag_software(camera, &buffer);
	printf("-get tag software \"%s\"\tret=%x\n", buffer, ret);
	free(buffer);
	return 0;	
}


bool _flash_mode_test_cb(camera_attr_flash_mode_e mode, void *user_data){
	camera_h camera = (camera_h) user_data;
	int ret;
	ret = camera_attr_set_flash_mode(camera, mode);
	printf("-set flash mode %d\tret=%x\n", mode,ret);
	ret = camera_attr_get_flash_mode(camera,&mode);
	printf("-get flash mode %d\tret=%x\n", mode,ret);		
	return true;
}


int flash_mode_test(camera_h camera){
	camera_attr_foreach_supported_flash_mode(camera, _flash_mode_test_cb,camera);
	return 0;
}

int gps_test(camera_h camera){
	double lng = 1.12;
	double lat = 1.13;
	double alt = 1.14;
	int ret;
	ret = camera_attr_set_geotag(camera, lat, lng , alt );
	if( ret != 0)
		return -1;
	ret = camera_attr_get_geotag(camera, &lat , &lng , &alt);
	if( ret != 0 )
		return -1;
	return 0;

}
int camera_attribute_test(){
	int ret;
	camera_h camera ;
	camera_create(CAMERA_DEVICE_CAMERA0 , &camera);
	camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);	
	printf("-----------------------create camera-----------------------------\n");

	preview_fps_test(camera);
	image_quality_test(camera);
	
	camera_start_preview(camera);
	printf("--------------------------preview-started-----------------------------------\n");
	
	ret = zoom_test(camera);
	ret += af_mode_test(camera);
	ret += exposure_mode_test(camera);
	ret += exposure_test(camera);
	ret += iso_test(camera);
	ret += brightness_test(camera);
	ret += contrast_test(camera);
	ret += whitebalance_test(camera);
	ret += effect_test(camera);
	ret += scene_mode_test(camera);
	ret += tag_enable_test(camera);
	ret += tag_orientation_test(camera);
	ret += tag_image_description_test(camera);
	ret += tag_software_test(camera);
	ret += flash_mode_test(camera);
	ret += gps_test(camera);

	camera_stop_preview(camera);
	camera_destroy(camera);
	return ret;
}


typedef struct {
	camera_h camera;
	camera_pixel_format_e in_format;
	bool iscalled;
	bool result;
} camera_preview_test_s;

 void _camera_preview_test_cb(void *stream_buffer, int buffer_size, int width, int height, camera_pixel_format_e format, void *user_data){
 	camera_preview_test_s * data = (camera_preview_test_s*)user_data;
	data->iscalled = true;
	if( format == data->in_format )
		data->result = true;
	
}

bool _preview_format_test_cb(camera_pixel_format_e format, void *user_data){
	int *table = (int*)user_data;
	table[format] = 1;	
	return true;
}


int camera_preview_test(){
	int ret;
	camera_h camera ;
	int i;
	int timeout = 0;
	camera_preview_test_s preview_test_data;
	int enable_preview_format[CAMERA_PIXEL_FORMAT_JPEG+1] = {0,};
	
	
	camera_create(CAMERA_DEVICE_CAMERA0 , &camera);
	camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);	
	camera_set_preview_cb(camera, 	 _camera_preview_test_cb	, &preview_test_data);

	ret = camera_foreach_supported_preview_format(camera, _preview_format_test_cb,enable_preview_format);

	printf("-----------------------PREVIEW FORMAT TEST-----------------------------\n");
	
	for(i =0; i<= CAMERA_PIXEL_FORMAT_JPEG ; i++){
		if( enable_preview_format[i] ){
			timeout = 5;
			preview_test_data.in_format = i;
			preview_test_data.camera = camera;
			preview_test_data.iscalled = false;
			preview_test_data.result = false;
			camera_set_preview_format(camera, i);
			printf("-------------PREVIEW FORMAT %d TEST--------------------\n", i);
			camera_start_preview(camera);
			while( preview_test_data.iscalled==false && timeout-- > 5 )
				sleep(1);

			camera_stop_preview(camera);
			if( preview_test_data.iscalled && preview_test_data.result ){
				printf("PASS\n");
			}else{
				printf("preview_test_data.result = %d\n", preview_test_data.result);
				printf("FAIL\n");
				camera_destroy(camera);
				return -1;
			}
			
		}
	}

	camera_destroy(camera);
	return 0;
	
	
}


typedef struct{
	bool iscalled;
	bool ispreviewed;
	bool iscapturing;
	bool iscaptured;
	camera_state_e state;
} state_change_data;

void _state_change_test_cb(camera_state_e previous , camera_state_e current , bool by_asm, void *user_data){
	state_change_data * data = (state_change_data*)user_data;
	data->iscalled = true;
	if( current == CAMERA_STATE_PREVIEW )
		data->ispreviewed = true;
	if( current == CAMERA_STATE_CAPTURED )
		data->iscaptured = true;
	if( current == CAMERA_STATE_CAPTURING )
		data->iscapturing = true;
	data->state = current;
}

void _capture_test_cb(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data){
	printf("capture callback\n");
}


int camera_state_change_test(){
	camera_h camera ;
	state_change_data data;
	bool ispass = true;
	int ret=0;
	
	camera_create(CAMERA_DEVICE_CAMERA0 , &camera);
	camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);
	camera_set_state_changed_cb(camera, _state_change_test_cb, &data);

	printf("------------------- PREVIEW STATE Change test------------------\n");
	data.iscalled = false;
	data.state = 0;
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %x\n", ret);
	sleep(1);
	if( data.iscalled && data.state == CAMERA_STATE_PREVIEW )
		printf("PASS\n");
	else{
		printf("FAIL\n");
		ispass = false;
	}


	printf("------------------- CREATED STATE Change test------------------\n");
	
	data.iscalled = false;
	data.state = 0;
	ret = camera_stop_preview(camera);
	printf("camera_stop_preview ret = %x\n", ret);
	sleep(1);
	if( data.iscalled && data.state == CAMERA_STATE_CREATED)
		printf("PASS\n");
	else{
		printf("FAIL\n");
		ispass = false;
	}


	printf("------------------- CAPTURED STATE Change test------------------\n");

	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %x\n", ret);
	sleep(1);
	data.iscalled = false;
	data.state = 0;
	data.iscaptured = false;
	data.ispreviewed= false;	
	data.iscapturing = false;	
	ret = camera_start_capture(camera, _capture_test_cb, NULL, NULL);
	printf("camera_start_capture ret = %x\n", ret);
	sleep(3);
	if( data.iscalled &&  data.iscaptured && data.iscapturing && data.state == CAMERA_STATE_CAPTURED)
		printf("PASS\n");
	else{
		printf("FAIL\n");
		ispass = false;
	}
	
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %x\n", ret);
	ret = camera_stop_preview(camera);
	printf("camera_stop_preview ret = %x\n", ret);
	ret = camera_destroy(camera);
	printf("camera_destroy ret = %x\n", ret);

	return ispass ? 0: -1;
	
}

void _capture_test2_cb(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data){
	int *iscalled = (int*)user_data;
	*iscalled = 1;
}

int capture_test(){
	camera_h camera ;
	int iscalled;
	camera_state_e state ;
	bool ispass = true;
	int timeout = 10;

	printf("---------------------CAPTURE Test -----------------\n");
	
	camera_create(CAMERA_DEVICE_CAMERA0 , &camera);
	camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);	
	camera_start_preview(camera);
	iscalled = 0;
	camera_start_capture(camera, _capture_test2_cb, NULL,  &iscalled);

	while( camera_get_state(camera, &state ) == 0 && state != CAMERA_STATE_CAPTURED && timeout-- > 0 )
		sleep(1);
	
	if( iscalled == 1 )
		printf("PASS\n");
	else{
		printf("FAIL\n");
		ispass = false;
	}

	
	camera_start_preview(camera);
	camera_stop_preview(camera);
	camera_destroy(camera);
	return ispass ? 0: -1;
	
}


typedef struct{
	int width[100];
	int height[100];
	int count;
} resolution_stack;


bool capture_resolution_test_cb(int width, int height, void *user_data){
	resolution_stack *data = (resolution_stack*)user_data;
	data->width[data->count] = width;
	data->height[data->count] = height;
	data->count++;

	printf("%dx%d\n",width, height);
	
	return true;
}

typedef struct{
	int expected_width;
	int expected_height;
	bool ispass;
}preview_test_data;
void _capture_test3_cb(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data){
	preview_test_data *data = (preview_test_data*)user_data;
	if( data->expected_height == image->height && data->expected_width == image->width )
		data->ispass = true;
}


int capture_resolution_test(){
	camera_h camera ;
	resolution_stack resolution_list;
	int i;
	camera_state_e state ;
	int ret = 0;

	camera_create(CAMERA_DEVICE_CAMERA0 , &camera);
	camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);
	resolution_list.count = 0;
	camera_foreach_supported_capture_resolution(camera, capture_resolution_test_cb, &resolution_list);
	//camera_set_state_changed_cb(camera, state_cb, NULL);

	printf("-----------------CAPTURE RESOLUTION TEST---------------------\n");

	for(i =0 ; i < resolution_list.count ; i++){
		preview_test_data data;
		data.ispass = false;
		data.expected_width = resolution_list.width[i];
		data.expected_height = resolution_list.height[i];
		int timeout = 10;

		printf("-----------------CAPTURE RESOLUTION (%dx%d)---------------------\n",data.expected_width  ,data.expected_height);
		
		printf("resolution set test %x\n", (unsigned int)camera_set_capture_resolution(camera,data.expected_width  ,data.expected_height));

		camera_start_preview(camera);
		
		camera_start_capture(camera, _capture_test3_cb , NULL,  &data);
		
		while( camera_get_state(camera, &state ) == 0 && state != CAMERA_STATE_CAPTURED && timeout-- > 0){
			sleep(1);
		}

		camera_start_preview(camera);
		camera_stop_preview(camera);
		if( !data.ispass ){
			ret += -1;
			printf("FAIL\n");
		}else{
			printf("PASS\n");		
		}
	}
		
	return ret;
}

bool preview_resolution_cb(int width, int height, void *user_data)
{
	printf("%dx%d\n", width, height);
	return true;
}


void preview_test(){
	camera_h camera ;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	camera_set_display(camera, CAMERA_DISPLAY_TYPE_EVAS,img);
	//camera_foreach_supported_preview_resolution(camera,preview_resolution_cb, NULL);
	camera_start_preview(camera);
}


void rotation_test(){
	camera_h camera;
	int ret;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("camera_create %d\n", ret);
	camera_set_display(camera, CAMERA_DISPLAY_TYPE_X11 , GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_NONE);
	camera_start_preview(camera);
	sleep(3);
	printf("180\n");
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_180);

	sleep(3);
	printf("270\n");
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);

	sleep(3);
	printf("90\n");
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_90);

	sleep(10);
	camera_stop_preview(camera);
	camera_destroy(camera);
	
}


void _focus_changed_cb2(camera_focus_state_e state, void *user_data){
	char* table[] = { "CAMERA_FOCUS_STATE_RELEASED", "CAMERA_FOCUS_STATE_ONGOING" , "CAMERA_FOCUS_STATE_FOCUSED","CAMERA_FOCUS_STATE_FAILED" };
	

	printf("focus state %s\n", table[state]);
}


void focus_test(){
	camera_h camera;
	int ret;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("camera_create %d\n", ret);
	camera_set_display(camera, CAMERA_DISPLAY_TYPE_X11 , GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);
	camera_set_focus_changed_cb(camera, _focus_changed_cb2, NULL);
	camera_start_preview(camera);

	printf("enter to start focusing\n"); getchar();
	camera_start_focusing(camera, false);
	sleep(3);

	printf("enter to start focusing 2\n"); 	getchar();
	camera_start_focusing(camera, false);
	sleep(3);

	printf("enter to start focusing 3\n"); getchar();
	camera_start_focusing(camera, false);
	sleep(3);

	printf("enter to cancel focusing\n");getchar();
	camera_cancel_focusing(camera);
	sleep(3);

	printf("enter to start CAF\n"); getchar();
	camera_start_focusing(camera, true);
	sleep(3);

	printf("enter to stop preview\n"); getchar();
	camera_stop_preview(camera);
	sleep(3);

	printf("enter to start preview\n"); getchar();
	camera_start_preview(camera);
	sleep(3);

	printf("enter to change AF mode to macro\n"); getchar();
	camera_attr_set_af_mode(camera, CAMERA_ATTR_AF_MACRO);
	sleep(3);


	printf("enter to stop focusing\n");getchar();
	camera_cancel_focusing(camera);
	sleep(3);

	printf("enter to set af mode macro\n");getchar();
	camera_attr_set_af_mode(camera, CAMERA_ATTR_AF_MACRO);
	sleep(3);

	printf("enter to start focusing\n"); getchar();
	camera_start_focusing(camera, false);
	sleep(3);


	printf("enter to CAF\n"); getchar();
	camera_start_focusing(camera, true);
	sleep(3);

	printf("enter to stop focusing\n");getchar();
	camera_cancel_focusing(camera);
	sleep(3);

	printf("enter to set af mode macro\n");getchar();
	camera_attr_set_af_mode(camera, CAMERA_ATTR_AF_MACRO);
	sleep(3);

	printf("enter to CAF\n"); getchar();
	camera_start_focusing(camera, true);
	sleep(3);

	printf("enter to stop preview\n"); getchar();
	camera_stop_preview(camera);
	camera_destroy(camera);
	
}


void camera_lens_rotation_test(){
	camera_h camera;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	camera_set_display(camera, CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	int angle;
	camera_attr_get_lens_orientation(camera, &angle);
	printf("angle =%d\n",angle);
	camera_start_preview(camera);

	sleep(20);
	camera_stop_preview(camera);
	camera_destroy(camera);


	camera_create(CAMERA_DEVICE_CAMERA1, &camera);
	camera_set_display(camera, CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));

	camera_attr_get_lens_orientation(camera, &angle);
	printf("angle =%d\n",angle);
	camera_start_preview(camera);

	sleep(20);
	camera_stop_preview(camera);
	camera_destroy(camera);
	
	
}

void contrast_test2(){
	camera_h camera;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	camera_attr_set_contrast(camera, 1);
}

void rotation_flip_test(){
	camera_h camera;
	camera_flip_e flip;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("flip in = %d\n", CAMERA_FLIP_HORIZONTAL);	
	camera_attr_set_stream_flip(camera, CAMERA_FLIP_HORIZONTAL);
	camera_attr_get_stream_flip(camera, &flip);
	printf("flip out = %d\n", flip);

	printf("flip in = %d\n", CAMERA_FLIP_NONE);		
	camera_attr_set_stream_flip(camera, CAMERA_FLIP_NONE);
	camera_attr_get_stream_flip(camera, &flip);
	printf("flip out = %d\n", flip);

	printf("flip in = %d\n", CAMERA_FLIP_VERTICAL);		
	camera_attr_set_stream_flip(camera, CAMERA_FLIP_VERTICAL);
	camera_attr_get_stream_flip(camera, &flip);
	printf("flip out = %d\n", flip);

	printf("flip in = %d\n", CAMERA_FLIP_BOTH);		
	camera_attr_set_stream_flip(camera, CAMERA_FLIP_BOTH);
	camera_attr_get_stream_flip(camera, &flip);
	printf("flip out = %d\n", flip);

	
}



typedef struct{
	camera_h camera;
	int count;
	bool completed;
}conti_test_data;

void conti_capturing_cb(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data){
	conti_test_data *test_data = (conti_test_data*)user_data;
	printf("capture callback!!%d\n", test_data->count++);
	if( test_data->count == 5 )
		camera_stop_continuous_capture(test_data->camera);
}

void _capture_completed_cb(void *user_data){
	conti_test_data *test_data = (conti_test_data*)user_data;
	test_data->completed = true;
	printf("capture completed\n");
}

int continuous_capture_test(){
	printf("--------------continuous capture test--------------------\n");
	camera_h camera;
	conti_test_data test_data;
	int timeout =30;
	int ret;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("camera_create %x\n", ret);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	printf("camera_set_display %x\n", ret);
	ret = camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);
	printf("camera_set_x11_display_rotation %x\n", ret);
	ret = camera_start_preview(camera);
	printf("camera_start_preview %x\n", ret);

	test_data.camera = camera;
	test_data.count = 0;
	test_data.completed = false;
	ret = camera_start_continuous_capture(camera, 10, 10, conti_capturing_cb, _capture_completed_cb, &test_data);
	printf("camera_start_continuous_capture ret = %x\n", ret);
	if( ret != 0 ){
		printf("fail conti capture\n");
		camera_stop_preview(camera);
		camera_destroy(camera);
		return -1;
	}

	while( test_data.completed == false && timeout-- > 0 )
		sleep(1);

	camera_start_preview(camera);

	if( test_data.count != 5|| test_data.completed == false ){
		camera_stop_preview(camera);
		camera_destroy(camera);
		return -1;
	}


	test_data.camera = camera;
	test_data.count = 0;
	test_data.completed = false;
	timeout = 30;

	ret = camera_start_continuous_capture(camera, 10, 1000, conti_capturing_cb, _capture_completed_cb, &test_data);

	if( ret != 0 ){
		printf("fail conti capture\n");
		camera_stop_preview(camera);
		camera_destroy(camera);
		return -1;
	}

	sleep(2);
	camera_stop_continuous_capture(camera);

	while( test_data.completed == false && timeout-- > 0 )
		sleep(1);

	camera_start_preview(camera);
	camera_stop_preview(camera);
	camera_destroy(camera);

	printf("total capture count = %d\n", test_data.count);
	if( test_data.completed == false)
		return -1;
	return 0;
}


void _face_detected(camera_detected_face_s *faces, int count, void *user_data){
	printf("face detected!!\n");
	int i;
	for(i = 0 ; i < count ; i++){
		printf("%d) - %dx%d\n", faces[i].id, faces[i].x, faces[i].y);
	}
}


int face_detection_test(){
	printf("--------------face_detection_test--------------------\n");
	camera_h camera;
	int ret;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("camera_create %x\n", ret);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	printf("camera_set_display %x\n", ret);
	ret = camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);
	printf("camera_set_x11_display_rotation %x\n", ret);
	ret = camera_start_preview(camera);
	printf("camera_start_preview %x\n", ret);
	if( camera_is_supported_face_detection(camera) ){
		ret = camera_start_face_detection(camera, _face_detected, NULL);
		printf("camera_start_face_detection %x\n", ret);
	}else
	{
		printf("not supported face detection\n");
	}
	return 0;
}

int face_count ;
void _face_detected2(camera_detected_face_s *faces, int count, void *user_data){
	face_count = count;
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n-------------------------------------------\n");
	int i;
	for( i = 0 ; i < count ; i++ ){
		printf("%d) %dx%d \n", faces[i].id, faces[i].x , faces[i].y);
	}
}


int face_zoom_test(){
	printf("--------------face_zoom_test--------------------\n");
	camera_h camera;
	int ret;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("camera_create %x\n", ret);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	printf("camera_set_display %x\n", ret);
	ret = camera_set_x11_display_rotation(camera, CAMERA_ROTATION_270);
	printf("camera_set_x11_display_rotation %x\n", ret);
	ret = camera_start_preview(camera);
	printf("camera_start_preview %x\n", ret);
	if( camera_is_supported_face_detection(camera) ){
		ret = camera_start_face_detection(camera, _face_detected2, NULL);
		printf("camera_start_face_detection %x\n", ret);
	}else
	{
		printf("not supported face detection\n");
	}
	while(1){
		char select[255];
		//printf("current faces %d\n", face_count);
		//printf("x is zoom cancel\n");
		//printf("select face>");
		gets(select);
		if( select[0] == 'x' ){
			camera_cancel_face_zoom(camera);
		}else if( select[0] != '\0'){
			int face = select[0] - '0';
			//printf("input : <%s>\n", select);
			if( face >= 0 ){
				//printf("select %d\n", face);
				camera_face_zoom(camera, face);
			}
		}
	}
	return 0;
}

char *table[] = {
"CAMERA_PIXEL_FORMAT_NV12",           /**< NV12 pixel format */
"CAMERA_PIXEL_FORMAT_NV12T",          /**< NV12 Tiled pixel format */
"CAMERA_PIXEL_FORMAT_NV16",           /**< NV16 pixel format */
"CAMERA_PIXEL_FORMAT_NV21",           /**< NV21 pixel format */
"CAMERA_PIXEL_FORMAT_YUYV",           /**< YUYV(YUY2) pixel format */
"CAMERA_PIXEL_FORMAT_UYVY",           /**< UYVY pixel format */
"CAMERA_PIXEL_FORMAT_422P",           /**< YUV422(Y:U:V) planar pixel format */
"CAMERA_PIXEL_FORMAT_I420",           /**< I420 pixel format */
"CAMERA_PIXEL_FORMAT_YV12",           /**< YV12 pixel format */
"CAMERA_PIXEL_FORMAT_RGB565",         /**< RGB565 pixel format */
"CAMERA_PIXEL_FORMAT_RGB888",         /**< RGB888 pixel format */
"CAMERA_PIXEL_FORMAT_RGBA",           /**< RGBA pixel format */
"CAMERA_PIXEL_FORMAT_ARGB",           /**< ARGB pixel format */
"CAMERA_PIXEL_FORMAT_JPEG"           /**< Encoded pixel format */
};


bool supported_preview_format_test(camera_pixel_format_e format,void *user_data){
	printf("%s\n", table[format]);
	return true;
}

int preview_format_test(){
	camera_h camera;
	camera_pixel_format_e format;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("----------CAMERA_DEVICE_CAMERA0 -----------------\n");
	camera_foreach_supported_preview_format(camera, supported_preview_format_test, NULL);
	camera_get_preview_format(camera, &format);
	printf("default - %s\n", table[format]);

	camera_destroy(camera);
	camera_create(CAMERA_DEVICE_CAMERA1, &camera);
	printf("----------CAMERA_DEVICE_CAMERA1 -----------------\n");
	camera_foreach_supported_preview_format(camera, supported_preview_format_test, NULL);
	camera_get_preview_format(camera, &format);
	printf("default - %s\n", table[format]);
	camera_destroy(camera);

	return 0;

}

void _hdr_progress_cb(int percent, void *user_data){
	printf("percent = %d\n", percent);
}

void _hdr_capturing_cb(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data){
	printf("hdr capturing!\n");
}

void _hdr_capture_completed_cb(void *user_data){
	printf("hdr capture complete\n");
}



int hdr_capture_test(){
	camera_h camera;
	int ret;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	if( !camera_attr_is_supported_hdr_capture(camera) ){
		printf("Not supported HDR Capture\n");
		return 0;
	}

	camera_attr_set_hdr_capture_progress_cb(camera, _hdr_progress_cb, NULL);
	camera_attr_set_hdr_mode(camera, CAMERA_ATTR_HDR_MODE_KEEP_ORIGINAL);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_start_preview(camera);
	camera_start_capture(camera, _hdr_capturing_cb , _hdr_capture_completed_cb , NULL);

	return 0;
}

Eina_Bool captured_event_check5(void *data){
	printf("!!!captured_event_check5\n");
	camera_h camera = (camera_h)data;
	camera_start_capture(camera, _hdr_capturing_cb , _hdr_capture_completed_cb , NULL);
	camera_state_e state;
	camera_get_state(camera,&state);
	while( state == CAMERA_STATE_CAPTURING ){
		//printf("current state = %d\n", state);
		usleep(10000);
		camera_get_state(camera,&state);
	}
	printf("current state = %d\n", state);
	camera_start_preview(camera);
	camera_stop_preview(camera);
	camera_destroy(camera);

	return false;
}


int hdr_capture_test2(){
	camera_h camera;
	int ret;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	if( !camera_attr_is_supported_hdr_capture(camera) ){
		printf("Not supported HDR Capture\n");
		return 0;
	}

	camera_attr_set_hdr_capture_progress_cb(camera, _hdr_progress_cb, NULL);
	camera_attr_set_hdr_mode(camera, CAMERA_ATTR_HDR_MODE_KEEP_ORIGINAL);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_start_preview(camera);
	ecore_idler_add(captured_event_check5, camera);

	return 0;
}


void _captured_check_capturing_cb(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data){
	printf("capturing!\n");
}

void _captured_check_capture_completed_cb(void *user_data){
	printf("capture complete\n");
}

Eina_Bool captured_event_check4(void *data){
	printf("!!!HDR captured event check4\n");
	camera_h camera;
	int ret;
	camera_state_e state;

	ret= camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %d\n", ret);
	camera_attr_set_hdr_mode(camera, CAMERA_ATTR_HDR_MODE_KEEP_ORIGINAL);
	camera_start_capture(camera, _captured_check_capturing_cb , _captured_check_capture_completed_cb , NULL);
	printf("camera_start_capture ret = %d\n", ret);
	camera_get_state(camera,&state);
	while( state == CAMERA_STATE_CAPTURING ){
		//printf("current state = %d\n", state);
		usleep(10000);
		camera_get_state(camera,&state);
	}
	printf("current state = %d\n", state);
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %d\n", ret);
	ret = camera_stop_preview(camera);
	printf("camera_stop_preview ret = %d\n", ret);
	ret = camera_destroy(camera);
	printf("camera_destroy ret = %d\n", ret);
	return false;
}

Eina_Bool captured_event_check3(void *data){
	printf("!!!continuous break captured event check3\n");
	camera_h camera;
	int ret;
	camera_state_e state;

	ret= camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("camera_create ret = %d\n", ret);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %d\n", ret);
	ret = camera_start_continuous_capture(camera, 5 , 1000, _captured_check_capturing_cb, _captured_check_capture_completed_cb, NULL);
	printf("camera_start_continuous_capture ret = %d\n", ret);
	camera_get_state(camera,&state);
	int count = 0;
	while( state == CAMERA_STATE_CAPTURING ){
		//printf("current state = %d\n", state);
		usleep(10000);
		count++;
		if( count == 10)
			camera_stop_continuous_capture(camera);
		camera_get_state(camera,&state);
	}
	printf("current state = %d\n", state);
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %d\n", ret);
	ret = camera_stop_preview(camera);
	printf("camera_stop_preview ret = %d\n", ret);
	ret = camera_destroy(camera);
	printf("camera_destroy ret = %d\n", ret);
	ecore_idler_add(captured_event_check4, NULL);
	return false;
}

Eina_Bool captured_event_check2(void *data){
	printf("!!!continuous shot captured event check2\n");
	camera_h camera;
	int ret;
	camera_state_e state;

	ret= camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	printf("camera_create ret = %d\n", ret);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %d\n", ret);
	ret = camera_start_continuous_capture(camera, 5 , 1000, _captured_check_capturing_cb, _captured_check_capture_completed_cb, NULL);
	printf("camera_start_continuous_capture ret = %d\n", ret);
	camera_get_state(camera,&state);
	while( state == CAMERA_STATE_CAPTURING ){
		//printf("current state = %d\n", state);
		usleep(10000);
		camera_get_state(camera,&state);
	}
	printf("current state = %d\n", state);
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %d\n", ret);
	ret = camera_stop_preview(camera);
	printf("camera_stop_preview ret = %d\n", ret);
	ret = camera_destroy(camera);
	printf("camera_destroy ret = %d\n", ret);
	ecore_idler_add(captured_event_check3, NULL);
	return false;
}


Eina_Bool captured_event_check(void *data){
	printf("!!!Normal captured event check\n");
	camera_h camera;
	camera_state_e state;

	int ret;
	ret= camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %d\n", ret);
	camera_start_capture(camera, _captured_check_capturing_cb , _captured_check_capture_completed_cb , NULL);
	printf("camera_start_capture ret = %d\n", ret);
	camera_get_state(camera,&state);
	while( state == CAMERA_STATE_CAPTURING ){
		//printf("current state = %d\n", state);
		usleep(10000);
		camera_get_state(camera,&state);
	}
	printf("current state = %d\n", state);
	ret = camera_start_preview(camera);
	printf("camera_start_preview ret = %d\n", ret);
	ret = camera_stop_preview(camera);
	printf("camera_stop_preview ret = %d\n", ret);
	ret = camera_destroy(camera);
	printf("camera_destroy ret = %d\n", ret);

	ecore_idler_add(captured_event_check2, NULL);
	return false;
}

void rotate_test(){
	camera_h camera;
	int ret;
	ret= camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	ret = camera_set_display(camera,CAMERA_DISPLAY_TYPE_X11, GET_DISPLAY(preview_win));
	camera_set_x11_display_rotation(camera, CAMERA_ROTATION_NONE);
	ret = camera_start_preview(camera);
	int angle;
	camera_attr_get_lens_orientation(camera, &angle);
	printf("camera lens angle %d\n", angle);
	printf("camera_start_preview ret = %d\n", ret);
}

void supported_ZSL_test(){
	camera_h camera;
	int ret;
	ret= camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	if( camera_is_supported_zero_shutter_lag(camera))
		printf("support zero shutter lag\n");
	else
		printf("not support zero shutter lag\n");
}

int camera_test(){

	int ret=0;

	//ret = camera_attribute_test();
	//ret += camera_preview_test();
	//ret += camera_state_change_test();
	//ret += capture_test();
	//ret += capture_resolution_test();
	//ret += stillshot_test();
	//camera_lens_rotation_test();
	//contrast_test2();
	//rotation_flip_test();
	//ret += continuous_capture_test();
	//ret = face_detection_test();
	//face_zoom_test();
	//preview_format_test();
	//hdr_capture_test();
	//hdr_capture_test();
	//hdr_capture_test2();
	//rotate_test();
	//supported_ZSL_test();
	focus_test();
	return ret;
}



void* test_main(void *arg){
	int ret = 0;
	
	ret = camera_test();
	if( ret == 0 )
		printf("--------------CAMERA TEST ALL PASS--------------------------\n");
	else
		printf("--------------CAMERA TEST FAIL %d--------------------------\n", -ret);

	
	return 0;

}

int main(int argc, char ** argv)
{
	int w,h;
	elm_init(argc, argv);



	mEvasWindow = elm_win_add(NULL, "VIDEO OVERLAY", ELM_WIN_BASIC);
	elm_win_title_set(mEvasWindow, "video oeverlay window");
	elm_win_borderless_set(mEvasWindow, EINA_TRUE);
	ecore_x_window_size_get(ecore_x_window_root_first_get(),	&w, &h);
	evas_object_resize(mEvasWindow, w, h);
	elm_win_indicator_state_set(mEvasWindow, EINA_TRUE);

	//elm_win_rotation_set(mEvasWindow, 270);
	//elm_win_fullscreen_set(mEvasWindow, 1);
	preview_win = elm_win_xwindow_get(mEvasWindow);

	evas_object_show(mEvasWindow);	

	img = evas_object_image_add(evas_object_evas_get(mEvasWindow));
	evas_object_resize(mEvasWindow, w, h);

	evas_object_image_fill_set(img, 0, 0, w, h);

	evas_object_show(img);

	pthread_t gloop_thread;

	pthread_create(&gloop_thread, NULL, test_main,  NULL);

	//ecore_idler_add(captured_event_check, NULL);

	elm_run();
	elm_shutdown();
	

	return 0;
}

