/* Copyright: Eric Wing 2003 */

#ifndef SIMPLE_MUTEX_H
#define SIMPLE_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

	#if defined(_WIN32)
		#if defined(SIMPLE_MUTEX_BUILD_LIBRARY)
			#define SIMPLE_MUTEX_DECLSPEC __declspec(dllexport)
		#else
			#define SIMPLE_MUTEX_DECLSPEC
		#endif
	#else
		#if defined(SIMPLE_MUTEX_BUILD_LIBRARY)
			#if defined (__GNUC__) && __GNUC__ >= 4
				#define SIMPLE_MUTEX_DECLSPEC __attribute__((visibility("default")))
			#else
				#define SIMPLE_MUTEX_DECLSPEC
			#endif
		#else
			#define SIMPLE_MUTEX_DECLSPEC
		#endif
	#endif

	#if defined(_WIN32)
		#define SIMPLE_MUTEX_CALL __cdecl
	#else
		#define SIMPLE_MUTEX_CALL
	#endif

	
	
/* Optional API symbol name rewrite to help avoid duplicate symbol conflicts.
	For example:   -DSIMPLE_MUTEX_NAMESPACE_PREFIX=ALmixer
*/
	
#if defined(SIMPLE_MUTEX_NAMESPACE_PREFIX)
	#define SIMPLE_MUTEX_RENAME_PUBLIC_SYMBOL_WITH_NAMESPACE(namespace, symbol) namespace##symbol
	#define SIMPLE_MUTEX_RENAME_PUBLIC_SYMBOL(symbol) SIMPLE_MUTEX_RENAME_PUBLIC_SYMBOL_WITH_NAMESPACE(SIMPLE_MUTEX_NAMESPACE_PREFIX, symbol)
	
	#define SimpleMutex_CreateMutex	SIMPLE_MUTEX_RENAME_PUBLIC_SYMBOL(SimpleMutex_CreateMutex)
	#define SimpleMutex_DestroyMutex		SIMPLE_MUTEX_RENAME_PUBLIC_SYMBOL(SimpleMutex_DestroyMutex)
	#define SimpleMutex_LockMutex		SIMPLE_MUTEX_RENAME_PUBLIC_SYMBOL(SimpleMutex_LockMutex)
	#define SimpleMutex_UnlockMutex		SIMPLE_MUTEX_RENAME_PUBLIC_SYMBOL(SimpleMutex_UnlockMutex)

	/* structs don't export symbols */
	/*
	#define SimpleMutex				SIMPLE_MUTEX_RENAME_PUBLIC_SYMBOL(SimpleMutex)
	*/
#endif /* defined(SIMPLE_MUTEX_NAMESPACE_PREFIX) */



typedef struct SimpleMutex SimpleMutex;

extern SIMPLE_MUTEX_DECLSPEC SimpleMutex* SIMPLE_MUTEX_CALL SimpleMutex_CreateMutex(void);
extern SIMPLE_MUTEX_DECLSPEC void SIMPLE_MUTEX_CALL SimpleMutex_DestroyMutex(SimpleMutex* simple_mutex);
extern SIMPLE_MUTEX_DECLSPEC int SIMPLE_MUTEX_CALL SimpleMutex_LockMutex(SimpleMutex* simple_mutex);
extern SIMPLE_MUTEX_DECLSPEC void SIMPLE_MUTEX_CALL SimpleMutex_UnlockMutex(SimpleMutex* simple_mutex);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif

