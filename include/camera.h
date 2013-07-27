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



#ifndef __TIZEN_MULTIMEDIA_CAMERA_H__
#define __TIZEN_MULTIMEDIA_CAMERA_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif


#define CAMERA_ERROR_CLASS          TIZEN_ERROR_MULTIMEDIA_CLASS | 0x00


/**
 * @file camera.h
 * @brief This file contains the Camera API, related structures and enumerations
 */


/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */


/**
 * @brief	Enumerations of the error code for Camera.
 */
typedef enum
{
    CAMERA_ERROR_NONE                   = TIZEN_ERROR_NONE,                     /**< Successful */
    CAMERA_ERROR_INVALID_PARAMETER      = TIZEN_ERROR_INVALID_PARAMETER,        /**< Invalid parameter */
    CAMERA_ERROR_INVALID_STATE          = CAMERA_ERROR_CLASS | 0x02,            /**< Invalid state */
    CAMERA_ERROR_OUT_OF_MEMORY          = TIZEN_ERROR_OUT_OF_MEMORY,            /**< Out of memory */
    CAMERA_ERROR_DEVICE                 = CAMERA_ERROR_CLASS | 0x04,            /**< Device error */
    CAMERA_ERROR_INVALID_OPERATION      = TIZEN_ERROR_INVALID_OPERATION,        /**< Internal error */
    CAMERA_ERROR_SOUND_POLICY           = CAMERA_ERROR_CLASS | 0x06,            /**< Blocked by Audio Session Manager */
    CAMERA_ERROR_SECURITY_RESTRICTED    = CAMERA_ERROR_CLASS | 0x07,            /**< Restricted by security system policy */
    CAMERA_ERROR_DEVICE_BUSY            = CAMERA_ERROR_CLASS | 0x08,            /**< The device is using in other applications or working some operation */
    CAMERA_ERROR_DEVICE_NOT_FOUND       = CAMERA_ERROR_CLASS | 0x09,            /**< No camera device */
    CAMERA_ERROR_SOUND_POLICY_BY_CALL   = CAMERA_ERROR_CLASS | 0x0a,            /**< Blocked by Audio Session Manager - CALL */
    CAMERA_ERROR_SOUND_POLICY_BY_ALARM  = CAMERA_ERROR_CLASS | 0x0b,            /**< Blocked by Audio Session Manager - ALARM */
} camera_error_e;


/**
 * @brief	Enumerations of the camera state.
 */
typedef enum
{
    CAMERA_STATE_NONE,       /**< Before create */
    CAMERA_STATE_CREATED,    /**< Created, but not initialized yet */
    CAMERA_STATE_PREVIEW,    /**< Preview */
    CAMERA_STATE_CAPTURING,  /**< While capturing */
    CAMERA_STATE_CAPTURED    /**< After capturing */
} camera_state_e;


/**
 * @brief	Enumerations of the camera device.
 */
typedef enum
{
    CAMERA_DEVICE_CAMERA0 = 0, /**< Primary camera */
    CAMERA_DEVICE_CAMERA1      /**< Secondary camera */
} camera_device_e;


/**
 * @brief	Enumerations of the camera pixel format.
 */
typedef enum
{
	CAMERA_PIXEL_FORMAT_INVALID	= -1,  /**< Invalid pixel format */
	CAMERA_PIXEL_FORMAT_NV12,           /**< NV12 pixel format */
	CAMERA_PIXEL_FORMAT_NV12T,          /**< NV12 Tiled pixel format */
	CAMERA_PIXEL_FORMAT_NV16,           /**< NV16 pixel format */
	CAMERA_PIXEL_FORMAT_NV21,           /**< NV21 pixel format */
	CAMERA_PIXEL_FORMAT_YUYV,           /**< YUYV(YUY2) pixel format */
	CAMERA_PIXEL_FORMAT_UYVY,           /**< UYVY pixel format */
	CAMERA_PIXEL_FORMAT_422P,           /**< YUV422(Y:U:V) planar pixel format */
	CAMERA_PIXEL_FORMAT_I420,           /**< I420 pixel format */
	CAMERA_PIXEL_FORMAT_YV12,           /**< YV12 pixel format */
	CAMERA_PIXEL_FORMAT_RGB565,         /**< RGB565 pixel format */
	CAMERA_PIXEL_FORMAT_RGB888,         /**< RGB888 pixel format */
	CAMERA_PIXEL_FORMAT_RGBA,           /**< RGBA pixel format */
	CAMERA_PIXEL_FORMAT_ARGB,           /**< ARGB pixel format */
	CAMERA_PIXEL_FORMAT_JPEG,           /**< Encoded pixel format */
} camera_pixel_format_e;


/**
 * @brief	Enumerations of the camera display type.
 */
typedef enum
{
  CAMERA_DISPLAY_TYPE_X11 = 0,		/**< X surface display */
  CAMERA_DISPLAY_TYPE_EVAS = 1,		/**< Evas object surface display */
  CAMERA_DISPLAY_TYPE_NONE = 3		/**< This disposes of buffers */
} camera_display_type_e;


/**
 * @brief	The handle to the camera.
 * @see	recorder_create_videorecorder()
 */
typedef struct camera_s *camera_h;


/**
 * @brief	The handle to the camera display.
 */
typedef void *camera_display_h;


#ifndef GET_DISPLAY

/**
 * @brief	Gets a display handle from x window id or evas object
 */
#define GET_DISPLAY(x) (void*)(x)

#endif

/**
 * @}
 */


/**
 * @addtogroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @{
 */


/**
 * @brief Enumerations of the camera rotation type.
 */
typedef enum
{
	CAMERA_ROTATION_NONE,	/**< No rotation */
	CAMERA_ROTATION_90,		/**< 90 degree rotation */
	CAMERA_ROTATION_180,	/**< 180 degree rotation */
	CAMERA_ROTATION_270,	/**< 270 degree rotation */
} camera_rotation_e;


/**
 * @brief Enumerations of the camera flip type.
 */
typedef enum
{
	CAMERA_FLIP_NONE,		/**< No Flip */
	CAMERA_FLIP_HORIZONTAL, /**< Horizontal flip */
	CAMERA_FLIP_VERTICAL, /**< Vertical flip */
	CAMERA_FLIP_BOTH	/** Horizontal and vertical flip */
}camera_flip_e;

/**
 * @brief Enumerations of the camera display mode.
 */
typedef enum
{
	CAMERA_DISPLAY_MODE_LETTER_BOX = 0,	/**< Letter box*/
	CAMERA_DISPLAY_MODE_ORIGIN_SIZE,	/**< Origin size*/
	CAMERA_DISPLAY_MODE_FULL,	/**< full screen*/
	CAMERA_DISPLAY_MODE_CROPPED_FULL,	/**< Cropped full screen*/
} camera_display_mode_e;

/**
 * @brief Enumerations of the camera policy.
 */
typedef enum
{
	CAMERA_POLICY_NONE = 0,         /**< None */
	CAMERA_POLICY_SOUND,            /**< Sound policy */
	CAMERA_POLICY_SOUND_BY_CALL,    /**< Sound policy by CALL */
	CAMERA_POLICY_SOUND_BY_ALARM,   /**< Sound policy by ALARM */
	CAMERA_POLICY_SECURITY          /**< Security policy */
} camera_policy_e;


/**
 * @brief Struct of the image data
 */
 typedef struct
{
	unsigned char *data;       				/**< The image buffer */
	unsigned int size;            				/**< The size of buffer */
	int width;                        			/**< The width of image */
	int height;                      				/**< The height of image */
	camera_pixel_format_e format; /**< The format of image pixel */
	unsigned char *exif;					   /**< The exif raw data */
	unsigned int exif_size;					/**< The size of exif data */
}camera_image_data_s;


/**
 * @brief Struct of the face detection
 */
typedef struct
{
	int id;				/**< The id of each face */
	int score;		/**< The confidence level for the detection of the face */
	int x;				/**< The x coordinates of face */
	int y;				/**< The y coordinates of face */
	int width;		/**< The width of face */
	int height;		/**< The height of face */
}camera_detected_face_s;


/**
 * @brief Struct of the preview stream data
 */
typedef struct
{
	camera_pixel_format_e format;   /**< The format of frame pixel */
	int width;                                    /**< The width of frame */
	int height;                                  /**< The height of frame */
	int num_of_planes;                   /**< The number of planes */
	unsigned int timestamp;             /**< The timestamp of frame */
	union {
		struct {
			unsigned char *yuv;             /**< The yuv data pointer*/
			unsigned int size;                 /**< The size of data*/
		} single_plane;                      /**< single plane frame data */

		struct {
			unsigned char *y;                 /**< The y data pointer*/
			unsigned char *uv;               /**< The uv data pointer*/
			unsigned int y_size;             /**< The size of y data*/
			unsigned int uv_size;           /**< The size of uv data*/
		} double_plane;                    /**< double plane frame data */

		struct {
			unsigned char *y;                /**< The y data pointer*/
			unsigned char *u;                /**< The u data pointer*/
			unsigned char *v;                /**< The v data pointer*/
			unsigned int y_size;            /**< The size of y data*/
			unsigned int u_size;            /**< The size of u data*/
			unsigned int v_size;            /**< The size of v data*/
		} triple_plane;                     /**< triple plane frame data */
	} data;
}camera_preview_data_s;


/**
 * @}
 */


/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */


/**
 * @brief Enumerations of the color tone which provides an impression of looking through a tinted glass.
 */
typedef enum
{
    CAMERA_ATTR_EFFECT_NONE = 0,     /**< None */
    CAMERA_ATTR_EFFECT_MONO,         /**< Mono */
    CAMERA_ATTR_EFFECT_SEPIA,        /**< Sepia */
    CAMERA_ATTR_EFFECT_NEGATIVE,     /**< Negative */
    CAMERA_ATTR_EFFECT_BLUE,         /**< Blue */
    CAMERA_ATTR_EFFECT_GREEN,        /**< Green */
    CAMERA_ATTR_EFFECT_AQUA,         /**< Aqua */
    CAMERA_ATTR_EFFECT_VIOLET,       /**< Violet */
    CAMERA_ATTR_EFFECT_ORANGE,       /**< Orange */
    CAMERA_ATTR_EFFECT_GRAY,         /**< Gray */
    CAMERA_ATTR_EFFECT_RED,          /**< Red */
    CAMERA_ATTR_EFFECT_ANTIQUE,      /**< Antique */
    CAMERA_ATTR_EFFECT_WARM,         /**< Warm */
    CAMERA_ATTR_EFFECT_PINK,         /**< Pink */
    CAMERA_ATTR_EFFECT_YELLOW,       /**< Yellow */
    CAMERA_ATTR_EFFECT_PURPLE,       /**< Purple */
    CAMERA_ATTR_EFFECT_EMBOSS,       /**< Emboss */
    CAMERA_ATTR_EFFECT_OUTLINE,      /**< Outline */
    CAMERA_ATTR_EFFECT_SOLARIZATION, /**< Solarization */
    CAMERA_ATTR_EFFECT_SKETCH,       /**< Sketch */
    CAMERA_ATTR_EFFECT_WASHED,     /**< Washed */
    CAMERA_ATTR_EFFECT_VINTAGE_WARM,     /**< Vintage warm  */
    CAMERA_ATTR_EFFECT_VINTAGE_COLD,     /**< Vintage cold */
    CAMERA_ATTR_EFFECT_POSTERIZATION,     /**< Posterization */
    CAMERA_ATTR_EFFECT_CARTOON,     /**< Cartoon */
    CAMERA_ATTR_EFFECT_SELECTIVE_RED,     /**< Selective color - Red */
    CAMERA_ATTR_EFFECT_SELECTIVE_GREEN,     /**< Selective color - Green */
    CAMERA_ATTR_EFFECT_SELECTIVE_BLUE,     /**< Selective color - Blue */
    CAMERA_ATTR_EFFECT_SELECTIVE_YELLOW,     /**< Selective color - Yellow */
    CAMERA_ATTR_EFFECT_SELECTIVE_RED_YELLOW,     /**< Selective color - Red and Yellow */
} camera_attr_effect_mode_e;


/**
 * @brief Enumerations of the white balance levels of the camera.
 */
typedef enum
{
    CAMERA_ATTR_WHITE_BALANCE_NONE = 0,     /**< None */
    CAMERA_ATTR_WHITE_BALANCE_AUTOMATIC,    /**< Automatic */
    CAMERA_ATTR_WHITE_BALANCE_DAYLIGHT,     /**< Daylight */
    CAMERA_ATTR_WHITE_BALANCE_CLOUDY,       /**< Cloudy */
    CAMERA_ATTR_WHITE_BALANCE_FLUORESCENT,  /**< Fluorescent */
    CAMERA_ATTR_WHITE_BALANCE_INCANDESCENT, /**< Incandescent */
    CAMERA_ATTR_WHITE_BALANCE_SHADE,        /**< Shade */
    CAMERA_ATTR_WHITE_BALANCE_HORIZON,      /**< Horizon */
    CAMERA_ATTR_WHITE_BALANCE_FLASH,        /**< Flash */
    CAMERA_ATTR_WHITE_BALANCE_CUSTOM,       /**< Custom */
} camera_attr_whitebalance_e;


/**
 * @brief Enumerations of the scene mode.
 * The mode of operation can be in daylight, night and back-light.
 */
typedef enum
{
    CAMERA_ATTR_SCENE_MODE_NORMAL = 0,     /**< Normal */
    CAMERA_ATTR_SCENE_MODE_PORTRAIT,       /**< Portrait */
    CAMERA_ATTR_SCENE_MODE_LANDSCAPE,      /**< Landscape */
    CAMERA_ATTR_SCENE_MODE_SPORTS,         /**< Sports */
    CAMERA_ATTR_SCENE_MODE_PARTY_N_INDOOR, /**< Party & indoor */
    CAMERA_ATTR_SCENE_MODE_BEACH_N_INDOOR, /**< Beach & indoor */
    CAMERA_ATTR_SCENE_MODE_SUNSET,         /**< Sunset */
    CAMERA_ATTR_SCENE_MODE_DUSK_N_DAWN,    /**< Dusk & dawn */
    CAMERA_ATTR_SCENE_MODE_FALL_COLOR,     /**< Fall */
    CAMERA_ATTR_SCENE_MODE_NIGHT_SCENE,    /**< Night scene */
    CAMERA_ATTR_SCENE_MODE_FIREWORK,       /**< Firework */
    CAMERA_ATTR_SCENE_MODE_TEXT,           /**< Text */
    CAMERA_ATTR_SCENE_MODE_SHOW_WINDOW,    /**< Show window */
    CAMERA_ATTR_SCENE_MODE_CANDLE_LIGHT,   /**< Candle light */
    CAMERA_ATTR_SCENE_MODE_BACKLIGHT,      /**< Backlight */
} camera_attr_scene_mode_e;


/**
 * @brief	Enumerations of the auto focus mode.
 */
typedef enum
{
    CAMERA_ATTR_AF_NONE = 0,    /**< auto-focus is not set */
    CAMERA_ATTR_AF_NORMAL,      /**< auto-focus normally  */
    CAMERA_ATTR_AF_MACRO,       /**< auto-focus in macro mode(close distance)  */
    CAMERA_ATTR_AF_FULL,        /**< auto-focus in full mode(all range scan, limited by dev spec) */
} camera_attr_af_mode_e;


/**
 * @brief	Enumerations of the camera focus state.
 */
typedef enum
{
    CAMERA_FOCUS_STATE_RELEASED = 0, /**< Focus released.*/
    CAMERA_FOCUS_STATE_ONGOING,      /**< Focus in progress*/
    CAMERA_FOCUS_STATE_FOCUSED,      /**< Focus success*/
    CAMERA_FOCUS_STATE_FAILED,       /**< Focus failed*/
} camera_focus_state_e;


/**
 * @brief	Enumerations for the ISO levels of the camera.
 */
typedef enum
{
    CAMERA_ATTR_ISO_AUTO = 0, /**< ISO auto mode*/
    CAMERA_ATTR_ISO_50,       /**< ISO 50*/
    CAMERA_ATTR_ISO_100,      /**< ISO 100*/
    CAMERA_ATTR_ISO_200,      /**< ISO 200*/
    CAMERA_ATTR_ISO_400,      /**< ISO 400*/
    CAMERA_ATTR_ISO_800,      /**< ISO 800*/
    CAMERA_ATTR_ISO_1600,     /**< ISO 1600*/
    CAMERA_ATTR_ISO_3200,     /**< ISO 3200*/
} camera_attr_iso_e;


/**
 * @brief	Enumerations of the camera exposure modes.
 */
typedef enum
{
    CAMERA_ATTR_EXPOSURE_MODE_OFF = 0,   /**< Off*/
    CAMERA_ATTR_EXPOSURE_MODE_ALL,       /**< All mode*/
    CAMERA_ATTR_EXPOSURE_MODE_CENTER,    /**< Center mode*/
    CAMERA_ATTR_EXPOSURE_MODE_SPOT,      /**< Spot mode*/
    CAMERA_ATTR_EXPOSURE_MODE_CUSTOM,    /**< Custom mode*/
} camera_attr_exposure_mode_e;


/**
 * @brief	Enumerations for the orientation values of tag.
 */
typedef enum
{
    CAMERA_ATTR_TAG_ORIENTATION_TOP_LEFT = 1,      /**< Row #0 is top, Column #0 is left */
    CAMERA_ATTR_TAG_ORIENTATION_TOP_RIGHT = 2,     /**< Row #0 is top, Column #0 is right (flipped) */
    CAMERA_ATTR_TAG_ORIENTATION_BOTTOM_RIGHT = 3,  /**< Row #0 is bottom, Column #0 is right */
    CAMERA_ATTR_TAG_ORIENTATION_BOTTOM_LEFT = 4,   /**< Row #0 is bottom, Column #0 is left (flipped) */
    CAMERA_ATTR_TAG_ORIENTATION_LEFT_TOP = 5,      /**< Row #0 is left, Column #0 is top (flipped) */
    CAMERA_ATTR_TAG_ORIENTATION_RIGHT_TOP = 6,     /**< Row #0 is right, Column #0 is top */
    CAMERA_ATTR_TAG_ORIENTATION_RIGHT_BOTTOM = 7,  /**< Row #0 is right, Column #0 is bottom (flipped) */
    CAMERA_ATTR_TAG_ORIENTATION_LEFT_BOTTOM = 8,   /**< Row #0 is left, Column #0 is bottom */
} camera_attr_tag_orientation_e;


/**
 * @brief	Enumerations of the flash mode.
 */
typedef enum
{
    CAMERA_ATTR_FLASH_MODE_OFF = 0,          /**< Always off */
    CAMERA_ATTR_FLASH_MODE_ON,               /**< Always splashes */
    CAMERA_ATTR_FLASH_MODE_AUTO,             /**< Depending on intensity of light, strobe starts to flash. */
    CAMERA_ATTR_FLASH_MODE_REDEYE_REDUCTION, /**< Red eye reduction. Multiple flash before capturing. */
    CAMERA_ATTR_FLASH_MODE_SLOW_SYNC,        /**< Slow sync curtain synchronization*/
    CAMERA_ATTR_FLASH_MODE_FRONT_CURTAIN,    /**< Front curtain synchronization. */
    CAMERA_ATTR_FLASH_MODE_REAR_CURTAIN,     /**< Rear curtain synchronization. */
    CAMERA_ATTR_FLASH_MODE_PERMANENT,        /**< keep turned on until turning off */
} camera_attr_flash_mode_e;


/**
 * @brief	Enumerations of the preview fps.
 */
typedef enum
{
    CAMERA_ATTR_FPS_AUTO = 0, /**< AUTO FPS */
    CAMERA_ATTR_FPS_8 = 8,    /**< 8 FPS */
    CAMERA_ATTR_FPS_15 = 15,  /**< 15 FPS */
    CAMERA_ATTR_FPS_24 = 24,  /**< 24 FPS */
    CAMERA_ATTR_FPS_25 = 25,  /**< 25 FPS */
    CAMERA_ATTR_FPS_30 = 30,  /**< 30 FPS */
    CAMERA_ATTR_FPS_60 = 60,  /**< 60 FPS */
    CAMERA_ATTR_FPS_120 = 120 /**< 120 FPS */
} camera_attr_fps_e;

/**
 * @brief Enumerations of the theater mode
 */
typedef enum
{
	CAMERA_ATTR_THEATER_MODE_DISABLE = 0, /**< Disable theater mode - External display show same image with device display. */
	CAMERA_ATTR_THEATER_MODE_ENABLE = 2,  /**< Enable theater mode - Preview image is displayed on external display with full screen mode. But preview image is not shown on device display. */
	CAMERA_ATTR_THEATER_MODE_CLONE = 1    /**< Clone mode - Preview image is displayed on external display with full screen mode. Also preview image is shown with UI on device display*/
} camera_attr_theater_mode_e;

/**
 * @brief Enumerations of HDR capture mode
 */
typedef enum
{
	CAMERA_ATTR_HDR_MODE_DISABLE = 0,   /**< Disable HDR capture */
	CAMERA_ATTR_HDR_MODE_ENABLE,          /**< Enable HDR capture */
	CAMERA_ATTR_HDR_MODE_KEEP_ORIGINAL /**< Enable HDR capture and keep original image data */
} camera_attr_hdr_mode_e;


/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */


/**
 * @brief	Called when the camera state changes.
 *
 * @param[in] previous      The previous state of the camera
 * @param[in] current       The current state of the camera
 * @param[in] by_policy     @c true if the state is changed by policy, otherwise @c false
 * @param[in] user_data     The user data passed from the callback registration function
 * @pre camera_start_preview(), camera_start_capture() or camera_stop_preview()
 * will invoke this callback if you registers this callback unsing camera_set_state_changed_cb().
 * @see	camera_set_state_changed_cb()
 */
typedef void (*camera_state_changed_cb)(camera_state_e previous, camera_state_e current,
        bool by_policy, void *user_data);

/**
 * @brief	Called when the camera interrupted by policy
 *
 * @param[in] policy     		The policy that interrupting the camera
 * @param[in] previous      The previous state of the camera
 * @param[in] current       The current state of the camera
 * @param[in] user_data     The user data passed from the callback registration function
 * @see	camera_set_interrupted_cb()
 */
typedef void (*camera_interrupted_cb)(camera_policy_e policy, camera_state_e previous, camera_state_e current, void *user_data);



/**
 * @brief	Called when the camera focus state changes.
 * @details When the camera auto focus completes or a change to the focus state occurs,
 * this callback is invoked. \n \n
 * Changes of focus state are as follows: \n
 * #CAMERA_FOCUS_STATE_RELEASED -> start focusing -> #CAMERA_FOCUS_STATE_ONGOING -> working ->
 * #CAMERA_FOCUS_STATE_FOCUSED or #CAMERA_FOCUS_STATE_FAILED
 *
 * @param[in] state         The current state of the auto-focus
 * @param[in] user_data     The user data passed from the callback registration function
 * @pre camera_start_focusing() will invoke this callback if you register it using camera_set_focus_changed_cb ().
 * @see	camera_set_focus_changed_cb()
 * @see	camera_unset_focus_changed_cb()
 * @see	camera_start_focusing()
 * @see camera_cancel_focusing()
 */
typedef void (*camera_focus_changed_cb)(camera_focus_state_e state, void *user_data);


/**
 * @brief	Called to be notified for delivering copy of new preview frame when every preview frame is displayed.
 *
 * @remarks This function is issued in the context of gstreamer (video sink thread) so you should not directly invoke UI update code.\n
 * When camera is used as a recorder then this callback function won't be called.
 *
 * @param[in] frame     Reference pointer to preview stream data
 * @param[in] user_data     	The user data passed from the callback registration function
 * @pre	camera_start_preview() will invoke this callback function if you register this callback using camera_set_preview_cb().
 * @see	camera_start_preview()
 * @see	camera_set_preview_cb()
 * @see	camera_unset_preview_cb()
 */
typedef void (*camera_preview_cb)(camera_preview_data_s *frame, void *user_data);

/**
 * @brief	Called to get information about image data taken by the camera once per frame while capturing.
 *
 * @remarks This function is issued in the context of gstreamer (video source thread) so you should not directly invoke UI update code.
 * You must not call camera_start_preview() within this callback.
 *
 * @param[in] image     The image data of captured picture
 * @param[in] postview  The image data of postvew
 * @param[in] thumbnail The image data of thumbnail ( It could be NULL, if available thumbnail data is not existed. )
 * @param[in] user_data     The user data passed from the callback registration function
 * @pre	camera_start_capture() or camera_start_continuous_capture() will invoke this callback function if you register this callback using camera_start_capture() or camera_start_continuous_capture()
 * @see	camera_start_capture()
 * @see	camera_start_continuous_capture()
 * @see	camera_capture_completed_cb()
 */
typedef void (*camera_capturing_cb)(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data);



/**
 * @brief	Called when the camera capturing completes.
 *
 * @remarks The callback is called after end of camera_capturing_cb().\n
 * If you want to show the user preview after finishing capturing,  an application can use camera_start_preview() after calling this callback.
 *
 * @param[in] user_data     The user data passed from the callback registration function
 *
 * @pre	This callback function is invoked if you register this callback using camera_start_capture() or camera_start_continuous_capture().
 * @see	camera_start_capture()
 * @see	camera_start_continuous_capture()
 * @see	camera_capturing_cb()
 */
typedef void (*camera_capture_completed_cb)(void *user_data);


/**
 * @brief	Called when the error occurred.
 *
 * @remarks
 * This callback inform critical error situation.\n
 * When invoked this callback, user should release the resource and terminate application.\n
 * These error code will be occurred\n
 * #CAMERA_ERROR_DEVICE\n
 * #CAMERA_ERROR_INVALID_OPERATION\n
 * #CAMERA_ERROR_OUT_OF_MEMORY\n
 *
 * @param[in] error		The error code
 * @param[in] current_state	The current state of the camera
 * @param[in] user_data		The user data passed from the callback registration function
 *
 * @pre	This callback function is invoked if you register this callback using camera_set_error_cb().
 * @see	camera_set_error_cb()
 * @see	camera_unset_error_cb()
 */
typedef void (*camera_error_cb)(camera_error_e error, camera_state_e current_state, void *user_data);

/**
 * @brief Called when face detected in the preview frame
 *
 * @param[in] faces The detected face array
 * @param[in] count The length of array
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @see	camera_start_face_detection()
 */
typedef void (*camera_face_detected_cb)(camera_detected_face_s *faces, int count, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief	Called once for each supported preview resolution.
 *
 * @param[in] width         The preview image width
 * @param[in] height        The preview image height
 * @param[in] user_data     The user data passed from the foreach function
 *
 * @return	@c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_foreach_supported_preview_resolution() will invoke this callback.
 *
 * @see	camera_foreach_supported_preview_resolution()
 */
typedef bool (*camera_supported_preview_resolution_cb)(int width, int height, void *user_data);


/**
 * @brief   Called once for each supported capture resolution.
 *
 * @param[in] width         The capture resolution width
 * @param[in] height        The capture resolution height
 * @param[in] user_data     The user data passed from the foreach function
 *
 * @return	@c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_foreach_supported_capture_resolution() will invoke this callback.
 *
 * @see	camera_foreach_supported_capture_resolution()
 */
typedef bool (*camera_supported_capture_resolution_cb)(int width, int height, void *user_data);


/**
 * @brief	Called once for the pixel format of each supported capture format.
 *
 * @param[in] format        The supported pixel format
 * @param[in] user_data     The user data passed from the foreach function
 * @return	@c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_foreach_supported_capture_format() will invoke this callback.
 *
 * @see	camera_foreach_supported_capture_format()
 */
typedef bool (*camera_supported_capture_format_cb)(camera_pixel_format_e format,
        void *user_data);

/**
 * @brief   Called once for the pixel format of each supported preview format.
 *
 * @param[in] format        The supported preview data format
 * @param[in] user_data     The user data passed from the foreach function
 * @return	@c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_foreach_supported_preview_format() will invoke this callback.
 *
 * @see	camera_foreach_supported_preview_format()
 */
typedef bool (*camera_supported_preview_format_cb)(camera_pixel_format_e format,
        void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */

/**
 * @brief Creates a new camera handle for controlling a camera.
 *
 * @remarks You can create multiple handles on a context at the same time. However,
 * camera cannot guarantee proper operation because of limitation of resources, such as
 * camera device, audio device, and display device.\n
 * a @a camera must be released with camera_destroy() by you.
 *
 * @param[in]   device    The hardware camera to access
 * @param[out]  camera	A newly returned handle to the camera
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_OUT_OF_MEMORY Out of memory
 * @retval      #CAMERA_ERROR_SOUND_POLICY Sound policy error
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @post   If it succeeds the camera state will be #CAMERA_STATE_CREATED.
 *
 * @see	camera_destroy()
 */
int camera_create(camera_device_e device, camera_h *camera);

/**
 * @brief Destroys the camera handle and releases all its resources.
 *
 * @param[in]	camera	The handle to the camera
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see camera_create()
 */
int camera_destroy(camera_h camera);

/**
 * @brief Starts capturing and drawing preview frames on the screen.
 *
 * @param[in]	camera	The handle to the camera
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_SOUND_POLICY Sound policy error
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval		#CAMERA_ERROR_DEVICE_BUSY The device is using in other applications or working some operation
 * @retval		#CAMERA_ERROR_DEVICE_NOT_FOUND No camera device
 * @pre    The camera state should be #CAMERA_STATE_CREATED, or #CAMERA_STATE_CAPTURED.\n
 * You must set display handle. \n
 * If needed, modify preview fps(camera_attr_set_preview_fps()),
 * preview resolution(camera_set_preview_resolution()) or preview format(camera_set_preview_format())
 * @post   If it succeeds, the camera state will be #CAMERA_STATE_PREVIEW.\n
 * camera_preview_cb() will be called when preview image data becomes available.
 *
 * @see	camera_stop_preview()
 * @see camera_set_display()
 * @see camera_set_preview_cb()
 * @see camera_foreach_supported_preview_resolution()
 * @see camera_set_preview_resolution()
 * @see camera_get_preview_resolution()
 * @see camera_foreach_supported_preview_format()
 * @see camera_set_preview_format()
 * @see camera_get_preview_format()
 * @see camera_attr_foreach_supported_fps()
 * @see camera_attr_set_preview_fps()
 * @see camera_attr_get_preview_fps()
 */
int camera_start_preview(camera_h camera);

/**
 * @brief  Stops capturing and drawing preview frames.
 *
 * @param[in]	camera	The handle to the camera
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @pre         The camera state should be #CAMERA_STATE_PREVIEW.
 * @post        The camera state will be #CAMERA_STATE_CREATED.
 *
 * @see	camera_start_preview()
 * @see	camera_unset_preview_cb()
 */
int camera_stop_preview(camera_h camera);

/**
 * @brief Starts capturing of still images.
 *
 * @remarks  This function causes the transition of camera state from #CAMERA_STATE_CAPTURING to #CAMERA_STATE_CAPTURED automatically\n
 * and the corresponding callback function camera_capturing_cb() and camera_capture_completed_cb() will be invoked\n
 * Captured image will be delivered through camera_capturing_cb().\n
 * You will be notified by camera_capture_completed_cb() callback when camera_capturing_cb() gets completed. \n
 * You should restart camera's preview with calling camera_start_preview().
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] capturing_cb The callback for capturing data
 * @param[in] completed_cb The callback for notification of completed
 * @param[in] user_data The user data
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 *
 * @pre         The camera state must be #CAMERA_STATE_PREVIEW. \n
 * If needed, modify capture resolution(camera_set_capture_resolution()),
 * capture format(camera_set_capture_format()), or image quality(camera_attr_set_image_quality())
 * @post   If it succeeds the camera state will be #CAMERA_STATE_CAPTURED.
 *
 * @see camera_start_preview()
 * @see camera_start_continuous_capture();
 * @see camera_foreach_supported_capture_resolution()
 * @see camera_set_capture_resolution()
 * @see camera_get_capture_resolution()
 * @see camera_foreach_supported_capture_format()
 * @see camera_set_capture_format()
 * @see camera_get_capture_format()
 * @see camera_attr_set_image_quality()
 * @see camera_attr_get_image_quality()
 */
int camera_start_capture(camera_h camera, camera_capturing_cb capturing_cb , camera_capture_completed_cb completed_cb , void *user_data);

/**
 * @brief Starts continuous capturing of still images.
 *
 * @remarks
 * If not supported zero shutter lag. the capture resolution could be changed to the preview resolution.\n
 * This function causes the transition of camera state from #CAMERA_STATE_CAPTURING to #CAMERA_STATE_CAPTURED automatically\n
 * and the corresponding callback function camera_capturing_cb() and camera_capture_completed_cb() will be invoked\n
 * Each Captured image will be delivered through camera_capturing_cb().\n
 * You will be notified by camera_capture_completed_cb() callback when entire capture is completed.\n
 * You should restart camera's preview with calling camera_start_preview().\n
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]	count	The number of still images
 * @param[in] interval	The interval of capture ( millisecond )
 * @param[in] capturing_cb The callback for capturing data
 * @param[in] completed_cb The callback for notification of completed
 * @param[in] user_data The user data
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 *
 * @post   If it succeeds the camera state will be #CAMERA_STATE_CAPTURED.
 *
 * @see camera_start_preview()
 * @see camera_start_capture();
 * @see camera_stop_continuous_capture()
 * @see camera_is_supported_zero_shutter_lag()
 */
int camera_start_continuous_capture(camera_h camera, int count, int interval, camera_capturing_cb capturing_cb, camera_capture_completed_cb completed_cb , void *user_data);

/**
 * @brief Abort continuous capturing.
 *
 * @remarks The camera state will be changed to the #CAMERA_STATE_CAPTURED state
 * @param[in]	camera	The handle to the camera
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @pre    The camera state should be #CAMERA_STATE_PREVIEW
 *
 * @see camera_start_continuous_capture()
 */
int camera_stop_continuous_capture(camera_h camera);


/**
 * @brief Gets the state of the camera.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]	state	The current state of camera
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_create()
 * @see camera_start_preview()
 * @see camera_stop_preview()
 * @see camera_start_capture()
 */
int camera_get_state(camera_h camera, camera_state_e *state);

/**
 * @brief Starts camera auto-focusing, Asynchronously
 *
 * @remarks If continuous status is true, the camera continuously tries to focus
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] continuous	The status of continuous focusing
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @pre    The camera state should be #CAMERA_STATE_PREVIEW
 * @post	The camera focus state will be #CAMERA_FOCUS_STATE_ONGOING.
 *
 * @see camera_cancel_focusing()
 * @see camera_set_focus_changed_cb()
 * @see camera_focus_changed_cb()
 * @see camera_attr_set_af_mode()
 */
int camera_start_focusing(camera_h camera, bool continuous);

/**
 * @brief Stops camera auto focusing.
 *
 * @param[in]	camera	The handle to the camera
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @pre    The camera state should be #CAMERA_STATE_PREVIEW
 *
 * @see camera_start_focusing()
 * @see	camera_focus_changed_cb()
 */
int camera_cancel_focusing(camera_h camera);

/**
 * @brief Sets the display handle to show preview images
 *
 * @remarks This function must be called before previewing (see camera_start_preview()).
 *
 * @param[in] camera	The handle to the camera
 * @param[in] type	The display type
 * @param[in] display	The display handle from #GET_DISPLAY()
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre    The camera state must be #CAMERA_STATE_CREATED
 *
 * @see camera_start_preview()
 * @see GET_DISPLAY()
 */
int camera_set_display(camera_h camera, camera_display_type_e type, camera_display_h display);

/**
 * @brief Sets the resolution of preview.
 *
 * @remarks  This function should be called before previewing (camera_start_preview()).
 *
 * @param[in] camera	The handle to the camera
 * @param[in] width	The preview width
 * @param[in] height	The preview height
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre    The camera state must be #CAMERA_STATE_CREATED
 *
 * @see camera_start_preview()
 * @see	camera_get_preview_resolution()
 * @see	camera_foreach_supported_preview_resolution()
 */
int camera_set_preview_resolution(camera_h camera, int width, int height);

/**
 * @brief Gets the resolution of preview.
 *
 * @param[in] camera	The handle to the camera
 * @param[out] width	The preview width
 * @param[out] height	The preview height
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_set_preview_resolution()
 * @see	camera_foreach_supported_preview_resolution()
 */
int camera_get_preview_resolution(camera_h camera, int *width, int *height);

/**
 * @brief Gets the recommended preview resolution
 *
 * @remarks Depend on capture resolution aspect ratio and display resolution, the recommended preview resolution is determined.
 *
 * @param[in] camera	The handle to the camera
 * @param[out] width	The preview width
 * @param[out] height	The preview height
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_set_preview_resolution()
 * @see	camera_foreach_supported_preview_resolution()
 */
int camera_get_recommended_preview_resolution(camera_h camera, int *width, int *height);

/**
 * @brief Starts the face detection.
 * @remarks
 * This should be called after preview is started.\n
 * The callback will invoked when face detected in preview frame.\n
 * Internally starting continuous focus and focusing on detected face.\n
 * When the face detection is running, camera_start_focusing(), camera_cancel_focusing(), camera_attr_set_af_mode(), 	camera_attr_set_af_area(), camera_attr_set_exposure_mode() and camera_attr_set_whitebalance() settings are ignored.\n
 * If invoke camera_stop_preview(), face detection is stopped. and then resuming preview with camera_start_preview(), you should call this method again to resume face detection.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] callback  The callback for notify detected face
 * @param[in] user_data   The user data to be passed to the callback function
 *
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval    #CAMERA_ERROR_INVALID_STATE Not preview state
 * @retval    #CAMERA_ERROR_INVALID_OPERATION Not supported this feature
 *
 * @pre    The camera state must be #CAMERA_STATE_PREVIEW
 *
 * @see camera_stop_face_detection()
 * @see camera_face_detected_cb()
 * @see camera_is_supported_face_detection()
 */
int camera_start_face_detection(camera_h camera, camera_face_detected_cb callback, void * user_data);

/**
 * @brief Stops the face detection.
 *
 * @param[in] camera	The handle to the camera
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre    This should be called after face detection was started.
 *
 * @see camera_start_face_detection()
 * @see camera_is_supported_face_detection()
 */
int camera_stop_face_detection(camera_h camera);

/**
 * @brief Zooming on the detected face
 *
 * @remarks The face id is getting from camera_face_detected_cb().\n
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] face_id	The face id to zoom
 *
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval    #CAMERA_ERROR_INVALID_STATE face zoom was already enabled.
 * @retval    #CAMERA_ERROR_INVALID_OPERATION Not supported this feature
 *
 * @pre This should be called after face detection was started.
 *
 * @see camera_cancel_face_zoom()
 * @see camera_start_face_detection()
 */
int camera_face_zoom(camera_h camera, int face_id);

/**
 * @brief Cancel zooming on the face
 *
 * @param[in]	camera	The handle to the camera
 *
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_face_zoom()
 * @see camera_start_face_detection()
 */
int camera_cancel_face_zoom(camera_h camera);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported camera preview resolutions by invoking callback function once for each supported camera preview resolution.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] 	callback    The callback function to invoke
 * @param[in] 	user_data	The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes camera_supported_preview_resolution_cb() repeatly to retrieve each supported preview resolution.
 *
 * @see	camera_set_preview_resolution()
 * @see	camera_get_preview_resolution()
 * @see	camera_supported_preview_resolution_cb()
 */
int camera_foreach_supported_preview_resolution(camera_h camera,
        camera_supported_preview_resolution_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */


/**
 * @brief Sets the display rotation.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 *
 *
 * @remarks  This function should be called before previewing (see camera_start_preview())\n
 * This function is valid only for #CAMERA_DISPLAY_TYPE_X11
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   rotation The display rotation
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Display type is not X11
 *
 * @see camera_start_preview()
 * @see	camera_get_x11_display_rotation()
 */
int camera_set_x11_display_rotation(camera_h camera, camera_rotation_e rotation);

/**
 * @brief Gets the display rotation.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 *
 * @remarks  This function is valid only for #CAMERA_DISPLAY_TYPE_X11.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  rotation  The display rotation
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_set_x11_display_rotation()
 */
int camera_get_x11_display_rotation(camera_h camera, camera_rotation_e *rotation);

/**
 * @brief Sets the display flip.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 *
 *
 * @remarks  This function is valid only for #CAMERA_DISPLAY_TYPE_X11
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] flip The display flip
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Display type is not X11
 *
 * @see	camera_get_x11_display_flip()
 */
int camera_set_x11_display_flip(camera_h camera, camera_flip_e flip);

/**
 * @brief Gets the display flip.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 *
 * @remarks  This function is valid only for #CAMERA_DISPLAY_TYPE_X11.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  flip  The display flip
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_set_x11_display_flip()
 */
int camera_get_x11_display_flip(camera_h camera, camera_flip_e *flip);


/**
 * @brief Sets the visible property for X11 display.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 *
 * @remarks  This function is valid only for #CAMERA_DISPLAY_TYPE_X11.
 * @param[in] camera	The handle to the camera
 * @param[in] visible	The display visibility property
 *
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_is_x11_display_visible()
 */
int camera_set_x11_display_visible(camera_h camera, bool visible);

/**
 * @brief Gets the visible property of X11 display.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 *
 * @remarks  This function is valid only for #CAMERA_DISPLAY_TYPE_X11
 * @param[in] camera	The handle to the camera
 * @param[out] visible	@c true if camera display is visible, otherwise @c false
 *
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_set_x11_display_visible()
 */
int camera_is_x11_display_visible(camera_h camera, bool *visible);


/**
 * @brief Sets the X11 display aspect ratio.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 *
 * @remarks  This function is valid only for #CAMERA_DISPLAY_TYPE_X11
 * @param[in] camera	The handle to the camera
 * @param[in] ratio	The display apect ratio
 *
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_get_x11_display_mode()
 */
int camera_set_x11_display_mode(camera_h camera , camera_display_mode_e mode);


/**
 * @brief Gets the X11 display aspect ratio.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 *
 * @remarks  This function is valid only for #CAMERA_DISPLAY_TYPE_X11.
 * @param[in] camera	The handle to the camera
 * @param[out] ratio	The display apect ratio
 *
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_set_x11_display_mode()
 */
int camera_get_x11_display_mode(camera_h camera, camera_display_mode_e *mode);


/**
 * @brief Sets the resolution of capture image.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] width	The capture width
 * @param[in] height	The capture height
 * @return		0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre         The camera state must be #CAMERA_STATE_CREATED or #CAMERA_STATE_PREVIEW.
 *
 * @see camera_start_capture()
 * @see	camera_get_capture_resolution()
 * @see	camera_foreach_supported_capture_resolution()
 */
int camera_set_capture_resolution(camera_h camera, int width, int height);


/**
 * @brief Gets the resolution of capture image.
 *
 *
 * @param[in] camera	The handle to the camera
 * @param[out] width	The capture width
 * @param[out] height	The capture height
 * @return	   0 on success, otherwise a negative error value.
 * @retval     #CAMERA_ERROR_NONE Successful
 * @retval     #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_set_capture_resolution()
 * @see camera_foreach_supported_capture_resolution()
 */
int camera_get_capture_resolution(camera_h camera, int *width, int *height);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported camera capture resolutions by invoking the callback function once for each supported camera capture resolution.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes camera_supported_capture_resolution_cb() repeatly to retrieve each supported capture resolution.
 *
 * @see camera_set_capture_resolution()
 * @see camera_get_capture_resolution()
 * @see	camera_supported_capture_resolution_cb()
 */
int camera_foreach_supported_capture_resolution(camera_h camera,
        camera_supported_capture_resolution_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */


/**
 * @brief Sets the format of an image to capture.
 *
 * @remarks  This function should be called before capturing (see camera_start_capture()).
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  format  The format of capture image
 *
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 *
 * @pre	The camera state must be CAMERA_STATE_CREATED or CAMERA_STATE_PREVIEW.
 *
 * @see camera_start_capture()
 * @see	camera_get_capture_format()
 * @see	camera_foreach_supported_capture_format()
 */
int camera_set_capture_format(camera_h camera, camera_pixel_format_e format);

/**
 * @brief Gets the format of capture image to capture.
 *
 * @param[in] camera	The handle to the camera
 * @param[out] format	The format of capture image
 * @return	   0 on success, otherwise a negative error value.
 * @retval     #CAMERA_ERROR_NONE Successful
 * @retval     #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_set_capture_format()
 * @see	camera_foreach_supported_capture_format()
 */
int camera_get_capture_format(camera_h camera, camera_pixel_format_e *format);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported camera capture formats by invoking callback function once for each supported camera capture format.
 *
 *
 * @param[in] camera	The handle to the camera
 * @param[in] callback	The callback function to invoke
 * @param[in] user_data	The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes camera_supported_capture_format_cb() repeatdly to retrieve each supported capture format.
 *
 * @see	camera_set_capture_format()
 * @see	camera_get_capture_format()
 * @see	camera_supported_capture_format_cb()
 */
int camera_foreach_supported_capture_format(camera_h camera,
        camera_supported_capture_format_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */

/**
 * @brief Sets the preview data format.
 *
 *
 * @remarks  This function should be called before previewing (see camera_start_preview()).
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  format  The preview data format
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre         The camera state must be CAMERA_STATE_CREATED
 *
 * @see camera_start_preview()
 * @see	camera_get_preview_format()
 * @see	camera_foreach_supported_preview_format()
 */
int camera_set_preview_format(camera_h camera, camera_pixel_format_e format);

/**
 * @brief Gets the format of preview stream.
 *
 * @param[in] camera	The handle to the camera
 * @param[out] format	The preview data format
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_set_preview_format()
 * @see	camera_foreach_supported_preview_format()
 */
int camera_get_preview_format(camera_h camera, camera_pixel_format_e *format);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported camera preview formats by invoking callback function once for each supported camera preview format.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] callback	The callback function to invoke
 * @param[in] user_data	The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes camera_supported_preview_format_cb() repeatly to retrieve each supported preview format.
 *
 * @see	camera_set_preview_format()
 * @see	camera_get_preview_format()
 * @see	camera_supported_preview_format_cb()
 */
int camera_foreach_supported_preview_format(camera_h camera,
        camera_supported_preview_format_cb callback, void *user_data);


/**
 * @biref Gets face detection feature supported state
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @param[in]	camera The handle to the camera
 * @return true on supported, otherwise false
 *
 * @see camera_start_face_detection()
 * @see camera_stop_face_detection()
 */
bool camera_is_supported_face_detection(camera_h camera);

/**
 * @biref Gets zero shutter lag feature supported state
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @remarks If supporting zero shutter lag, you can do continuous shot with full capture size
 * @param[in]	camera The handle to the camera
 * @return true on supported, otherwise false
 *
 */
bool camera_is_supported_zero_shutter_lag(camera_h camera);

/**
 * @biref Gets camera device count
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @remarks If device supports primary and secondary camera, this returns 2. If 1 is returned, device supports only primary camera.
 * @param[in]	camera		The handle to the camera
 * @param[out]	device count	Device count
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 */
int camera_get_device_count(camera_h camera, int *device_count);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */

/**
 * @brief	Registers a callback function to be called once per frame when previewing.
 *
 * @remarks This callback does not work in video recorder mode.\n
 * This function should be called before previewing (see camera_start_preview())\n
 * registered callback is called on internal thread of camera.\n
 * You can retrieve video frame using registered callback.
 * The callback function holds the same buffer that will be drawn on the display device.
 * So if you change the buffer, it will be displayed on the device.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] callback    The callback function to register
 * @param[in] user_data   The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre		The camera state should be #CAMERA_STATE_CREATED.
 *
 * @see	camera_start_preview()
 * @see camera_unset_preview_cb()
 * @see	camera_preview_cb()
 */
int camera_set_preview_cb(camera_h camera, camera_preview_cb callback, void *user_data);

/**
 * @brief	Unregisters the callback function.
 *
 * @param[in]	camera	The handle to the camera
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_set_preview_cb()
 */
int camera_unset_preview_cb(camera_h camera);

/**
 * @brief	Registers a callback function to be called when camera state changes.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] callback	  The callback function to register
 * @param[in] user_data   The user data to be passed to the callback function
 *
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	 This function will invoke camera_state_changed_cb() when camera state changes.
 *
 * @see camera_unset_state_changed_cb()
 * @see	camera_state_changed_cb()
 */
int camera_set_state_changed_cb(camera_h camera, camera_state_changed_cb callback,
        void *user_data);

/**
 * @brief	Unregisters the callback function.
 *
 * @param[in]	camera	The handle to the camera
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see     camera_set_state_changed_cb()
 */
int camera_unset_state_changed_cb(camera_h camera);

/**
 * @brief	Registers a callback function to be called when camera interrupted by policy.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] callback	  The callback function to register
 * @param[in] user_data   The user data to be passed to the callback function
 *
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_unset_interrupted_cb()
 * @see	camera_interrupted_cb()
 */
int camera_set_interrupted_cb(camera_h camera, camera_interrupted_cb callback,
	    void *user_data);

/**
 * @brief	Unregisters the callback function.
 *
 * @param[in]	camera	The handle to the camera
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see     camera_set_interrupted_cb()
 */
int camera_unset_interrupted_cb(camera_h camera);


/**
 * @brief	Registers a callback function to be called when auto-focus state changes.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] callback	The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function will invoke camera_focus_changed_cb() when auto-focus state changes.
 *
 * @see	camera_start_focusing()
 * @see	camera_cancel_focusing()
 * @see	camera_unset_focus_changed_cb()
 * @see	camera_focus_changed_cb()
 */
int camera_set_focus_changed_cb(camera_h camera, camera_focus_changed_cb callback,
        void *user_data);


/**
 * @brief	Unregisters the callback function.
 *
 * @param[in]	camera	The handle to the camera
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see     camera_set_focus_changed_cb()
 */
int camera_unset_focus_changed_cb(camera_h camera);

/**
 * @brief	Registers a callback function to be called when an asynchronous operation error occurred.
 *
 * @remarks
 * This callback inform critical error situation.\n
 * When invoked this callback, user should release the resource and terminate application.\n
 * These error code will be occurred\n
 * #CAMERA_ERROR_DEVICE\n
 * #CAMERA_ERROR_INVALID_OPERATION\n
 * #CAMERA_ERROR_OUT_OF_MEMORY\n
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]	callback	The callback function to register
 * @param[in]	user_data	The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function will invoke camera_error_cb() when an asynchronous operation error occur.
 *
 * @see camera_unset_error_cb()
 * @see	camera_error_cb()
 */
int camera_set_error_cb(camera_h camera, camera_error_cb callback, void *user_data);


/**
 * @brief	Unregisters the callback function.
 *
 * @param[in]	camera	The handle to the camera
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see     camera_set_error_cb()
 */
int camera_unset_error_cb(camera_h camera);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Called to get each supported auto-focus mode.
 *
 * @param[in] mode The supported auto-focus mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_attr_foreach_supported_af_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_af_mode()
 */
typedef bool (*camera_attr_supported_af_mode_cb)(camera_attr_af_mode_e mode, void *user_data);

/**
 * @brief Called to get each supported exposure mode.
 *
 * @param[in] mode The supported exposure mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_attr_foreach_supported_exposure_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_exposure_mode()
 * @see	#camera_attr_exposure_mode_e
 */
typedef bool (*camera_attr_supported_exposure_mode_cb)(camera_attr_exposure_mode_e mode,
        void *user_data);

/**
 * @brief Called to get each supported ISO mode.
 *
 * @param[in] iso The supported iso mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_attr_foreach_supported_iso() will invoke this callback.
 * @see	camera_attr_foreach_supported_iso()
 */
typedef bool (*camera_attr_supported_iso_cb)(camera_attr_iso_e iso, void *user_data);

/**
 * @brief Called to get each supported white balance.
 *
 * @param[in] wb The supported white balance mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_attr_foreach_supported_whitebalance() will invoke this callback.
 * @see	camera_attr_foreach_supported_whitebalance()
 * @see	#camera_attr_whitebalance_e
 */
typedef bool (*camera_attr_supported_whitebalance_cb)(camera_attr_whitebalance_e wb,
        void *user_data);

/**
 * @brief Called to get each supported effect mode.
 *
 * @param[in] effect	The supported effect mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_attr_foreach_supported_effect() will invoke this callback.
 * @see	camera_attr_foreach_supported_effect()
 */
typedef bool (*camera_attr_supported_effect_cb)(camera_attr_effect_mode_e effect,
        void *user_data);

/**
 * @brief Called to get each supported scene mode.
 *
 * @param[in] mode The supported scene mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_attr_foreach_supported_scene_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_scene_mode()
 * @see	#camera_attr_scene_mode_e
 */
typedef bool (*camera_attr_supported_scene_mode_cb)(camera_attr_scene_mode_e mode,
        void *user_data);

/**
 * @brief Called to get each supported flash mode.
 *
 * @param[in] mode The supported flash mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_attr_foreach_supported_flash_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_flash_mode()
 */
typedef bool (*camera_attr_supported_flash_mode_cb)(camera_attr_flash_mode_e mode,
        void *user_data);

/**
 * @brief Called to get each supported FPS mode.
 *
 * @param[in] mode The supported FPS mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break outsp of the loop.
 * @pre		camera_attr_foreach_supported_fps() will invoke this callback.
 * @see	camera_attr_foreach_supported_fps()
 */
typedef bool (*camera_attr_supported_fps_cb)(camera_attr_fps_e fps, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the preview frame rate.
 *
 * @remarks  This function should be called before previewing (see camera_start_preview()).
 *
 * @param[in] camera	The handle to the camera
 * @param[in] fps	The frame rate
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 *
 * @see camera_start_preview()
 * @see	camera_attr_get_preview_fps()
 * @see	camera_attr_foreach_supported_fps()
 */
int camera_attr_set_preview_fps(camera_h camera, camera_attr_fps_e fps);

/**
 * @brief Gets the frames per second of a preview video stream.
 *
 * @param[in] camera	The handle to the camera
 * @param[out] fps  The frames per second of preview video stream
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_set_preview_fps()
 * @see	camera_attr_foreach_supported_fps()
 */
int camera_attr_get_preview_fps(camera_h camera, camera_attr_fps_e *fps);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported FPS modes by invoking callback function once for each supported FPS mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   callback  The callback function to invoke
 * @param[in]   user_data   The user data to be passed to the callback function
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post		This function invokes camera_attr_supported_fps_cb() repeatly to get each supported FPS mode.
 *
 * @see	camera_attr_set_preview_fps()
 * @see	camera_attr_get_preview_fps()
 * @see	camera_attr_supported_fps_cb()
 */
int camera_attr_foreach_supported_fps(camera_h camera, camera_attr_supported_fps_cb callback,
        void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the image quality.
 *
 * @details The range for image quality is 1 to 100. If @a quality is out of range, #CAMERA_ERROR_INVALID_PARAMETER error occurred.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] quality   The quality of image (1 ~ 100)
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre    The camera state must be #CAMERA_STATE_CREATED, #CAMERA_STATE_PREVIEW.
 *
 * @see camera_start_preview()
 * @see	camera_attr_get_image_quality()
 */
int camera_attr_set_image_quality(camera_h camera, int quality);

/**
 * @brief Gets the quality of capturing a still image.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out] quality	The quality of image(1 ~ 100)
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_image_quality()
 */
int camera_attr_get_image_quality(camera_h camera, int *quality);

/**
 * @brief Sets the zoom level.
 * @details The range for zoom level is getting from camera_attr_get_zoom_range(). If @a zoom is out of range, #CAMERA_ERROR_INVALID_PARAMETER error occurred.
 *
 * @param[in] camera	The handle to the camera
 * @param[in] zoom	The zoom level
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_get_zoom()
 * @see camera_attr_get_zoom_range()
 */
int camera_attr_set_zoom(camera_h camera, int zoom);

/**
 * @brief Gets the zoom level.
 *
 * @param[in] camera	The handle to the camera
 * @param[out] zoom	The zoom level
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_zoom()
 * @see camera_attr_get_zoom_range()
 */
int camera_attr_get_zoom(camera_h camera, int *zoom);

/**
 * @brief Gets the available zoom level.
 *
 * @param[in] camera	The handle to the camera
 * @param[out] min	The minimum zoom level
 * @param[out] max	The maximum zoom level
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_zoom()
 * @see camera_attr_get_zoom()
 */
int camera_attr_get_zoom_range(camera_h camera , int *min , int *max);


/**
 * @brief Sets the auto focus mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] mode	The auto focus mode
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_get_af_mode()
 * @see	camera_attr_foreach_supported_af_mode()
 * @see	#camera_attr_af_mode_e
 */
int camera_attr_set_af_mode(camera_h camera, camera_attr_af_mode_e mode);

/**
 * @brief Gets the auto focus mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out] mode	Auto focus mode
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_foreach_supported_af_mode()
 * @see camera_attr_set_af_mode()
 * @see	#camera_attr_af_mode_e
 */
int camera_attr_get_af_mode(camera_h camera, camera_attr_af_mode_e *mode);

/**
 * @brief Sets auto focus area
 *
 * @remarks This API is invalid in CAMERA_ATTR_AF_NONE mode.\n
 * The coordinates are mapped into preview area
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] x The x coordinates of focus area
 * @param[in] y The y coordinates of focus area
 *
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see camera_attr_set_af_mode()
 * @see camera_attr_clear_af_area()
 */
int camera_attr_set_af_area(camera_h camera, int x, int y);

/**
 * @brief Clear the auto focus area.
 *
 * @remarks The focusing area set to the center area
 *
 * @param[in]	camera	The handle to the camera
 *
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 *
 * @see camera_attr_set_af_mode()
 * @see camera_attr_set_af_area()
 *
 */
int camera_attr_clear_af_area(camera_h camera);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported auto focus modes by invoking callback function once for each supported auto focus mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   callback  The callback function to invoke
 * @param[in]   user_data   The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post	This function invokes camera_attr_supported_af_mode_cb() to get all supported auto focus modes.
 *
 * @see camera_attr_set_af_mode()
 * @see camera_attr_get_af_mode()
 * @see	camera_attr_supported_af_mode_cb()
 */
int camera_attr_foreach_supported_af_mode(camera_h camera,
        camera_attr_supported_af_mode_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the exposure mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] mode The exposure mode
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_get_exposure_mode()
 * @see camera_attr_foreach_supported_exposure_mode()
 */
int camera_attr_set_exposure_mode(camera_h camera, camera_attr_exposure_mode_e mode);

/**
 * @brief Gets the exposure mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out] mode Exposure mode
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_exposure_mode()
 * @see camera_attr_foreach_supported_exposure_mode()
 */
int camera_attr_get_exposure_mode(camera_h camera, camera_attr_exposure_mode_e *mode);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported exposure modes by invoking callback function once for each supported exposure mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   callback	The callback function to invoke
 * @param[in]	user_data	The user data to be passed to the callback function
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post		This function invokes camera_attr_supported_exposure_mode_cb() to get all supported exposure modes.
 *
 * @see camera_attr_set_exposure_mode()
 * @see camera_attr_get_exposure_mode()
 * @see	camera_attr_supported_exposure_mode_cb()
 */
int camera_attr_foreach_supported_exposure_mode(camera_h camera,
        camera_attr_supported_exposure_mode_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the exposure value.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]	value	The exposure value
 * @return	  0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_get_exposure()
 */
int camera_attr_set_exposure(camera_h camera, int value);

/**
 * @brief Gets the exposure value.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  value    Exposure value
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_set_exposure()
 */
int camera_attr_get_exposure(camera_h camera, int *value);

/**
 * @brief Gets the available exposure value.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  min The minimum exposure value
 * @param[out]  max The maximum exposure value
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_set_exposure()
 */
int camera_attr_get_exposure_range(camera_h camera, int *min, int *max);

/**
 * @brief Sets the ISO level.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   iso	The ISO Level
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_get_iso()
 * @see camera_attr_foreach_supported_iso()
 */
int camera_attr_set_iso(camera_h camera, camera_attr_iso_e iso);

/**
 * @brief Gets the ISO level.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  iso	ISO Level
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_set_iso()
 * @see camera_attr_foreach_supported_iso()
 */
int camera_attr_get_iso(camera_h camera, camera_attr_iso_e *iso);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported ISO levels by invoking callback function once for each supported ISO level.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   callback	The callback function to invoke
 * @param[in]   user_data	The user data to be passed to the callback function
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post		This function invokes camera_attr_supported_iso_cb() to get all supported ISO levels.
 *
 * @see	camera_attr_set_iso()
 * @see camera_attr_get_iso()
 * @see	camera_attr_supported_iso_cb()
 */
int camera_attr_foreach_supported_iso(camera_h camera, camera_attr_supported_iso_cb callback,
        void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */


/**
 * @brief Sets the theater mode
 *
 * @remarks If you want to display preview image on external display with full screen mode, use this function.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]	mode	The mode to change
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @pre		This function is valid only when external display was connected.
 *
 * @see	camera_attr_get_theater_mode()
 */
int camera_attr_set_theater_mode(camera_h camera, camera_attr_theater_mode_e mode);

/**
 * @brief Gets the theater mode
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]	mode	Currnet theater mode
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_get_theater_mode()
 */
int camera_attr_get_theater_mode(camera_h camera, camera_attr_theater_mode_e *mode);


/**
 * @brief Sets the brightness level.
 *
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   level   The brightness level
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_get_brightness()
 * @see camera_attr_get_brightness_range()
 */
int camera_attr_set_brightness(camera_h camera, int level);

/**
 * @brief Gets the brightness level.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  level   The brightness level
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_brightness()
 * @see camera_attr_get_brightness_range()
 */
int camera_attr_get_brightness(camera_h camera, int *level);

/**
 * @brief Gets the available brightness level.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  min   The minimum brightness level
 * @param[out]  max   The maximum brightness level
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_brightness()
 * @see camera_attr_get_brightness()
 */
int camera_attr_get_brightness_range(camera_h camera, int *min, int *max);

/**
 * @brief Sets the contrast level.
 *
 * @param[in]   camera  The handle to the camera
 * @param[in]  level   The contrast level
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_get_contrast()
 * @see camera_attr_get_contrast_range()
 */
int camera_attr_set_contrast(camera_h camera, int level);


/**
 * @brief Gets the contrast level.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  level   The contrast level
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_contrast()
 * @see camera_attr_get_contrast_range()
 */
int camera_attr_get_contrast(camera_h camera, int *level);

/**
 * @brief Gets the available contrast level.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  min   The minimum contrast level
 * @param[out]  max   The maximum contrast level
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_contrast()
 * @see camera_attr_get_contrast()
 */
int camera_attr_get_contrast_range(camera_h camera, int *min , int *max);

/**
 * @brief Sets the white balance mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   whitebalance      The white balance mode
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_foreach_supported_whitebalance()
 * @see camera_attr_get_whitebalance()
 */
int camera_attr_set_whitebalance(camera_h camera, camera_attr_whitebalance_e whitebalance);


/**
 * @brief Gets the white balance mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  whitebalance	The white balance mode
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_foreach_supported_whitebalance()
 * @see camera_attr_set_whitebalance()
 */
int camera_attr_get_whitebalance(camera_h camera, camera_attr_whitebalance_e *whitebalance);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported white balances by invoking callback function once for each supported white balance.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   callback  The callback function to invoke
 * @param[in]   user_data   The user data to be passed to the callback function
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post		This function invokes camera_attr_supported_whitebalance_cb() to get all supported white balances.
 *
 * @see camera_attr_set_whitebalance()
 * @see camera_attr_get_whitebalance()
 * @see	camera_attr_supported_whitebalance_cb()
 */
int camera_attr_foreach_supported_whitebalance(camera_h camera,
        camera_attr_supported_whitebalance_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the camera effect mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   effect  The camera effect mode
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_foreach_supported_effect()
 * @see camera_attr_get_effect()
 */
int camera_attr_set_effect(camera_h camera, camera_attr_effect_mode_e effect);


/**
 * @brief Gets the camera effect mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  effect   The camera effect mode
 * @return      0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_foreach_supported_effect()
 * @see camera_attr_set_effect()
 */
int camera_attr_get_effect(camera_h camera, camera_attr_effect_mode_e *effect);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported effect modes by invoking callback function once for each supported effect mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   callback  The callback function to invoke
 * @param[in]   user_data   The user data to be passed to the callback function
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post		This function invokes camera_attr_supported_effect_cb() to get all supported effect modes.
 *
 * @see camera_attr_set_effect()
 * @see camera_attr_get_effect()
 * @see	camera_attr_supported_effect_cb()
 */
int camera_attr_foreach_supported_effect(camera_h camera,
        camera_attr_supported_effect_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the scene mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   mode    The scene mode
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_foreach_supported_scene_mode()
 * @see camera_attr_get_scene_mode()
 */
int camera_attr_set_scene_mode(camera_h camera, camera_attr_scene_mode_e mode);

/**
 * @brief Gets the scene mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  mode    The scene mode
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_foreach_supported_scene_mode()
 * @see camera_attr_set_scene_mode()
 */
int camera_attr_get_scene_mode(camera_h camera, camera_attr_scene_mode_e *mode);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported scene modes by invoking callback function once for each supported scene mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   callback  The callback function to invoke
 * @param[in]   user_data   The user data to be passed to the callback function
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post		This function invokes camera_attr_supported_scene_mode_cb() to get all supported scene modes.
 *
 * @see	camera_attr_set_scene_mode()
 * @see camera_attr_get_scene_mode()
 * @see camera_attr_supported_scene_mode_cb()
 */
int camera_attr_foreach_supported_scene_mode(camera_h camera,
        camera_attr_supported_scene_mode_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Enables to write EXIF(Exchangeable image file format) tags in a JPEG file.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   enable    @c true to enable write EXIF tags in a JPEG file, otherwise @c false
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_attr_is_enabled_tag()
 */
int camera_attr_enable_tag(camera_h camera, bool enable);

/**
 * @brief Gets the value that indicates whether to write EXIF(Exchangeable image file format) tags in a JPEG file is enabled.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  enabled  @c true if camera information is enabled, otherwise @c false
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_attr_enable_tag()
 */
int camera_attr_is_enabled_tag(camera_h camera, bool *enabled);

/**
 * @brief Sets a camera image description in EXIF(Exchangeable image file format) tag.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   description The string with description
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_attr_get_tag_image_description()
 */
int camera_attr_set_tag_image_description(camera_h camera, const char *description);

/**
 * @brief Gets the camera image description in EXIF(Exchangeable image file format) tag.
 *
 * @remarks @a description must be released with free() by you.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]   description  A pointer to a string
 * @return       0 on success, otherwise a negative error value.
 * @retval       #CAMERA_ERROR_NONE Successful
 * @retval       #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see          camera_attr_set_tag_image_description()
 */
int camera_attr_get_tag_image_description(camera_h camera, char **description);

/**
 * @brief Sets the camera orientation in EXIF(Exchangeable image file format) tag.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   orientation The information of the camera orientation
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_get_tag_orientation()
 */
int camera_attr_set_tag_orientation(camera_h camera, camera_attr_tag_orientation_e orientation);

/**
 * @brief Gets the camera orientation in EXIF(Exchangeable image file format) tag.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  orientation The information of the camera orientation
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_set_tag_orientation()
 */
int camera_attr_get_tag_orientation(camera_h camera, camera_attr_tag_orientation_e *orientation);

/**
 * @brief Sets the software information in EXIF(Exchangeable image file format) tag.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   software    The software information tag
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_attr_get_tag_software()
 */
int camera_attr_set_tag_software(camera_h camera, const char *software);

/**
 * @brief Gets the software information in EXIF(Exchangeable image file format) tag.
 *
 * @remarks @a software must be released with free() by you.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]   software    A pointer to a string
 * @return       0 on success, otherwise a negative error value.
 * @retval       #CAMERA_ERROR_NONE Successful
 * @retval       #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see          camera_attr_set_tag_software()
 */
int camera_attr_get_tag_software(camera_h camera, char **software);

/**
 * @brief Sets the geotag(GPS data) in EXIF(Exchangeable image file format) tag.
 *
 * @param[in]	camera The handle to the camera
 * @param[in] latitude Latitude data
 * @param[in] longitude Longitude data
 * @param[in] altitude Altitude data
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_attr_get_geotag()
 * @see         camera_attr_remove_geotag()
 */
int camera_attr_set_geotag(camera_h camera, double latitude , double longitude, double altitude);

/**
 * @brief Gets the geotag(GPS data) in EXIF(Exchangeable image file format) tag.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out] latitude Latitude data
 * @param[out] longitude Longitude data
 * @param[out] altitude Altitude data
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_attr_set_geotag()
 * @see         camera_attr_remove_geotag()
 */
int camera_attr_get_geotag(camera_h camera, double *latitude , double *longitude, double *altitude);

/**
 * @brief Remove the geotag(GPS data) in EXIF(Exchangeable image file format) tag.
 *
 * @param[in]	camera	The handle to the camera
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see         camera_attr_set_geotag()
 * @see         camera_attr_get_geotag()
 */
int camera_attr_remove_geotag(camera_h camera);

/**
 * @brief Sets the camera flash mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   mode    The flash mode
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see	camera_attr_foreach_supported_flash_mode()
 * @see camera_attr_get_flash_mode()
 */
int camera_attr_set_flash_mode(camera_h camera, camera_attr_flash_mode_e mode);

/**
 * @brief Gets the camera flash mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out]  mode    The flash mode
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_foreach_supported_flash_mode()
 * @see camera_attr_set_flash_mode()
 */
int camera_attr_get_flash_mode(camera_h camera, camera_attr_flash_mode_e *mode);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported flash modes by invoking callback funcion once for each supported flash mode.
 *
 * @param[in]	camera	The handle to the camera
 * @param[in]   callback  The callback function to invoke
 * @param[in]   user_data   The user data passed to the callback registration function
 * @return      0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @post		This function invokes camera_attr_supported_flash_mode_cb() to get all supported flash modes.
 *
 * @see	camera_attr_set_flash_mode()
 * @see camera_attr_get_flash_mode()
 * @see	camera_attr_supported_flash_mode_cb()
 */
int camera_attr_foreach_supported_flash_mode(camera_h camera,
        camera_attr_supported_flash_mode_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Gets the camera lens orientation angle.
 *
 * @param[in]	camera	The handle to the camera
 * @param[out] angle The orientation angle
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_set_x11_display_rotation()
 */
int camera_attr_get_lens_orientation(camera_h camera, int *angle);


/**
 * @brief Sets stream rotation
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] rotation	The stream rotation
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre	The camera state must be CAMERA_STATE_CREATED.
 *
 * @see camera_attr_get_stream_rotation()
 */
int camera_attr_set_stream_rotation(camera_h camera , camera_rotation_e rotation);

/**
 * @brief Gets stream rotation
 *
 * @param[in]	camera	The handle to the camera
 * @param[out] rotation	The stream rotation
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre	The camera state must be CAMERA_STATE_CREATED.
 *
 * @see camera_attr_set_stream_rotation()
 */
int camera_attr_get_stream_rotation(camera_h camera , camera_rotation_e *rotation);



/**
 * @brief Sets stream flip
 *
 * @param[in]	camera	The handle to the camera
 * @param[in] flip  The stream flip
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre	The camera state must be CAMERA_STATE_CREATED.
 *
 * @see camera_attr_set_stream_rotation()
 */
int camera_attr_set_stream_flip(camera_h camera , camera_flip_e flip);

/**
 * @brief Gets stream flip
 *
 * @param[in]	camera	The handle to the camera
 * @param[out] flip  The stream flip
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @pre	The camera state must be CAMERA_STATE_CREATED.
 *
 * @see camera_attr_set_stream_rotation()
 */
int camera_attr_get_stream_flip(camera_h camera , camera_flip_e *flip);



/**
 * @brief	Called when HDR capture process was updated
 *
 * @param[in] percent         The progress percent of HDR capture
 * @param[in] user_data     The user data passed from the callback registration function
 * @pre camera_start_capture() will invoke this callback if you register it using camera_attr_set_hdr_capture_progress_cb().
 *
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 */
typedef void (*camera_attr_hdr_progress_cb)(int percent, void *user_data);


/**
 * @brief Sets the mode of HDR(High dynamic range) capture.
 * @remarks
 * Taking multiple pictures at different exposure level and intelligently stitching them together so that we eventually arrive at a picture that is representative in both dark and bright areas.\n
 * If this attribute is setting true. camera_attr_hdr_progress_cb is invoked when capture.\n
 * If you set #CAMERA_ATTR_HDR_MODE_KEEP_ORIGINAL, the capturing callback is invoked twice. The first callback is delivering origin image data. The second callback is delivering improved image data.
 *
 * @param[in]	camera The handle to the camera
 * @param[in]	mode The mode of HDR capture
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 *
 */
int camera_attr_set_hdr_mode(camera_h camera, camera_attr_hdr_mode_e mode);

/**
 * @brief Gets the mode of HDR(High dynamic range) capture.
 *
 * @param[in]	camera The handle to the camera
 * @param[out]	mode The mode of HDR capture
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 *
 */
int camera_attr_get_hdr_mode(camera_h camera, camera_attr_hdr_mode_e *mode);

__attribute__ ((deprecated)) int camera_attr_enable_hdr_capture(camera_h camera, bool enable);
__attribute__ ((deprecated)) int camera_attr_is_enabled_hdr_capture(camera_h camera, bool *enabled);

/**
 * @brief	Registers a callback function to be called when HDR capture is progressing.
 * @remarks This callback notify progress of HDR process.
 *
 * @param[in]	camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data passed to the callback registration function
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_hdr_mode()
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 */
int camera_attr_set_hdr_capture_progress_cb(camera_h camera, camera_attr_hdr_progress_cb callback, void* user_data);



/**
 * @brief	Unregisters the callback function.
 *
 * @param[in]	camera The handle to the camera
 * @return	  0 on success, otherwise a negative error value.
 * @retval    #CAMERA_ERROR_NONE Successful
 * @retval    #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_set_hdr_mode()
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 */
int camera_attr_unset_hdr_capture_progress_cb(camera_h camera);

/**
 * @biref Gets HDR capture supported state
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @param[in]	camera The handle to the camera
 * @return true on supported, otherwise false
 *
 * @see camera_attr_set_hdr_mode()
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 */
bool camera_attr_is_supported_hdr_capture(camera_h camera);

/**
 * @brief Enable/Disable Anti-shake feature
 *
 * @remarks  This feature used for image capture
 * @param[in]	camera The handle to the camera
 * @param[in]	enable The state of anti-shake
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_is_enabled_anti_shake()
 * @see camera_attr_is_supported_anti_shake()
 *
 */
int camera_attr_enable_anti_shake(camera_h camera, bool enable);

/**
 * @brief Gets state of Anti-shake feature
 *
 * @param[in]	camera The handle to the camera
 * @param[out]	enabled The state of anti-shake
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_enable_anti_shake()
 * @see camera_attr_is_supported_anti_shake()
 *
 */
int camera_attr_is_enabled_anti_shake(camera_h camera , bool *enabled);

/**
 * @biref Gets Anti-shake feature supported state
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @param[in]	camera The handle to the camera
 * @return true on supported, otherwise false
 *
 * @see camera_attr_enable_anti_shake()
 * @see camera_attr_is_enabled_anti_shake()
 */
bool camera_attr_is_supported_anti_shake(camera_h camera);

/**
 * @brief Enable/Disable video stabilization feature
 * @remarks
 * If enabling video stabilization, zero shutter lag is disabling.\n
 * This feature used for video recording.
 *
 * @param[in]	camera The handle to the camera
 * @param[in]	enable The state of video stabilization
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_is_enabled_video_stabilization()
 * @see camera_attr_is_supported_video_stabilization()
 *
 */
int camera_attr_enable_video_stabilization(camera_h camera, bool enable);

/**
 * @brief Gets state of video stabilization feature
 *
 * @param[in]	camera The handle to the camera
 * @param[out]	enabled The state of video stabilization
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_enable_video_stabilization()
 * @see camera_attr_is_supported_video_stabilization()
 *
 */
int camera_attr_is_enabled_video_stabilization(camera_h camera, bool *enabled);

/**
 * @biref Gets Video stabilization feature supported state
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @param[in]	camera The handle to the camera
 * @return true on supported, otherwise false
 *
 * @see camera_attr_enable_video_stabilization()
 * @see camera_attr_is_enabled_video_stabilization()
 */
bool camera_attr_is_supported_video_stabilization(camera_h camera);




/**
 * @brief Enable/Disable auto contrast
 *
 * @param[in]	camera The handle to the camera
 * @param[in]	enable The state of auto contrast
 *
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_is_enabled_auto_contrast()
 */
int camera_attr_enable_auto_contrast(camera_h camera, bool enable);

/**
 * @brief Gets state of auto contrast
 *
 * @param[in]	camera The handle to the camera
 * @param[out]	enabled The state of auto contrast
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 *
 * @see camera_attr_enable_auto_contrast()
 *
 */
int camera_attr_is_enabled_auto_contrast(camera_h camera, bool *enabled);

/**
 * @brief Disable shutter sound.
 * @remarks In some country, this operation was not permitted.
 *
 * @param[in] camera The handle to the camera
 * @param[in] disable If true, disabling shutter sound
 * @return	    0 on success, otherwise a negative error value.
 * @retval      #CAMERA_ERROR_NONE Successful
 * @retval      #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval      #CAMERA_ERROR_INVALID_OPERATION Not permitted disabling shutter sound
 */
int camera_attr_disable_shutter_sound(camera_h camera, bool disable);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_CAMERA_H__ */

