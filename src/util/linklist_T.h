#ifndef _LIST_INT_DATATYPES
#define _LIST_INT_DATATYPES

#include <stdint.h>

// a node containing int data for a single-linked list
typedef struct ListNode 
{
	int32_t data;
	struct ListNode *next;
} ListNode;



#endif
