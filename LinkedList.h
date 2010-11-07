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

