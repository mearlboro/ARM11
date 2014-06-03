#ifndef _LIST_INT_INTERFACE
#define _LIST_INT_INTERFACE

#include <stdint.h>
#include "linklist_T.h" 

// clears the list and frees all memory
// returns the pointer to the head of the empty list
extern ListNode *list_empty(ListNode *head);

// inserts a node with the given data at the head of the list
// returns the pointer to the new head
extern ListNode *list_insert(ListNode *head, int32_t data);

// removes (if it exists) the node with given data
// returns a pointer to the head
extern ListNode *list_remove(ListNode *head, int32_t data);

// returns (if it exists) a pointer to the node with given data
extern ListNode *list_search(ListNode *head, int32_t data);

// outputs the data
extern void list_print(ListNode *head);


#endif
