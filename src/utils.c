#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define ERR_CHECK(ec, p)	 	 \
	 	if (p) 					 \
		{				 		 \
			ERR_PRINT(ec);		 \
			exit(EXIT_FAILURE);	 \
		}						


#define ERR_MEM(p)  ERR_CHECK(EC_MEMORY, p);		
#define ERR_FILE(p) ERR_CHECK(EC_FILE, p);	

#define ERR_PRINT(ec) (perror(ERRORS[ec].message))


typedef enum 
{
	EC_NOERROR,
	EC_UKNOWN,
	EC_MEMORY,
	EC_FILE,
	EC_SYSTEM,
} EC;


typedef struct Error 
{	
	EC code;
	const char *message;
} Error;


static Error ERRORS[] = 
{
	{ EC_NOERROR, "Everything went better than expected ;)" 		    },
	{ EC_UKNOWN,  "Uknown error encountered... I have been defeated :(" },
	{ EC_MEMORY,  "Memory manipulation related error!"					},
	{ EC_FILE,    "File manipulation related error!" 					},
	{ EC_SYSTEM,  "System related error... Not your fault ^^'"			},
};





























































/*

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

*/
