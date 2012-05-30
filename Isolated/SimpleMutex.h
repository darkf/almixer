/* Copyright: Eric Wing 2003 */

#ifndef SIMPLE_MUTEX_H
#define SIMPLE_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif
	

typedef struct SimpleMutex SimpleMutex;

SimpleMutex* SimpleMutex_CreateMutex(void);
void SimpleMutex_DestroyMutex(SimpleMutex* simple_mutex);
int SimpleMutex_LockMutex(SimpleMutex* simple_mutex);
void SimpleMutex_UnlockMutex(SimpleMutex* simple_mutex);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif

