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
#include "../../ALmixer_internal.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
extern "C" {
/* *INDENT-ON* */
#endif

#if 0
#include <EGL/eglplatform.h>
#include <android/native_window_jni.h>

#include "ALmixer_rect.h"
#endif /* #if 0 */

#if 0
/* Interface from the SDL library into the Android Java activity */
/* extern ALmixer_bool ALmixer_Android_JNI_CreateContext(int majorVersion, int minorVersion, int red, int green, int blue, int alpha, int buffer, int depth, int stencil, int buffers, int samples);
extern ALmixer_bool ALmixer_Android_JNI_DeleteContext(void); */
extern void ALmixer_Android_JNI_SwapWindow();
extern void ALmixer_Android_JNI_SetActivityTitle(const char *title);
extern ALmixer_bool ALmixer_Android_JNI_GetAccelerometerValues(float values[3]);
extern void ALmixer_Android_JNI_ShowTextInput(ALmixer_Rect *inputRect);
extern void ALmixer_Android_JNI_HideTextInput();
extern ANativeWindow* ALmixer_Android_JNI_GetNativeWindow(void);

/* Audio support */
extern int ALmixer_Android_JNI_OpenAudioDevice(int sampleRate, int is16Bit, int channelCount, int desiredBufferFrames);
extern void* ALmixer_Android_JNI_GetAudioBuffer();
extern void ALmixer_Android_JNI_WriteAudioBuffer();
extern void ALmixer_Android_JNI_CloseAudioDevice();
#endif /* #if 0 */

#include "ALmixer_RWops.h"

int ALmixer_Android_JNI_FileOpen(ALmixer_RWops* ctx, const char* fileName, const char* mode);
int64_t ALmixer_Android_JNI_FileSize(ALmixer_RWops* ctx);
int64_t ALmixer_Android_JNI_FileSeek(ALmixer_RWops* ctx, int64_t offset, int whence);
size_t ALmixer_Android_JNI_FileRead(ALmixer_RWops* ctx, void* buffer, size_t size, size_t maxnum);
size_t ALmixer_Android_JNI_FileWrite(ALmixer_RWops* ctx, const void* buffer, size_t size, size_t num);
int ALmixer_Android_JNI_FileClose(ALmixer_RWops* ctx);

#if 0
/* Clipboard support */
int ALmixer_Android_JNI_SetClipboardText(const char* text);
char* ALmixer_Android_JNI_GetClipboardText();
ALmixer_bool ALmixer_Android_JNI_HasClipboardText();

/* Power support */
int ALmixer_Android_JNI_GetPowerInfo(int* plugged, int* charged, int* battery, int* seconds, int* percent);
    
/* Joystick support */
void ALmixer_Android_JNI_PollInputDevices();


/* Touch support */
int ALmixer_Android_JNI_GetTouchDeviceIds(int **ids);
#endif /* #if 0 */

/* Threads */
#include <jni.h>
JNIEnv *ALmixer_Android_JNI_GetEnv(void);
int ALmixer_Android_JNI_SetupThread(void);

#if 0
/* Generic messages */
int ALmixer_Android_JNI_SendMessage(int command, int param);

#endif /* #if 0 */


/* These were added for ALmixer to call into */
void ALmixer_Android_Core_Init(jclass activity_class);
void ALmixer_Android_Core_Quit(void);



/* Ends C function definitions when using C++ */
#ifdef __cplusplus
/* *INDENT-OFF* */
}
/* *INDENT-ON* */
#endif

/* vi: set ts=4 sw=4 expandtab: */
