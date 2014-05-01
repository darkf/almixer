/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libALmixer.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef _ALmixer_config_h
#define _ALmixer_config_h

#include "ALmixer_platform.h"

/**
 *  \file ALmixer_config.h
 */

/* Add any platform that doesn't build using the configure system. */
#ifdef USING_PREMAKE_CONFIG_H
#include "ALmixer_config_premake.h"
#elif defined(__WIN32__)
#include "ALmixer_config_windows.h"
#elif defined(__WINRT__)
#include "ALmixer_config_winrt.h"
#elif defined(__MACOSX__)
#include "ALmixer_config_macosx.h"
#elif defined(__IPHONEOS__)
#include "ALmixer_config_iphoneos.h"
#elif defined(__ANDROID__)
#include "ALmixer_config_android.h"
#elif defined(__PSP__)
#include "ALmixer_config_psp.h"
#else
/* This is a minimal configuration just to get ALmixer running on new platforms */
#include "ALmixer_config_minimal.h"
#endif /* platform config */

#ifdef USING_GENERATED_CONFIG_H
#error Wrong ALmixer_config.h, check your include path?
#endif

#endif /* _ALmixer_config_h */
