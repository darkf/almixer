#ifndef SIMPLE_THREAD
#define SIMPLE_THREAD

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stddef.h>

typedef struct SimpleThread SimpleThread;


SimpleThread* SimpleThread_CreateThread(int (*user_function)(void*), void* user_data);

size_t SimpleThread_GetCurrentThreadID(void);
size_t SimpleThread_GetThreadID(SimpleThread* simple_thread);

void SimpleThread_WaitThread(SimpleThread* simple_thread, int* thread_status);


int SimpleThread_GetThreadPriority(SimpleThread* simple_thread);
void SimpleThread_SetThreadPriority(SimpleThread* simple_thread, int priority_level);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif

#endif

