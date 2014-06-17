#ifndef MAP_H
#define MAP_H

////////////////////////////////////////////////////////////////////////////////

typedef int (*map_cmp)(void *, void *);

typedef struct map_entry
{
	void *key;
	void *value;
	struct map_entry *left, *right;
} map_entry;

typedef struct map
{
	map_entry *root;
	map_cmp    cmp;
} map;

typedef void (*map_func)(map_entry *);

typedef enum
{
	 MAP_FREE_KEY  = 1,
	 MAP_FREE_VAL  = 2
} map_free_flag;

////////////////////////////////////////////////////////////////////////////////

map  *map_new(map_cmp);

void *map_del(map *, void *);

void *map_get(map *, void *);

void  map_put(map *, void *, void *);

void  map_free(map *, map_free_flag);

void  map_iter(map *, map_func);

////////////////////////////////////////////////////////////////////////////////

int map_cmp_str(void *, void *);

int map_cmp_int(void *, void *);

////////////////////////////////////////////////////////////////////////////////

#endif
