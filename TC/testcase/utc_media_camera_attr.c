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

#define MY_ASSERT( fun , test , msg ) \
{\
	if( !test ) \
		dts_fail(fun , msg ); \
}		

static void startup(void);
static void cleanup(void);

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_media_camera_attr_set_preview_fps_negative(void);
static void utc_media_camera_attr_set_preview_fps_positive(void);

static void utc_media_camera_attr_set_image_quality_negative(void);
static void utc_media_camera_attr_set_image_quality_positive(void);

static void utc_media_camera_attr_get_preview_fps_negative(void);
static void utc_media_camera_attr_get_preview_fps_positive(void);

static void utc_media_camera_attr_get_image_quality_negative(void);
static void utc_media_camera_attr_get_image_quality_positive(void);

static void utc_media_camera_attr_set_zoom_negative(void);
static void utc_media_camera_attr_set_zoom_positive(void);

static void utc_media_camera_attr_set_af_mode_negative(void);
static void utc_media_camera_attr_set_af_mode_positive(void);

static void utc_media_camera_attr_set_exposure_mode_negative(void);
static void utc_media_camera_attr_set_exposure_mode_positive(void);

static void utc_media_camera_attr_set_exposure_negative(void);
static void utc_media_camera_attr_set_exposure_positive(void);

static void utc_media_camera_attr_set_iso_negative(void);
static void utc_media_camera_attr_set_iso_positive(void);

static void utc_media_camera_attr_set_brightness_negative(void);
static void utc_media_camera_attr_set_brightness_positive(void);

static void utc_media_camera_attr_set_contrast_negative(void);
static void utc_media_camera_attr_set_contrast_positive(void);

static void utc_media_camera_attr_set_whitebalance_negative(void);
static void utc_media_camera_attr_set_whitebalance_positive(void);

static void utc_media_camera_attr_get_effect_negative(void);
static void utc_media_camera_attr_get_effect_positive(void);

static void utc_media_camera_attr_get_scene_mode_negative(void);
static void utc_media_camera_attr_get_scene_mode_positive(void);

static void utc_media_camera_attr_is_enable_tag_negative(void);
static void utc_media_camera_attr_is_enable_tag_positive(void);

static void utc_media_camera_attr_get_tag_image_description_negative(void);
static void utc_media_camera_attr_get_tag_image_description_positive(void);

static void utc_media_camera_attr_get_tag_orientation_negative(void);
static void utc_media_camera_attr_get_tag_orientation_positive(void);

static void utc_media_camera_attr_get_tag_software_negative(void);
static void utc_media_camera_attr_get_tag_software_positive(void);

static void utc_media_camera_attr_get_geotag_negative(void);
static void utc_media_camera_attr_get_geotag_positive(void);

static void utc_media_camera_attr_get_flash_mode_negative(void);
static void utc_media_camera_attr_get_flash_mode_positive(void);

static void utc_media_camera_attr_foreach_supported_af_mode_negative(void);
static void utc_media_camera_attr_foreach_supported_af_mode_positive(void);

static void utc_media_camera_attr_foreach_supported_exposure_mode_negative(void);
static void utc_media_camera_attr_foreach_supported_exposure_mode_positive(void);

static void utc_media_camera_attr_foreach_supported_iso_negative(void);
static void utc_media_camera_attr_foreach_supported_iso_positive(void);

static void utc_media_camera_attr_foreach_supported_whitebalance_negative(void);
static void utc_media_camera_attr_foreach_supported_whitebalance_positive(void);

static void utc_media_camera_attr_foreach_supported_effect_negative(void);
static void utc_media_camera_attr_foreach_supported_effect_positive(void);

static void utc_media_camera_attr_foreach_supported_scene_mode_negative(void);
static void utc_media_camera_attr_foreach_supported_scene_mode_positive(void);

static void utc_media_camera_attr_foreach_supported_flash_mode_negative(void);
static void utc_media_camera_attr_foreach_supported_flash_mode_positive(void);

static void utc_media_camera_attr_foreach_supported_fps_negative(void);
static void utc_media_camera_attr_foreach_supported_fps_positive(void);

static void utc_media_camera_attr_get_lens_orientation_negative(void);
static void utc_media_camera_attr_get_lens_orientation_positive(void);


struct tet_testlist tet_testlist[] = {
	{utc_media_camera_attr_set_preview_fps_negative , 1},
	{utc_media_camera_attr_set_preview_fps_positive , 2 },
	{utc_media_camera_attr_set_image_quality_negative, 3 },
	{utc_media_camera_attr_set_image_quality_positive, 4},
	{utc_media_camera_attr_get_preview_fps_negative, 5},
	{utc_media_camera_attr_get_preview_fps_positive, 6},
	{utc_media_camera_attr_get_image_quality_negative, 7},
	{utc_media_camera_attr_get_image_quality_positive, 8},
	{utc_media_camera_attr_set_zoom_negative, 9},
	{utc_media_camera_attr_set_zoom_positive, 10},
	{utc_media_camera_attr_set_af_mode_negative, 11},
	{utc_media_camera_attr_set_af_mode_positive, 12},
	{utc_media_camera_attr_set_exposure_mode_negative, 13},
	{utc_media_camera_attr_set_exposure_mode_positive, 14},
	{utc_media_camera_attr_set_exposure_negative, 15},
	{utc_media_camera_attr_set_exposure_positive, 16},
	{utc_media_camera_attr_set_iso_negative, 17},
	{utc_media_camera_attr_set_iso_positive, 18},
	{utc_media_camera_attr_set_brightness_negative, 19},
	{utc_media_camera_attr_set_brightness_positive, 20},
	{utc_media_camera_attr_set_contrast_negative, 21},
	{utc_media_camera_attr_set_contrast_positive, 22},
	{utc_media_camera_attr_set_whitebalance_negative, 23},
	{utc_media_camera_attr_set_whitebalance_positive, 24},
	{utc_media_camera_attr_get_effect_negative, 25},
	{utc_media_camera_attr_get_effect_positive, 26},
	{utc_media_camera_attr_get_scene_mode_negative, 27},
	{utc_media_camera_attr_get_scene_mode_positive, 28},
	{utc_media_camera_attr_is_enable_tag_negative, 29},
	{utc_media_camera_attr_is_enable_tag_positive, 30},
	{utc_media_camera_attr_get_tag_image_description_negative, 31},
	{utc_media_camera_attr_get_tag_image_description_positive, 32},
	{utc_media_camera_attr_get_tag_orientation_negative, 33},
	{utc_media_camera_attr_get_tag_orientation_positive, 34},
	{utc_media_camera_attr_get_tag_software_negative, 35},
	{utc_media_camera_attr_get_tag_software_positive, 36},
	{utc_media_camera_attr_get_geotag_negative, 37},
	{utc_media_camera_attr_get_geotag_positive, 38},
	{utc_media_camera_attr_get_flash_mode_negative, 43},
	{utc_media_camera_attr_get_flash_mode_positive, 44},
	{utc_media_camera_attr_foreach_supported_af_mode_negative, 45},
	{utc_media_camera_attr_foreach_supported_af_mode_positive, 46},
	{utc_media_camera_attr_foreach_supported_exposure_mode_negative, 47},
	{utc_media_camera_attr_foreach_supported_exposure_mode_positive, 48},
	{utc_media_camera_attr_foreach_supported_iso_negative, 49},
	{utc_media_camera_attr_foreach_supported_iso_positive, 50},
	{utc_media_camera_attr_foreach_supported_whitebalance_negative, 51},
	{utc_media_camera_attr_foreach_supported_whitebalance_positive, 52},
	{utc_media_camera_attr_foreach_supported_effect_negative, 53},
	{utc_media_camera_attr_foreach_supported_effect_positive, 54},
	{utc_media_camera_attr_foreach_supported_scene_mode_negative, 55},
	{utc_media_camera_attr_foreach_supported_scene_mode_positive, 56},
	{utc_media_camera_attr_foreach_supported_flash_mode_negative, 57},
	{utc_media_camera_attr_foreach_supported_flash_mode_positive, 58},
	{utc_media_camera_attr_foreach_supported_fps_negative, 59},
	{utc_media_camera_attr_foreach_supported_fps_positive, 60},
	{utc_media_camera_attr_get_lens_orientation_negative, 61},
	{utc_media_camera_attr_get_lens_orientation_positive, 62},	
	{ NULL, 0 },
};

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

static void utc_media_camera_attr_set_preview_fps_negative(void)
{
	fprintf(stderr, "%s test\n", __func__);
	int ret;
	ret = camera_attr_set_preview_fps(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}
static void utc_media_camera_attr_set_preview_fps_positive(void)
{
	int ret;
	ret = camera_attr_set_preview_fps(camera, CAMERA_ATTR_FPS_AUTO);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "CAMERA_ATTR_FPS_AUTO set is faild");
}

static void utc_media_camera_attr_set_image_quality_negative(void)
{
	int ret;
	ret = camera_attr_set_image_quality(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");

}

static void utc_media_camera_attr_set_image_quality_positive(void)
{
	int ret;
	ret = camera_attr_set_image_quality(camera, 100);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "set 100 is faild");

}	

static void utc_media_camera_attr_get_preview_fps_negative(void)
{
	int ret;
	ret = camera_attr_get_preview_fps(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
	
}	

static void utc_media_camera_attr_get_preview_fps_positive(void)
{
	int ret;
	camera_attr_fps_e value;
	ret = camera_attr_get_preview_fps(camera, &value);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "fail get preview fps");
}	

static void utc_media_camera_attr_get_image_quality_negative(void)
{
	int ret;
	ret = camera_attr_get_image_quality(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}
static void utc_media_camera_attr_get_image_quality_positive(void)
{
	int ret;
	int value;
	ret = camera_attr_get_image_quality(camera, &value);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "fail get image quality");
}

static void utc_media_camera_attr_set_zoom_negative(void)
{
	int ret;
	ret = camera_attr_set_zoom(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");

}
static void utc_media_camera_attr_set_zoom_positive(void)
{
	int ret;
	int min,max;
	ret = camera_attr_get_zoom_range(camera, &min, &max);
	
	if( ret != 0 )
		dts_fail(__func__ , "Failed getting zoom range" );
	
	if( max == -1 )
		dts_pass(__func__, "this target is not supported zoom");
	
	ret = camera_attr_set_zoom(camera,min);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "set 10 is failed");
}

static void utc_media_camera_attr_set_af_mode_negative(void)
{
	int ret;
	ret = camera_attr_set_af_mode(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");

}
static void utc_media_camera_attr_set_af_mode_positive(void)
{
	int ret;
	ret = camera_attr_set_af_mode(camera, CAMERA_ATTR_AF_NONE);
	printf("camera_attr_set_af_mode ret=%x\n", ret);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "set CAMERA_ATTR_AF_NONE is faild");

}

static void utc_media_camera_attr_set_exposure_mode_negative(void)
{
	int ret;
	ret = camera_attr_set_exposure_mode(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");
}
static void utc_media_camera_attr_set_exposure_mode_positive(void)
{
	int ret;
	ret = camera_attr_set_exposure_mode(camera, CAMERA_ATTR_EXPOSURE_MODE_ALL);
	printf("camera_attr_set_exposure_mode %x\n", ret);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "set CAMERA_ATTR_EXPOSURE_MODE_ALL is faild");
}

static void utc_media_camera_attr_set_exposure_negative(void)
{
	int ret;
	ret = camera_attr_set_exposure(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");

}
static void utc_media_camera_attr_set_exposure_positive(void)
{
	int ret;
	int min,max;
	ret = camera_attr_get_exposure_range(camera, &min, &max);

	if( ret != 0 )
		dts_fail(__func__ , "Failed getting exposure range" );
	
	if( max == -1 )
		dts_pass(__func__, "this target is not supproted exposure ");
	
	
	ret = camera_attr_set_exposure(camera, min);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "set 1 is faild");

}

static void utc_media_camera_attr_set_iso_negative(void)
{
	int ret;
	ret = camera_attr_set_iso(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");
}
static void utc_media_camera_attr_set_iso_positive(void)
{
	int ret;
	ret = camera_attr_set_iso(camera, CAMERA_ATTR_ISO_AUTO);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "CAMERA_ATTR_ISO_AUTO set is faild");

}

static void utc_media_camera_attr_set_brightness_negative(void)
{
	int ret;
	ret = camera_attr_set_brightness(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");

}
static void utc_media_camera_attr_set_brightness_positive(void)
{
	int ret;
	int min, max;
	ret = camera_attr_get_brightness_range(camera, &min, &max);
	if( ret != 0 )
		dts_fail(__func__ , "Failed getting brightness range" );
	
	if( max == -1 )
		dts_pass(__func__, "this target is not supported brightness ");	
	ret = camera_attr_set_brightness(camera, min+1);
	printf("camera_attr_set_brightness ret = %d, min = %d\n", ret, min);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "set 1 is faild");

}

static void utc_media_camera_attr_set_contrast_negative(void)
{
	int ret;
	ret = camera_attr_set_contrast(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");
}
static void utc_media_camera_attr_set_contrast_positive(void)
{
	int ret;
	int min, max;
	ret = camera_attr_get_contrast_range(camera, &min, &max);
	if( ret != 0 )
		dts_fail(__func__ , "Failed getting contrast range" );
	
	if( max == -1 )
		dts_pass(__func__, "this target is not supported contrast ");	
	ret = camera_attr_set_contrast(camera, min);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "set 1 is faild");
}

static void utc_media_camera_attr_set_whitebalance_negative(void)
{
	int ret;
	ret = camera_attr_set_whitebalance(camera, -1);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "-1 is not allowed");
}
static void utc_media_camera_attr_set_whitebalance_positive(void)
{
	int ret;
	ret = camera_attr_set_whitebalance(camera, CAMERA_ATTR_WHITE_BALANCE_AUTOMATIC);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "set CAMERA_ATTR_WHITE_BALANCE_AUTOMATIC is faild");

}

static void utc_media_camera_attr_get_effect_negative(void)
{
	int ret;
	ret = camera_attr_get_effect(camera, 0);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}
static void utc_media_camera_attr_get_effect_positive(void)
{
	int ret;
	camera_attr_effect_mode_e value;
	ret = camera_attr_get_effect(camera, &value);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "get is faild");

}

static void utc_media_camera_attr_get_scene_mode_negative(void)
{
	int ret;
	ret = camera_attr_get_scene_mode(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}
static void utc_media_camera_attr_get_scene_mode_positive(void)
{
	int ret;
	camera_attr_scene_mode_e value;
	ret = camera_attr_get_scene_mode(camera, &value);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "get is faild");

}

static void utc_media_camera_attr_is_enable_tag_negative(void)
{
	int ret;
	ret = camera_attr_is_enabled_tag(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}
static void utc_media_camera_attr_is_enable_tag_positive(void)
{
	int ret;
	bool enable;
	ret = camera_attr_is_enabled_tag(camera, &enable);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "get enable tag is faild");
}

static void utc_media_camera_attr_get_tag_image_description_negative(void)
{
	int ret;
	ret = camera_attr_get_tag_image_description(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}
static void utc_media_camera_attr_get_tag_image_description_positive(void)
{
	int ret;
	char *buffer;
	ret = camera_attr_get_tag_image_description(camera, &buffer);
	if( ret == 0)
		free(buffer);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "get is faild");
}

static void utc_media_camera_attr_get_tag_orientation_negative(void)
{
	int ret;
	ret = camera_attr_get_tag_orientation(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}
static void utc_media_camera_attr_get_tag_orientation_positive(void)
{
	int ret;
	camera_attr_tag_orientation_e value;
	ret = camera_attr_get_tag_orientation(camera, &value);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "get is faild");
}

static void utc_media_camera_attr_get_tag_software_negative(void)
{
	int ret;
	ret = camera_attr_get_tag_software(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}
static void utc_media_camera_attr_get_tag_software_positive(void)
{
	int ret;
	char *buffer;
	ret = camera_attr_get_tag_software(camera, &buffer);
	if(ret == 0 )
		free(buffer);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "get is faild");
}

static void utc_media_camera_attr_get_geotag_negative(void)
{
	int ret;
	ret = camera_attr_get_geotag(camera, NULL, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}
static void utc_media_camera_attr_get_geotag_positive(void)
{
	int ret;
	double value1,value2, value3;
	ret = camera_attr_get_geotag(camera, &value1, &value2, &value3);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "get is faild");

}


static void utc_media_camera_attr_get_flash_mode_negative(void)
{
	int ret;
	ret = camera_attr_get_flash_mode(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}
static void utc_media_camera_attr_get_flash_mode_positive(void)
{
	int ret;
	camera_attr_flash_mode_e value;
	ret = camera_attr_get_flash_mode(camera, &value);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "get is faild");

}

static void utc_media_camera_attr_foreach_supported_af_mode_negative(void)
{
	int ret;
	ret = camera_attr_foreach_supported_af_mode(camera, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}

bool _cb_af_mode(camera_attr_af_mode_e mode, void *user_data)
{
	return false;
}

static void utc_media_camera_attr_foreach_supported_af_mode_positive(void)
{
	int ret;
	ret = camera_attr_foreach_supported_af_mode(camera, _cb_af_mode, NULL);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "faild");
}

static void utc_media_camera_attr_foreach_supported_exposure_mode_negative(void)
{
	int ret;
	ret = camera_attr_foreach_supported_exposure_mode(camera, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}

bool _cb_exposure_mode(camera_attr_exposure_mode_e mode , void *user_data)
{
	return false;
}

static void utc_media_camera_attr_foreach_supported_exposure_mode_positive(void)
{
	int ret;
	ret = camera_attr_foreach_supported_exposure_mode(camera, _cb_exposure_mode, NULL);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "camera_attr_foreach_supported_exposure_mode is failed");

}

static void utc_media_camera_attr_foreach_supported_iso_negative(void)
{
	int ret;
	ret = camera_attr_foreach_supported_iso(camera, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}

bool _cb_iso(camera_attr_iso_e iso , void *user_data)
{
	return false;
}

static void utc_media_camera_attr_foreach_supported_iso_positive(void)
{
	int ret;
	ret = camera_attr_foreach_supported_iso(camera, _cb_iso, NULL);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "camera_attr_foreach_supported_exposure_mode is faild");
}

static void utc_media_camera_attr_foreach_supported_whitebalance_negative(void)
{
	int ret;
	ret = camera_attr_foreach_supported_whitebalance(camera, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
}

bool _cb_whitebalance_cb(camera_attr_whitebalance_e wb , void *user_data)
{
	return false;
}

static void utc_media_camera_attr_foreach_supported_whitebalance_positive(void)
{
	int ret;
	ret = camera_attr_foreach_supported_whitebalance(camera, _cb_whitebalance_cb, NULL);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "camera_attr_foreach_supported_whitebalance is faild");

}

static void utc_media_camera_attr_foreach_supported_effect_negative(void)
{
	int ret;
	ret = camera_attr_foreach_supported_effect(camera, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}

bool _cb_effect_cb(camera_attr_effect_mode_e effect , void *user_data)
{
	return false;
}

static void utc_media_camera_attr_foreach_supported_effect_positive(void)
{
	int ret;
	ret = camera_attr_foreach_supported_effect(camera, _cb_effect_cb, NULL);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "camera_attr_foreach_supported_effect is faild");

}

static void utc_media_camera_attr_foreach_supported_scene_mode_negative(void)
{
	int ret;
	ret = camera_attr_foreach_supported_scene_mode(camera, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}

bool _cb_scene_mode_cb(camera_attr_scene_mode_e mode , void *user_data)
{
	return false;
}

static void utc_media_camera_attr_foreach_supported_scene_mode_positive(void)
{
	int ret;
	ret = camera_attr_foreach_supported_scene_mode(camera, _cb_scene_mode_cb, NULL);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "camera_attr_foreach_supported_scene_mode is faild");

}

static void utc_media_camera_attr_foreach_supported_flash_mode_negative(void)
{
	int ret;
	ret = camera_attr_foreach_supported_flash_mode(camera, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}

bool _cb_flash_mode_cb(camera_attr_flash_mode_e mode,  void *user_data)
{
	return false;
}

static void utc_media_camera_attr_foreach_supported_flash_mode_positive(void)
{
	int ret;
	ret = camera_attr_foreach_supported_flash_mode(camera, _cb_flash_mode_cb, NULL);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "camera_attr_foreach_supported_flash_mode is faild");

}

static void utc_media_camera_attr_foreach_supported_fps_negative(void)
{
	int ret;
	ret = camera_attr_foreach_supported_fps(camera, NULL, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");

}

bool _cb_fps_cb(camera_attr_fps_e fps, void *user_data)
{
	return false;
}

static void utc_media_camera_attr_foreach_supported_fps_positive(void)
{
	int ret;
	ret = camera_attr_foreach_supported_fps(camera, _cb_fps_cb, NULL);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "camera_attr_foreach_supported_fps is faild");

}


static void utc_media_camera_attr_get_lens_orientation_negative(void)
{
	int ret;
	ret = camera_attr_get_lens_orientation(camera, NULL);
	dts_check_ne(__func__, ret, CAMERA_ERROR_NONE, "NULL is not allowed");
	
}
static void utc_media_camera_attr_get_lens_orientation_positive(void)
{
	int ret;
	int rotate;
	ret = camera_attr_get_lens_orientation(camera, &rotate);
	dts_check_eq(__func__, ret, CAMERA_ERROR_NONE, "camera_attr_get_lens_orientation is faild");
}

