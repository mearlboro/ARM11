#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "linklist_I.h"

ListNode* list_empty(ListNode *head) 
{
	ListNode *p = head;

	while (p != NULL) 
	{
		head = head->next;
		free(p);
		p = head;
	}
	
	return head;
}	

ListNode* list_insert(ListNode *head, int32_t data) 
{
		ListNode *new = malloc(sizeof(ListNode)); 
		new->data     = data;
		new->next     = head;

		return new;
}

ListNode* list_remove(ListNode *head, int32_t data) 
{
	
		ListNode *found = list_search(head, data);
		
		if (found != NULL)
		{
			if (found != head)
			{
				ListNode *prev;
				for (prev=head; prev->next != found; prev=prev->next);
			
				prev->next = found->next;
				free(found);
			}
			else
			{
				ListNode *temp = head;
				head = head->next;
				free(temp);
			}
		}

	return head;
}


ListNode* list_search(ListNode *head, int32_t data) 
{
	ListNode *found;
	for (found=head; found!=NULL; found=found->next) 
	{
		if(found->data == data) 
			return found;
	}
	
	return NULL;
}


void list_print(ListNode *head) 
{
	printf("\n");

	ListNode *current;
	for (current=head; current!=NULL; current=current->next) 
	{
		printf("%d ", current->data);
	}
	
}
