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
static GThread *event_thread;
	


static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_media_camera_create_negative(void);
static void utc_media_camera_create_positive(void);

static void utc_media_camera_destroy_negative(void);
static void utc_media_camera_destroy_positive(void);

static void utc_media_camera_start_preview_negative(void);
static void utc_media_camera_start_preview_positive(void);

static void utc_media_camera_stop_preview_negative(void);
static void utc_media_camera_stop_preview_positive(void);

static void utc_media_camera_start_capture_negative(void);
static void utc_media_camera_start_capture_positive(void);

static void utc_media_camera_get_state_negative(void);
static void utc_media_camera_get_state_positive(void);

static void utc_media_camera_start_focusing_negative(void);
static void utc_media_camera_start_focusing_positive(void);

static void utc_media_camera_cancel_focusing_negative(void);
static void utc_media_camera_cancel_focusing_positive(void);

struct tet_testlist tet_testlist[] = {

	{utc_media_camera_create_negative , 1},
	{utc_media_camera_create_positive , 2},
	{utc_media_camera_destroy_negative , 3},
	{utc_media_camera_destroy_positive , 4},		
	{utc_media_camera_start_preview_negative , 5},
	{utc_media_camera_start_preview_positive , 6},
	{utc_media_camera_stop_preview_negative , 7},
	{utc_media_camera_stop_preview_positive , 8},
	{utc_media_camera_get_state_negative , 9},	
	{utc_media_camera_get_state_positive , 10},	
	{utc_media_camera_start_focusing_negative ,11},	
	{utc_media_camera_start_focusing_positive , 12},	
	{utc_media_camera_cancel_focusing_negative , 13},	
	{utc_media_camera_cancel_focusing_positive , 14},
	{utc_media_camera_start_capture_negative , 15},
	{utc_media_camera_start_capture_positive , 16},

	{ NULL, 0 },
};


gpointer GmainThread(gpointer data){
	g_mainloop = g_main_loop_new (NULL, 0);
	g_main_loop_run (g_mainloop);
	
	return NULL;
}


static void startup(void)
{
	if( !g_thread_supported() )
	{
		g_thread_init(NULL);
	}
	
	GError *gerr = NULL;
	event_thread = g_thread_create(GmainThread, NULL, 1, &gerr);
}

static void cleanup(void)
{
	g_main_loop_quit (g_mainloop);
	g_thread_join(event_thread);
}

static void utc_media_camera_create_negative(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	
	int ret ;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, NULL);

	MY_ASSERT(__func__ , ret != 0 , "NULL is not allowed");

	dts_pass(__func__, "PASS");	
}
static void utc_media_camera_create_positive(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret ;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	
	MY_ASSERT(__func__ , ret == 0 , "create fail");
	
	camera_destroy(camera);
	dts_pass(__func__, "PASS");	
}

static void utc_media_camera_destroy_negative(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	ret = camera_destroy(NULL);
	MY_ASSERT(__func__ , ret != 0 , "NULL is not allowed");
	dts_pass(__func__, "PASS");	
}
static void utc_media_camera_destroy_positive(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret ;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	MY_ASSERT(__func__ , ret == 0 , "camera create fail");
	ret = camera_destroy(camera);
	MY_ASSERT(__func__ , ret == 0 , "camera destroy is faild");
	
	dts_pass(__func__, "PASS");		
}

static void utc_media_camera_start_preview_negative(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret ;
	ret = camera_start_preview(NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}

static void utc_media_camera_start_preview_positive(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "camera create is faild");
	ret = camera_start_preview(camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "camera start preview is faild");		
	camera_stop_preview(camera);
	camera_destroy(camera);	

	dts_pass(__func__, "PASS");		
}

static void utc_media_camera_stop_preview_negative(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "camera create is faild");

	ret = camera_stop_preview(camera);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "invalid state");

	camera_destroy(camera);	
	
	dts_pass(__func__, "PASS");	
}

static void utc_media_camera_stop_preview_positive(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	ret = camera_start_preview(camera);
	printf("------------ camera_start_preview %x\n", ret);
	MY_ASSERT(__func__ , ret == CAMERA_ERROR_NONE, "camera start preview is faild");
	
	ret = camera_stop_preview(camera);
	MY_ASSERT(__func__, ret ==  CAMERA_ERROR_NONE, "camera_stop_preview is faild");
	
	camera_destroy(camera);	
	dts_pass(__func__, "PASS");
}

static void utc_media_camera_start_capture_negative(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);

	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "camera create is faild");
	ret = camera_start_capture(camera, NULL, NULL, NULL);
	MY_ASSERT(__func__, ret !=CAMERA_ERROR_NONE, "invalid state");	
	ret = camera_destroy(camera);	
	printf("---------------utc_media_camera_start_capture_negative-------------camera_destroy ret = %x\n", ret);
	dts_pass(__func__, "PASS");		
}

static void utc_media_camera_start_capture_positive(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	
	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);
	camera_set_display(camera, CAMERA_DISPLAY_TYPE_X11,0);
	ret = camera_start_preview(camera);
	printf("------------ camera_start_preview %x\n", ret);	
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "camera start preview is faild");
	ret = camera_start_capture(camera, NULL, NULL, NULL);
	MY_ASSERT(__func__, ret==CAMERA_ERROR_NONE, "camera_start_capture is faild");
	sleep(10);
	camera_state_e state;
	camera_get_state(camera, &state);
	printf("----------camera state = %d\n", state);
	ret = camera_start_preview(camera);
	printf(" ---camera_start_preview %x\n", ret);
	ret = camera_stop_preview(camera);	
	printf(" ---camera_stop_preview %x\n", ret);	
	ret = camera_destroy(camera);	
	printf(" ---camera_destroy %x\n", ret);	
	printf("---------------%s- end---------------------\n", __func__);
	
	dts_pass(__func__, "PASS");		
}

static void utc_media_camera_get_state_negative(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);	
	ret = camera_get_state(camera, NULL);
	MY_ASSERT(__func__, ret!=CAMERA_ERROR_NONE, "invalid state");	
	camera_destroy(camera);	
	dts_pass(__func__, "PASS");	
}

static void utc_media_camera_get_state_positive(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	camera_state_e state;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);	
	ret = camera_get_state(camera, &state);
	MY_ASSERT(__func__, ret==CAMERA_ERROR_NONE, "camera_get_state fail");	
	camera_destroy(camera);	
	dts_pass(__func__, "PASS");		
}

static void utc_media_camera_start_focusing_negative(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	camera_create(CAMERA_DEVICE_CAMERA0, &camera);	
	ret = camera_start_focusing(camera, false);
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "invalid state");		
	printf("-------------camera_start_focusing %x\n", ret);
	ret = camera_destroy(camera);
	printf("-------------camera_destroy %x\n", ret);	
	dts_pass(__func__, "PASS");		

}

static void utc_media_camera_start_focusing_positive(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);	
	ret = camera_start_preview(camera);
	printf("-------------------------- camera_start_preview ret = %x\n", ret);
	
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "create camera fail");
	ret = camera_start_focusing(camera, false);
	printf("-------------------------- camera_start_focusing ret = %x\n", ret);	
	MY_ASSERT(__func__, ret==CAMERA_ERROR_NONE, "fail focusing");
	ret = camera_stop_preview(camera);
	printf("--------- camera_stop_preview %x\n", ret);
	ret = camera_destroy(camera);
	printf("--------- camera_destroy %x\n", ret);	
	dts_pass(__func__, "PASS");		
}

static void utc_media_camera_cancel_focusing_negative(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);	
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "create camera fail");
	ret = camera_cancel_focusing(camera);
	printf("---- camera_cancel_focusing %x\n", ret);	
	MY_ASSERT(__func__, ret != CAMERA_ERROR_NONE, "invalid state");
	camera_destroy(camera);
	dts_pass(__func__, "PASS");	
}

static void utc_media_camera_cancel_focusing_positive(void)
{
	fprintf(stderr, "--------------- %s - START --------------\n", __func__);
	int ret;
	camera_h camera;
	ret = camera_create(CAMERA_DEVICE_CAMERA0, &camera);	
	printf("------ 	camera_create %x\n", ret);	
	ret = camera_start_preview(camera);
	printf("------ 	camera_start_preview %x\n", ret);
	ret = camera_start_focusing(camera, false);
	printf("------ 	camera_start_focusing %x\n", ret);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "prepare fail");
	
	ret = camera_cancel_focusing(camera);
	MY_ASSERT(__func__, ret == CAMERA_ERROR_NONE, "cancel focusing fail");
	camera_stop_preview(camera);
	camera_destroy(camera);
	dts_pass(__func__, "PASS");		
}
