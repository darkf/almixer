/*
 * Platform specific public APIs are intended to go here.
 */

#ifndef _ALMIXER_PLATFORMEXTENSIONS_H_
#define _ALMIXER_PLATFORMEXTENSIONS_H_

#ifdef __ANDROID__

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#include "ALmixer.h"
#include <jni.h>
/**
 * On Android when ALmixer is compiled without SDL, you MUST provide the Activity class.
 * The Android AssetManager requires a context (which your main Activity provides), which 
 * as a standalone libary, ALmixer has no knowledge of. So in order for any file related 
 * access to work, you MUST provide the Activity class on initialization so ALmixer can
 * access your files.
 * When compiled with SDL as the backend, this function is still safe to call and is currently a no-op.
 *
 * @note There is no Quit counterpart because it is automatically handled by the standard ALmixer_Quit*
 * family of functions.
 */
extern ALMIXER_DECLSPEC void ALMIXER_CALL ALmixer_Android_Init(jobject activity_class);


/* Quit is handled automatically by ALmixer_Quit so it is not public */

/**
 * You probably won't need this, but just in case, it is provided.
 */
extern ALMIXER_DECLSPEC JavaVM* ALMIXER_CALL ALmixer_Android_GetJavaVM(void);



/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif /* #ifdef __ANDROID__ */


#endif /* _ALMIXER_PLATFORMEXTENSIONS_H_ */


