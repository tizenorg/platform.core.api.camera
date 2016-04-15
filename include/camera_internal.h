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

#ifndef __TIZEN_MULTIMEDIA_CAMERA_INTERNAL_H__
#define __TIZEN_MULTIMEDIA_CAMERA_INTERNAL_H__

#include <camera.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file camera_internal.h
 * @brief This file contains the internal Camera API, related structures and enumerations.
 * @since_tizen 3.0
 */

/**
 * @addtogroup CAPI_MEDIA_CAMERA_INTERNAL_MODULE
 * @{
 */

/**
 * @brief Sets the evas rendering flag.
 *
 * @since_tizen 3.0
 * @param[in] camera The handle to the camera
 * @param[in] enable If @c true enable EVAS rendering, otherwise @c false
 * @return @c 0 on success, otherwise a negative error value
 * @retval #CAMERA_ERROR_NONE Successful
 * @retval #CAMERA_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #CAMERA_ERROR_INVALID_STATE Invalid state
 * @retval #CAMERA_ERROR_INVALID_OPERATION Invalid operation
 * @retval #CAMERA_ERROR_NOT_SUPPORTED The feature is not supported
 */
int camera_set_evas_rendering(camera_h camera, bool enable);

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_MULTIMEDIA_CAMERA_INTERNAL_H__ */
