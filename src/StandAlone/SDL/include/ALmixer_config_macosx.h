/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libALMIXER.org>

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

#ifndef _ALMIXER_config_macosx_h
#define _ALMIXER_config_macosx_h

#include "ALMIXER_platform.h"

/* This gets us MAC_OS_X_VERSION_MIN_REQUIRED... */
#include <AvailabilityMacros.h>

/* This is a set of defines to configure the ALMIXER features */

#ifdef __LP64__
    #define SIZEOF_VOIDP 8
#else
    #define SIZEOF_VOIDP 4
#endif

/* Useful headers */
#define HAVE_ALLOCA_H       1
#define HAVE_SYS_TYPES_H    1
#define HAVE_STDIO_H    1
#define STDC_HEADERS    1
#define HAVE_STRING_H   1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H   1
#define HAVE_CTYPE_H    1
#define HAVE_MATH_H 1
#define HAVE_SIGNAL_H   1

/* C library functions */
#define HAVE_MALLOC 1
#define HAVE_CALLOC 1
#define HAVE_REALLOC    1
#define HAVE_FREE   1
#define HAVE_ALLOCA 1
#define HAVE_GETENV 1
#define HAVE_SETENV 1
#define HAVE_PUTENV 1
#define HAVE_UNSETENV   1
#define HAVE_QSORT  1
#define HAVE_ABS    1
#define HAVE_BCOPY  1
#define HAVE_MEMSET 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE    1
#define HAVE_MEMCMP 1
#define HAVE_STRLEN 1
#define HAVE_STRLCPY    1
#define HAVE_STRLCAT    1
#define HAVE_STRDUP 1
#define HAVE_STRCHR 1
#define HAVE_STRRCHR    1
#define HAVE_STRSTR 1
#define HAVE_STRTOL 1
#define HAVE_STRTOUL    1
#define HAVE_STRTOLL    1
#define HAVE_STRTOULL   1
#define HAVE_STRTOD 1
#define HAVE_ATOI   1
#define HAVE_ATOF   1
#define HAVE_STRCMP 1
#define HAVE_STRNCMP    1
#define HAVE_STRCASECMP 1
#define HAVE_STRNCASECMP 1
#define HAVE_VSSCANF 1
#define HAVE_VSNPRINTF  1
#define HAVE_CEIL   1
#define HAVE_COPYSIGN   1
#define HAVE_COS    1
#define HAVE_COSF   1
#define HAVE_FABS   1
#define HAVE_FLOOR  1
#define HAVE_LOG    1
#define HAVE_POW    1
#define HAVE_SCALBN 1
#define HAVE_SIN    1
#define HAVE_SINF   1
#define HAVE_SQRT   1
#define HAVE_SIGACTION  1
#define HAVE_SETJMP 1
#define HAVE_NANOSLEEP  1
#define HAVE_SYSCONF    1
#define HAVE_SYSCTLBYNAME 1
#define HAVE_ATAN 1
#define HAVE_ATAN2 1
#define HAVE_ACOS 1
#define HAVE_ASIN 1

/* Enable various audio drivers */
#define ALMIXER_AUDIO_DRIVER_COREAUDIO  1
#define ALMIXER_AUDIO_DRIVER_DISK   1
#define ALMIXER_AUDIO_DRIVER_DUMMY  1

/* Enable various input drivers */
#define ALMIXER_JOYSTICK_IOKIT  1
#define ALMIXER_HAPTIC_IOKIT    1

/* Enable various shared object loading systems */
#define ALMIXER_LOADSO_DLOPEN   1

/* Enable various threading systems */
#define ALMIXER_THREAD_PTHREAD  1
#define ALMIXER_THREAD_PTHREAD_RECURSIVE_MUTEX  1

/* Enable various timer systems */
#define ALMIXER_TIMER_UNIX  1

/* Enable various video drivers */
#define ALMIXER_VIDEO_DRIVER_COCOA  1
#define ALMIXER_VIDEO_DRIVER_DUMMY  1
#undef ALMIXER_VIDEO_DRIVER_X11
#define ALMIXER_VIDEO_DRIVER_X11_DYNAMIC "/usr/X11R6/lib/libX11.6.dylib"
#define ALMIXER_VIDEO_DRIVER_X11_DYNAMIC_XEXT "/usr/X11R6/lib/libXext.6.dylib"
#define ALMIXER_VIDEO_DRIVER_X11_DYNAMIC_XINERAMA "/usr/X11R6/lib/libXinerama.1.dylib"
#define ALMIXER_VIDEO_DRIVER_X11_DYNAMIC_XINPUT2 "/usr/X11R6/lib/libXi.6.dylib"
#define ALMIXER_VIDEO_DRIVER_X11_DYNAMIC_XRANDR "/usr/X11R6/lib/libXrandr.2.dylib"
#define ALMIXER_VIDEO_DRIVER_X11_DYNAMIC_XSS "/usr/X11R6/lib/libXss.1.dylib"
#define ALMIXER_VIDEO_DRIVER_X11_DYNAMIC_XVIDMODE "/usr/X11R6/lib/libXxf86vm.1.dylib"
#define ALMIXER_VIDEO_DRIVER_X11_XINERAMA 1
#define ALMIXER_VIDEO_DRIVER_X11_XRANDR 1
#define ALMIXER_VIDEO_DRIVER_X11_XSCRNSAVER 1
#define ALMIXER_VIDEO_DRIVER_X11_XSHAPE 1
#define ALMIXER_VIDEO_DRIVER_X11_XVIDMODE 1
#define ALMIXER_VIDEO_DRIVER_X11_HAS_XKBKEYCODETOKEYSYM 1

#ifdef MAC_OS_X_VERSION_10_8
/*
 * No matter the versions targeted, this is the 10.8 or later SDK, so you have
 *  to use the external Xquartz, which is a more modern Xlib. Previous SDKs
 *  used an older Xlib.
 */
#define ALMIXER_VIDEO_DRIVER_X11_XINPUT2 1
#define ALMIXER_VIDEO_DRIVER_X11_SUPPORTS_GENERIC_EVENTS 1
#define ALMIXER_VIDEO_DRIVER_X11_CONST_PARAM_XEXTADDDISPLAY 1
#endif

#ifndef ALMIXER_VIDEO_RENDER_OGL
#define ALMIXER_VIDEO_RENDER_OGL    1
#endif

/* Enable OpenGL support */
#ifndef ALMIXER_VIDEO_OPENGL
#define ALMIXER_VIDEO_OPENGL    1
#endif
#ifndef ALMIXER_VIDEO_OPENGL_CGL
#define ALMIXER_VIDEO_OPENGL_CGL    1
#endif
#ifndef ALMIXER_VIDEO_OPENGL_GLX
#define ALMIXER_VIDEO_OPENGL_GLX    1
#endif

/* Enable system power support */
#define ALMIXER_POWER_MACOSX 1

/* enable filesystem support */
#define ALMIXER_FILESYSTEM_COCOA   1

/* Enable assembly routines */
#define ALMIXER_ASSEMBLY_ROUTINES   1
#ifdef __ppc__
#define ALMIXER_ALTIVEC_BLITTERS    1
#endif

#endif /* _ALMIXER_config_macosx_h */
