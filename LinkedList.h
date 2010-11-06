#ifndef C_LINKED_LIST_H
#define C_LINKED_LIST_H

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

typedef struct LinkedListNode LinkedListNode;
typedef struct LinkedList LinkedList;

LinkedList* LinkedList_Create();

void LinkedList_Free(LinkedList* linked_list);

void* LinkedList_Front(LinkedList* linked_list);
void* LinkedList_Back(LinkedList* linked_list);

unsigned int LinkedList_PushFront(LinkedList* linked_list, void* new_item);

unsigned int LinkedList_PushBack(LinkedList* linked_list, void* new_item);

void* LinkedList_PopFront(LinkedList* linked_list);

void* LinkedList_PopBack(LinkedList* linked_list);

size_t LinkedList_Size(LinkedList* linked_list);

void LinkedList_Clear(LinkedList* linked_list);

void* LinkedListNode_GetData(LinkedListNode* list_node);

LinkedListNode* LinkedList_Find(LinkedList* linked_list, void* the_data, LinkedListNode* start_node);

unsigned int LinkedList_Remove(LinkedList* linked_list, LinkedListNode* list_node);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
	
#endif /* C_LINKED_LIST_H */

