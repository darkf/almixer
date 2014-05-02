#ifdef __ANDROID__
#include "ALmixer.h"
#include "ALmixer_PlatformExtensions.h"
#include <jni.h>

#ifndef ALMIXER_COMPILE_WITH_SDL
	#include "ALmixer_android.h"
#endif

static JavaVM* s_javaVM = NULL;

JavaVM* ALmixer_Android_GetJavaVM()
{
	return s_javaVM;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* java_vm, void* reserved)
{
    s_javaVM = java_vm;

    return JNI_VERSION_1_6;
}

void ALmixer_Android_Init(jobject activity_object)
{
#ifndef ALMIXER_COMPILE_WITH_SDL
	ALmixer_Android_Core_Init(activity_object);
#endif

}

/* Do I need a Quit function to release the Activity class? */
void ALmixer_Android_Quit()
{
#ifndef ALMIXER_COMPILE_WITH_SDL
	ALmixer_Android_Core_Quit();
#endif

}

#endif /* #ifdef __ANDROID__ */


