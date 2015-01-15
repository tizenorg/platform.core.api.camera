/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef __TIZEN_MEDIA_CAMERA_DOC_H__
#define __TIZEN_MEDIA_CAMERA_DOC_H__


/**
 * @file camera_doc.h
 * @brief This file contains high level documentation for the Camera API.
 */

/**
 * @ingroup CAPI_MEDIA_FRAMEWORK
 * @defgroup CAPI_MEDIA_CAMERA_MODULE Camera
 * @brief The @ref CAPI_MEDIA_CAMERA_MODULE API provides functions for camera preview, capture, and focusing.
 *
 * @section CAPI_MEDIA_CAMERA_MODULE_HEADER Required Header
 *   \#include <camera.h>
 *
 * @section CAPI_MEDIA_CAMERA_MODULE_OVERVIEW Overview
 * The Camera API allows application developers to support using the camera. It includes functions that capture photos and support setting up
 * notifications for state changes of capturing, previewing, focusing, information about resolution and binary data format, and functions for artistic
 * picture manipulations like sepia, negative, sketch, and many more.
 *
 * The Camera API allows creation of components required in taking pictures including:
 * - selecting proper exposure parameters
 * - selecting a proper output format
 * - displaying a photo preview
 * - triggering the capture phase
 * - controlling and displaying the camera state
 * - getting supported formats and picture resolutions
 * - getting a preview format and a resolution
 *
 * The Camera API also notifies you (by callback mechanism) when a significant picture parameter changes e.g. focus.
 *
 * @subsection CAPI_MEDIA_CAMERA_LIFE_CYCLE_STATE_DIAGRAM State Diagram
 * @image html capi_media_camera_state_diagram.png
 *
 * @subsection CAPI_MEDIA_CAMERA_LIFE_CYCLE_STATE_TRANSITIONS State Transitions
 * <div><table class="doxtable" >
 * <tr>
 *    <th><b>FUNCTION</b></th>
 *    <th><b>PRE-STATE</b></th>
 *    <th><b>POST-STATE</b></th>
 *    <th><b>SYNC TYPE</b></th>
 * </tr>
 * <tr>
 *    <td> camera_create() </td>
 *    <td> NONE </td>
 *    <td> CREATED </td>
 *    <td> SYNC </td>
 * </tr>
 * <tr>
 *    <td> camera_destroy() </td>
 *    <td> CREATED </td>
 *    <td> NONE </td>
 *    <td> SYNC </td>
 * </tr>
 * <tr>
 *    <td> camera_start_preview() </td>
 *    <td> CREATED/CAPTURED </td>
 *    <td> PREVIEW </td>
 *    <td> SYNC </td>
 * </tr>
 * <tr>
 *    <td> camera_stop_preview() </td>
 *    <td> PREVIEW </td>
 *    <td> CREATED </td>
 *    <td> SYNC </td>
 * </tr>
 * <tr>
 *    <td> camera_start_capture() </td>
 *    <td> PREVIEW </td>
 *    <td> CAPTURING -> CAPTURED* </td>
 *    <td> SYNC </td>
 * </tr>
 * </table></div>
 *
 * (*) The transition from the CAPTURING state to the CAPTURED state will be processed automatically. \n
 *
 * @subsection CAPI_MEDIA_CAMERA_LIFE_CYCLE_CALLBACK_OPERATIONS Callback(Event) Operations
 * The callback mechanism is used to notify the application about significant camera events.
 * <div><table class="doxtable" >
 *     <tr>
 *        <th><b>REGISTER</b></th>
 *        <th><b>UNREGISTER</b></th>
 *        <th><b>CALLBACK</b></th>
 *        <th><b>DESCRIPTION</b></th>
 *     </tr>
 *     <tr>
 *        <td>camera_set_preview_cb()</td>
 *        <td>camera_unset_preview_cb()</td>
 *        <td>camera_preview_cb()</td>
 *        <td>This callback is used to display the preview (it delivers copies of previous frames) </td>
 *     </tr>
 *		 <tr>
 *        <td>camera_set_state_changed_cb()</td>
 *        <td>camera_unset_state_changed_cb()</td>
 *        <td>camera_state_changed_cb()</td>
 *        <td>This callback is used to notify that the camera state has changed. Not executed in the recorder mode </td>
 *     </tr>
 *		 <tr>
 *        <td>camera_set_focus_changed_cb()</td>
 *        <td>camera_unset_focus_changed_cb()</td>
 *        <td>camera_focus_changed_cb()</td>
 *        <td>This callback is used to notify that the camera has changed focus (and it is ready to shoot) </td>
 *     </tr>
 *		 <tr>
 *        <td>camera_set_error_cb()</td>
 *        <td>camera_unset_error_cb()</td>
 *        <td>camera_error_cb()</td>
 *        <td>This callback is used to notify that an error has occurred</td>
 *     </tr>
 *</table></div>
 *
 * @subsection CAPI_MEDIA_CAMERA_FOREACH_OPERATIONS Foreach Operations
 * <div><table class="doxtable" >
 *     <tr>
 *        <th><b>FOREACH</b></th>
 *        <th><b>CALLBACK</b></th>
 *        <th><b>DESCRIPTION</b></th>
 *     </tr>
 *     <tr>
 *        <td>camera_foreach_supported_preview_resolution()</td>
 *        <td>camera_supported_preview_resolution_cb()</td>
 *        <td>Supported preview resolutions</td>
 *     </tr>
 *     <tr>
 *        <td>camera_foreach_supported_capture_resolution()</td>
 *        <td>camera_supported_capture_resolution_cb()</td>
 *        <td>Supported camera capture resolutions</td>
 *     </tr>
 *     <tr>
 *        <td>camera_foreach_supported_capture_format()</td>
 *        <td>camera_supported_capture_format_cb()</td>
 *        <td>Supported camera capture formats</td>
 *     </tr>
 *     <tr>
 *        <td>camera_foreach_supported_preview_format()</td>
 *        <td>camera_supported_preview_format_cb()</td>
 *        <td>Supported camera preview format</td>
 *     </tr>
 *</table></div>
 *
 * @section CAPI_MEDIA_CAMERA_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/camera\n
 *  - http://tizen.org/feature/camera.back\n
 *  - http://tizen.org/feature/camera.back.flash\n
 *  - http://tizen.org/feature/camera.front\n
 *  - http://tizen.org/feature/camera.front.flash
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 *
 */


/**
 * @ingroup CAPI_MEDIA_CAMERA_MODULE
 * @defgroup CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE Attributes
 * @brief The @ref CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE API provides functions to fetch camera attributes. 
 *
 * @section CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE_HEADER Required Header
 *   \#include <camera.h>
 *
 * @section CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE_OVERVIEW Overview
 * The Camera Attributes API provides functions to get/set basic camera attributes.
 *
 * With the Camera Attributes API you are able to set and get camera attributes like:
 * <ul>
 *  <li>Preview frame rate</li>
 *  <li>Supported FPS</li>
 *  <li>Image quality</li>
 *  <li>Zoom level</li>
 *  <li>Auto focus mode</li>
 *  <li>Supported auto focus mode</li>
 *  <li>Exposure mode</li>
 *  <li>Supported exposure mode</li>
 *  <li>Exposure value</li>
 *  <li>ISO level</li>
 *  <li>Supported ISO</li>
 *  <li>Brightness level</li>
 *  <li>Contrast level</li>
 *  <li>White balance mode</li>
 *  <li>Supported white balance</li>
 *  <li>Effect mode</li>
 *  <li>Supported effect mode</li>
 *  <li>Scene mode</li>
 *  <li>Supported scene mode</li>
 *  <li>On/off camera information</li>
 *  <li>Image description</li>
 *  <li>Orientation information</li>
 *  <li>Software tag</li>
 *  <li>GPS latitude data</li>
 *  <li>GPS longitude data</li>
 *  <li>GPS altitude data</li>
 *  <li>Flash mode</li>
 *  <li>Supported flash mode</li>
 * </ul>
 *
 * @subsection CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE_FOREACH_OPERATIONS Foreach Operations
 * <div><table class="doxtable" >
 *     <tr>
 *        <th><b>FOREACH</b></th>
 *        <th><b>CALLBACK</b></th>
 *        <th><b>DESCRIPTION</b></th>
 *     </tr>
 *     <tr>
 *        <td>camera_attr_foreach_supported_fps()</td>
 *        <td>camera_attr_supported_fps_cb()</td>
 *        <td>Supported FPS</td>
 *     </tr>
 *     <tr>
 *        <td>camera_attr_foreach_supported_af_mode()</td>
 *        <td>camera_attr_supported_af_mode_cb()</td>
 *        <td>Supported auto focus mode</td>
 *     </tr>
 *     <tr>
 *        <td>camera_attr_foreach_supported_exposure_mode()</td>
 *        <td>camera_attr_supported_exposure_mode_cb()</td>
 *        <td>Supported exposure mode</td>
 *     </tr>
 *     <tr>
 *        <td>camera_attr_foreach_supported_iso()</td>
 *        <td>camera_attr_supported_iso_cb()</td>
 *        <td>Supported ISO</td>
 *     </tr>
 *     <tr>
 *        <td>camera_attr_foreach_supported_whitebalance()</td>
 *        <td>camera_attr_supported_whitebalance_cb()</td>
 *        <td>Supported whitebalance</td>
 *     </tr>
 *     <tr>
 *        <td>camera_attr_foreach_supported_effect()</td>
 *        <td>camera_attr_supported_effect_cb()</td>
 *        <td>Supported effect mode</td>
 *     </tr>
 *     <tr>
 *        <td>camera_attr_foreach_supported_scene_mode()</td>
 *        <td>camera_attr_supported_scene_mode_cb()</td>
 *        <td>Supported scene mode</td>
 *     </tr>
 *     <tr>
 *        <td>camera_attr_foreach_supported_flash_mode()</td>
 *        <td>camera_attr_supported_flash_mode_cb()</td>
 *        <td>Supported flash mode</td>
 *     </tr>
 *</table></div>
 *
 * @section CAPI_MEDIA_CAMERA_ATTRIBUTES_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/camera\n
 *  - http://tizen.org/feature/camera.back\n
 *  - http://tizen.org/feature/camera.back.flash\n
 *  - http://tizen.org/feature/camera.front\n
 *  - http://tizen.org/feature/camera.front.flash
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 *
 */

/**
 * @ingroup CAPI_MEDIA_CAMERA_MODULE
 * @defgroup CAPI_MEDIA_CAMERA_CAPABILITY_MODULE Capability
 * @brief The @ref CAPI_MEDIA_CAMERA_CAPABILITY_MODULE API provides capability information of the camera.
 * @section CAPI_MEDIA_CAMERA_CAPABILITY_MODULE_HEADER Required Header
 *   \#include <camera.h>
 *
 * @section CAPI_MEDIA_CAMERA_CAPABILITY_MODULE_OVERVIEW Overview
 * The Camera Capability API provides functions to obtain capability information of the camera device.
 *
 * @section CAPI_MEDIA_CAMERA_CAPABILITY_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/camera\n
 *  - http://tizen.org/feature/camera.back\n
 *  - http://tizen.org/feature/camera.back.flash\n
 *  - http://tizen.org/feature/camera.front\n
 *  - http://tizen.org/feature/camera.front.flash
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 *
 */

/**
 * @ingroup CAPI_MEDIA_CAMERA_MODULE
 * @defgroup CAPI_MEDIA_CAMERA_DISPLAY_MODULE Display
 * @brief The @ref CAPI_MEDIA_CAMERA_DISPLAY_MODULE API provides functions to control display.
 * @section CAPI_MEDIA_CAMERA_DISPLAY_MODULE_HEADER Required Header
 *   \#include <camera.h>
 *
 * @section CAPI_MEDIA_CAMERA_DISPLAY_MODULE_OVERVIEW Overview
 * The Display API allows you to manage display for the camera. This API provides you with
 * functions that set and get various display properties :
 * <ul>
 *   <li>rotation</li>
 *   <li>display visibility</li>
 *   <li>display mode</li>
 * </ul>
 *
 * @section CAPI_MEDIA_CAMERA_DISPLAY_MODULE_FEATURE Related Features
 * This API is related with the following features:\n
 *  - http://tizen.org/feature/camera\n
 *  - http://tizen.org/feature/camera.back\n
 *  - http://tizen.org/feature/camera.back.flash\n
 *  - http://tizen.org/feature/camera.front\n
 *  - http://tizen.org/feature/camera.front.flash
 *
 * It is recommended to design feature related codes in your application for reliability.\n
 * You can check if a device supports the related features for this API by using @ref CAPI_SYSTEM_SYSTEM_INFO_MODULE, thereby controlling the procedure of your application.\n
 * To ensure your application is only running on the device with specific features, please define the features in your manifest file using the manifest editor in the SDK.\n
 * More details on featuring your application can be found from <a href="../org.tizen.mobile.native.appprogramming/html/ide_sdk_tools/feature_element.htm"><b>Feature Element</b>.</a>
 *
 */

#endif /* __TIZEN_MEDIA_CAMERA_DOC_H__ */


