#ifndef MAP_H
#define MAP_H

////////////////////////////////////////////////////////////////////////////////

//#define MAP_ITER(m, f) (entry_iter(m->root, f))

//#define MAP_PUT(m, k, v) (m->root = entry_put(m->root, k, v, m->cmp))

//#define MAP_GET(m, k) (entry_get(m->root, k, m->cmp))

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

typedef void (*map_fun)(map_entry *);

////////////////////////////////////////////////////////////////////////////////

map  *map_new(map_cmp);

void *map_del(map *, void *);

void *map_get(map *, void *);

void  map_put(map *, void *, void *);

void map_free(map *);

void map_iter(map *, map_fun);

////////////////////////////////////////////////////////////////////////////////

int map_cmp_str(void *, void *);

int map_cmp_int(void *, void *);

////////////////////////////////////////////////////////////////////////////////

#endif
