/*
 LinkedList
 Copyright (C) 2010  Eric Wing <ewing . public @ playcontrol.net>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.
 
 You should have received a copy of the GNU Library General Public
 License along with this library; if not, write to the Free
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* I'm so sick of constantly rewriting linked lists in C. 
So here's some initial work to write a very simple reusable implementation.
The API could use a lot more functions, but I'll add them as I need them.
*/

#ifndef C_LINKED_LIST_H
#define C_LINKED_LIST_H

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif


#ifndef DOXYGEN_SHOULD_IGNORE_THIS
/** @cond DOXYGEN_SHOULD_IGNORE_THIS */

/* Note: For Doxygen to produce clean output, you should set the 
 * PREDEFINED option to remove C_LINKED_LIST_DECLSPEC, C_LINKED_LIST_CALL, and
 * the DOXYGEN_SHOULD_IGNORE_THIS blocks.
 * PREDEFINED = DOXYGEN_SHOULD_IGNORE_THIS=1 C_LINKED_LIST_DECLSPEC= C_LINKED_LIST_CALL=
 */

/** Windows needs to know explicitly which functions to export in a DLL. */
#if defined(_WIN32)
	#if defined(C_LINKED_LIST_BUILD_LIBRARY)
		#define C_LINKED_LIST_DECLSPEC __declspec(dllexport)
	#else
		#define C_LINKED_LIST_DECLSPEC
	#endif
#else
	#if defined(C_LINKED_LIST_BUILD_LIBRARY)
		#if defined (__GNUC__) && __GNUC__ >= 4
			#define C_LINKED_LIST_DECLSPEC __attribute__((visibility("default")))
		#else
			#define C_LINKED_LIST_DECLSPEC
		#endif
	#else
		#define C_LINKED_LIST_DECLSPEC
	#endif
#endif

/* For Windows, by default, use the C calling convention */
#if defined(_WIN32)
	#define C_LINKED_LIST_CALL __cdecl
#else
	#define C_LINKED_LIST_CALL
#endif

/** @endcond DOXYGEN_SHOULD_IGNORE_THIS */
#endif /* DOXYGEN_SHOULD_IGNORE_THIS */
	

/* Optional API symbol name rewrite to help avoid duplicate symbol conflicts.
	For example:   -DLINKED_LIST_NAMESPACE_PREFIX=ALmixer
*/
	
#if defined(LINKED_LIST_NAMESPACE_PREFIX)
	#define LINKED_LIST_RENAME_PUBLIC_SYMBOL_WITH_NAMESPACE(namespace, symbol) namespace##symbol
	#define LINKED_LIST_RENAME_PUBLIC_SYMBOL(symbol) LINKED_LIST_RENAME_PUBLIC_SYMBOL_WITH_NAMESPACE(LINKED_LIST_NAMESPACE_PREFIX, symbol)
	
	#define LinkedList_Create	LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_Create)
	#define LinkedList_Free		LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_Free)
	#define LinkedList_Front		LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_Front)
	#define LinkedList_Back		LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_Back)
	#define LinkedList_PushFront		LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_PushFront)
	#define LinkedList_PushBack		LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_PushBack)
	#define LinkedList_PopFront			LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_PopFront)
	#define LinkedList_PopBack			LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_PopBack)
	#define LinkedList_Size			LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_Size)
	#define LinkedList_Clear		LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_Clear)
	#define LinkedListNode_GetData			LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedListNode_GetData)
	#define LinkedList_Find			LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_Find)
	#define LinkedList_Remove	LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList_Remove)
	#define LinkedListIterator_GetIteratorAtBegin	LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedListIterator_GetIteratorAtBegin)
	#define LinkedListIterator_IteratorNext	LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedListIterator_IteratorNext)
	#define LinkedListIterator_GetNode	LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedListIterator_GetNode)

	/* structs don't export symbols */
	/*
	#define LinkedList						LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedList)
	#define LinkedListNode						LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedListNode)
	#define LinkedListIterator				LINKED_LIST_RENAME_PUBLIC_SYMBOL(LinkedListIterator)
	*/
#endif /* defined(LINKED_LIST_NAMESPACE_PREFIX) */


	
#include <stddef.h>

typedef struct LinkedListNode LinkedListNode;
typedef struct LinkedList LinkedList;

struct C_LINKED_LIST_DECLSPEC LinkedListIterator
{
	/* These are all implementation details.
	 * You should probably not directly touch.
	 */
	LinkedList* linkedList;
	LinkedListNode* currentNode;
	int atEnd;
};
typedef struct LinkedListIterator LinkedListIterator;

extern C_LINKED_LIST_DECLSPEC LinkedList* C_LINKED_LIST_CALL LinkedList_Create(void);

extern C_LINKED_LIST_DECLSPEC void C_LINKED_LIST_CALL LinkedList_Free(LinkedList* linked_list);

extern C_LINKED_LIST_DECLSPEC void* C_LINKED_LIST_CALL LinkedList_Front(LinkedList* linked_list);

extern C_LINKED_LIST_DECLSPEC void* C_LINKED_LIST_CALL LinkedList_Back(LinkedList* linked_list);

extern C_LINKED_LIST_DECLSPEC unsigned int C_LINKED_LIST_CALL LinkedList_PushFront(LinkedList* linked_list, void* new_item);

extern C_LINKED_LIST_DECLSPEC unsigned int C_LINKED_LIST_CALL LinkedList_PushBack(LinkedList* linked_list, void* new_item);

extern C_LINKED_LIST_DECLSPEC void* C_LINKED_LIST_CALL LinkedList_PopFront(LinkedList* linked_list);

extern C_LINKED_LIST_DECLSPEC void* C_LINKED_LIST_CALL LinkedList_PopBack(LinkedList* linked_list);

extern C_LINKED_LIST_DECLSPEC size_t C_LINKED_LIST_CALL LinkedList_Size(LinkedList* linked_list);

extern C_LINKED_LIST_DECLSPEC void C_LINKED_LIST_CALL LinkedList_Clear(LinkedList* linked_list);

extern C_LINKED_LIST_DECLSPEC void* C_LINKED_LIST_CALL LinkedListNode_GetData(LinkedListNode* list_node);

extern C_LINKED_LIST_DECLSPEC LinkedListNode* C_LINKED_LIST_CALL LinkedList_Find(LinkedList* linked_list, void* the_data, LinkedListNode* start_node);

extern C_LINKED_LIST_DECLSPEC unsigned int C_LINKED_LIST_CALL LinkedList_Remove(LinkedList* linked_list, LinkedListNode* list_node);

extern C_LINKED_LIST_DECLSPEC LinkedListIterator C_LINKED_LIST_CALL LinkedListIterator_GetIteratorAtBegin(LinkedList* linked_list);
extern C_LINKED_LIST_DECLSPEC int C_LINKED_LIST_CALL LinkedListIterator_IteratorNext(LinkedListIterator* linkedlist_iterator);
extern C_LINKED_LIST_DECLSPEC LinkedListNode* C_LINKED_LIST_CALL LinkedListIterator_GetNode(LinkedListIterator* linkedlist_iterator);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
	
#endif /* C_LINKED_LIST_H */

