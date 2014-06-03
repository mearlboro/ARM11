#include <stdlib.h>
#include <stdio.h>

#include "data_types/linklist.c"
#include "data_types/symtable.c"


int main(int argc, char **argv) {


	ListNode * head = malloc(sizeof(ListNode));
	head->data = 4;
	head->next = NULL;	
	head=list_insert(head,13);
	list_print(head);
	head=list_insert(head,4);
	list_print(head);
	head=list_insert(head,6);
	list_print(head);
	head=list_insert(head,10);
	list_print(head);
	head=list_remove(head,6);
	list_print(head);
	head=list_remove(head,4);
	printf("%d", (list_search(head, 4) == NULL) ? 0 : 1);

	SymbolTable *new = symtable_init();

	symtable_put(new,"lala",0x99);
	symtable_print(new);
	symtable_remove(new, "lala");
	symtable_print(new);

  return EXIT_SUCCESS;
}
