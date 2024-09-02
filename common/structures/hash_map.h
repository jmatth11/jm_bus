#ifndef JM_BUS_HASH_MAP_H
#define JM_BUS_HASH_MAP_H

#include "types/array_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct hash_map;

struct hash_map * hash_map_create(size_t) __THROWNL;
void hash_map_destroy(struct hash_map *) __THROWNL __nonnull((1));

bool hash_map_get(struct hash_map *, const char *, int_array *out) __THROWNL __nonnull((1,2));
bool hash_map_set(struct hash_map *, const char *, int) __THROWNL __nonnull((1,2));
bool hash_map_remove(struct hash_map*, const char *) __THROWNL __nonnull((1,2));
bool hash_map_remove_value(struct hash_map* hm, const char *key, const int value) __THROWNL __nonnull((1,2));

__END_DECLS

#endif
