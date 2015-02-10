#ifndef SIMPLE_THREAD
#define SIMPLE_THREAD

#ifdef __cplusplus
extern "C" {
#endif
	
	#if defined(_WIN32)
		#if defined(SIMPLE_THREAD_BUILD_LIBRARY)
			#define SIMPLE_THREAD_DECLSPEC __declspec(dllexport)
		#else
			#define SIMPLE_THREAD_DECLSPEC
		#endif
	#else
		#if defined(SIMPLE_THREAD_BUILD_LIBRARY)
			#if defined (__GNUC__) && __GNUC__ >= 4
				#define SIMPLE_THREAD_DECLSPEC __attribute__((visibility("default")))
			#else
				#define SIMPLE_THREAD_DECLSPEC
			#endif
		#else
			#define SIMPLE_THREAD_DECLSPEC
		#endif
	#endif

	#if defined(_WIN32)
		#define SIMPLE_THREAD_CALL __cdecl
	#else
		#define SIMPLE_THREAD_CALL
	#endif


/* Optional API symbol name rewrite to help avoid duplicate symbol conflicts.
	For example:   -DSIMPLE_THREAD_NAMESPACE_PREFIX=ALmixer
*/
	
#if defined(SIMPLE_THREAD_NAMESPACE_PREFIX)
	#define SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL_WITH_NAMESPACE(namespace, symbol) namespace##symbol
	#define SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL(symbol) SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL_WITH_NAMESPACE(SIMPLE_THREAD_NAMESPACE_PREFIX, symbol)
	
	#define SimpleThread_CreateThread	SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL(SimpleThread_CreateThread)
	#define SimpleThread_GetCurrentThreadID		SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL(SimpleThread_GetCurrentThreadID)
	#define SimpleThread_GetThreadID		SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL(SimpleThread_GetThreadID)
	#define SimpleThread_WaitThread		SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL(SimpleThread_WaitThread)
	#define SimpleThread_GetThreadPriority		SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL(SimpleThread_GetThreadPriority)
	#define SimpleThread_SetThreadPriority		SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL(SimpleThread_SetThreadPriority)

	/* structs don't export symbols */
	/*
	#define SimpleThread				SIMPLE_THREAD_RENAME_PUBLIC_SYMBOL(SimpleThread)
	*/
#endif /* defined(SIMPLE_THREAD_NAMESPACE_PREFIX) */


#include <stddef.h>

typedef struct SimpleThread SimpleThread;

typedef enum
{
    SIMPLE_THREAD_PRIORITY_UNKNOWN = -1,
    SIMPLE_THREAD_PRIORITY_LOW = 0,
    SIMPLE_THREAD_PRIORITY_NORMAL = 1,
    SIMPLE_THREAD_PRIORITY_HIGH = 2
} SimpleThreadPriority;

extern SIMPLE_THREAD_DECLSPEC SimpleThread* SIMPLE_THREAD_CALL SimpleThread_CreateThread(int (*user_function)(void*), void* user_data);

extern SIMPLE_THREAD_DECLSPEC size_t SIMPLE_THREAD_CALL SimpleThread_GetCurrentThreadID(void);
extern SIMPLE_THREAD_DECLSPEC size_t SIMPLE_THREAD_CALL SimpleThread_GetThreadID(SimpleThread* simple_thread);

extern SIMPLE_THREAD_DECLSPEC void SIMPLE_THREAD_CALL SimpleThread_WaitThread(SimpleThread* simple_thread, int* thread_status);


extern SIMPLE_THREAD_DECLSPEC SimpleThreadPriority SIMPLE_THREAD_CALL SimpleThread_GetThreadPriority(SimpleThread* simple_thread);
extern SIMPLE_THREAD_DECLSPEC void SIMPLE_THREAD_CALL SimpleThread_SetThreadPriority(SimpleThread* simple_thread, SimpleThreadPriority priority_level);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif

