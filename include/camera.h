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
#include <media_packet.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file camera.h
 * @brief This file contains the Camera API, related structures and enumerations.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */

#define CAMERA_ERROR_CLASS          TIZEN_ERROR_CAMERA | 0x00

/**
 * @brief Enumeration for the error codes of Camera.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
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
    CAMERA_ERROR_DEVICE_BUSY            = CAMERA_ERROR_CLASS | 0x08,            /**< The device is using another application or working on some operation */
    CAMERA_ERROR_DEVICE_NOT_FOUND       = CAMERA_ERROR_CLASS | 0x09,            /**< No camera device */
    CAMERA_ERROR_SOUND_POLICY_BY_CALL   = CAMERA_ERROR_CLASS | 0x0a,            /**< Blocked by Audio Session Manager - CALL */
    CAMERA_ERROR_SOUND_POLICY_BY_ALARM  = CAMERA_ERROR_CLASS | 0x0b,            /**< Blocked by Audio Session Manager - ALARM */
    CAMERA_ERROR_ESD                    = CAMERA_ERROR_CLASS | 0x0c,            /**< ESD situation */
    CAMERA_ERROR_PERMISSION_DENIED      = TIZEN_ERROR_PERMISSION_DENIED,        /**< The access to the resources can not be granted*/
    CAMERA_ERROR_NOT_SUPPORTED          = TIZEN_ERROR_NOT_SUPPORTED,            /**< The feature is not supported */
} camera_error_e;

/**
 * @brief Enumeration for the camera state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_STATE_NONE,       /**< Before creating */
    CAMERA_STATE_CREATED,    /**< Created, but not initialized yet */
    CAMERA_STATE_PREVIEW,    /**< Preview */
    CAMERA_STATE_CAPTURING,  /**< While capturing */
    CAMERA_STATE_CAPTURED    /**< After capturing */
} camera_state_e;

/**
 * @brief Enumeration for the camera device.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_DEVICE_CAMERA0 = 0, /**< Primary camera */
    CAMERA_DEVICE_CAMERA1      /**< Secondary camera */
} camera_device_e;

/**
 * @brief Enumeration for the camera pixel format.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
	CAMERA_PIXEL_FORMAT_INVALID = -1,   /**< Invalid pixel format */
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
 * @brief Enumeration for the camera display type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
	CAMERA_DISPLAY_TYPE_OVERLAY = 0,	/**< Overlay surface display */
	CAMERA_DISPLAY_TYPE_EVAS,		/**< Evas object surface display */
	CAMERA_DISPLAY_TYPE_NONE		/**< This disposes off buffers */
} camera_display_type_e;

/**
 * @brief Enumeration for the camera policy.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
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
 * @brief Enumeration for the camera rotation type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
	CAMERA_ROTATION_NONE,	/**< No rotation */
	CAMERA_ROTATION_90,		/**< 90 degree rotation */
	CAMERA_ROTATION_180,	/**< 180 degree rotation */
	CAMERA_ROTATION_270,	/**< 270 degree rotation */
} camera_rotation_e;


/**
 * @brief Enumeration for the camera flip type.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
	CAMERA_FLIP_NONE,	/**< No Flip */
	CAMERA_FLIP_HORIZONTAL,	/**< Horizontal flip */
	CAMERA_FLIP_VERTICAL,	/**< Vertical flip */
	CAMERA_FLIP_BOTH	/** Horizontal and vertical flip */
} camera_flip_e;

/**
 * @brief Enumeration for the camera focus state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_FOCUS_STATE_RELEASED = 0, /**< Focus released */
    CAMERA_FOCUS_STATE_ONGOING,      /**< Focus in progress */
    CAMERA_FOCUS_STATE_FOCUSED,      /**< Focus succeeded */
    CAMERA_FOCUS_STATE_FAILED,       /**< Focus failed */
} camera_focus_state_e;

/**
 * @brief The structure type of the image data.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
 typedef struct
{
	unsigned char *data;		/**< The image buffer */
	unsigned int size;		/**< The size of the buffer */
	int width;			/**< The width of the image */
	int height;			/**< The height of the image */
	camera_pixel_format_e format;	/**< The format of the image pixel */
	unsigned char *exif;		/**< The exif raw data */
	unsigned int exif_size;		/**< The size of the exif data */
} camera_image_data_s;

/**
 * @brief The structure type for face detection.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct
{
	int id;		/**< The ID of each face */
	int score;	/**< The confidence level for the detection of the face */
	int x;		/**< The x coordinates of the face */
	int y;		/**< The y coordinates of the face */
	int width;	/**< The width of the face */
	int height;	/**< The height of the face */
} camera_detected_face_s;

/**
 * @brief The structure type to preview stream data.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef struct
{
	camera_pixel_format_e format;	/**< The format of the frame pixel */
	int width;			/**< The width of the frame */
	int height;			/**< The height of the frame */
	int num_of_planes;		/**< The number of planes */
	unsigned int timestamp;		/**< The timestamp of the frame */
	union {
		struct {
			unsigned char *yuv;	/**< The yuv data pointer */
			unsigned int size;	/**< The size of data */
		} single_plane;			/**< single plane frame data */

		struct {
			unsigned char *y;	/**< The y data pointer */
			unsigned char *uv;	/**< The uv data pointer */
			unsigned int y_size;	/**< The size of y data */
			unsigned int uv_size;	/**< The size of uv data */
		} double_plane;			/**< double plane frame data */

		struct {
			unsigned char *y;	/**< The y data pointer */
			unsigned char *u;	/**< The u data pointer */
			unsigned char *v;	/**< The v data pointer */
			unsigned int y_size;	/**< The size of y data */
			unsigned int u_size;	/**< The size of u data */
			unsigned int v_size;	/**< The size of v data */
		} triple_plane;			/**< triple plane frame data */
	} data;
} camera_preview_data_s;

/**
 * @brief The Camera handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @see	recorder_create_videorecorder()
 */
typedef struct camera_s *camera_h;

/**
 * @brief The Camera display handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef void *camera_display_h;

#ifndef GET_DISPLAY

/**
 * @brief Gets a display handle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
#define GET_DISPLAY(x) (void*)(x)

#endif

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @{
 */

/**
 * @brief Enumeration for the camera display mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
	CAMERA_DISPLAY_MODE_LETTER_BOX = 0,    /**< Letter box */
	CAMERA_DISPLAY_MODE_ORIGIN_SIZE,       /**< Origin size */
	CAMERA_DISPLAY_MODE_FULL,	       /**< Full screen */
	CAMERA_DISPLAY_MODE_CROPPED_FULL,      /**< Cropped full screen */
} camera_display_mode_e;

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Enumeration for the color tone, which provides the impression of looking through a tinted glass.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_ATTR_EFFECT_NONE = 0,              /**< None */
    CAMERA_ATTR_EFFECT_MONO,                  /**< Mono */
    CAMERA_ATTR_EFFECT_SEPIA,                 /**< Sepia */
    CAMERA_ATTR_EFFECT_NEGATIVE,              /**< Negative */
    CAMERA_ATTR_EFFECT_BLUE,                  /**< Blue */
    CAMERA_ATTR_EFFECT_GREEN,                 /**< Green */
    CAMERA_ATTR_EFFECT_AQUA,                  /**< Aqua */
    CAMERA_ATTR_EFFECT_VIOLET,                /**< Violet */
    CAMERA_ATTR_EFFECT_ORANGE,                /**< Orange */
    CAMERA_ATTR_EFFECT_GRAY,                  /**< Gray */
    CAMERA_ATTR_EFFECT_RED,                   /**< Red */
    CAMERA_ATTR_EFFECT_ANTIQUE,               /**< Antique */
    CAMERA_ATTR_EFFECT_WARM,                  /**< Warm */
    CAMERA_ATTR_EFFECT_PINK,                  /**< Pink */
    CAMERA_ATTR_EFFECT_YELLOW,                /**< Yellow */
    CAMERA_ATTR_EFFECT_PURPLE,                /**< Purple */
    CAMERA_ATTR_EFFECT_EMBOSS,                /**< Emboss */
    CAMERA_ATTR_EFFECT_OUTLINE,               /**< Outline */
    CAMERA_ATTR_EFFECT_SOLARIZATION,          /**< Solarization */
    CAMERA_ATTR_EFFECT_SKETCH,                /**< Sketch */
    CAMERA_ATTR_EFFECT_WASHED,                /**< Washed */
    CAMERA_ATTR_EFFECT_VINTAGE_WARM,          /**< Vintage warm  */
    CAMERA_ATTR_EFFECT_VINTAGE_COLD,          /**< Vintage cold */
    CAMERA_ATTR_EFFECT_POSTERIZATION,         /**< Posterization */
    CAMERA_ATTR_EFFECT_CARTOON,               /**< Cartoon */
    CAMERA_ATTR_EFFECT_SELECTIVE_RED,         /**< Selective color - Red */
    CAMERA_ATTR_EFFECT_SELECTIVE_GREEN,       /**< Selective color - Green */
    CAMERA_ATTR_EFFECT_SELECTIVE_BLUE,        /**< Selective color - Blue */
    CAMERA_ATTR_EFFECT_SELECTIVE_YELLOW,      /**< Selective color - Yellow */
    CAMERA_ATTR_EFFECT_SELECTIVE_RED_YELLOW,  /**< Selective color - Red and Yellow */
    CAMERA_ATTR_EFFECT_OTHER_GRAPHICS,        /**< Other Graphic effects */
} camera_attr_effect_mode_e;

/**
 * @brief Enumeration for the white balance levels of the camera.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
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
 * @brief Enumeration for the scene mode.
 * @details The mode of operation can be in daylight, night, or back-light.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
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
    CAMERA_ATTR_SCENE_MODE_AQUA,           /**< Aqua */
} camera_attr_scene_mode_e;

/**
 * @brief Enumeration for the auto focus mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_ATTR_AF_NONE = 0,    /**< auto-focus is not set */
    CAMERA_ATTR_AF_NORMAL,      /**< auto-focus in the normal mode  */
    CAMERA_ATTR_AF_MACRO,       /**< auto-focus in the macro mode(close distance)  */
    CAMERA_ATTR_AF_FULL,        /**< auto-focus in the full mode(all range scan, limited by device spec) */
} camera_attr_af_mode_e;

/**
 * @brief Enumeration for the ISO levels of the camera.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_ATTR_ISO_AUTO = 0, /**< ISO auto mode */
    CAMERA_ATTR_ISO_50,       /**< ISO 50 */
    CAMERA_ATTR_ISO_100,      /**< ISO 100 */
    CAMERA_ATTR_ISO_200,      /**< ISO 200 */
    CAMERA_ATTR_ISO_400,      /**< ISO 400 */
    CAMERA_ATTR_ISO_800,      /**< ISO 800 */
    CAMERA_ATTR_ISO_1600,     /**< ISO 1600 */
    CAMERA_ATTR_ISO_3200,     /**< ISO 3200 */
} camera_attr_iso_e;

/**
 * @brief Enumeration for the camera exposure modes.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_ATTR_EXPOSURE_MODE_OFF = 0,   /**< Off */
    CAMERA_ATTR_EXPOSURE_MODE_ALL,       /**< All mode */
    CAMERA_ATTR_EXPOSURE_MODE_CENTER,    /**< Center mode */
    CAMERA_ATTR_EXPOSURE_MODE_SPOT,      /**< Spot mode */
    CAMERA_ATTR_EXPOSURE_MODE_CUSTOM,    /**< Custom mode */
} camera_attr_exposure_mode_e;

/**
 * @brief Enumeration for the orientation values of tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_ATTR_TAG_ORIENTATION_TOP_LEFT = 1,      /**< Row #0 is at the top, Column #0 is to the left */
    CAMERA_ATTR_TAG_ORIENTATION_TOP_RIGHT = 2,     /**< Row #0 is at the top, Column #0 is to the right (flipped) */
    CAMERA_ATTR_TAG_ORIENTATION_BOTTOM_RIGHT = 3,  /**< Row #0 is at the bottom, Column #0 is to the right */
    CAMERA_ATTR_TAG_ORIENTATION_BOTTOM_LEFT = 4,   /**< Row #0 is at the bottom, Column #0 is to the left (flipped) */
    CAMERA_ATTR_TAG_ORIENTATION_LEFT_TOP = 5,      /**< Row #0 is to the left, Column #0 is at the top (flipped) */
    CAMERA_ATTR_TAG_ORIENTATION_RIGHT_TOP = 6,     /**< Row #0 is to the right, Column #0 is at the top */
    CAMERA_ATTR_TAG_ORIENTATION_RIGHT_BOTTOM = 7,  /**< Row #0 is to the right, Column #0 is at the bottom (flipped) */
    CAMERA_ATTR_TAG_ORIENTATION_LEFT_BOTTOM = 8,   /**< Row #0 is to the left, Column #0 is at the bottom */
} camera_attr_tag_orientation_e;

/**
 * @brief Enumeration for the flash mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_ATTR_FLASH_MODE_OFF = 0,          /**< Always off */
    CAMERA_ATTR_FLASH_MODE_ON,               /**< Always splashes */
    CAMERA_ATTR_FLASH_MODE_AUTO,             /**< Depending on intensity of light, strobe starts to flash */
    CAMERA_ATTR_FLASH_MODE_REDEYE_REDUCTION, /**< Red eye reduction. Multiple flash before capturing */
    CAMERA_ATTR_FLASH_MODE_SLOW_SYNC,        /**< Slow sync curtain synchronization */
    CAMERA_ATTR_FLASH_MODE_FRONT_CURTAIN,    /**< Front curtain synchronization */
    CAMERA_ATTR_FLASH_MODE_REAR_CURTAIN,     /**< Rear curtain synchronization */
    CAMERA_ATTR_FLASH_MODE_PERMANENT,        /**< Keep turned on until turning off */
} camera_attr_flash_mode_e;

/**
 * @brief Enumeration to preview FPS.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
    CAMERA_ATTR_FPS_AUTO = 0, /**< AUTO FPS */
    CAMERA_ATTR_FPS_7 = 7,    /**< 7 FPS */
    CAMERA_ATTR_FPS_8 = 8,    /**< 8 FPS */
    CAMERA_ATTR_FPS_15 = 15,  /**< 15 FPS */
    CAMERA_ATTR_FPS_20 = 20,  /**< 20 FPS */
    CAMERA_ATTR_FPS_24 = 24,  /**< 24 FPS */
    CAMERA_ATTR_FPS_25 = 25,  /**< 25 FPS */
    CAMERA_ATTR_FPS_30 = 30,  /**< 30 FPS */
    CAMERA_ATTR_FPS_60 = 60,  /**< 60 FPS */
    CAMERA_ATTR_FPS_90 = 90,  /**< 90 FPS */
    CAMERA_ATTR_FPS_120 = 120 /**< 120 FPS */
} camera_attr_fps_e;

/**
 * @brief Enumeration for the theater mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
	CAMERA_ATTR_THEATER_MODE_DISABLE = 0, /**< Disable theater mode - External display shows same image as device display */
	CAMERA_ATTR_THEATER_MODE_ENABLE = 2,  /**< Enable theater mode - Preview image is displayed on external display with full screen mode, but preview image is not shown on device display */
	CAMERA_ATTR_THEATER_MODE_CLONE = 1    /**< Clone mode - Preview image is displayed on external display with full screen mode. Also preview image is shown by the UI on device display */
} camera_attr_theater_mode_e;

/**
 * @brief Enumeration for HDR capture mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 */
typedef enum
{
	CAMERA_ATTR_HDR_MODE_DISABLE = 0,  /**< Disable HDR capture */
	CAMERA_ATTR_HDR_MODE_ENABLE,       /**< Enable HDR capture */
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
 * @brief Called when the camera state is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] previous The previous state of the camera
 * @param[in] current The current state of the camera
 * @param[in] by_policy If @c true the state is changed by policy, otherwise @c false
 * @param[in] user_data The user data passed from the callback registration function
 * @pre camera_start_preview(), camera_start_capture() or camera_stop_preview()
 *      will invoke this callback if you register this callback using camera_set_state_changed_cb().
 * @see	camera_set_state_changed_cb()
 */
typedef void (*camera_state_changed_cb)(camera_state_e previous, camera_state_e current,
        bool by_policy, void *user_data);

/**
 * @brief Called when the camera is interrupted by policy.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] policy The policy that interrupted the camera
 * @param[in] previous The previous state of the camera
 * @param[in] current The current state of the camera
 * @param[in] user_data The user data passed from the callback registration function
 * @see	camera_set_interrupted_cb()
 */
typedef void (*camera_interrupted_cb)(camera_policy_e policy, camera_state_e previous, camera_state_e current, void *user_data);

/**
 * @brief Called when the camera focus state is changed.
 * @details When the camera auto focus completes or a change to the focus state occurs,
 *          this callback is invoked. \n \n
 *          Changes of focus state are as follows: \n
 *          #CAMERA_FOCUS_STATE_RELEASED -> start focusing -> #CAMERA_FOCUS_STATE_ONGOING -> working ->
 *          #CAMERA_FOCUS_STATE_FOCUSED or #CAMERA_FOCUS_STATE_FAILED.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] state The current state of the auto-focus
 * @param[in] user_data The user data passed from the callback registration function
 * @pre camera_start_focusing() will invoke this callback if you register it using camera_set_focus_changed_cb().
 * @see	camera_set_focus_changed_cb()
 * @see	camera_unset_focus_changed_cb()
 * @see	camera_start_focusing()
 * @see camera_cancel_focusing()
 */
typedef void (*camera_focus_changed_cb)(camera_focus_state_e state, void *user_data);

/**
 * @brief Called to register for notifications about delivering a copy of the new preview frame when every preview frame is displayed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks This function is issued in the context of gstreamer so the UI update code should not be directly invoked.\n
 *          If the camera is used as a recorder then this callback function won't be called.
 *
 * @param[in] frame The reference pointer to preview stream data
 * @param[in] user_data The user data passed from the callback registration function
 * @pre	camera_start_preview() will invoke this callback function if you register this callback using camera_set_preview_cb().
 * @see	camera_start_preview()
 * @see	camera_set_preview_cb()
 * @see	camera_unset_preview_cb()
 */
typedef void (*camera_preview_cb)(camera_preview_data_s *frame, void *user_data);

/**
 * @brief Called to register for notifications about delivering media packet when every preview frame is displayed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks This function is issued in the context of gstreamer so the UI update code should not be directly invoked.\n
 *          If the camera is used as a recorder then this callback function won't be called.\n
 *          and the packet should be released by media_packet_destroy() after use.
 *
 * @param[in] pkt Reference pointer to media packet
 * @param[in] user_data The user data passed from the callback registration function
 * @pre	camera_start_preview() will invoke this callback function if you register this callback using camera_set_media_packet_preview_cb().
 * @see	camera_start_preview()
 * @see	camera_set_media_packet_preview_cb()
 * @see	camera_unset_media_packet_preview_cb()
 */
typedef void (*camera_media_packet_preview_cb)(media_packet_h pkt, void *user_data);

/**
 * @brief Called to get information about image data taken by the camera once per frame while capturing.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks This function is issued in the context of gstreamer (video source thread) so the UI update code should not be directly invoked.
 *          You must not call camera_start_preview() within this callback.
 *
 * @param[in] image The image data of the captured picture
 * @param[in] postview The image data of the postview
 * @param[in] thumbnail The image data of the thumbnail (it should be @c NULL if the available thumbnail data does not exist)
 * @param[in] user_data The user data passed from the callback registration function
 * @pre	camera_start_capture() or camera_start_continuous_capture() will invoke this callback function if it is registered using camera_start_capture() or camera_start_continuous_capture().
 * @see	camera_start_capture()
 * @see	camera_start_continuous_capture()
 * @see	camera_capture_completed_cb()
 */
typedef void (*camera_capturing_cb)(camera_image_data_s* image, camera_image_data_s* postview, camera_image_data_s* thumbnail, void *user_data);

/**
 * @brief Called when the camera capturing completes.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks The callback is called after camera_capturing_cb() is completed.\n
 *          If you want to show the user a preview after capturing is finished, an application can use camera_start_preview() after calling this callback.
 *
 * @param[in] user_data The user data passed from the callback registration function
 * @pre	This callback function is invoked if it is registered using camera_start_capture() or camera_start_continuous_capture().
 * @see	camera_start_capture()
 * @see	camera_start_continuous_capture()
 * @see	camera_capturing_cb()
 */
typedef void (*camera_capture_completed_cb)(void *user_data);

/**
 * @brief Called when an error occurs.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 *
 * @remarks This callback informs about a critical error situation.\n
 *          When this callback is invoked, the user should release the resource and terminate the application.\n
 *          In case of errors, one of these codes occur:\n
 * #CAMERA_ERROR_DEVICE,\n
 * #CAMERA_ERROR_INVALID_OPERATION,\n
 * #CAMERA_ERROR_OUT_OF_MEMORY.\n
 *
 * @param[in] error The error code
 * @param[in] current_state The current state of the camera
 * @param[in] user_data	The user data passed from the callback registration function
 * @pre	This callback function is invoked if it is registered using camera_set_error_cb().
 * @see	camera_set_error_cb()
 * @see	camera_unset_error_cb()
 */
typedef void (*camera_error_cb)(camera_error_e error, camera_state_e current_state, void *user_data);

/**
 * @brief Called when a face is detected in the preview frame.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] faces The detected face array
 * @param[in] count The length of the array
 * @param[in] user_data The user data passed from the callback registration function
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
 * @brief Called once for each supported preview resolution.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] width The preview image width
 * @param[in] height The preview image height
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_foreach_supported_preview_resolution() will invoke this callback.
 * @see	camera_foreach_supported_preview_resolution()
 */
typedef bool (*camera_supported_preview_resolution_cb)(int width, int height, void *user_data);

/**
 * @brief Called once for each supported capture resolution.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] width The capture resolution width
 * @param[in] height The capture resolution height
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_foreach_supported_capture_resolution() will invoke this callback.
 * @see	camera_foreach_supported_capture_resolution()
 */
typedef bool (*camera_supported_capture_resolution_cb)(int width, int height, void *user_data);

/**
 * @brief Called once for the pixel format of each supported capture format.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] format The supported pixel format
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_foreach_supported_capture_format() will invoke this callback.
 * @see	camera_foreach_supported_capture_format()
 */
typedef bool (*camera_supported_capture_format_cb)(camera_pixel_format_e format,
        void *user_data);

/**
 * @brief Called once for the pixel format of each supported preview format.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] format The supported preview data format
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_foreach_supported_preview_format() will invoke this callback.
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks Multiple handles on a context at the same time are allowed to be created. However,
 *          camera cannot guarantee proper operation because of limited resources, such as
 *          camera device, audio device, and display device.\n.
 *          A @a camera must be released using camera_destroy().
 * @param[in] device The hardware camera to access
 * @param[out] camera A newly returned handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_OUT_OF_MEMORY Out of memory
 * @retval #CAMERA_ERROR_SOUND_POLICY Sound policy error
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post If it succeeds, the camera state will be #CAMERA_STATE_CREATED.
 *
 * @see	camera_destroy()
 */
int camera_create(camera_device_e device, camera_h *camera);

/**
 * @brief Destroys the camera handle and releases all its resources.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_create()
 */
int camera_destroy(camera_h camera);

/**
 * @brief Starts capturing and drawing preview frames on the screen.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_SOUND_POLICY Sound policy error
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_DEVICE_BUSY The device is being used in another application or is performing other operations
 * @retval #CAMERA_ERROR_DEVICE_NOT_FOUND No camera device
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre    The camera state must be set to #CAMERA_STATE_CREATED or #CAMERA_STATE_CAPTURED.\n
 *         You must set the display handle. \n
 *         If needed, modify preview FPS(camera_attr_set_preview_fps()),
 *         preview resolution(camera_set_preview_resolution()), or preview format(camera_set_preview_format()).
 * @post   If it succeeds, the camera state will be #CAMERA_STATE_PREVIEW.\n
 *         camera_preview_cb() will be called when preview image data becomes available.
 *
 * @see	camera_stop_preview()
 * @see camera_set_display()
 * @see camera_set_preview_cb()
 * @see camera_set_media_packet_preview_cb()
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
 * @brief Stops capturing and drawing preview frames.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_PREVIEW.
 * @post The camera state will be #CAMERA_STATE_CREATED.
 * @see	camera_start_preview()
 * @see	camera_unset_preview_cb()
 * @see	camera_unset_media_packet_preview_cb()
 */
int camera_stop_preview(camera_h camera);

/**
 * @brief Starts capturing of still images.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function causes the transition of the camera state from #CAMERA_STATE_CAPTURING to #CAMERA_STATE_CAPTURED automatically\n
 *          and the corresponding callback function camera_capturing_cb() and camera_capture_completed_cb() will be invoked\n
 *          The captured image will be delivered through camera_capturing_cb().\n
 *          camera_capture_completed_cb() callback notifies about completion of camera_capturing_cb(). \n
 *          The camera's preview should be restarted by calling camera_start_preview().
 * @param[in] camera The handle to the camera
 * @param[in] capturing_cb The callback for capturing data
 * @param[in] completed_cb The callback for notification of completion
 * @param[in] user_data The user data
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_PREVIEW. \n
 *      If needed, modify capture resolution(camera_set_capture_resolution()),
 *      capture format(camera_set_capture_format()), or image quality(camera_attr_set_image_quality()).
 * @post If it succeeds the camera state will be #CAMERA_STATE_CAPTURED.
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
 * @brief Starts continuously capturing still images.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks If this is not supported zero shutter lag occurs. The capture resolution could be changed to the preview resolution.\n
 *          This function causes the transition of the camera state from #CAMERA_STATE_CAPTURING to #CAMERA_STATE_CAPTURED automatically\n
 *          and the corresponding callback function camera_capturing_cb() and camera_capture_completed_cb() will be invoked\n
 *          Each Captured image will be delivered through camera_capturing_cb().\n
 *          The camera_capture_completed_cb() callback notifies about the completion of an entire capture.\n
 *          The camera's preview should be restarted by calling camera_start_preview().\n.
 * @param[in] camera The handle to the camera
 * @param[in] count The number of still images
 * @param[in] interval The interval of the capture (millisecond)
 * @param[in] capturing_cb The callback for capturing data
 * @param[in] completed_cb The callback for notification of completion
 * @param[in] user_data The user data
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post   If it succeeds the camera state will be #CAMERA_STATE_CAPTURED.
 *
 * @see camera_start_preview()
 * @see camera_start_capture();
 * @see camera_stop_continuous_capture()
 * @see camera_is_supported_zero_shutter_lag()
 */
int camera_start_continuous_capture(camera_h camera, int count, int interval, camera_capturing_cb capturing_cb, camera_capture_completed_cb completed_cb , void *user_data);

/**
 * @brief Aborts continuous capturing.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks The camera state will be changed to #CAMERA_STATE_CAPTURED.
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_PREVIEW.
 *
 * @see camera_start_continuous_capture()
 */
int camera_stop_continuous_capture(camera_h camera);

/**
 * @brief Gets the state of the camera.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] state The current state of the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_create()
 * @see camera_start_preview()
 * @see camera_stop_preview()
 * @see camera_start_capture()
 */
int camera_get_state(camera_h camera, camera_state_e *state);

/**
 * @brief Starts camera auto-focusing, asynchronously.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks If continuous status is @c true, the camera continuously tries to focus.
 * @param[in] camera The handle to the camera
 * @param[in] continuous The status of continuous focusing
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_PREVIEW.
 * @post The camera focus state will be #CAMERA_FOCUS_STATE_ONGOING.
 *
 * @see camera_cancel_focusing()
 * @see camera_set_focus_changed_cb()
 * @see camera_focus_changed_cb()
 * @see camera_attr_set_af_mode()
 */
int camera_start_focusing(camera_h camera, bool continuous);

/**
 * @brief Stops camera auto focusing.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_PREVIEW.
 *
 * @see camera_start_focusing()
 * @see	camera_focus_changed_cb()
 */
int camera_cancel_focusing(camera_h camera);

/**
 * @brief Sets the display handle to show preview images.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This function must be called before previewing (see camera_start_preview()).
 * @param[in] camera The handle to the camera
 * @param[in] type The display type
 * @param[in] display The display handle from #GET_DISPLAY
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_CREATED.
 *
 * @see camera_start_preview()
 * @see #GET_DISPLAY
 */
int camera_set_display(camera_h camera, camera_display_type_e type, camera_display_h display);

/**
 * @brief Sets the resolution of the preview.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function should be called before previewing (camera_start_preview()).
 * @param[in] camera The handle to the camera
 * @param[in] width The preview width
 * @param[in] height The preview height
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_CREATED or #CAMERA_STATE_PREVIEW.
 *
 * @see camera_start_preview()
 * @see	camera_get_preview_resolution()
 * @see	camera_foreach_supported_preview_resolution()
 */
int camera_set_preview_resolution(camera_h camera, int width, int height);

/**
 * @brief Gets the resolution of the preview.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] width The preview width
 * @param[out] height The preview height
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_set_preview_resolution()
 * @see	camera_foreach_supported_preview_resolution()
 */
int camera_get_preview_resolution(camera_h camera, int *width, int *height);

/**
 * @brief Gets the recommended preview resolution.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks Depending on the capture resolution aspect ratio and display resolution, the recommended preview resolution is determined.
 * @param[in] camera The handle to the camera
 * @param[out] width The preview width
 * @param[out] height The preview height
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_set_preview_resolution()
 * @see	camera_foreach_supported_preview_resolution()
 */
int camera_get_recommended_preview_resolution(camera_h camera, int *width, int *height);

/**
 * @brief Starts face detection.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This should be called after the preview is started.\n
 *          This callback will be invoked when the face is detected in the preview frame.\n
 *          Internally it starts continuous focus and focusing on the detected face.\n
 *          When face detection is running, the camera_start_focusing(), camera_cancel_focusing(), camera_attr_set_af_mode(), camera_attr_set_af_area(), camera_attr_set_exposure_mode(), and camera_attr_set_whitebalance() settings are ignored.\n
 *          If camera_stop_preview() is invoked, face detection is stopped and then preview is resumed using camera_start_preview(), this method should be called again to resume face detection.
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback to notify face detection
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Not preview state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Not supported this feature
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be #CAMERA_STATE_PREVIEW.
 *
 * @see camera_stop_face_detection()
 * @see camera_face_detected_cb()
 * @see camera_is_supported_face_detection()
 */
int camera_start_face_detection(camera_h camera, camera_face_detected_cb callback, void * user_data);

/**
 * @brief Stops face detection.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre This should be called after face detection is started.
 *
 * @see camera_start_face_detection()
 * @see camera_is_supported_face_detection()
 */
int camera_stop_face_detection(camera_h camera);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Gets continuous capture feature's supported state.
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] camera The handle to the camera
 * @return @c true on supported, otherwise false
 * @exception #CAMERA_ERROR_NONE Successful
 * @exception #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @exception #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 *
 */
 bool camera_is_supported_continuous_capture(camera_h camera);

/**
 * @brief Retrieves all supported camera preview resolutions by invoking the callback function once for each supported camera preview resolution.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_supported_preview_resolution_cb() repeatedly to retrieve each supported preview resolution.
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
 * @ingroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @brief Sets the display rotation.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function should be called before previewing (see camera_start_preview())
 * @param[in] camera The handle to the camera
 * @param[in] rotation The display rotation
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Display type is incorrect
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_start_preview()
 * @see	camera_get_display_rotation()
 */
int camera_set_display_rotation(camera_h camera, camera_rotation_e rotation);

/**
 * @ingroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @brief Gets the display rotation.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] rotation The display rotation
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_display_rotation()
 */
int camera_get_display_rotation(camera_h camera, camera_rotation_e *rotation);

/**
 * @ingroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @brief Sets the display flip.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] flip The display flip
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Display type is incorrect
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_get_display_flip()
 */
int camera_set_display_flip(camera_h camera, camera_flip_e flip);

/**
 * @ingroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @brief Gets the display flip.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] flip The display flip
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_display_flip()
 */
int camera_get_display_flip(camera_h camera, camera_flip_e *flip);

/**
 * @ingroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @brief Sets the visible property for display.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] visible The display visibility property
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_is_display_visible()
 */
int camera_set_display_visible(camera_h camera, bool visible);

/**
 * @ingroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @brief Gets the visible property of display.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] visible @c true if camera display is visible, otherwise @c false
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_display_visible()
 */
int camera_is_display_visible(camera_h camera, bool *visible);

/**
 * @ingroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @brief Sets the display mode.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] mode The display mode
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_get_display_mode()
 */
int camera_set_display_mode(camera_h camera , camera_display_mode_e mode);

/**
 * @ingroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE
 * @brief Gets the display mode.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] mode The display mode
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_display_mode()
 */
int camera_get_display_mode(camera_h camera, camera_display_mode_e *mode);

/**
 * @brief Sets the resolution of the captured image.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] width The capture width
 * @param[in] height The capture height
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_CREATED or #CAMERA_STATE_PREVIEW.
 * @see camera_start_capture()
 * @see	camera_get_capture_resolution()
 * @see	camera_foreach_supported_capture_resolution()
 */
int camera_set_capture_resolution(camera_h camera, int width, int height);

/**
 * @brief Gets the resolution of the captured image.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] width The capture width
 * @param[out] height The capture height
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported camera captured resolutions by invoking the callback function once for each supported camera capture resolution.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_supported_capture_resolution_cb() repeatedly to retrieve each supported capture resolution.
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
 * @brief Sets the format of an image to be captured.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This function should be called before capturing (see camera_start_capture()).
 * @param[in] camera The handle to the camera
 * @param[out] format The format of the image to be captured
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre	The camera state must be set to #CAMERA_STATE_CREATED or #CAMERA_STATE_PREVIEW.
 * @see camera_start_capture()
 * @see	camera_get_capture_format()
 * @see	camera_foreach_supported_capture_format()
 */
int camera_set_capture_format(camera_h camera, camera_pixel_format_e format);

/**
 * @brief Gets the format of the image to be captured.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] format The format of the image to be captured
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported camera capture formats by invoking the callback function once for each supported camera capture format.
 *
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_supported_capture_format_cb() repeatedly to retrieve each supported capture format.
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This function should be called before previewing (see camera_start_preview()).
 * @param[in] camera The handle to the camera
 * @param[in] format The preview data format
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_CREATED.
 * @see camera_start_preview()
 * @see	camera_get_preview_format()
 * @see	camera_foreach_supported_preview_format()
 */
int camera_set_preview_format(camera_h camera, camera_pixel_format_e format);

/**
 * @brief Gets the format of the preview stream.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] format The preview data format
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported camera preview formats by invoking the callback function once for each supported camera preview format.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_supported_preview_format_cb() repeatly to retrieve each supported preview format.
 * @see	camera_set_preview_format()
 * @see	camera_get_preview_format()
 * @see	camera_supported_preview_format_cb()
 */
int camera_foreach_supported_preview_format(camera_h camera,
        camera_supported_preview_format_cb callback, void *user_data);

/**
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @brief Gets the face detection feature's supported state.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] camera The handle to the camera
 * @return @c true if supported, otherwise @c false
 * @see camera_start_face_detection()
 * @see camera_stop_face_detection()
 * @exception #CAMERA_ERROR_NONE Successful
 * @exception #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @exception #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 */
bool camera_is_supported_face_detection(camera_h camera);

/**
 * @brief Gets the zero shutter lag feature's supported state.
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks If supporting zero shutter lag, continuous shot can be done with full capture size. \n
 *                The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] camera The handle to the camera
 * @return @c true if supported, otherwise @c false
 * @exception #CAMERA_ERROR_NONE Successful
 * @exception #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @exception #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 *
 */
bool camera_is_supported_zero_shutter_lag(camera_h camera);

/**
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @brief Gets the camera device count.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks If the device supports primary and secondary camera, this returns @c 2. If @c 1 is returned, the device only supports primary camera.
 * @param[in] camera The handle to the camera
 * @param[out] device_count The device count
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 *
 */
int camera_get_device_count(camera_h camera, int *device_count);

/**
 * @brief Gets the media packet preview callback feature's supported state.
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] camera The handle to the camera
 * @return @c true if supported, otherwise @c false
 * @exception #CAMERA_ERROR_NONE Successful
 * @exception #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @exception #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 *
 */
bool camera_is_supported_media_packet_preview_cb(camera_h camera);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_MODULE
 * @{
 */

/**
 * @brief Registers a callback function to be called once per frame when previewing.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This callback does not work in the video recorder mode.\n
 *          This function should be called before previewing (see camera_start_preview()).\n
 *          A registered callback is called on the internal thread of the camera.\n
 *          A video frame can be retrieved using a registered callback.\n
 *          The callback function holds the same buffer that will be drawn on the display device.\n
 *          So if you change the buffer in a registerd callback, it will be displayed on the device\n
 *          and the buffer is only available in a registerd callback.
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be registered
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre	The camera's state must be set to #CAMERA_STATE_CREATED.
 * @see	camera_start_preview()
 * @see camera_unset_preview_cb()
 * @see	camera_preview_cb()
 */
int camera_set_preview_cb(camera_h camera, camera_preview_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_preview_cb()
 */
int camera_unset_preview_cb(camera_h camera);

/**
 * @brief Registers a media packet callback function to be called once per frame when previewing.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This callback does not work in video recorder mode.\n
 *          This function should be called before previewing (see camera_start_preview())\n
 *          A registered callback is called on the internal thread of the camera.\n
 *          A video frame can be retrieved using a registered callback as a media packet.\n
 *          The callback function holds the same buffer that will be drawn on the display device.\n
 *          So if you change the media packet in a registerd callback, it will be displayed on the device\n
 *          and the media packet is available until it's destroyed by media_packet_destroy().
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be registered
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre	The camera's state should be #CAMERA_STATE_CREATED.
 * @see	camera_start_preview()
 * @see	camera_unset_media_packet_preview_cb()
 * @see	camera_media_packet_preview_cb()
 */
int camera_set_media_packet_preview_cb(camera_h camera, camera_media_packet_preview_cb callback, void *user_data);

/**
 * @brief Unregisters the media packet callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_set_media_packet_preview_cb()
 */
int camera_unset_media_packet_preview_cb(camera_h camera);

/**
 * @brief Registers a callback function to be called when the camera state changes.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function will invoke camera_state_changed_cb() when the camera state changes.
 * @see camera_unset_state_changed_cb()
 * @see	camera_state_changed_cb()
 */
int camera_set_state_changed_cb(camera_h camera, camera_state_changed_cb callback,
        void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_state_changed_cb()
 */
int camera_unset_state_changed_cb(camera_h camera);

/**
 * @brief Registers a callback function to be called when the camera is interrupted by policy.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_unset_interrupted_cb()
 * @see	camera_interrupted_cb()
 */
int camera_set_interrupted_cb(camera_h camera, camera_interrupted_cb callback,
	    void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_interrupted_cb()
 */
int camera_unset_interrupted_cb(camera_h camera);

/**
 * @brief Registers a callback function to be called when the auto-focus state changes.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function will invoke camera_focus_changed_cb() when the auto-focus state changes.
 * @see	camera_start_focusing()
 * @see	camera_cancel_focusing()
 * @see	camera_unset_focus_changed_cb()
 * @see	camera_focus_changed_cb()
 */
int camera_set_focus_changed_cb(camera_h camera, camera_focus_changed_cb callback,
        void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_focus_changed_cb()
 */
int camera_unset_focus_changed_cb(camera_h camera);

/**
 * @brief Registers a callback function to be called when an asynchronous operation error occurs.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This callback informs about a critical error situation.\n
 *          When this callback is invoked, the user should release the resource and terminate the application.\n
 *          In case of errors, one of the following codes will occur:\n
 *          #CAMERA_ERROR_DEVICE,\n
 *          #CAMERA_ERROR_INVALID_OPERATION,\n
 *          #CAMERA_ERROR_OUT_OF_MEMORY.
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function will invoke camera_error_cb() when an asynchronous operation error occurs.

 * @see camera_unset_error_cb()
 * @see	camera_error_cb()
 */
int camera_set_error_cb(camera_h camera, camera_error_cb callback, void *user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_error_cb()
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] mode The supported auto-focus mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_af_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_af_mode()
 */
typedef bool (*camera_attr_supported_af_mode_cb)(camera_attr_af_mode_e mode, void *user_data);

/**
 * @brief Called to get each supported exposure mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] mode The supported exposure mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_exposure_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_exposure_mode()
 * @see	#camera_attr_exposure_mode_e
 */
typedef bool (*camera_attr_supported_exposure_mode_cb)(camera_attr_exposure_mode_e mode,
        void *user_data);

/**
 * @brief Called to get each supported ISO mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] iso The supported ISO mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_iso() will invoke this callback.
 * @see	camera_attr_foreach_supported_iso()
 */
typedef bool (*camera_attr_supported_iso_cb)(camera_attr_iso_e iso, void *user_data);

/**
 * @brief Called to get each supported white balance.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] wb The supported white balance mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_whitebalance() will invoke this callback.
 * @see	camera_attr_foreach_supported_whitebalance()
 * @see	#camera_attr_whitebalance_e
 */
typedef bool (*camera_attr_supported_whitebalance_cb)(camera_attr_whitebalance_e wb,
        void *user_data);

/**
 * @brief Called to get each supported effect mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] effect The supported effect mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_effect() will invoke this callback.
 * @see	camera_attr_foreach_supported_effect()
 */
typedef bool (*camera_attr_supported_effect_cb)(camera_attr_effect_mode_e effect,
        void *user_data);

/**
 * @brief Called to get each supported scene mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] mode The supported scene mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_scene_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_scene_mode()
 * @see	#camera_attr_scene_mode_e
 */
typedef bool (*camera_attr_supported_scene_mode_cb)(camera_attr_scene_mode_e mode,
        void *user_data);

/**
 * @brief Called to get each supported flash mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] mode The supported flash mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_flash_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_flash_mode()
 */
typedef bool (*camera_attr_supported_flash_mode_cb)(camera_attr_flash_mode_e mode,
        void *user_data);

/**
 * @brief Called to get each supported FPS mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] mode The supported FPS mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n otherwise @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_fps() will invoke this callback.
 * @see	camera_attr_foreach_supported_fps()
 */
typedef bool (*camera_attr_supported_fps_cb)(camera_attr_fps_e fps, void *user_data);

/**
 * @brief Called to get each supported stream flip mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] mode The supported stream flip mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_stream_flip() will invoke this callback.
 * @see	camera_attr_foreach_supported_stream_flip()
 */
typedef bool (*camera_attr_supported_stream_flip_cb)(camera_flip_e flip, void *user_data);

/**
 * @brief Called to get each supported stream rotation mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] mode The supported stream rotation mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_stream_rotation() will invoke this callback.
 * @see	camera_attr_foreach_supported_stream_rotation()
 */
typedef bool (*camera_attr_supported_stream_rotation_cb)(camera_rotation_e rotation, void *user_data);

/**
 * @brief Called to get each supported theater mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] mode The supported theater mode
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop, \n @c false to break out of the loop
 * @pre	camera_attr_foreach_supported_theater_mode() will invoke this callback.
 * @see	camera_attr_foreach_supported_theater_mode()
 */
typedef bool (*camera_attr_supported_theater_mode_cb)(camera_attr_theater_mode_e mode, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the preview frame rate.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This function should be called before previewing (see camera_start_preview()).
 * @param[in] camera The handle to the camera
 * @param[in] fps The frame rate
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_start_preview()
 * @see	camera_attr_get_preview_fps()
 * @see	camera_attr_foreach_supported_fps()
 */
int camera_attr_set_preview_fps(camera_h camera, camera_attr_fps_e fps);

/**
 * @brief Gets the frames per second of a preview video stream.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] fps The frames per second of the preview video stream
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported FPS modes by invoking the callback function once for each supported FPS mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_fps_cb() repeatly to get each supported FPS mode.
 * @see	camera_attr_set_preview_fps()
 * @see	camera_attr_get_preview_fps()
 * @see	camera_attr_supported_fps_cb()
 */
int camera_attr_foreach_supported_fps(camera_h camera, camera_attr_supported_fps_cb callback,
        void *user_data);

/**
 * @brief Retrieves all supported FPS modes by invoking the callback function once for each supported FPS mode.
 * @since_tizen 2.4
 * @param[in] camera The handle to the camera
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] width Required preview resolution's width
 * @param[in] height Required preview resolution's height
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_fps_cb() repeatly to get each supported FPS mode.
 * @see	camera_attr_set_preview_fps()
 * @see	camera_attr_get_preview_fps()
 * @see	camera_attr_supported_fps_cb()
 */
int camera_attr_foreach_supported_fps_by_resolution(camera_h camera,  int width, int height,
	camera_attr_supported_fps_cb callback ,void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets quality of the image.
 * @details The range for image quality is 1 to 100. If @a quality is out of range, #CAMERA_ERROR_INVALID_PARAMETER error occurred.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] quality The quality of image (1 ~ 100)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre The camera state must be set to #CAMERA_STATE_CREATED or #CAMERA_STATE_PREVIEW.
 * @see camera_start_preview()
 * @see	camera_attr_get_image_quality()
 */
int camera_attr_set_image_quality(camera_h camera, int quality);

/**
 * @brief Gets the quality of a still image, which is captured.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] quality The quality of the image(1 ~ 100)
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_image_quality()
 */
int camera_attr_get_image_quality(camera_h camera, int *quality);

/**
 * @brief Sets the zoom level.
 * @details The range for the zoom level is received from camera_attr_get_zoom_range(). If @a zoom is out of range, the #CAMERA_ERROR_INVALID_PARAMETER error occurs.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] zoom The zoom level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_get_zoom()
 * @see camera_attr_get_zoom_range()
 */
int camera_attr_set_zoom(camera_h camera, int zoom);

/**
 * @brief Gets the zoom level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] zoom The zoom level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_zoom()
 * @see camera_attr_get_zoom_range()
 */
int camera_attr_get_zoom(camera_h camera, int *zoom);

/**
 * @brief Gets the available zoom level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks If the min value is greater than the max value, it means that this feature is not supported.
 * @param[in] camera The handle to the camera
 * @param[out] min The minimum zoom level
 * @param[out] max The maximum zoom level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_zoom()
 * @see camera_attr_get_zoom()
 */
int camera_attr_get_zoom_range(camera_h camera , int *min , int *max);


/**
 * @brief Sets the auto focus mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] mode The auto focus mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_attr_get_af_mode()
 * @see	camera_attr_foreach_supported_af_mode()
 * @see	#camera_attr_af_mode_e
 */
int camera_attr_set_af_mode(camera_h camera, camera_attr_af_mode_e mode);

/**
 * @brief Gets the auto focus mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[out] mode The auto focus mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_foreach_supported_af_mode()
 * @see camera_attr_set_af_mode()
 * @see	#camera_attr_af_mode_e
 */
int camera_attr_get_af_mode(camera_h camera, camera_attr_af_mode_e *mode);

/**
 * @brief Sets auto focus area.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This API is invalid in the #CAMERA_ATTR_AF_NONE mode.\n
 *          The coordinates are mapped to preview area.
 * @param[in] camera The handle to the camera
 * @param[in] x The x coordinates of the focus area
 * @param[in] y The y coordinates of the focus area
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_af_mode()
 * @see camera_attr_clear_af_area()
 */
int camera_attr_set_af_area(camera_h camera, int x, int y);

/**
 * @brief Clears the auto focus area.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks The focusing area is set to the center.
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_af_mode()
 * @see camera_attr_set_af_area()
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
 * @brief Retrieves all supported auto focus modes by invoking the callback function once for each supported auto focus mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_af_mode_cb() to get all the supported auto focus modes.
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] mode The exposure mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_get_exposure_mode()
 * @see camera_attr_foreach_supported_exposure_mode()
 */
int camera_attr_set_exposure_mode(camera_h camera, camera_attr_exposure_mode_e mode);

/**
 * @brief Gets the exposure mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] mode The exposure mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported exposure modes by invoking the callback function once for each supported exposure mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_exposure_mode_cb() to get all the supported exposure modes.
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] value The exposure value
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_attr_get_exposure()
 */
int camera_attr_set_exposure(camera_h camera, int value);

/**
 * @brief Gets the exposure value.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] value The exposure value
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_attr_set_exposure()
 */
int camera_attr_get_exposure(camera_h camera, int *value);

/**
 * @brief Gets the available exposure value.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks If the min value is greater than the max value, it means that this feature is not supported.
 * @param[in] camera The handle to the camera
 * @param[out] min The minimum exposure value
 * @param[out] max The maximum exposure value
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_attr_set_exposure()
 */
int camera_attr_get_exposure_range(camera_h camera, int *min, int *max);

/**
 * @brief Sets the ISO level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] iso The ISO level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_attr_get_iso()
 * @see camera_attr_foreach_supported_iso()
 */
int camera_attr_set_iso(camera_h camera, camera_attr_iso_e iso);

/**
 * @brief Gets the ISO level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] iso The ISO level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported ISO levels by invoking the callback function once for each supported ISO level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data	The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_iso_cb() to get all the supported ISO levels.
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
 * @brief Sets the theater mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks If you want to display the preview image on the external display with the full screen mode, use this function.
 * @param[in] camera The handle to the camera
 * @param[in] mode The mode to change
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre	This function is valid only when the external display is connected.
 * @see	camera_attr_get_theater_mode()
 */
int camera_attr_set_theater_mode(camera_h camera, camera_attr_theater_mode_e mode);

/**
 * @brief Gets the theater mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] mode Currnet theater mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_attr_get_theater_mode()
 */
int camera_attr_get_theater_mode(camera_h camera, camera_attr_theater_mode_e *mode);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported theater modes by invoking callback function once for each supported theater modes.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_theater_mode_cb() to get all supported theater modes.
 * @see camera_attr_set_theater_mode()
 * @see camera_attr_get_theater_mode()
 * @see	camera_attr_supported_theater_mode_cb()
 */
int camera_attr_foreach_supported_theater_mode(camera_h camera,
        camera_attr_supported_theater_mode_cb callback, void *user_data);

/**
 * @}
 */


/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the brightness level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] level The brightness level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_get_brightness()
 * @see camera_attr_get_brightness_range()
 */
int camera_attr_set_brightness(camera_h camera, int level);

/**
 * @brief Gets the brightness level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] level The brightness level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_brightness()
 * @see camera_attr_get_brightness_range()
 */
int camera_attr_get_brightness(camera_h camera, int *level);

/**
 * @brief Gets the available brightness level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks If the min value is greater than the max value, it means that this feature is not supported.
 * @param[in] camera The handle to the camera
 * @param[out] min The minimum brightness level
 * @param[out] max The maximum brightness level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_brightness()
 * @see camera_attr_get_brightness()
 */
int camera_attr_get_brightness_range(camera_h camera, int *min, int *max);

/**
 * @brief Sets the contrast level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] level The contrast level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_get_contrast()
 * @see camera_attr_get_contrast_range()
 */
int camera_attr_set_contrast(camera_h camera, int level);

/**
 * @brief Gets the contrast level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] level The contrast level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_contrast()
 * @see camera_attr_get_contrast_range()
 */
int camera_attr_get_contrast(camera_h camera, int *level);

/**
 * @brief Gets the available contrast level.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks If the min value is greater than the max value, it means that this feature is not supported.
 * @param[in] camera The handle to the camera
 * @param[out] min The minimum contrast level
 * @param[out] max The maximum contrast level
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_contrast()
 * @see camera_attr_get_contrast()
 */
int camera_attr_get_contrast_range(camera_h camera, int *min , int *max);

/**
 * @brief Sets the white balance mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] whitebalance The white balance mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_foreach_supported_whitebalance()
 * @see camera_attr_get_whitebalance()
 */
int camera_attr_set_whitebalance(camera_h camera, camera_attr_whitebalance_e whitebalance);

/**
 * @brief Gets the white balance mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] whitebalance The white balance mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported white balances by invoking the callback function once for each supported white balance.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to be invoked
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_whitebalance_cb() to get all the supported white balances.
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] effect The camera effect mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_foreach_supported_effect()
 * @see camera_attr_get_effect()
 */
int camera_attr_set_effect(camera_h camera, camera_attr_effect_mode_e effect);


/**
 * @brief Gets the camera effect mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] effect The camera effect mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported effect modes by invoking the callback function once for each supported effect mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_effect_cb() to get all the supported effect modes.
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] mode The scene mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_foreach_supported_scene_mode()
 * @see camera_attr_get_scene_mode()
 */
int camera_attr_set_scene_mode(camera_h camera, camera_attr_scene_mode_e mode);

/**
 * @brief Gets the scene mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] mode The scene mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported scene modes by invoking the callback function once for each supported scene mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_scene_mode_cb() to get all the supported scene modes.
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
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] enable If @c true writing EXIF tags in a JPEG file is enabled, otherwise @c false
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see         camera_attr_is_enabled_tag()
 */
int camera_attr_enable_tag(camera_h camera, bool enable);

/**
 * @brief Gets the value that indicates whether writing EXIF(Exchangeable image file format) tags in a JPEG file is enabled.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] enabled  If @c true camera information is enabled, otherwise @c false
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see         camera_attr_enable_tag()
 */
int camera_attr_is_enabled_tag(camera_h camera, bool *enabled);

/**
 * @brief Sets the camera image description in the EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] description The string with description
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_get_tag_image_description()
 */
int camera_attr_set_tag_image_description(camera_h camera, const char *description);

/**
 * @brief Gets the camera image description in EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a description using free().
 * @param[in] camera The handle to the camera
 * @param[out] description A pointer to a string
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_tag_image_description()
 */
int camera_attr_get_tag_image_description(camera_h camera, char **description);

/**
 * @brief Sets the camera orientation in the EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] orientation The camera orientation
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_attr_get_tag_orientation()
 */
int camera_attr_set_tag_orientation(camera_h camera, camera_attr_tag_orientation_e orientation);

/**
 * @brief Gets the camera orientation in the EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] orientation The camera orientation
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see	camera_attr_set_tag_orientation()
 */
int camera_attr_get_tag_orientation(camera_h camera, camera_attr_tag_orientation_e *orientation);

/**
 * @brief Sets the software information in the EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] software The software information tag
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_get_tag_software()
 */
int camera_attr_set_tag_software(camera_h camera, const char *software);

/**
 * @brief Gets the software information in the EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks You must release @a software using free().
 * @param[in] camera The handle to the camera
 * @param[out] software A pointer to a string
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_tag_software()
 */
int camera_attr_get_tag_software(camera_h camera, char **software);

/**
 * @brief Sets the geotag(GPS data) in the EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] latitude The latitude data
 * @param[in] longitude The longitude data
 * @param[in] altitude The altitude data
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_get_geotag()
 * @see camera_attr_remove_geotag()
 */
int camera_attr_set_geotag(camera_h camera, double latitude , double longitude, double altitude);

/**
 * @brief Gets the geotag(GPS data) in the EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] latitude The latitude data
 * @param[out] longitude The longitude data
 * @param[out] altitude The altitude data
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_geotag()
 * @see camera_attr_remove_geotag()
 */
int camera_attr_get_geotag(camera_h camera, double *latitude , double *longitude, double *altitude);

/**
 * @brief Removes the geotag(GPS data) in the EXIF(Exchangeable image file format) tag.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_geotag()
 * @see camera_attr_get_geotag()
 */
int camera_attr_remove_geotag(camera_h camera);

/**
 * @brief Sets the camera's flash mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks Since 2.4, while setting the flash mode, if the flash was preempted by other APIs,\n
            then this function returns #CAMERA_ERROR_DEVICE_BUSY error.
 * @param[in] camera The handle to the camera
 * @param[in] mode The flash mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @retval #CAMERA_ERROR_DEVICE_BUSY The flash was preempted by other API
 * @see	camera_attr_foreach_supported_flash_mode()
 * @see camera_attr_get_flash_mode()
 */
int camera_attr_set_flash_mode(camera_h camera, camera_attr_flash_mode_e mode);

/**
 * @brief Gets the camera's flash mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] mode The flash mode
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
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
 * @brief Retrieves all supported flash modes by invoking the callback function once for each supported flash mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data passed to the callback registration function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_flash_mode_cb() to get all supported flash modes.
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
 * @brief Gets the camera len's orientation angle.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] angle The orientation angle
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_set_display_rotation()
 */
int camera_attr_get_lens_orientation(camera_h camera, int *angle);

/**
 * @brief Sets the stream rotation.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] rotation The stream rotation
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre	The camera state must be set to #CAMERA_STATE_CREATED.
 * @see camera_attr_get_stream_rotation()
 */
int camera_attr_set_stream_rotation(camera_h camera , camera_rotation_e rotation);

/**
 * @brief Gets the stream rotation.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] rotation	The stream rotation
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre	The camera state must be set to #CAMERA_STATE_CREATED.
 * @see camera_attr_set_stream_rotation()
 */
int camera_attr_get_stream_rotation(camera_h camera , camera_rotation_e *rotation);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported stream rotation modes by invoking callback function once for each supported stream rotation mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_stream_rotation_cb() to get all supported stream rotation mode.
 * @see camera_attr_set_stream_rotation()
 * @see camera_attr_get_stream_rotation()
 * @see camera_attr_supported_stream_rotation_cb()
 */
int camera_attr_foreach_supported_stream_rotation(camera_h camera,
        camera_attr_supported_stream_rotation_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Sets the stream flip.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] flip The stream flip
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre	The camera state must be set to #CAMERA_STATE_CREATED.
 * @see camera_attr_set_stream_rotation()
 */
int camera_attr_set_stream_flip(camera_h camera , camera_flip_e flip);

/**
 * @brief Gets the stream flip.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] flip  The stream flip
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @pre	The camera state must be set to #CAMERA_STATE_CREATED.
 * @see camera_attr_set_stream_rotation()
 */
int camera_attr_get_stream_flip(camera_h camera , camera_flip_e *flip);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Retrieves all supported stream flip modes by invoking callback function once for each supported stream flip mode.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data to be passed to the callback function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @post This function invokes camera_attr_supported_stream_flip_cb() to get all supported stream flip mode.
 * @see camera_attr_set_stream_flip()
 * @see camera_attr_get_stream_flip()
 * @see camera_attr_supported_stream_flip_cb()
 */
int camera_attr_foreach_supported_stream_flip(camera_h camera,
        camera_attr_supported_stream_flip_cb callback, void *user_data);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Called when the HDR capture process is updated.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] percent The progress percentage of HDR capture
 * @param[in] user_data The user data passed from the callback registration function
 * @pre camera_start_capture() will invoke this callback if you register it using camera_attr_set_hdr_capture_progress_cb().
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 */
typedef void (*camera_attr_hdr_progress_cb)(int percent, void *user_data);

/**
 * @brief Sets the mode of HDR(High dynamic range) capture.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks Taking multiple pictures at different exposure levels and intelligently stitching them together so that we eventually arrive at a picture that is representative in both dark and bright areas.\n
 *          If this attribute is set to @c true. camera_attr_hdr_progress_cb() is invoked during capture.\n
 *          If you set #CAMERA_ATTR_HDR_MODE_KEEP_ORIGINAL, the capturing callback is invoked twice. The first callback is delivering origin image data. The second callback is delivering improved image data.
 * @param[in] camera The handle to the camera
 * @param[in] mode The mode of HDR capture
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 *
 */
int camera_attr_set_hdr_mode(camera_h camera, camera_attr_hdr_mode_e mode);

/**
 * @brief Gets the mode of HDR(High dynamic range) capture.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] mode The mode of HDR capture
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 */
int camera_attr_get_hdr_mode(camera_h camera, camera_attr_hdr_mode_e *mode);

/**
 * @brief Registers a callback function to be called when HDR capture is progressing.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks This callback notifies progress of the HDR process.
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to invoke
 * @param[in] user_data The user data passed to the callback registration function
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_hdr_mode()
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 */
int camera_attr_set_hdr_capture_progress_cb(camera_h camera, camera_attr_hdr_progress_cb callback, void* user_data);

/**
 * @brief Unregisters the callback function.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_hdr_mode()
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_is_supported_hdr_capture()
 */
int camera_attr_unset_hdr_capture_progress_cb(camera_h camera);

/**
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @brief Gets the support state of HDR capture.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] camera The handle to the camera
 * @return @c true if supported, otherwise @c false
 * @exception #CAMERA_ERROR_NONE Successful
 * @exception #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @exception #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_set_hdr_mode()
 * @see camera_attr_get_hdr_mode()
 * @see camera_attr_set_hdr_capture_progress_cb()
 * @see camera_attr_unset_hdr_capture_progress_cb()
 */
bool camera_attr_is_supported_hdr_capture(camera_h camera);

/**
 * @brief Enables/Disables the anti-shake feature.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This feature is used for image capture.
 * @param[in] camera The handle to the camera
 * @param[in] enable If @c true the anti-shake feature is enabled, otherwise @c false
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_is_enabled_anti_shake()
 * @see camera_attr_is_supported_anti_shake()
 *
 */
int camera_attr_enable_anti_shake(camera_h camera, bool enable);

/**
 * @brief Gets the state of the anti-shake feature.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] enabled The state of anti-shake
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_enable_anti_shake()
 * @see camera_attr_is_supported_anti_shake()
 */
int camera_attr_is_enabled_anti_shake(camera_h camera , bool *enabled);

/**
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @brief Gets the support state of the anti-shake feature.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] camera The handle to the camera
 * @return @c true if supported, otherwise @c false
 * @exception #CAMERA_ERROR_NONE Successful
 * @exception #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @exception #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_enable_anti_shake()
 * @see camera_attr_is_enabled_anti_shake()
 */
bool camera_attr_is_supported_anti_shake(camera_h camera);

/**
 * @brief Enables/Disables the video stabilization feature.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @remarks If video stabilization is enabled, zero shutter lag is disabled.\n
 *          This feature is used to record a video.
 * @param[in] camera The handle to the camera
 * @param[in] enable If @c true video stabilization is enabled, otherwise @c false
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_is_enabled_video_stabilization()
 * @see camera_attr_is_supported_video_stabilization()
 *
 */
int camera_attr_enable_video_stabilization(camera_h camera, bool enable);

/**
 * @brief Gets the state of the video stabilization feature.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] enabled The state of video stabilization
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_enable_video_stabilization()
 * @see camera_attr_is_supported_video_stabilization()
 */
int camera_attr_is_enabled_video_stabilization(camera_h camera, bool *enabled);

/**
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @brief Gets the support state of the video stabilization feature.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] camera The handle to the camera
 * @return @c true if supported, otherwise @c false
 * @exception #CAMERA_ERROR_NONE Successful
 * @exception #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @exception #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_enable_video_stabilization()
 * @see camera_attr_is_enabled_video_stabilization()
 */
bool camera_attr_is_supported_video_stabilization(camera_h camera);

/**
 * @brief Enables/Disables auto contrast.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @privlevel public
 * @privilege %http://tizen.org/privilege/camera
 * @param[in] camera The handle to the camera
 * @param[in] enable If @c true auto contrast is enabled, otherwise @c false
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_is_enabled_auto_contrast()
 */
int camera_attr_enable_auto_contrast(camera_h camera, bool enable);

/**
 * @brief Gets the state of auto contrast.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] camera The handle to the camera
 * @param[out] enabled The state of auto contrast
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_enable_auto_contrast()
 */
int camera_attr_is_enabled_auto_contrast(camera_h camera, bool *enabled);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @{
 */

/**
 * @brief Gets state of support of auto contrast feature.
 * @ingroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks The specific error code can be obtained using the get_last_result() method. Error codes are described in Exception section.
 * @param[in] camera The handle to the camera
 * @return true on supported, otherwise false
 * @exception #CAMERA_ERROR_NONE Successful
 * @exception #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @exception #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @exception #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 * @see camera_attr_enable_auto_contrast()
 * @see camera_attr_is_enabled_auto_contrast()
 */
bool camera_attr_is_supported_auto_contrast(camera_h camera);

/**
 * @}
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE
 * @{
 */

/**
 * @brief Disables shutter sound.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @remarks In some countries, this operation is not permitted.
 * @param[in] camera The handle to the camera
 * @param[in] disable If @c true shutter sound is disabled, otherwise @c false
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_OPERATION Disabling shutter sound is not permitted
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 */
int camera_attr_disable_shutter_sound(camera_h camera, bool disable);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_CAMERA_H__ */

