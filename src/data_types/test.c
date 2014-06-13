#include <stdlib.h>
#include <stdio.h>

#include "linklist_I.h"
#include "symtable_I.h"



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


	SymbolTable *new =  calloc(TABLE_SIZE, sizeof(struct TableEntry));
	//symtable_print(new);
	symtable_put(new,"lala",182);
	symtable_print(new);
	symtable_remove(new, "lala");
	symtable_print(new);
    symtable_clear(new);
  return EXIT_SUCCESS;
}
