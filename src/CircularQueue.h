/*
    CircularQueue
    Copyright (C) 2002  Eric Wing <ewing . public @ playcontrol.net>

	zlib license.
*/


#ifndef C_CIRCULAR_QUEUE_H
#define C_CIRCULAR_QUEUE_H

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/** @cond DOXYGEN_SHOULD_IGNORE_THIS */

/* Note: For Doxygen to produce clean output, you should set the 
 * PREDEFINED option to remove C_CIRCULAR_QUEUE_DECLSPEC, C_CIRCULAR_QUEUE_CALL, and
 * the DOXYGEN_SHOULD_IGNORE_THIS blocks.
 * PREDEFINED = DOXYGEN_SHOULD_IGNORE_THIS=1 C_CIRCULAR_QUEUE_DECLSPEC= C_CIRCULAR_QUEUE_CALL=
 */

/** Windows needs to know explicitly which functions to export in a DLL. */
#if defined(_WIN32)
	#if defined(C_CIRCULAR_QUEUE_BUILD_LIBRARY)
		#define C_CIRCULAR_QUEUE_DECLSPEC __declspec(dllexport)
	#else
		#define C_CIRCULAR_QUEUE_DECLSPEC
	#endif
#else
	#if defined(C_CIRCULAR_QUEUE_BUILD_LIBRARY)
		#if defined (__GNUC__) && __GNUC__ >= 4
			#define C_CIRCULAR_QUEUE_DECLSPEC __attribute__((visibility("default")))
		#else
			#define C_CIRCULAR_QUEUE_DECLSPEC
		#endif
	#else
		#define C_CIRCULAR_QUEUE_DECLSPEC
	#endif
#endif

/* For Windows, by default, use the C calling convention */
#if defined(_WIN32)
	#define C_CIRCULAR_QUEUE_CALL __cdecl
#else
	#define C_CIRCULAR_QUEUE_CALL
#endif

/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */

	
/* Optional API symbol name rewrite to help avoid duplicate symbol conflicts.
	For example:   -DCIRCULAR_QUEUE_NAMESPACE_PREFIX=ALmixer
*/
	
#if defined(CIRCULAR_QUEUE_NAMESPACE_PREFIX)
	#define CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL_WITH_NAMESPACE(namespace, symbol) namespace##symbol
	#define CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(symbol) CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL_WITH_NAMESPACE(CIRCULAR_QUEUE_NAMESPACE_PREFIX, symbol)
	
	#define CircularQueueUnsignedInt_CreateQueue	CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_CreateQueue)
	#define CircularQueueUnsignedInt_FreeQueue		CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_FreeQueue)
	#define CircularQueueUnsignedInt_PushBack		CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_PushBack)
	#define CircularQueueUnsignedInt_PushFront		CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_PushFront)
	#define CircularQueueUnsignedInt_PopFront		CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_PopFront)
	#define CircularQueueUnsignedInt_PopBack		CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_PopBack)
	#define CircularQueueUnsignedInt_Front			CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_Front)
	#define CircularQueueUnsignedInt_Back			CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_Back)
	#define CircularQueueUnsignedInt_Size			CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_Size)
	#define CircularQueueUnsignedInt_MaxSize		CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_MaxSize)
	#define CircularQueueUnsignedInt_Clear			CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_Clear)
	#define CircularQueueUnsignedInt_Print			CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_Print)
	#define CircularQueueUnsignedInt_ValueAtIndex	CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt_ValueAtIndex)

	#define CircularQueueVoid_CreateQueue			CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_CreateQueue)
	#define CircularQueueVoid_FreeQueue				CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_FreeQueue)
	#define CircularQueueVoid_PushBack				CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_PushBack)
	#define CircularQueueVoid_PushFront				CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_PushFront)
	#define CircularQueueVoid_PopFront				CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_PopFront)
	#define CircularQueueVoid_PopBack				CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_PopBack)
	#define CircularQueueVoid_Front					CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_Front)
	#define CircularQueueVoid_Back					CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_Back)
	#define CircularQueueVoid_Size					CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_Size)
	#define CircularQueueVoid_MaxSize				CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_MaxSize)
	#define CircularQueueVoid_Clear					CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_Clear)
	#define CircularQueueVoid_Print					CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_Print)
	#define CircularQueueVoid_ValueAtIndex			CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid_ValueAtIndex)

	/* structs don't export symbols */
	/*
	#define CircularQueueUnsignedInt				CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueUnsignedInt)
	#define CircularQueueVoid						CIRCULAR_QUEUE_RENAME_PUBLIC_SYMBOL(CircularQueueVoid)
	*/
#endif /* defined(CIRCULAR_QUEUE_NAMESPACE_PREFIX) */


/**
 * @file
 * This is a C-based Circular queue class. 
 * This class provides very simple circular queue functionality,
 * with an API similar to the C++ STL queue class.
 * Currently, a queue cannot be resized.
 * Because C doesn't do templates and I really don't want 
 * to write my own polymorphism, you must select the proper queue
 * for your data types. 
 * I currently provide an unisigned int version
 * and a void* version. The void* version will let you use any type,
 * but remember that you are responsible for casting and maintaining 
 * your own type safety. I have found the unsigned int version to be 
 * very useful because if you have a map somewhere that associates 
 * unique identifier numbers to data (e.g. OpenGL displaylists/textures),
 * then you can just deal with the id numbers and don't have to deal with 
 * the casting and typesafety issues. I recommend you don't overlook the 
 * usefulness of this version.
 * 
 * @warning Do not mix the CircularQueues created from the different versions.
 * They are incompatible. Use only CircularQueueUnsignedInt objects with 
 * CircularQueueUnsignedInt_* functions, etc.
 * 
 * Example Usage:
 * @code
 *	CircularQueueUnsignedInt* myqueue;
 *	unsigned int ret_val;
 *
 *	myqueue = CircularQueueUnsignedInt_CreateQueue(3);
 *	if(NULL == myqueue)
 *	{
 *		fprintf(stderr, "Error, could not create queue\n");
 *		return 0;
 *	}
 *
 *
 *	ret_val = CircularQueueUnsignedInt_PushBack(myqueue, 1);
 *	ret_val = CircularQueueUnsignedInt_PushBack(myqueue, 2);
 *	ret_val = CircularQueueUnsignedInt_PushBack(myqueue, 3);
 *	if(0 == ret_val)
 *	{
 *		fprintf(stderr, "Error, Could not pushback\n");
 *		exit(1);
 *	}
 *
 *	ret_val = CircularQueueUnsignedInt_PopBack(myqueue);
 *	if(0 == ret_val)
 *	{
 *		fprintf(stderr, "Error, Could not popback\n");
 *		exit(2);
 *	}
 *
 *	fprintf(stderr, "Testing queue, should have 1,2\n");
 *	CircularQueueUnsignedInt_Print(myqueue);
 *
 *	ret_val = CircularQueueUnsignedInt_PushFront(myqueue, 4);
 *	if(0 == ret_val)
 *	{
 *		fprintf(stderr, "Error, Could not pushfront\n");
 *		exit(1);
 *	}
 *	fprintf(stderr, "Testing queue, should have 4,1,2\n");
 *	CircularQueueUnsignedInt_Print(myqueue);
 * 
 * @endcode
 */

/* This is a trick I picked up from Lua. Doing the typedef separately 
 * (and I guess before the definition) instead of a single 
 * entry: typedef struct {...} YourName; seems to allow me
 * to use forward declarations. Doing it the other way (like SDL)
 * seems to prevent me from using forward declarions as I get conflicting
 * definition errors. I don't really understand why though.
 */
typedef struct CircularQueueUnsignedInt CircularQueueUnsignedInt;
/**
 * This is the essentially the CircularQueue object.
 * This contains all the data associated with a CircularQueue instance.
 * This version is for unsigned int data types. In the future, I suppose
 * I could add a void* data type at the very least and maybe some 
 * other data types.
 * This should be considered an opaque data type. 
 */
struct CircularQueueUnsignedInt
{
	unsigned int maxSize; /**< Max size of the queue. */
	unsigned int currentSize; /**< Current number of entries in the queue. */
	unsigned int headIndex; /**< The index of where the current head is. */
	unsigned int tailIndex; /**< The index of where the current tail is. */
	unsigned int* internalQueue; /**< The array representing the queue. */
};

/**
 * This creates a new CircularQueue (for unsigned int) instance.
 * This will create a new circular queue instance which holds 
 * unsigned int's in its queue.
 *
 * @note This implementation does not allow a circular queue to be resized.
 * 
 * @param max_size This specifies the maximum number of elements the circular
 * can hold.
 *
 * @return Returns a pointer to a CircularQueue which is the 
 * instance variable (if successful) or NULL on failure.
 *
 * @see CircularQueueUnsignedInt_FreeQueue
 */
extern C_CIRCULAR_QUEUE_DECLSPEC CircularQueueUnsignedInt* C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_CreateQueue(unsigned int max_size);

/**
 * This destroys a CircularQueue instance.
 * This will destroy the memory associated with the circular queue instance.
 * Whenever you create a CircularQueue instance, you should always to 
 * remember to balance it with a FreeQueue.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @see CircularQueueUnsignedInt_CreateQueue
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_FreeQueue(CircularQueueUnsignedInt* queue);

/**
 * This pushes a new value into the back of the queue.
 * If the queue is full, the function will fail and return 0.
 *
 * @param queue The pointer to the CircularQueue instance.
 * @param value The value you want to push into the queue.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_PushBack(CircularQueueUnsignedInt* queue, unsigned int value);

/**
 * This pushes a new value into the front of the queue.
 * If the queue is full, the function will fail and return 0.
 *
 * @param queue The pointer to the CircularQueue instance.
 * @param value The value you want to push into the queue.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_PushFront(CircularQueueUnsignedInt* queue, unsigned int value);

/**
 * This removes the value at the front of the queue.
 * If the queue is empty, the function will return 0.
 * Note that this function does not return the value popped, but 
 * an error flag. If you need the value, you must call Front() 
 * to retrieve the value before popping it.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns 1 on success, or 0 on failure.
 * @see CircularQueueUnsignedInt_Front
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_PopFront(CircularQueueUnsignedInt* queue);

/**
 * This removes the value at the back of the queue.
 * If the queue is empty, the function will return 0.
 * Note that this function does not return the value popped, but 
 * an error flag. If you need the value, you must call Back() 
 * to retrieve the value before popping it.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns 1 on success, or 0 on failure.
 * @see CircularQueueUnsignedInt_Back
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_PopBack(CircularQueueUnsignedInt* queue);

/**
 * This gets the value at the front of the queue.
 * If the queue is empty, the value returned will be 0.
 * Because this 0 return value is ambiguous because it could also could 
 * be a legitimate value in the queue, if you need more robust error 
 * checking for if the queue is empty, you should get the size of the 
 * queue using the Size() function.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns the value stored at the queue or 0 if the queue is empty 
 * (or if there is an error).
 *
 * @see CircularQueueUnsignedInt_Size
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_Front(CircularQueueUnsignedInt* queue);

/**
 * This gets the value at the back of the queue.
 * If the queue is empty, the value returned will be 0.
 * Because this 0 return value is ambiguous because it could also could 
 * be a legitimate value in the queue, if you need more robust error 
 * checking for if the queue is empty, you should get the size of the 
 * queue using the Size() function.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns the value stored at the queue or 0 if the queue is empty 
 * (or 0 if there is an error).
 *
 * @see CircularQueueUnsignedInt_Size
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_Back(CircularQueueUnsignedInt* queue);

/**
 * This gets the current number of entries that are in the queue.
 * This is number is not to be confused with the MaxSize.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns the number of entries currently in queue, or 0 if 
 * there is an error.
 *
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_Size(CircularQueueUnsignedInt* queue);

/**
 * This gets the maximum number of entries that are allowed in the queue at
 * a given time.
 * This is the number that you used in the CreateQueue function.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns the maximum number of entries allowed in the queue.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_MaxSize(CircularQueueUnsignedInt* queue);
/**
 * This empties the entire queue.
 * This will remove all entries that are in the queue.
 * This does not destroy any memory. Use FreeQueue() to actually destroy 
 * the queue.
 *
 * @param queue The pointer to the CircularQueue instance.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_Clear(CircularQueueUnsignedInt* queue);

/**
 * This is a debugging function that will print all the elements in the 
 * queue to stderr. 
 * This function is provided as convenience, but should not be considered
 * as part of the standard API. Treat this function as deprecated 
 * as it's implementation may change or be removed entirely.
 * 
 * @param queue The pointer to the CircularQueue instance.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_Print(CircularQueueUnsignedInt* queue);

/**
 * This returns the element located at the specified index,
 * where index=0 represents the head/front of the queue.
 *
 * @param queue The pointer to the CircularQueue instance.
 * @param the_index The index of the element you want where 0 represents the 
 * head/front of the queue and Size-1 is the back.
 *
 * @return Returns the value located at the index on success, or 0 on failure.
 * Be careful to not to confuse an error for a legitmate 0 value.
 * Any index from 0 to Size-1 (where Size>0) will be a valid index.
 *
 * This example traverses through the whole queue and prints out each value.
 * @code
 * fprintf(stderr, "Queue: ");
 * for(i=0;i<CircularQueueUnsignedInt_Size(xValueQueue);i++)
 * {
 *     ret_val = CircularQueueUnsignedInt_ValueAtIndex(xValueQueue, i);
 *     fprintf(stderr, "%d ", ret_val);
 *
 * }
 * fprintf(stderr, "\n");
 * @endcode
 *
 * @note The implementation uses a modulo operation to compute the index, so 
 * this may not be the speediest operation in a tight loop.
 * This implementation was not optimized for random access, though it still 
 * is technically O(1).
 *
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueUnsignedInt_ValueAtIndex(CircularQueueUnsignedInt* queue, unsigned int the_index);


/* This is a trick I picked up from Lua. Doing the typedef separately 
 * (and I guess before the definition) instead of a single 
 * entry: typedef struct {...} YourName; seems to allow me
 * to use forward declarations. Doing it the other way (like SDL)
 * seems to prevent me from using forward declarions as I get conflicting
 * definition errors. I don't really understand why though.
 */
typedef struct CircularQueueVoid CircularQueueVoid;
/**
 * This is the essentially the CircularQueue object.
 * This contains all the data associated with a CircularQueue instance.
 * This version is for unsigned int data types. In the future, I suppose
 * I could add a void* data type at the very least and maybe some 
 * other data types.
 * This should be considered an opaque data type. 
 */
struct CircularQueueVoid
{
	unsigned int maxSize; /**< Max size of the queue. */
	unsigned int currentSize; /**< Current number of entries in the queue. */
	unsigned int headIndex; /**< The index of where the current head is. */
	unsigned int tailIndex; /**< The index of where the current tail is. */
	void** internalQueue; /**< The array representing the queue. */
};

/**
 * This creates a new CircularQueue (for void* ) instance.
 * This will create a new circular queue instance which holds 
 * unsigned int's in its queue.
 *
 * @note This implementation does not allow a circular queue to be resized.
 * 
 * @param max_size This specifies the maximum number of elements the circular
 * can hold.
 *
 * @return Returns a pointer to a CircularQueue which is the 
 * instance variable (if successful) or NULL on failure.
 *
 * @see CircularQueueVoid_FreeQueue
 */
extern C_CIRCULAR_QUEUE_DECLSPEC CircularQueueVoid* C_CIRCULAR_QUEUE_CALL CircularQueueVoid_CreateQueue(unsigned int max_size);

/**
 * This destroys a CircularQueue instance.
 * This will destroy the memory associated with the circular queue instance.
 * Whenever you create a CircularQueue instance, you should always to 
 * remember to balance it with a FreeQueue.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @see CircularQueueVoid_CreateQueue
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void C_CIRCULAR_QUEUE_CALL CircularQueueVoid_FreeQueue(CircularQueueVoid* queue);

/**
 * This pushes a new value into the back of the queue.
 * If the queue is full, the function will fail and return 0.
 *
 * @param queue The pointer to the CircularQueue instance.
 * @param value The value you want to push into the queue.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueVoid_PushBack(CircularQueueVoid* queue, void* value);

/**
 * This pushes a new value into the front of the queue.
 * If the queue is full, the function will fail and return 0.
 *
 * @param queue The pointer to the CircularQueue instance.
 * @param value The value you want to push into the queue.
 *
 * @return Returns 1 on success, or 0 on failure.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueVoid_PushFront(CircularQueueVoid* queue, void* value);

/**
 * This removes the value at the front of the queue.
 * If the queue is empty, the function will return 0.
 * Note that this function does not return the value popped, but 
 * an error flag. If you need the value, you must call Front() 
 * to retrieve the value before popping it.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns 1 on success, or 0 on failure.
 * @see CircularQueueVoid_Front
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueVoid_PopFront(CircularQueueVoid* queue);

/**
 * This removes the value at the back of the queue.
 * If the queue is empty, the function will return 0.
 * Note that this function does not return the value popped, but 
 * an error flag. If you need the value, you must call Back() 
 * to retrieve the value before popping it.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns 1 on success, or 0 on failure.
 * @see CircularQueueVoid_Back
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueVoid_PopBack(CircularQueueVoid* queue);

/**
 * This gets the value at the front of the queue.
 * If the queue is empty, the value returned will be 0.
 * Because this 0 return value is ambiguous because it could also could 
 * be a legitimate value in the queue, if you need more robust error 
 * checking for if the queue is empty, you should get the size of the 
 * queue using the Size() function.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns the value stored at the queue or 0 if the queue is empty 
 * (or if there is an error).
 *
 * @see CircularQueueVoid_Size
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void* C_CIRCULAR_QUEUE_CALL CircularQueueVoid_Front(CircularQueueVoid* queue);

/**
 * This gets the value at the back of the queue.
 * If the queue is empty, the value returned will be 0.
 * Because this 0 return value is ambiguous because it could also could 
 * be a legitimate value in the queue, if you need more robust error 
 * checking for if the queue is empty, you should get the size of the 
 * queue using the Size() function.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns the value stored at the queue or 0 if the queue is empty 
 * (or 0 if there is an error).
 *
 * @see CircularQueueVoid_Size
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void* C_CIRCULAR_QUEUE_CALL CircularQueueVoid_Back(CircularQueueVoid* queue);

/**
 * This gets the current number of entries that are in the queue.
 * This is number is not to be confused with the MaxSize.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns the number of entries currently in queue, or 0 if 
 * there is an error.
 *
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueVoid_Size(CircularQueueVoid* queue);

/**
 * This gets the maximum number of entries that are allowed in the queue at
 * a given time.
 * This is the number that you used in the CreateQueue function.
 *
 * @param queue The pointer to the CircularQueue instance.
 *
 * @return Returns the maximum number of entries allowed in the queue.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC unsigned int C_CIRCULAR_QUEUE_CALL CircularQueueVoid_MaxSize(CircularQueueVoid* queue);
/**
 * This empties the entire queue.
 * This will remove all entries that are in the queue.
 * This does not destroy any memory. Use FreeQueue() to actually destroy 
 * the queue.
 *
 * @param queue The pointer to the CircularQueue instance.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void C_CIRCULAR_QUEUE_CALL CircularQueueVoid_Clear(CircularQueueVoid* queue);

/**
 * This is a debugging function that will print all the addresses
 * of elements in the queue to stderr. 
 * This function is provided as convenience, but should not be considered
 * as part of the standard API. Treat this function as deprecated 
 * as it's implementation may change or be removed entirely.
 * 
 * @param queue The pointer to the CircularQueue instance.
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void C_CIRCULAR_QUEUE_CALL CircularQueueVoid_Print(CircularQueueVoid* queue);

/**
 * This returns the element located at the specified index,
 * where index=0 represents the head/front of the queue.
 *
 * @param queue The pointer to the CircularQueue instance.
 * @param the_index The index of the element you want where 0 represents the 
 * head/front of the queue and Size-1 is the back.
 *
 * @return Returns the element located at the index on success, or NULL on failure.
 * Be careful to not to confuse an error for a legitmate NULL value.
 * Any index from 0 to Size-1 (where Size>0) will be a valid index.
 *
 * This example traverses through the whole queue and prints out each value.
 * @code
 * fprintf(stderr, "Queue: ");
 * for(i=0;i<CircularQueueVoid_ValueAtIndex(xValueQueue);i++)
 * {
 *     void* ret_val = CircularQueueUnsignedInt_ValueAtIndex(xValueQueue, i);
 *     fprintf(stderr, "%x ", ret_val);
 *
 * }
 * fprintf(stderr, "\n");
 *
 * @note The implementation uses a modulo operation to compute the index, so 
 * this may not be the speediest operation in a tight loop.
 * This implementation was not optimized for random access, though it still 
 * is technically O(1).
 *
 */
extern C_CIRCULAR_QUEUE_DECLSPEC void* C_CIRCULAR_QUEUE_CALL CircularQueueVoid_ValueAtIndex(CircularQueueVoid* queue, unsigned int the_index);




/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif



#endif /* C_CIRCULAR_QUEUE_H */

