/*
* Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef __TIZEN_MULTIMEDIA_CAMERA_INTERNAL_H__
#define	__TIZEN_MULTIMEDIA_CAMERA_INTERNAL_H__
#include <camera.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @file camera_internal.h
  * @brief This file contains the Camera Product-internal API for framework, related structures and enumerations.
  */

/**
 * @internal
 * @addtogroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @{
 */

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Called when the media camera needs updated xid.
 * @remarks If current display type is not #CAMERA_DISPLAY_TYPE_OVERLAY, no operation is performed.
 * @param[in] user_data The user data passed from the callback registration function
 * @return The updated xid
 * @pre It will be invoked when camera needs updated xid and if this callback is registered using camera_set_x11_display_pixmap().
 * @see	camera_set_x11_display_pixmap()
 */
typedef unsigned int (*camera_x11_pixmap_updated_cb)(void *user_data);

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Sets the display rotation.
 *
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function should be called before previewing (see camera_start_preview())\n
 *          This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[in] rotation The display rotation
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Display type is not X11
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @see camera_start_preview()
 * @see	camera_get_x11_display_rotation()
 */
int camera_set_x11_display_rotation(camera_h camera, camera_rotation_e rotation);

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Gets the display rotation.
 *
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[out] rotation The display rotation
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @see camera_set_x11_display_rotation()
 */
int camera_get_x11_display_rotation(camera_h camera, camera_rotation_e *rotation);

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Sets the display flip.
 *
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[in] flip The display flip
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Display type is not X11
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @see	camera_get_x11_display_flip()
 */
int camera_set_x11_display_flip(camera_h camera, camera_flip_e flip);

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Gets the display flip.
 *
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[out] flip The display flip
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @see camera_set_x11_display_flip()
 */
int camera_get_x11_display_flip(camera_h camera, camera_flip_e *flip);

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Sets the visible property for X11 display.
 *
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[in] visible The display visibility property
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @see camera_is_x11_display_visible()
 */
int camera_set_x11_display_visible(camera_h camera, bool visible);

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Gets the visible property of X11 display.
 *
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[out] visible If @c true the camera display is visible, otherwise @c false
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @see camera_set_x11_display_visible()
 */
int camera_is_x11_display_visible(camera_h camera, bool *visible);

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Sets the X11 display mode.
 *
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[in] mode The display mode
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @see camera_get_x11_display_mode()
 */
int camera_set_x11_display_mode(camera_h camera , camera_display_mode_e mode);

/**
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @brief Gets the X11 display mode.
 *
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/camera
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[out] mode The display mode
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_PERMISSION_DENIED The access to the resources can not be granted
 * @see camera_set_x11_display_mode()
 */
int camera_get_x11_display_mode(camera_h camera, camera_display_mode_e *mode);

/**
 * @brief Registers a callback function to be invoked when camera needs updated xid.
 * @ingroup CAPI_MEDIA_CAMERA_X11_DISPLAY_MODULE
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[in] callback The callback function to register
 * @param[in] user_data	The user data to be passed to the callback function
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre	The camera state must be #CAMERA_STATE_CREATED by camera_create().
 * @post camera_set_x11_display_pixmap() will be invoked.
 *
 * @see camera_set_x11_display_pixmap()
 */
int camera_set_x11_display_pixmap(camera_h camera, camera_x11_pixmap_updated_cb callback, void *user_data);

/**
 * @brief Registers a callback function to be invoked when camera needs updated xid.
 * @ingroup CAPI_MEDIA_CAMERA_MUSED_MODULE
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[in] type The type of the display
 * @param[in] display_handle The handle of the created display
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre	The camera state must be #CAMERA_STATE_CREATED by camera_create().
 * @post camera_set_mused_display() will be invoked.
 *
 * @see camera_set_mused_display()
 */
int camera_set_mused_display(camera_h camera, camera_display_type_e type, void *display_handle);

/**
 * @brief Registers a callback function to be invoked when camera needs updated xid.
 * @ingroup CAPI_MEDIA_CAMERA_MUSED_MODULE
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[in] caps The caps information of the server's video element
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre	The camera state must be #CAMERA_STATE_CREATED by camera_create().
 * @post camera_get_video_caps() will be invoked.
 *
 * @see camera_get_video_caps()
 */
int camera_get_video_caps(camera_h camera, char **caps);

/**
 * @brief Registers a callback function to be invoked when camera needs updated xid.
 * @ingroup CAPI_MEDIA_CAMERA_MUSED_MODULE
 * @remarks This function is valid only for #CAMERA_DISPLAY_TYPE_OVERLAY.
 * @param[in] camera The handle to the camera
 * @param[in] socket_path The socket file path for the display data ipc
 *
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @pre	The camera state must be #CAMERA_STATE_CREATED by camera_create().
 * @post camera_set_shm_socket_path_for_mused() will be invoked.
 *
 * @see camera_set_shm_socket_path_for_mused()
 */
int camera_set_shm_socket_path_for_mused(camera_h camera, char *socket_path);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif //__TIZEN_MULTIMEDIA_CAMERA_INTERNAL_H__
