/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2014 Sam Lantinga <slouken@libsdl.org>

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

/**
 *  \file ALmixer_system.h
 *
 *  Include file for platform specific SDL API functions
 */

#ifndef _ALmixer_system_h
#define _ALmixer_system_h

#include "ALmixer_stdinc.h"
#if 0
#include "ALmixer_keyboard.h"
#include "ALmixer_render.h"
#include "ALmixer_video.h"
#endif /* #if 0 */

/* I don't want anything in here exported for ALmixer */
#define DECLSPEC
#define SDLCALL


#include "ALmixer_begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#if 0
/* Platform specific functions for Windows */
#ifdef __WIN32__

/* Returns the D3D9 adapter index that matches the specified display index.
   This adapter index can be passed to IDirect3D9::CreateDevice and controls
   on which monitor a full screen application will appear.
*/
extern DECLSPEC int SDLCALL ALmixer_Direct3D9GetAdapterIndex( int displayIndex );

/* Returns the D3D device associated with a renderer, or NULL if it's not a D3D renderer.
   Once you are done using the device, you should release it to avoid a resource leak.
 */
typedef struct IDirect3DDevice9 IDirect3DDevice9;
extern DECLSPEC IDirect3DDevice9* SDLCALL ALmixer_RenderGetD3D9Device(ALmixer_Renderer * renderer);

/* Returns the DXGI Adapter and Output indices for the specified display index. 
   These can be passed to EnumAdapters and EnumOutputs respectively to get the objects
   required to create a DX10 or DX11 device and swap chain.
 */
extern DECLSPEC void SDLCALL ALmixer_DXGIGetOutputInfo( int displayIndex, int *adapterIndex, int *outputIndex );

#endif /* __WIN32__ */


/* Platform specific functions for iOS */
#if defined(__IPHONEOS__) && __IPHONEOS__

extern DECLSPEC int SDLCALL ALmixer_iPhoneSetAnimationCallback(ALmixer_Window * window, int interval, void (*callback)(void*), void *callbackParam);
extern DECLSPEC void SDLCALL ALmixer_iPhoneSetEventPump(ALmixer_bool enabled);

#endif /* __IPHONEOS__ */
#endif /* #if 0 */


/* Platform specific functions for Android */
#if defined(__ANDROID__) && __ANDROID__

#if 0
/* Get the JNI environment for the current thread
   This returns JNIEnv*, but the prototype is void* so we don't need jni.h
 */
extern DECLSPEC void * SDLCALL ALmixer_AndroidGetJNIEnv();

/* Get the SDL Activity object for the application
   This returns jobject, but the prototype is void* so we don't need jni.h
   The jobject returned by ALmixer_AndroidGetActivity is a local reference.
   It is the caller's responsibility to properly release it
   (using env->Push/PopLocalFrame or manually with env->DeleteLocalRef)
 */
extern DECLSPEC void * SDLCALL ALmixer_AndroidGetActivity();

/* See the official Android developer guide for more information:
   http://developer.android.com/guide/topics/data/data-storage.html
*/
#define ALmixer_ANDROID_EXTERNAL_STORAGE_READ   0x01
#define ALmixer_ANDROID_EXTERNAL_STORAGE_WRITE  0x02
#endif /* #if 0 */

/* Get the path used for internal storage for this application.
   This path is unique to your application and cannot be written to
   by other applications.
 */
extern DECLSPEC const char * SDLCALL ALmixer_AndroidGetInternalStoragePath();

#if 0
/* Get the current state of external storage, a bitmask of these values:
    ALmixer_ANDROID_EXTERNAL_STORAGE_READ
    ALmixer_ANDROID_EXTERNAL_STORAGE_WRITE
   If external storage is currently unavailable, this will return 0.
*/
extern DECLSPEC int SDLCALL ALmixer_AndroidGetExternalStorageState();

/* Get the path used for external storage for this application.
   This path is unique to your application, but is public and can be
   written to by other applications.
 */
extern DECLSPEC const char * SDLCALL ALmixer_AndroidGetExternalStoragePath();
#endif /* #if 0 */

#endif /* __ANDROID__ */
#if 0
/* Platform specific functions for WinRT */
#if defined(__WINRT__) && __WINRT__

/**
 *  \brief WinRT / Windows Phone path types
 */
typedef enum
{
    /** \brief The installed app's root directory.
        Files here are likely to be read-only. */
    ALmixer_WINRT_PATH_INSTALLED_LOCATION,

    /** \brief The app's local data store.  Files may be written here */
    ALmixer_WINRT_PATH_LOCAL_FOLDER,

    /** \brief The app's roaming data store.  Unsupported on Windows Phone.
        Files written here may be copied to other machines via a network
        connection.
    */
    ALmixer_WINRT_PATH_ROAMING_FOLDER,

    /** \brief The app's temporary data store.  Unsupported on Windows Phone.
        Files written here may be deleted at any time. */
    ALmixer_WINRT_PATH_TEMP_FOLDER
} ALmixer_WinRT_Path;


/**
 *  \brief Retrieves a WinRT defined path on the local file system
 *
 *  \note Documentation on most app-specific path types on WinRT
 *      can be found on MSDN, at the URL:
 *      http://msdn.microsoft.com/en-us/library/windows/apps/hh464917.aspx
 *
 *  \param pathType The type of path to retrieve.
 *  \ret A UCS-2 string (16-bit, wide-char) containing the path, or NULL
 *      if the path is not available for any reason.  Not all paths are
 *      available on all versions of Windows.  This is especially true on
 *      Windows Phone.  Check the documentation for the given
 *      ALmixer_WinRT_Path for more information on which path types are
 *      supported where.
 */
extern DECLSPEC const wchar_t * SDLCALL ALmixer_WinRTGetFSPathUNICODE(ALmixer_WinRT_Path pathType);

/**
 *  \brief Retrieves a WinRT defined path on the local file system
 *
 *  \note Documentation on most app-specific path types on WinRT
 *      can be found on MSDN, at the URL:
 *      http://msdn.microsoft.com/en-us/library/windows/apps/hh464917.aspx
 *
 *  \param pathType The type of path to retrieve.
 *  \ret A UTF-8 string (8-bit, multi-byte) containing the path, or NULL
 *      if the path is not available for any reason.  Not all paths are
 *      available on all versions of Windows.  This is especially true on
 *      Windows Phone.  Check the documentation for the given
 *      ALmixer_WinRT_Path for more information on which path types are
 *      supported where.
 */
extern DECLSPEC const char * SDLCALL ALmixer_WinRTGetFSPathUTF8(ALmixer_WinRT_Path pathType);

#endif /* __WINRT__ */

#endif /* #if 0 */

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "ALmixer_close_code.h"

#endif /* _ALmixer_system_h */

/* vi: set ts=4 sw=4 expandtab: */
