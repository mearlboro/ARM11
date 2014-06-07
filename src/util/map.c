#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "utils.h"

///////////////////////////////////////////////////////////////////////  COMPARE

int map_cmp_str(void *mic, void *mik)
{
	return strcmp((char *) mic, (char *) mik);
}


int map_cmp_int(void *mic, void *mik)
{
	return *((int *) mic) - *((int *) mik);
}

///////////////////////////////////////////////////////////////////////////  NEW

map *map_new(map_cmp cmp)
{
	map *m  = mem_chk(malloc(sizeof(map)), "map_new");
	m->cmp  = cmp;
	m->root = NULL;
	return m;
}


static map_entry *entry_new(void *key, void *value)
{
	map_entry *e = mem_chk(malloc(sizeof(map_entry)), "map_entry");
	e->key       = key;
	e->value     = value;
	e->left      = NULL;
	e->right     = NULL;
	return e;
}

//////////////////////////////////////////////////////////////////////////  FREE

static void entry_free(map_entry *e)
{
	if (e == NULL) return;
	entry_free(e->left);
	entry_free(e->right);
	free(e);
}


void map_free(map *m)
{
	entry_free(m->root);
	free(m);
}

///////////////////////////////////////////////////////////////////////////  GET

static void *entry_get(map_entry *e, void *key, map_cmp cmp)
{
	if (e == NULL) return NULL;
	
	int c = cmp(key, e->key);
	
	if      (c == 0) return e->value;
	else if (c < 0)  return entry_get(e->left,  key, cmp);
	else             return entry_get(e->right, key, cmp);
}


void *map_get(map *m, void *key)
{
	return entry_get(m->root, key, m->cmp);
}

///////////////////////////////////////////////////////////////////////////  PUT

static map_entry *entry_put(map_entry *e, void *key, void *value, map_cmp cmp)
{
	if (e == NULL) return entry_new(key, value);
	
	int c = cmp(key, e->key);
	
	if		  (c == 0) e->value = value;
	else if	(c < 0)  e->left  = entry_put(e->left,  key, value, cmp);
	else					   e->right = entry_put(e->right, key, value, cmp);
	
	return e;
}


void map_put(map *m, void *key, void *value)
{
	m->root = entry_put(m->root, key, value, m->cmp);
}

//////////////////////////////////////////////////////////////////////////  ITER

static void entry_iter(map_entry *e, map_fun function)
{
	if (e == NULL) return;
	entry_iter(e->left, function);
	function(e);
	entry_iter(e->right, function);
}


void map_iter(map *m, map_fun f)
{
	entry_iter(m->root, f);
}

//////////////////////////////////////////////////////////////////////////  TEST

/* static void map_print_str_int(map_entry *e)
{
	printf("(%s, %i)\n", (char *) e->key, *(int *) e->value);
}


static void foo()
{
	map *m = map_new(&map_cmp_str);
	
	char *strs[4] =
	{
		"L'ultimo inquisitore",
		"A Clockwork Orange",
		"minou2004",
		"Pulp Fiction"
	};
	
	int *ints = mem_chk(malloc(sizeof(int) * 4), "foo");
	
	for (int i = 0; i < 4; i++)
	{
		ints[i] = i * i;
		map_put(m, strs[i], &ints[i]);
	}
	
	map_iter(m, &map_print_str_int);
	
	
	map_free(m);
} */

