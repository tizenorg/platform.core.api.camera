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



#include <tet_api.h>
#include <media/camera.h>
#include <stdio.h>
#include <glib.h>
#include <pthread.h>


#define MY_ASSERT( fun , test , msg ) \
{\
	if( !test ) \
		dts_fail(fun , msg ); \
}		

static GMainLoop *g_mainloop = NULL;
	

static void utc_camera_set_display_negative(void);
static void utc_camera_set_display_positive(void);

static void utc_camera_set_preview_resolution_negative(void);
static void utc_camera_set_preview_resolution_positive(void);

static void utc_camera_set_display_rotation_negative(void);
static void utc_camera_set_display_rotation_positive(void);

static void utc_camera_set_capture_resolution_negative(void);
static void utc_camera_set_capture_resolution_positive(void);

static void utc_camera_set_capture_format_negative(void);
static void utc_camera_set_capture_format_positive(void);

static void utc_camera_set_preview_format_negative(void);
static void utc_camera_set_preview_format_positive(void);

static void utc_camera_get_preview_resolution_negative(void);
static void utc_camera_get_preview_resolution_positive(void);

static void utc_camera_get_display_rotation_negative(void);
static void utc_camera_get_display_rotation_positive(void);

static void utc_camera_get_capture_resolution_negative(void);
static void utc_camera_get_capture_resolution_positive(void);

static void utc_camera_get_preview_format_negative(void);
static void utc_camera_get_preview_format_positive(void);

static void utc_camera_set_preview_cb_negative(void);
static void utc_camera_set_preview_cb_positive(void);

static void utc_camera_unset_preview_cb_negative(void);
static void utc_camera_unset_preview_cb_positive(void);

static void utc_camera_set_state_changed_cb_negative(void);
static void utc_camera_set_state_changed_cb_positive(void);

static void utc_camera_unset_state_changed_cb_negative(void);
static void utc_camera_unset_state_changed_cb_positive(void);

static void utc_camera_set_focus_changed_cb_negative(void);
static void utc_camera_set_focus_changed_cb_positive(void);

static void utc_camera_unset_focus_changed_cb_negative(void);
static void utc_camera_unset_focus_changed_cb_positive(void);

static void utc_camera_foreach_supported_preview_resolution_negative(void);
static void utc_camera_foreach_supported_preview_resolution_positive(void);

static void utc_camera_foreach_supported_capture_resolution_negative(void);
static void utc_camera_foreach_supported_capture_resolution_positive(void);

static void utc_camera_foreach_supported_capture_format_negative(void);
static void utc_camera_foreach_supported_capture_format_positive(void);

static void utc_camera_foreach_supported_preview_format_negative(void);
static void utc_camera_foreach_supported_preview_format_positive(void);

static void utc_camera_set_x11_display_visible_positive(void);
static void utc_camera_set_x11_display_visible_negative(void);

static void utc_camera_is_x11_display_visible_positive(void);
static void utc_camera_is_x11_display_visible_negative(void);


static void utc_camera_set_x11_display_mode_positive(void);
static void utc_camera_set_x11_display_mode_negative(void);

static void utc_camera_get_x11_display_mode_positive(void);
static void utc_camera_get_x11_display_mode_negative(void);


static void utc_camera_get_capture_format_positive(void);
static void utc_camera_get_capture_format_negative(void);

static void utc_camera_set_error_cb_positive(void);
static void utc_camera_set_error_cb_negative(void);

static void utc_camera_unset_error_cb_positive(void);
static void utc_camera_unset_error_cb_negative(void);





static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

struct tet_testlist tet_testlist[] = {
	{utc_camera_set_display_negative , 1 },
	{utc_camera_set_display_positive , 2 },

	{utc_camera_set_preview_resolution_negative , 3 },
	{utc_camera_set_preview_resolution_positive , 4 },

	{utc_camera_set_display_rotation_negative , 5 },
	{utc_camera_set_display_rotation_positive , 6 },

	{utc_camera_set_capture_resolution_negative , 7 },
	{utc_camera_set_capture_resolution_positive, 8 },

	{utc_camera_set_capture_format_negative, 9 },
	{utc_camera_set_capture_format_positive, 10 },

	{utc_camera_set_preview_format_negative, 11 },
	{utc_camera_set_preview_format_positive, 12 },

	{utc_camera_get_preview_resolution_negative, 13 },
	{utc_camera_get_preview_resolution_positive, 14 },

	{utc_camera_get_display_rotation_negative, 15 },
	{utc_camera_get_display_rotation_positive, 16 },

	{utc_camera_get_capture_resolution_negative, 17 },
	{utc_camera_get_capture_resolution_positive, 18 },

	{utc_camera_get_preview_format_negative, 19 },
	{utc_camera_get_preview_format_positive, 20 },

	{utc_camera_set_preview_cb_negative, 21 },
	{utc_camera_set_preview_cb_positive, 22 },

	{utc_camera_unset_preview_cb_negative, 23 },
	{utc_camera_unset_preview_cb_positive, 24 },

	{utc_camera_set_state_changed_cb_negative, 29 },
	{utc_camera_set_state_changed_cb_positive, 30 },

	{utc_camera_unset_state_changed_cb_negative, 31 },
	{utc_camera_unset_state_changed_cb_positive, 32 },

	{utc_camera_set_focus_changed_cb_negative, 33 },
	{utc_camera_set_focus_changed_cb_positive, 34 },

	{utc_camera_unset_focus_changed_cb_negative, 35 },
	{utc_camera_unset_focus_changed_cb_positive, 36 },

	{utc_camera_foreach_supported_preview_resolution_negative, 41 },
	{utc_camera_foreach_supported_preview_resolution_positive, 42 },

	{utc_camera_foreach_supported_capture_resolution_negative, 43 },
	{utc_camera_foreach_supported_capture_resolution_positive, 44 },

	{utc_camera_foreach_supported_capture_format_negative, 45 },
	{utc_camera_foreach_supported_capture_format_positive, 46 },

	{utc_camera_foreach_supported_preview_format_negative, 47 },
	{utc_camera_foreach_supported_preview_format_positive, 48 },

	{ utc_camera_set_x11_display_visible_negative , 49 }, 	
	{ utc_camera_set_x11_display_visible_positive , 50 },

	{ utc_camera_is_x11_display_visible_negative , 51 }, 	
	{ utc_camera_is_x11_display_visible_positive , 52 },

	{ utc_camera_set_x11_display_mode_negative , 53 }, 	
	{ utc_camera_set_x11_display_mode_positive , 54 },
	
	{ utc_camera_get_x11_display_mode_negative , 55 }, 	
	{ utc_camera_get_x11_display_mode_positive , 56 },

	{ utc_camera_get_capture_format_positive , 57 }, 	
	{ utc_camera_get_capture_format_negative , 58 },

	{ utc_camera_set_error_cb_positive , 59 }, 	
	{ utc_camera_set_error_cb_negative , 60 },

	{ utc_camera_unset_error_cb_positive , 61 }, 	
	{ utc_camera_unset_error_cb_negative , 62 },

	
	{ NULL, 0 },
};


gpointer GmainThread(gpointer data){
	g_mainloop = g_main_loop_new (NULL, 0);
	g_main_loop_run (g_mainloop);
	
	return NULL;
}

camera_h camera;


static void startup(void)
{
	fprintf(stderr, "%s test\n", __func__);
	/* start of TC */
	int ret;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	if( ret != 0 )
		dts_fail("camera_create", "Could not create camera"); 	
}

static void cleanup(void)
{
	/* end of TC */
	camera_destroy(camera);
}


static void utc_camera_set_display_negative(void)
{
	fprintf(stderr, "%s test\n", __func__);
	int ret;
	ret = camera_set_display(NULL, CAMERA_DISPLAY_TYPE_X11, 0);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}
static void utc_camera_set_display_positive(void)
{
	int ret;
	ret = camera_set_display(camera, CAMERA_DISPLAY_TYPE_X11, 0);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE , "set display handle fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_set_preview_resolution_negative(void)
{
	int ret;
	ret = camera_set_preview_resolution(NULL,0,  0);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}


bool _preview_resolution_cb(int width, int height, void *user_data )
{
	int *resolution = (int*)user_data;
	resolution[0] = width;
	resolution[1] = height;
	return false;
}

static void utc_camera_set_preview_resolution_positive(void)
{
	int ret;
	int resolution[2];
	
	camera_foreach_supported_preview_resolution(camera, _preview_resolution_cb, resolution)	;
	ret = camera_set_preview_resolution(camera,resolution[0],  resolution[1]);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE , "set preview resolution is faild");

	dts_pass(__func__, "PASS");	
}

static void utc_camera_set_display_rotation_negative(void)
{
	int ret;
	ret = camera_set_x11_display_rotation(camera, -1);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE , "not allow -1");
	dts_pass(__func__, "PASS");	
}
static void utc_camera_set_display_rotation_positive(void)
{
	int ret;
	ret = camera_set_x11_display_rotation(camera, CAMERA_ROTATION_NONE);
	MY_ASSERT(__func__, ret== CAMERA_ERROR_NONE , "camera_set_display_rotation fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_set_capture_resolution_negative(void)
{
	int ret;
	ret = camera_set_capture_resolution(NULL, 0,0);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE , "not allow NULL");
	dts_pass(__func__, "PASS");	
}

bool _capture_resolution_cb(int width, int height, void *user_data)
{
	int *resolution = (int*) user_data;
	resolution[0] = width;
	resolution[1] = height;
	return false;
}

static void utc_camera_set_capture_resolution_positive(void)
{
	int ret;
	int resolution[2];
	
	camera_foreach_supported_capture_resolution(camera, _capture_resolution_cb, resolution);
	ret = camera_set_capture_resolution(camera, resolution[0],resolution[1]);
	
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE , "camera_set_capture_resolution fail");
	dts_pass(__func__, "PASS");	

}
static void utc_camera_set_capture_format_negative(void)
{
	int ret;
	ret = camera_set_capture_format(camera, -1);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE , "not allow -1");

	dts_pass(__func__, "PASS");	
}

bool _capture_format_cb(camera_pixel_format_e format , void *user_data)
{
	int *ret = (int*) user_data;
	*ret = format;
	return false;
}

static void utc_camera_set_capture_format_positive(void)
{
	int ret;
	int value;
	camera_foreach_supported_capture_format(camera, _capture_format_cb, &value);
	ret = camera_set_capture_format(camera, value);
	MY_ASSERT(__func__, ret== CAMERA_ERROR_NONE , "camera_set_capture_format fail");
	
	dts_pass(__func__, "PASS");	
}

static void utc_camera_set_preview_format_negative(void)
{
	int ret ;
	ret = camera_set_preview_format(camera, -1);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "-1 is not allowed");
	dts_pass(__func__, "PASS");	
}

bool _preview_format_cb(camera_pixel_format_e format , void *user_data)
{
	camera_pixel_format_e * ret = (camera_pixel_format_e*)user_data;
	*ret = format;
	return false;
}

static void utc_camera_set_preview_format_positive(void)
{
	int ret;
	camera_pixel_format_e format;
	camera_foreach_supported_preview_format(camera, _preview_format_cb, &format);
	ret = camera_set_preview_format(camera, format);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE,"set preview format fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_get_preview_resolution_negative(void)
{
	int ret ;
	ret = camera_get_preview_resolution(NULL, NULL,NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");

	dts_pass(__func__, "PASS");	
}
static void utc_camera_get_preview_resolution_positive(void)
{
	int ret;
	int value1;
	int value2;
	ret = camera_get_preview_resolution(camera, &value1,&value2);
	MY_ASSERT(__func__,ret == CAMERA_ERROR_NONE, "get preview resolution fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_get_display_rotation_negative(void)
{
	int ret ;
	ret = camera_get_x11_display_rotation(NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");

	dts_pass(__func__, "PASS");	

}
static void utc_camera_get_display_rotation_positive(void)
{
	int ret;
	camera_rotation_e value;
	ret = camera_get_x11_display_rotation(camera, &value);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail get display rotation");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_get_capture_resolution_negative(void)
{
	int ret ;
	ret = camera_get_capture_resolution(NULL, NULL,NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}
static void utc_camera_get_capture_resolution_positive(void)
{
	int ret;
	int value1, value2;
	ret = camera_get_capture_resolution(camera,&value1, &value2);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "get capture resolution fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_get_preview_format_negative(void)
{
	int ret ;
	ret = camera_get_preview_format(NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");

	dts_pass(__func__, "PASS");	
}
static void utc_camera_get_preview_format_positive(void)
{
	int ret;
	int value;
	ret=  camera_get_preview_format(camera, &value);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "get preview format fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_set_preview_cb_negative(void)
{
	int ret ;
	ret = camera_set_preview_cb(NULL, NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}

void _preview_cb(void* stream_buffer, int buffer_size, int width, int height, camera_pixel_format_e format, void *user_data)
{
}

static void utc_camera_set_preview_cb_positive(void)
{
	int ret;
	ret = camera_set_preview_cb(camera,_preview_cb,NULL);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "set preview cb fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_unset_preview_cb_negative(void)
{
	int ret ;
	ret = camera_unset_preview_cb(NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_unset_preview_cb_positive(void)
{
	int ret;
	ret = camera_unset_preview_cb(camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "unset preview cb fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_set_state_changed_cb_negative(void)
{
	int ret ;
	ret = camera_set_state_changed_cb(NULL, NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
	
}

void _changed_cb(camera_state_e previous , camera_state_e current , bool by_asm, void *user_data)
{
}

static void utc_camera_set_state_changed_cb_positive(void)
{
	int ret;
	ret = camera_set_state_changed_cb(camera, _changed_cb, NULL);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE , "set state change cb fail");
	dts_pass(__func__, "PASS");	
}
static void utc_camera_unset_state_changed_cb_negative(void)
{
	int ret ;
	ret = camera_unset_state_changed_cb(NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}
static void utc_camera_unset_state_changed_cb_positive(void)
{
	int ret ;
	ret = camera_unset_state_changed_cb(camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "camera_unset_state_changed_cb fail");

	dts_pass(__func__, "PASS");	
}
static void utc_camera_set_focus_changed_cb_negative(void)
{
	int ret ;
	ret = camera_set_focus_changed_cb(NULL, NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}

void _focus_changed_cb(camera_focus_state_e state, void *user_data)
{
}

static void utc_camera_set_focus_changed_cb_positive(void)
{
	int ret;
	ret = camera_set_focus_changed_cb(camera, _focus_changed_cb, NULL);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail");

	dts_pass(__func__, "PASS");	
}
static void utc_camera_unset_focus_changed_cb_negative(void)
{
	int ret ;
	ret = camera_unset_focus_changed_cb(NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}
static void utc_camera_unset_focus_changed_cb_positive(void)
{
	int ret ;
	ret = camera_unset_focus_changed_cb(camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_foreach_supported_preview_resolution_negative(void)
{
	int ret ;
	ret = camera_foreach_supported_preview_resolution(NULL, NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}

bool _preview_resolution_cb2(int width, int height, void *user_data )
{
	return false;
}

static void utc_camera_foreach_supported_preview_resolution_positive(void)
{
	int ret ;
	ret = camera_foreach_supported_preview_resolution(camera, _preview_resolution_cb2, NULL);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_foreach_supported_capture_resolution_negative(void)
{
	int ret ;
	ret = camera_foreach_supported_capture_resolution(NULL, NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}

bool _capture_resolution_cb2(int width, int height, void *user_data)
{
	return false;
}

static void utc_camera_foreach_supported_capture_resolution_positive(void)
{
	int ret ;
	ret = camera_foreach_supported_capture_resolution(camera, _capture_resolution_cb2, NULL);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_foreach_supported_capture_format_negative(void)
{
	int ret ;
	ret = camera_foreach_supported_capture_format(NULL, NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}

bool _capture_format_cb2(camera_pixel_format_e format , void *user_data)
{
	return false;
}

static void utc_camera_foreach_supported_capture_format_positive(void)
{
	int ret ;
	ret = camera_foreach_supported_capture_format(camera, _capture_format_cb2, NULL);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_foreach_supported_preview_format_negative(void)
{
	int ret ;
	ret = camera_foreach_supported_preview_format(NULL, NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}

bool _preview_format_cb2(camera_pixel_format_e format , void *user_data)
{
	return false;
}

static void utc_camera_foreach_supported_preview_format_positive(void)
{
	int ret ;
	ret = camera_foreach_supported_preview_format(camera, _preview_format_cb2,NULL);	
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}


static void utc_camera_set_x11_display_visible_positive(void){
	int ret;
	ret = camera_set_x11_display_visible(camera, true);
	MY_ASSERT(__func__, ret== CAMERA_ERROR_NONE , "camera_set_x11_display_visible fail");
	dts_pass(__func__, "PASS");		
}
static void utc_camera_set_x11_display_visible_negative(void){
	int ret;
	ret = camera_set_x11_display_visible(NULL, 0);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE , "not allow NULL");
	dts_pass(__func__, "PASS");		
}

static void utc_camera_is_x11_display_visible_positive(void){
	int ret;
	bool value;
	ret = camera_is_x11_display_visible(camera, &value);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail camera_is_x11_display_visible");
	dts_pass(__func__, "PASS");	
	
}
static void utc_camera_is_x11_display_visible_negative(void){
	int ret ;
	ret = camera_is_x11_display_visible(NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");

	dts_pass(__func__, "PASS");		
}


static void utc_camera_set_x11_display_mode_positive(void){
	int ret;
	ret = camera_set_x11_display_mode(camera, CAMERA_DISPLAY_MODE_LETTER_BOX);
	MY_ASSERT(__func__, ret== CAMERA_ERROR_NONE , "camera_set_x11_display_mode fail");
	dts_pass(__func__, "PASS");		
	
}
static void utc_camera_set_x11_display_mode_negative(void){
	int ret;
	ret = camera_set_x11_display_mode(NULL, 0);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE , "not allow NULL");
	dts_pass(__func__, "PASS");		
}

static void utc_camera_get_x11_display_mode_positive(void){
	int ret;
	camera_display_mode_e value;
	ret = camera_get_x11_display_mode(camera, &value);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail camera_set_x11_display_mode");
	dts_pass(__func__, "PASS");		
}
static void utc_camera_get_x11_display_mode_negative(void){
	int ret ;
	ret = camera_get_x11_display_mode(NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");

	dts_pass(__func__, "PASS");			
}



static void utc_camera_get_capture_format_positive(void)
{
	int ret;
	camera_pixel_format_e value;
	ret = camera_get_capture_format(camera, &value);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail camera_get_capture_format");
	dts_pass(__func__, "PASS");		
}
static void utc_camera_get_capture_format_negative(void)
{
	int ret ;
	ret = camera_get_capture_format(NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "NULL is not allowed");
	dts_pass(__func__, "PASS");			
}



void _error_cb(int error, camera_state_e current_state, void *user_data)
{
}

static void utc_camera_set_error_cb_positive(void)
{
	int ret;
	ret = camera_set_error_cb(camera, _error_cb, NULL);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_set_error_cb_negative(void)
{
	int ret;
	ret = camera_set_error_cb(NULL, NULL, NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}

static void utc_camera_unset_error_cb_positive(void)
{
	int ret;
	ret = camera_unset_error_cb(camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}
static void utc_camera_unset_error_cb_negative(void)
{
	int ret;
	ret = camera_unset_error_cb(NULL);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "fail");
	dts_pass(__func__, "PASS");	
}

