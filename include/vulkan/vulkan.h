#ifndef VULKAN_H_
#define VULKAN_H_ 1

/*
** Copyright 2015-2024 The Khronos Group Inc.
**
** SPDX-License-Identifier: Apache-2.0
*/

#include "vk_platform.h"
#include "vulkan_core.h"

#ifdef VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include "vulkan_win32.h"
#endif

#endif // VULKAN_H_