/*
* camera_testsuite
*
* Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
*
* Contact: Jeongmo Yang <jm80.yang@samsung.com>
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
* See the License f(r the specific language governing permissions and
* limitations under the License.
*
*/

/* ===========================================================================================
EDIT HISTORY FOR MODULE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.
when        who                     what, where, why
---------   --------------------    --------------------------------------------
*/


/*=======================================================================================
|  INCLUDE FILES                                                                        |
=======================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gst/gst.h>
#include <sys/time.h>
#include <dlog.h>
#include <camera.h>
#include <Ecore.h>
#include <Elementary.h>

/*-----------------------------------------------------------------------
|    GLOBAL VARIABLE DEFINITIONS:                                       |
-----------------------------------------------------------------------*/
#define EXPORT_API __attribute__((__visibility__("default")))

#define PACKAGE "camera_testsuite"

Evas_Object* img;
Evas_Object* eo;
GMainLoop *g_loop;
GIOChannel *stdin_channel;
camera_device_e cam_info;
GstCaps *filtercaps;
int resolution_set;
int g_current_state;
int src_w, src_h;
int isMultishot;
int camera_state;
int camera_print_state;
int multishot_num;
static GTimer *timer = NULL;

GTimeVal previous_time;
GTimeVal current_time;
GTimeVal res;

/*-----------------------------------------------------------------------
|    GLOBAL CONSTANT DEFINITIONS:                                       |
-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------
|    IMPORTED VARIABLE DECLARATIONS:                                    |
-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------
|    IMPORTED FUNCTION DECLARATIONS:                                    |
-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------
|    LOCAL #defines:                                                    |
-----------------------------------------------------------------------*/
#define test_ffmux_mp4


#define DISPLAY_X_0                         0                       //for direct FB
#define DISPLAY_Y_0                         0                       //for direct FB

#define DISPLAY_W_320                       320                 //for direct FB
#define DISPLAY_H_240                       240                     //for direct FB


#define IMAGE_ENC_QUALITY               85    // quality of jpeg
#define IMAGE_CAPTURE_COUNT_STILL       1     // the number of still-shot
#define IMAGE_CAPTURE_COUNT_MULTI       3     // default the number of multi-shot
#define IMAGE_CAPTURE_COUNT_INTERVAL    100   // mili seconds

#define MAX_FILE_SIZE_FOR_MMS           (250 * 1024)

#define EXT_JPEG                        "jpg"
#define EXT_MP4                         "mp4"
#define EXT_3GP                         "3gp"
#define EXT_AMR                         "amr"
#define EXT_MKV                         "mkv"

#define TARGET_FILENAME_PATH            "/opt/usr/media/"
#define STILL_CAPTURE_FILE_PATH_NAME    TARGET_FILENAME_PATH"StillshotCapture"
#define MULTI_CAPTURE_FILE_PATH_NAME    TARGET_FILENAME_PATH"MultishotCapture"
#define IMAGE_CAPTURE_THUMBNAIL_PATH    TARGET_FILENAME_PATH"thumbnail.jpg"
#define IMAGE_CAPTURE_SCREENNAIL_PATH   TARGET_FILENAME_PATH"screennail.yuv"
#define IMAGE_CAPTURE_EXIF_PATH         TARGET_FILENAME_PATH"exif.raw"
#define CAPTURE_FILENAME_LEN            256
#define MAX_STILLSHOT_CAPTURE_RESOLUTION_SUPPORTED 2

#define DEFAULT_CAM_DEVICE              MM_VIDEO_DEVICE_CAMERA1


#define CHECK_MM_ERROR(expr) \
    do {\
    int ret = 0; \
    ret = expr; \
    if (ret != 0) {\
    printf("[%s:%d] error code : %x \n", __func__, __LINE__, ret); \
    return; \
    }\
    } while(0)


#ifndef SAFE_FREE
#define SAFE_FREE(x)       if(x) {g_free(x); x = NULL;}
#endif

#define SENSOR_WHITEBALANCE_NUM     10
#define SENSOR_COLOR_TONE_NUM       31
#define SENSOR_FLIP_NUM         3
#define SENSOR_PROGRAM_MODE_NUM     15
#define SENSOR_FOCUS_NUM        6
#define SENSOR_INPUT_ROTATION       4
#define SENSOR_AF_SCAN_NUM      4
#define SENSOR_ISO_NUM          8
#define SENSOR_EXPOSURE_NUM     9
#define SENSOR_IMAGE_FORMAT     9


/*-----------------------------------------------------------------------
|    LOCAL CONSTANT DEFINITIONS:                                        |
-----------------------------------------------------------------------*/
enum
{
    MODE_VIDEO_CAPTURE, /* recording and image capture mode */
    MODE_AUDIO,     /* audio recording*/
    MODE_NUM,
};

enum
{
    MENU_STATE_MAIN,
    MENU_STATE_SETTING,
    MENU_STATE_NUM,
};

/*-----------------------------------------------------------------------
|    LOCAL DATA TYPE DEFINITIONS:                   |
-----------------------------------------------------------------------*/
typedef struct _cam_handle
{
    camera_h camera;
    int mode;                       /* image(capture)/video(recording) mode */
    int isMultishot;               /* flag for multishot mode */
    int stillshot_count;            /* total stillshot count */
    int multishot_count;            /* total multishot count */
    const char *stillshot_filename;       /* stored filename of  stillshot  */
    const char *multishot_filename;       /* stored filename of  multishot  */
    int menu_state;
    int isMute;
    unsigned long long elapsed_time;
} cam_handle_t;

typedef struct{
    int expected_width;
    int expected_height;
    int ispass;
} preview_test_data;

typedef struct{
    int width[100];
    int height[100];
    int count;
} resolution_stack;

typedef struct{
    camera_attr_af_mode_e mode;
    int count;
} af_stack;

typedef struct{
    int expected_mode;
    int ispass;
} af_test_data;

typedef struct{
    camera_attr_exposure_mode_e mode;
    int count;
} exposure_stack;

typedef struct{
    camera_attr_iso_e mode;
    int count;
} iso_stack;

typedef struct{
    camera_rotation_e mode;
    int count;
} camera_rotation_stack;


/*---------------------------------------------------------------------------
|    LOCAL VARIABLE DEFINITIONS:                                            |
---------------------------------------------------------------------------*/
static cam_handle_t *hcamcorder;

const char *wb[SENSOR_WHITEBALANCE_NUM]={
    "None",
    "Auto",
    "Daylight",
    "Cloudy",
    "Fluoroscent",
    "Incandescent",
    "Shade",
    "Horizon",
    "Flash",
    "Custom",
};

const char *ct[SENSOR_COLOR_TONE_NUM] = {
    "NONE",
    "MONO",
    "SEPIA",
    "NEGATIVE",
    "BLUE",
    "GREEN",
    "AQUA",
    "VIOLET",
    "ORANGE",
    "GRAY",
    "RED",
    "ANTIQUE",
    "WARM",
    "PINK",
    "YELLOW",
    "PURPLE",
    "EMBOSS",
    "OUTLINE",
    "SOLARIZATION",
    "SKETCH",
    "WASHED",
    "VINTAGE_WARM",
    "VINTAGE_COLD",
    "POSTERIZATION",
    "CARTOON",
    "SELECTVE_COLOR_RED",
    "SELECTVE_COLOR_GREEN",
    "SELECTVE_COLOR_BLUE",
    "SELECTVE_COLOR_YELLOW",
    "SELECTVE_COLOR_RED_YELLOW",
    "GRAPHICS"
};

const char *flip[SENSOR_FLIP_NUM] = {
    "Horizontal",
    "Vertical",
    "Not flipped",
};

const char *program_mode[SENSOR_PROGRAM_MODE_NUM] = {
    "NORMAL",
    "PORTRAIT",
    "LANDSCAPE",
    "SPORTS",
    "PARTY_N_INDOOR",
    "BEACH_N_INDOOR",
    "SUNSET",
    "DUSK_N_DAWN",
    "FALL_COLOR",
    "NIGHT_SCENE",
    "FIREWORK",
    "TEXT",
    "SHOW_WINDOW",
    "CANDLE_LIGHT",
    "BACKLIGHT",
};

const char *focus_mode[SENSOR_FOCUS_NUM] = {
    "None",
    "Pan",
    "Auto",
    "Manual",
    "Touch Auto",
    "Continuous Auto",
};

const char *camera_rotation[SENSOR_INPUT_ROTATION] = {
    "None",
    "90",
    "180",
    "270",
};

const char *iso_mode[SENSOR_ISO_NUM] = {
    "ISO Auto",
    "ISO 50",
    "ISO 100",
    "ISO 200",
    "ISO 400",
    "ISO 800",
    "ISO 1600",
    "ISO 3200",
};

const char *exposure_mode[SENSOR_EXPOSURE_NUM] = {
    "AE off",
    "AE all mode",
    "AE center mode",
    "AE spot 1 mode",
    "AE custom mode",
};

const char *image_fmt[SENSOR_IMAGE_FORMAT] = {
    "NV12",
    "NV12T",
    "NV16",
    "NV21",
    "YUYV",
    "UYVY",
    "422P",
    "I420",
    "YV12",
};

const char *face_zoom_mode[] = {
    "Face Zoom OFF",
    "Face Zoom ON",
};

const char *display_mode[] = {
    "Letter Box mode",
    "Original Size mode",
    "Full Screen mode",
    "Cropped Full Screen mode",
    "ROI mode",
};

const char *capture_sound[] = {
    "Default",
    "Extra 01",
    "Extra 02",
};

const char *rotate_mode[] = {
    "0",
    "90",
    "180",
    "270",
};

const char* strobe_mode[] = {
    "Strobe OFF",
    "Strobe ON",
    "Strobe Auto",
    "Strobe RedEyeReduction",
    "Strobe SlowSync",
    "Strobe FrontCurtain",
    "Strobe RearCurtain",
    "Strobe Permanent",
};

const char *detection_mode[2] = {
    "Face Detection OFF",
    "Face Detection ON",
};

const char *wdr_mode[] = {
    "WDR OFF",
    "WDR ON",
    "WDR AUTO",
};

const char *af_scan[SENSOR_AF_SCAN_NUM] = {
    "None",
    "Normal",
    "Macro mode",
    "Full mode",
};

const char *hdr_mode[] = {
    "HDR OFF",
    "HDR ON",
    "HDR ON and Original",
};

const char *ahs_mode[] = {
    "Anti-handshake OFF",
    "Anti-handshake ON",
    "Anti-handshake AUTO",
    "Anti-handshake MOVIE",
};

const char *vs_mode[] = {
    "Video-stabilization OFF",
    "Video-stabilization ON",
};

const char *visible_mode[] = {
    "Display OFF",
    "Display ON",
};


/*---------------------------------------------------------------------------
|    LOCAL FUNCTION PROTOTYPES:                                             |
---------------------------------------------------------------------------*/
static void print_menu();
static gboolean cmd_input(GIOChannel *channel);
static gboolean mode_change();
int camcordertest_set_attr_int(const char* attr_subcategory, int value);
bool preview_resolution_cb(int width, int height, void *user_data);


static inline void flush_stdin()
{
    int ch;
    while ((ch=getchar()) != EOF && ch != '\n');
}

void _camera_state_changed_cb(camera_state_e previous, camera_state_e current,bool by_policy, void *user_data){
    printf("camera state changed %d -> %d\n", previous , current);
}

bool preview_resolution_cb(int width, int height, void *user_data)
{
    resolution_stack *data = (resolution_stack*)user_data;

    if (data == NULL) {
        printf("NULL data\n");
        return false;
    }

    data->width[data->count] = width;
    data->height[data->count] = height;

    printf("%d. %dx%d\n", data->count, width, height);

    data->count++;

    return true;
}

bool capture_resolution_test_cb(int width, int height, void *user_data)
{
    resolution_stack *data = (resolution_stack*)user_data;

    if (data == NULL) {
        printf("NULL data\n");
        return false;
    }

    data->width[data->count] = width;
    data->height[data->count] = height;

    printf("%d. %dx%d\n", data->count, width, height);

    data->count++;

    return true;
}

bool af_mode_foreach_cb(camera_attr_iso_e mode , void *user_data)
{
    printf("%d.%s\n", mode,af_scan[mode]);
    return true;
}

bool exposure_mode_cb(camera_attr_af_mode_e mode , void *user_data)
{
    exposure_stack *data = (exposure_stack*)user_data;

    if (data == NULL) {
        printf("NULL data\n");
        return false;
    }

    data->mode = mode;
    data->count++;

    printf("%d.%s\n", mode,exposure_mode[mode]);
    return true;
}

bool iso_mode_cb(camera_attr_iso_e mode , void *user_data)
{
    printf("%d.%s\n", mode,iso_mode[mode]);
    return true;
}

bool camera_rotation_cb(camera_rotation_e mode , void *user_data)
{
    camera_rotation_stack *data = (camera_rotation_stack*)user_data;

    if (data == NULL) {
        printf("NULL data\n");
        return false;
    }

    data->mode = mode;
    data->count++;

    printf("%d.%s\n", mode,camera_rotation[mode]);
    return true;
}

bool preview_format_cb(camera_pixel_format_e mode , void *user_data)
{
    printf("%d.%s\n", mode,image_fmt[mode]);
    return true;
}

bool white_balance_cb(camera_attr_whitebalance_e mode , void *user_data)
{
    printf("%d.%s\n", mode,wb[mode]);
    return true;
}

bool colortone_cb(camera_attr_effect_mode_e mode , void *user_data)
{
    printf("%d.%s\n", mode,ct[mode]);
    return true;
}

bool program_mode_cb(camera_attr_scene_mode_e mode ,void *user_data)
{
    printf("%d.%s\n", mode,program_mode[mode]);
    return true;
}

bool strobe_mode_cb(camera_attr_flash_mode_e mode ,void *user_data)
{
    printf("%d.%s\n", mode,strobe_mode[mode]);
    return true;
}

void _face_detected(camera_detected_face_s *faces, int count, void *user_data){
    printf("face detected!!\n");
    int i;
    for (i = 0 ; i < count ; i++){
        printf("%d) - %dx%d\n", faces[i].id, faces[i].x, faces[i].y);
    }
}


void
    cam_utils_convert_YUYV_to_UYVY(unsigned char* dst, unsigned char* src, gint length)
{
    int i = 0;

    //memset dst
    memset(dst, 0x00, length);
    memcpy(dst, src+1, length-1);

    for (i = 0 ; i < length; i++)
    {
        if ( !(i % 2) )
        {
            dst[i+1] = src[i];
        }
    }
}


static void _file_write(char *path, void *data, int size)
{
    FILE *fp = NULL;

    if (!path || !data || size <= 0) {
        printf("ERROR %p %p %d\n", path, data, size);
        return;
    }

    fp = fopen(path, "w");
    if (fp == NULL) {
        printf("open error! [%s], errno %d\n", path, errno);
        return;
    } else {
        printf("open success [%s]\n", path);
        if (fwrite(data, size, 1, fp) != 1) {
            printf("write error! errno %d\n", errno);
        } else {
            printf("write success [%s]\n", path);
        }

        fclose(fp);
        fp = NULL;
    }
}

void capturing_cb(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data)
{
    char m_filename[CAPTURE_FILENAME_LEN];

    if (hcamcorder->isMultishot)
        snprintf(m_filename, CAPTURE_FILENAME_LEN, "/opt/usr/media/Stillshot%03d.jpg", hcamcorder->multishot_count++);
    else
        snprintf(m_filename, CAPTURE_FILENAME_LEN, "/opt/usr/media/Multishot%03d.jpg", hcamcorder->stillshot_count++);

    /* main image */
    if (image) {
        _file_write(m_filename, image->data, image->size);
    }

    return;
}

void capture_completed_cb(void *user_data)
{
    camera_start_preview(hcamcorder->camera);

    print_menu();

    return;
}

static void print_menu()
{
    switch (hcamcorder->menu_state)
    {
    case MENU_STATE_MAIN:
        if (hcamcorder->mode == MODE_VIDEO_CAPTURE) {
            g_print("\n\t=======================================\n");
            if (cam_info == CAMERA_DEVICE_CAMERA1 )
                g_print("\t   Video Capture (Front camera)\n");
            else if (cam_info == CAMERA_DEVICE_CAMERA0)
                g_print("\t   Video Capture (Rear camera)\n");
            g_print("\t=======================================\n");

            g_print("\t   '1' Stillshot test\n");
            g_print("\t   '2' Multishot test\n");
            g_print("\t   '3' Setting\n");
            g_print("\t   'b' back\n");
            g_print("\t=======================================\n");
        }
        break;

    case MENU_STATE_SETTING:
        g_print("\n\t=======================================\n");
        g_print("\t   Video Capture > Setting\n");
        g_print("\t=======================================\n");
        g_print("\t  >>>>>>>>>>>>>>>>>>>>>>>>>>>> [Camera]  \n");
        g_print("\t     '0' Preview resolution \n");
        g_print("\t     '1' Capture resolution \n");
        g_print("\t     '2' Digital zoom level \n");
        g_print("\t     '3' AF mode \n");
        g_print("\t     '4' AF scan range \n");
        g_print("\t     '5' Exposure mode \n");
        g_print("\t     '6' Exposure value \n");
        g_print("\t     '7' F number \n");
        g_print("\t     'i' ISO \n");
        g_print("\t     'r' Rotate camera input \n");
        g_print("\t     'f' Flip camera input \n");
        g_print("\t     'j' Jpeg quality \n");
        g_print("\t     'p' Picture format \n");
        g_print("\t     'E' EXIF orientation \n");
        g_print("\t  >>>>>>>>>>>>>>>>>>>> [Display/Filter]\n");
        g_print("\t     'v' Visible \n");
        g_print("\t     'o' Output mode \n");
        g_print("\t     'y' Rotate display \n");
        g_print("\t     'Y' Flip display \n");
        g_print("\t     'g' Brightness \n");
        g_print("\t     'c' Contrast \n");
        g_print("\t     'w' White balance \n");
        g_print("\t     't' Color tone \n");
        g_print("\t     'd' WDR \n");
        g_print("\t     'e' EV program mode \n");
        g_print("\t  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>> [etc.]\n");
        g_print("\t     'z' Strobe (Flash) \n");
        g_print("\t     'x' Capture mode (Still/Multishot/HDR)\n");
        g_print("\t     'l' Face detection \n");
        g_print("\t     'k' Anti-handshake \n");
        g_print("\t     'K' Video-stabilization \n");
        g_print("\t     'u' Touch AF area \n");
        g_print("\t     'b' back\n");
        g_print("\t=======================================\n");
        break;

    default:
        LOGE("unknow menu state !!\n");
        break;
    }

    return;
}


static void main_menu(gchar buf)
{
    int err = 0;
    camera_state_e capi_state;
    if (hcamcorder->mode == MODE_VIDEO_CAPTURE) {
        switch (buf) {
        case '1' : //Capture
            camera_get_state(hcamcorder->camera, &capi_state);
            camera_attr_set_image_quality(hcamcorder->camera, 100);
            camera_set_capture_format(hcamcorder->camera, CAMERA_PIXEL_FORMAT_JPEG);
            camera_start_capture(hcamcorder->camera, capturing_cb, capture_completed_cb, hcamcorder);
            break;

        case '2' : //multishot Capture
            g_print("multishot capture");
            hcamcorder->isMultishot = TRUE;
            int interval = 0 ,count=0;
            flush_stdin();
            printf("\ninput interval(ms) \n");
            err = scanf("%d", &interval);
            flush_stdin();
            printf("\ninput count(ms) \n");
            err = scanf("%d", &count);
            camera_attr_set_image_quality(hcamcorder->camera, 100);
            camera_set_capture_format(hcamcorder->camera, CAMERA_PIXEL_FORMAT_JPEG);
            camera_start_continuous_capture(hcamcorder->camera,count,interval ,capturing_cb, NULL, NULL);
            sleep(3);
            camera_start_preview(hcamcorder->camera);
            break;
        case '3' : // Setting
            hcamcorder->menu_state = MENU_STATE_SETTING;
            break;

        case 'b' : // back
            camera_stop_preview(hcamcorder->camera);
            camera_destroy(hcamcorder->camera);
            hcamcorder->camera = NULL;
            hcamcorder->menu_state = MENU_STATE_MAIN;
            mode_change();
            break;

        default:
            g_print("\t Invalid input \n");
            break;
        }
    } else {
        g_print("\t Invalid mode, back to upper menu \n");
        hcamcorder->menu_state = MENU_STATE_MAIN;
        mode_change();
    }
}


static void setting_menu(gchar buf)
{
    gboolean bret = FALSE;
    int idx = 0;
    int min = 0;
    int max = 0;
    int i=0;
    int value = 0;
    int err = 0;
    int x = 0, y = 0, width = 0, height = 0;

    if (hcamcorder->mode == MODE_VIDEO_CAPTURE) {
        switch (buf) {
            /* Camera setting */
        case '0':  // Setting > Preview Resolution setting
            g_print("*Select the preview resolution!\n");
            resolution_stack resolution_list;
            resolution_list.count = 0;
            camera_foreach_supported_preview_resolution(hcamcorder->camera, preview_resolution_cb, &resolution_list);
            flush_stdin();
            err = scanf("%d",&idx);
            int result = 0;
            preview_test_data data;
            data.ispass = false;

            if (resolution_list.count > idx && idx >= 0) {

                data.expected_width = resolution_list.width[idx];
                data.expected_height = resolution_list.height[idx];

                printf("-----------------PREVIEW RESOLUTION (%dx%d)---------------------\n",data.expected_width  ,data.expected_height);

                result = camera_set_preview_resolution(hcamcorder->camera,data.expected_width ,data.expected_height);
            } else {
                printf("invalid input %d\n", idx);
                result = -1;
            }

            resolution_list.count = 0;

            if (result != 0)
                printf("FAIL\n");
            else
                printf("PASS\n");

            break;
        case '1':
            // Setting > Capture Resolution setting
            g_print("*Select the preview resolution!\n");

            printf("-----------------CAPTURE RESOLUTION TEST: ---------------------\n");

            resolution_list.count = 0;
            camera_foreach_supported_capture_resolution(hcamcorder->camera, capture_resolution_test_cb, &resolution_list);
            flush_stdin();
            err = scanf("%d",&idx);
            data.ispass = false;

            if (resolution_list.count > idx && idx >= 0) {
                data.expected_width = resolution_list.width[idx];
                data.expected_height = resolution_list.height[idx];

                result = camera_set_capture_resolution(hcamcorder->camera, data.expected_width, data.expected_height);
                printf("camera_set_capture_resolution with width =%d, height=%d ret=0x%x\n", data.expected_width, data.expected_height, result);
            } else {
                printf("invalid input %d\n", idx);
                result = -1;
            }

            resolution_list.count = 0;

            if (result != 0)
                printf("FAIL\n");
            else
                printf("PASS\n");

            break;
        case '2' : // Setting > Digital zoom level
            g_print("*Digital zoom level  !\n");

            camera_attr_get_zoom_range(hcamcorder->camera,&min,&max);
            if (min >= max) {
                g_print("Not supported !! \n");
            } else {
                flush_stdin();
                g_print("\n Select  Digital zoom level min %d - max %d\n",min,max);
                err = scanf("%d",&idx);
                bret = camera_attr_set_zoom(hcamcorder->camera,idx);
            }
            break;
        case '3' : // Setting > AF mode

            g_print("*AF mode !\n");
            g_print("\t1. AF Start !\n");
            g_print("\t2. AF Stop !\n\n");

            flush_stdin();
            err = scanf("%d", &idx);
            switch (idx)
            {
            case 1:
                camera_start_focusing(hcamcorder->camera,0);
                break;
            case 2:
                camera_cancel_focusing(hcamcorder->camera);
                break;
            default:
                g_print("Wrong Input[%d] !! \n", idx);
                break;
            }
            break;

        case '4' : // Setting > AF scan range
            g_print("*AF scan range !\n");

            af_stack af_mode_list;
            af_mode_list.count = 0;
            camera_attr_foreach_supported_af_mode(hcamcorder->camera, (camera_attr_supported_af_mode_cb)af_mode_foreach_cb , NULL);

            af_test_data data1;
            data1.ispass = false;
            data1.expected_mode = af_mode_list.mode;
            flush_stdin();
            err = scanf("%d", &idx);
            bret=camera_attr_set_af_mode(hcamcorder->camera,idx);
            break;

        case '5' : // Setting > Exposure mode
            g_print("* Exposure mode!\n");
            camera_attr_foreach_supported_exposure_mode(hcamcorder->camera,(camera_attr_supported_exposure_mode_cb)exposure_mode_cb,NULL);

            flush_stdin();

            g_print("\n Select  Exposure mode \n");
            err = scanf("%d",&idx);
            bret = camera_attr_set_exposure_mode(hcamcorder->camera, idx);
            break;

        case '6' :  // Setting > Exposure value
            camera_attr_get_exposure_range(hcamcorder->camera,&min,&max);
            if (min >= max) {
                g_print("Not supported !! \n");
            }
            else {

                flush_stdin();
                g_print("\n Select  Exposure mode min%d -max %d\n",min,max);
                err = scanf("%d",&idx);
                bret = camera_attr_set_exposure(hcamcorder->camera, idx);
            }
            break;

        case '7' : // Setting > F number
            g_print("Not supported !! \n");
            break;

        case 'i' : // Setting > ISO
            g_print("*ISO !\n");
            camera_attr_foreach_supported_iso(hcamcorder->camera,iso_mode_cb, NULL);

            flush_stdin();

            err = scanf("%d",&idx);
            bret =  camera_attr_set_iso(hcamcorder->camera, idx);
            break;

        case 'r' : // Setting > Rotate camera input when recording
            g_print("*Rotate camera input\n");
            camera_attr_foreach_supported_stream_rotation(hcamcorder->camera,camera_rotation_cb,NULL);

            flush_stdin();
            err = scanf("%d",&idx);
            CHECK_MM_ERROR(camera_stop_preview(hcamcorder->camera));
            bret = camera_attr_set_stream_rotation(hcamcorder->camera, idx);
            CHECK_MM_ERROR(camera_start_preview(hcamcorder->camera));
            break;

        case 'f' : // Setting > Flip camera input
            flush_stdin();
            g_print("*Flip camera input\n");
            g_print(" 0. Flip NONE\n");
            g_print(" 1. Flip HORIZONTAL\n");
            g_print(" 2. Flip VERTICAL\n");
            g_print(" 3. Flip BOTH\n");

            err = scanf("%d", &idx);

            CHECK_MM_ERROR(camera_stop_preview(hcamcorder->camera));
            camera_attr_set_stream_flip(hcamcorder->camera,idx);
            CHECK_MM_ERROR(camera_start_preview(hcamcorder->camera));
            break;

        case 'j' : // Setting > Jpeg quality
            g_print("*Jpeg quality !\n");

            flush_stdin();
            g_print("\n Select  Jpeg quality \n");
            err = scanf("%d",&idx);
            bret = camera_attr_set_image_quality(hcamcorder->camera,idx);

            break;

        case 'p' : // Setting > Picture format
            g_print("* Picture format!\n");
            camera_foreach_supported_preview_format(hcamcorder->camera,preview_format_cb,NULL);

            flush_stdin();
            err = scanf("%d",&idx);
            bret = camera_set_preview_format(hcamcorder->camera,idx);
            CHECK_MM_ERROR(camera_stop_preview(hcamcorder->camera));
            CHECK_MM_ERROR(camera_start_preview(hcamcorder->camera));

            break;

        case 'E' : // Setting > EXIF orientation
            g_print("* EXIF Orientation\n");

            g_print("\t 1. TOP_LEFT\n");
            g_print("\t 2. TOP_RIGHT(flipped)\n");
            g_print("\t 3. BOTTOM_RIGHT\n");
            g_print("\t 4. BOTTOM_LEFT(flipped)\n");
            g_print("\t 5. LEFT_TOP(flipped)\n");
            g_print("\t 6. RIGHT_TOP\n");
            g_print("\t 7. RIGHT_BOTTOM(flipped)\n");
            g_print("\t 8. LEFT_BOTTOM\n");

            flush_stdin();
            err = scanf("%d", &idx);

            if (idx < 1 || idx > 8) {
                g_print("Wrong INPUT[%d]!! \n", idx);
            } else {
                camera_attr_set_tag_orientation(hcamcorder->camera,idx);
            }

            break;

            /* Display / Filter setting */
        case 'v' : // Display visible
            g_print("* Display visible setting !\n");

            g_print("\n Select Display visible \n");
            flush_stdin();
            for ( i = 0; i < 2; i++) {
                g_print("\t %d. %s\n", i+1, visible_mode[i]);
            }
            err = scanf("%d",&value);
            bret = camera_set_display_visible(hcamcorder->camera,idx-1);

            break;


        case 'o' : //  Setting > Display MODe
            g_print("* Display mode!\n");

            flush_stdin();
            for (i = 0 ; i < 5  ; i++) {
                g_print( "%d. %s\n", i, display_mode[i] );
            }
            err = scanf("%d",&idx);
            bret =  camera_set_display_mode(hcamcorder->camera,idx);
            break;

        case 'y' : // Setting > Rotate Display

            flush_stdin();
            g_print("\n Select Rotate mode\n");
            g_print("\t0. 0\n\t1. 90\n\t2. 180\n\t3. 270\n\n");
            err = scanf("%d",&idx);
            CHECK_MM_ERROR(camera_stop_preview(hcamcorder->camera));
            bret = camera_set_display_rotation(hcamcorder->camera,idx);
            CHECK_MM_ERROR(camera_start_preview(hcamcorder->camera));
            break;

        case 'Y' : // Setting > Flip Display
            flush_stdin();
            g_print("\n Select Rotate mode\n");
            g_print("\t0. NONE\n\t1. HORIZONTAL\n\t2. VERTICAL\n\t3. BOTH\n\n");
            err = scanf("%d",&idx);
            bret=camera_set_display_flip(hcamcorder->camera,idx);
            break;

        case 'g' : // Setting > Brightness
            g_print("*Brightness !\n");
            camera_attr_get_brightness_range(hcamcorder->camera,&min,&max);

            flush_stdin();
            g_print("\n Select  brightness min (%d) -max(%d)",min,max);
            err = scanf("%d",&idx);
            bret = camera_attr_set_brightness(hcamcorder->camera, idx);
            break;

        case 'c' : // Setting > Contrast
            g_print("*Contrast !\n");
            camera_attr_get_contrast_range(hcamcorder->camera,&min,&max);

            flush_stdin();
            g_print("\n Select  Contrast min(%d)-max(%d)",min,max);
            err = scanf("%d",&idx);
            bret = camera_attr_set_contrast(hcamcorder->camera,idx);
            break;



        case 'w' : // Setting > White balance
            g_print("*White balance !\n");

            flush_stdin();
            g_print("\n Select White balance \n");
            camera_attr_foreach_supported_whitebalance(hcamcorder->camera,white_balance_cb,NULL);
            err = scanf("%d",&idx);
            bret = camera_attr_set_whitebalance(hcamcorder->camera,idx);
            break;

        case 't' : // Setting > Color tone
            g_print("*Color tone !\n");
            camera_attr_foreach_supported_effect(hcamcorder->camera,colortone_cb,NULL);

            g_print("\n Select Color tone \n");
            flush_stdin();
            err = scanf("%d",&idx);
            bret = camera_attr_set_effect(hcamcorder->camera, idx);

            break;

        case 'd' : // Setting > WDR
            g_print("*WDR !\n");

            g_print("\n Select WDR Mode \n");
            flush_stdin();
            for (i = 0; i < 2; i++) {
                g_print("\t %d. %s\n", i+1, wdr_mode[i]);
            }
            err = scanf("%d",&idx);
            if (idx == 1)
                bret = camera_attr_enable_auto_contrast(hcamcorder->camera,0);
            else if (idx == 2)
                bret = camera_attr_enable_auto_contrast(hcamcorder->camera,1);

            break;

        case 'e' : // Setting > EV program mode
            g_print("* EV program mode!\n");
            camera_attr_foreach_supported_scene_mode(hcamcorder->camera,program_mode_cb,NULL);

            g_print("\n Select EV program mode \n");
            flush_stdin();
            err = scanf("%d",&idx);
            bret = camera_attr_set_scene_mode(hcamcorder->camera,idx);

            break;

            /* ext. setting */
        case 'z' : // Setting > Strobe setting
            g_print("*Strobe Mode\n");
            camera_attr_foreach_supported_flash_mode(hcamcorder->camera,strobe_mode_cb,NULL);
            g_print("\n Select Strobe Mode \n");
            flush_stdin();
            err = scanf("%d",&idx);
            bret = camera_attr_set_flash_mode(hcamcorder->camera, idx);

            break;

        case 'x' : // Setting > Capture mode ,Muitishot?
            g_print("*Select Capture mode!\n");
            flush_stdin();
            g_print(" \n\t1. Stillshot mode\n\t2. Multishot mode\n\t3. HDR capture\n");
            err = scanf("%d",&idx);

            switch (idx) {
            case 1:
                g_print("stillshot mode selected and capture callback is set!!!!\n");
                hcamcorder->isMultishot = FALSE;
                camera_attr_set_hdr_mode(hcamcorder->camera, 0);
                break;


            case 2:
                g_print("HDR Capture mode selected\n");
                hcamcorder->isMultishot = FALSE;

                g_print("\nSelect HDR capture mode\n");
                flush_stdin();
                for ( i = 0; i < 3; i++) {
                    g_print("\t %d. %s\n", i+1, hdr_mode[i]);
                }
                err = scanf("%d",&idx);
                bret = camera_attr_set_hdr_mode(hcamcorder->camera,idx-1);

                break;
            default:
                g_print("Wrong input, select again!!\n");
                break;
            }
            break;

        case 'l' : // Setting > Face detection setting
            //hcamcorder->menu_state = MENU_STATE_SETTING_DETECTION;
            g_print("* Face detect mode !\n");


            flush_stdin();
            for (i = 0; i < 2; i++) {
                g_print("\t %d. %s \n", i+1, detection_mode[i]);
            }
            err = scanf("%d",&idx);
            if (camera_is_supported_face_detection(hcamcorder->camera))
            {
                if ( idx >= 0 && idx < 2 ) {
                    bret = camera_start_face_detection(hcamcorder->camera,_face_detected,NULL);
                }
            }
            else {
                g_print("face detection_not supported");
            }

            break;

        case 'k' : // Setting > Anti-handshake
            g_print("*Anti-handshake !\n");

            g_print("\n Select Anti-handshake mode \n");
            flush_stdin();
            for (i = 0; i < 2; i++) {
                g_print("\t %d. %s\n", i+1, ahs_mode[i]);
            }
            err = scanf("%d",&idx);
            bret = camera_attr_enable_anti_shake(hcamcorder->camera,idx-1);

            break;

        case 'K' : // Setting > Video-stabilization
            g_print("*Video-stabilization !\n");

            g_print("\n Select Video-stabilization mode \n");
            flush_stdin();
            for (i = 0; i < 2; i++) {
                g_print("\t %d. %s\n", i+1, vs_mode[i]);
            }
            err = scanf("%d",&idx);

            if (idx == 1) {
                g_print("\n Restart preview with NV12 and 720p resolution\n");

                err = camera_stop_preview(hcamcorder->camera);

                camera_set_preview_resolution(hcamcorder->camera,1280,720);
                camera_set_preview_format(hcamcorder->camera, CAMERA_PIXEL_FORMAT_NV12);
                camera_attr_enable_video_stabilization(hcamcorder->camera, idx-1);

                if (err == 0) {
                    err = camera_start_preview(hcamcorder->camera);

                    if (err != 0)
                        g_print("\n Restart FAILED! %x\n", err);
                }
            }
            break;

        case 'u': // Touch AF area
            g_print("* Touch AF area !\n");

            flush_stdin();
            g_print( "\n Input x,y,width,height \n" );
            err = scanf( "%d,%d,%d,%d", &x, &y, &width, &height );
            err =camera_attr_set_af_area(hcamcorder->camera,width,height);

            if (err != 0 ) {
                g_print( "Failed to set touch AF area.(%x)\n", err);
            } else {
                g_print( "Succeed to set touch AF area.\n" );
            }
            break;

        case 'b' : // back
            hcamcorder->menu_state = MENU_STATE_MAIN;
            break;

        default :
            g_print("\t Invalid input \n");
            break;
        }
    } else {
        g_print("\t Invalid mode, back to upper menu \n");
        hcamcorder->menu_state = MENU_STATE_MAIN;
    }

    g_print("\t bret : 0x%x \n", bret);

    return;
}


/**
* This function is to execute command.
*
* @param   channel [in]    1st parameter
*
* @return  This function returns TRUE/FALSE
* @remark
* @see
*/
static gboolean cmd_input(GIOChannel *channel)
{
    gchar *buf = NULL;
    gsize read_size;
    GError *g_error = NULL;

    LOGD("ENTER");

    g_io_channel_read_line(channel, &buf, &read_size, NULL, &g_error);
    if (g_error) {
        LOGD("g_io_channel_read_chars error");
        g_error_free(g_error);
        g_error = NULL;
    }

    if (buf) {
        g_strstrip(buf);

        LOGD("Menu Status : %d", hcamcorder->menu_state);
        switch (hcamcorder->menu_state)
        {
        case MENU_STATE_MAIN:
            main_menu(buf[0]);
            break;
        case MENU_STATE_SETTING:
            setting_menu(buf[0]);
            break;
        default:
            break;
        }

        g_free(buf);
        buf = NULL;

        print_menu();
    } else {
        LOGD("No read input");
    }

    return TRUE;
}

static gboolean init_handle()
{
    hcamcorder->mode = MODE_VIDEO_CAPTURE;  /* image(capture)/video(recording) mode */
    hcamcorder->isMultishot =  FALSE;
    hcamcorder->stillshot_count = 0;        /* total stillshot count */
    hcamcorder->multishot_count = 0;        /* total multishot count */
    hcamcorder->stillshot_filename = STILL_CAPTURE_FILE_PATH_NAME;  /* stored filename of  stillshot  */
    hcamcorder->multishot_filename = MULTI_CAPTURE_FILE_PATH_NAME;  /* stored filename of  multishot  */
    hcamcorder->menu_state = MENU_STATE_MAIN;
    hcamcorder->isMute = FALSE;
    hcamcorder->elapsed_time = 0;
    multishot_num = IMAGE_CAPTURE_COUNT_MULTI;

    return TRUE;
}

void _preview_cb(camera_preview_data_s *frame, void *user_data)
{
#if 0
	FILE *fp = fopen("/opt/usr/media/test.yuv", "a");
	if (fp == NULL) {
		g_print("\n============ file open failed ===========================\n");
		return;
	}

	switch (frame->num_of_planes) {
	case 1:
		fwrite(frame->data.single_plane.yuv, 1, frame->data.single_plane.size, fp);
	case 2:
		fwrite(frame->data.double_plane.y, 1, frame->data.double_plane.y_size, fp);
		fwrite(frame->data.double_plane.uv, 1, frame->data.double_plane.uv_size, fp);
	case 3:
		fwrite(frame->data.triple_plane.y, 1, frame->data.triple_plane.y_size, fp);
		fwrite(frame->data.triple_plane.u, 1, frame->data.triple_plane.u_size, fp);
		fwrite(frame->data.triple_plane.v, 1, frame->data.triple_plane.v_size, fp);
	default:
		break;
	}

	g_print("file write done ---\n");

	fclose(fp);
	fp = NULL;
#else
	g_print("----- preview callback - format %d, %dx%d, num plane %d\n",
	        frame->format, frame->width, frame->height, frame->num_of_planes);
#endif

	return;
}

/**
* This function is to change camcorder mode.
*
* @param   type    [in]    image(capture)/video(recording) mode
*
* @return  This function returns TRUE/FALSE
* @remark
* @see     other functions
*/
static gboolean mode_change()
{
    int err = 0;
    char media_type = '\0';
    bool check= FALSE;

    init_handle();
    while (!check) {
        g_print("\n\t=======================================\n");
        g_print("\t   CAMERA_TESTSUITE\n");
        g_print("\t=======================================\n");
        g_print("\t   '1' Video Capture - Front Camera\n");
        g_print("\t   '2' Video Capture - Rear Camera\n");
        g_print("\t   'q' Exit\n");
        g_print("\t=======================================\n");

        g_print("\t  Enter the media type:\n\t");

        err = scanf("%c", &media_type);
        if (err == EOF) {
            g_print("\t!!!read input error!!!\n");
            continue;
        }

        switch (media_type) {
        case '1':
            hcamcorder->mode= MODE_VIDEO_CAPTURE;
            cam_info = CAMERA_DEVICE_CAMERA1;
            check = TRUE;
            break;
        case '2':
            hcamcorder->mode= MODE_VIDEO_CAPTURE;
            cam_info = CAMERA_DEVICE_CAMERA0;
            check = TRUE;
            break;
        case 'q':
            g_print("\t Quit Camcorder Testsuite!!\n");
            hcamcorder->mode = -1;
            if (g_main_loop_is_running(g_loop))
                g_main_loop_quit(g_loop);

            return FALSE;
        default:
            g_print("\t Invalid media type(%d)\n", media_type);
            continue;
        }
    }

    LOGD("camcorder_create");
    g_get_current_time(&previous_time);
    g_timer_reset(timer);

    err = camera_create(cam_info,&hcamcorder->camera);
    LOGD("camera_create()  : %12.6lfs", g_timer_elapsed(timer, NULL));

    if (err != 0) {
        LOGE("mmcamcorder_create = %x", err);
        return -1;
    } else {
        camera_state = CAMERA_STATE_NONE;
    }
    camera_print_state = CAMERA_STATE_CREATED;

    camera_set_state_changed_cb(hcamcorder->camera, _camera_state_changed_cb, NULL);
    camera_set_display_mode(hcamcorder->camera,0 ); //MM_DISPLAY_METHOD_LETTER_BOX
    camera_set_display(hcamcorder->camera, CAMERA_DISPLAY_TYPE_OVERLAY, GET_DISPLAY(eo));

    //camera_set_preview_cb(hcamcorder->camera, _preview_cb, hcamcorder->camera);

    camera_start_preview(hcamcorder->camera);
    g_get_current_time(&current_time);
    timersub(&current_time, &previous_time, &res);
    LOGD("Camera Starting Time  : %ld.%lds", res.tv_sec, res.tv_usec);
    camera_print_state = CAMERA_STATE_PREVIEW;

    return TRUE;
}


/**
* This function is the example main function for mmcamcorder API.
*
* @param
*
* @return  This function returns 0.
* @remark
* @see     other functions
*/
int main(int argc, char **argv)
{

    int bret;
    int w,h;
    elm_init(argc, argv);

    eo = elm_win_add(NULL, "VIDEO OVERLAY", ELM_WIN_BASIC);
    elm_win_title_set(eo, "TITLE");
    elm_win_borderless_set(eo, EINA_TRUE);
    elm_win_screen_size_get(eo, NULL, NULL, &w, &h);
    evas_object_resize(eo, w, h);
    evas_object_show(eo);
    elm_win_activate(eo);


#if !GLIB_CHECK_VERSION(2,35,0)
    if (!g_thread_supported())
        g_thread_init (NULL);
#endif

    timer = g_timer_new();

    gst_init(&argc, &argv);

    LOGD("gst_init() : %12.6lfs", g_timer_elapsed(timer, NULL));

    hcamcorder = (cam_handle_t *) g_malloc0(sizeof(cam_handle_t));
    camera_state = CAMERA_STATE_NONE;

    g_timer_reset(timer);

    bret = mode_change();
    if (!bret)
        return bret;

    print_menu();

    g_loop = g_main_loop_new(NULL, FALSE);

    stdin_channel = g_io_channel_unix_new(fileno(stdin));/* read from stdin */
    g_io_add_watch(stdin_channel, G_IO_IN, (GIOFunc)cmd_input, NULL);

    LOGD("RUN main loop");

    g_main_loop_run(g_loop);

    LOGD("STOP main loop");

    if (timer) {
        g_timer_stop(timer);
        g_timer_destroy(timer);
        timer = NULL;
    }
    g_free(hcamcorder);
    g_main_loop_unref(g_loop);
    g_io_channel_unref(stdin_channel);

    return bret;
}

/*EOF*/
