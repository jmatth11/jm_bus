#include "../deps/array_template/array_template.h"

#include "hash_map.h"
#include "helpers/strings.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct hash_map_entry {
  char *key;
  int value;
};

generate_array_template(map_entry, struct hash_map_entry *)

generate_array_template(map, map_entry_array)

struct hash_map {
  map_array entries;
};

static int fast_mod(int hash, int cap) {
  // fast approximate modulo-like operation
  int even_cap = (1 & cap) ? (cap +1) : cap;
  int mod = (hash & even_cap);
  mod += (1 & hash);
  if (mod >= cap) mod -= cap;
  return mod;
}

static bool hash_map_remove_entry(map_entry_array *arr, size_t idx) {
  if (idx >= arr->len || idx < 0) return false;
  // move item at index to end of array
  arr->map_entry_data[idx] = arr->map_entry_data[arr->len - 1];
  arr->map_entry_data[arr->len - 1] = NULL;
  // decrement the length to "remove" it
  --arr->len;
  return true;
}

struct hash_map * hash_map_create(size_t N) {
  struct hash_map *hm = malloc(sizeof(struct hash_map));
  if (hm == NULL) {
    fprintf(stderr, "error hash map init.\n");
    return NULL;
  }
  if (!init_map_array(&hm->entries, N)) {
    free(hm);
    fprintf(stderr, "error hash map init entries.\n");
    return NULL;
  }
  hm->entries.len = N;
  return hm;
}

void hash_map_destroy(struct hash_map *hm) {
  for (int i = 0; i < hm->entries.cap; ++i) {
    map_entry_array map_entry;
    get_map_array(&hm->entries, i, &map_entry);
    if (map_entry.len > 0) {
      for (int entry_idx = 0; entry_idx < map_entry.len; ++entry_idx) {
        struct hash_map_entry *entry = NULL;
        get_map_entry_array(&map_entry, entry_idx, &entry);
        if (entry != NULL) {
          free(entry->key);
          free(entry);
        }
      }
      free_map_entry_array(&map_entry);
    }
  }
  free_map_array(&hm->entries);
  free(hm);
}

bool hash_map_get(struct hash_map *hm, const char *key, int *out) {
  int hash = hash_from_str(key);
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    return false;
  }
  for (int i = 0; i < row->len; ++i) {
    struct hash_map_entry *entry = NULL;
    get_map_entry_array(row, i, &entry);
    if (entry != NULL) {
      if (strcmp(entry->key, key) == 0) {
        *out = entry->value;
        break;
      }
    }
  }
  return true;
}

bool hash_map_set(struct hash_map *hm, const char *key, int value) {
  int hash = hash_from_str(key);
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    if (!init_map_entry_array(row, 10)) {
      fprintf(stderr, "error init map entry in hash map get.\n");
      return false;
    }
    struct hash_map_entry *entry = malloc(sizeof(struct hash_map_entry));
    size_t key_len = strlen(key);
    entry->key = malloc(sizeof(char) * key_len);
    strncpy(entry->key, key, key_len);
    entry->value = value;
    insert_map_entry_array(row, entry);
    return true;
  }
  bool exists = false;
  for (int i = 0; i < row->len; ++i) {
    struct hash_map_entry *existing_entry = NULL;
    get_map_entry_array(row, i, &existing_entry);
    if (existing_entry != NULL && strcmp(existing_entry->key, key) == 0) {
      exists = true;
      existing_entry->value = value;
      break;
    }
  }
  if (!exists) {
    struct hash_map_entry *entry = malloc(sizeof(struct hash_map_entry));
    size_t key_len = strlen(key);
    entry->key = malloc(sizeof(char) * key_len);
    strncpy(entry->key, key, key_len);
    entry->value = value;
    insert_map_entry_array(row, entry);
  }
  return true;
}

bool hash_map_remove(struct hash_map* hm, const char *key) {
  int hash = hash_from_str(key);
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    return false;
  }
  int remove_idx = -1;
  for (int i = 0; i < row->len; ++i) {
    struct hash_map_entry *existing_entry = NULL;
    get_map_entry_array(row, i, &existing_entry);
    if (existing_entry != NULL && strcmp(existing_entry->key, key) == 0) {
      remove_idx = i;
      free(existing_entry->key);
      free(existing_entry);
      break;
    }
  }
  if (remove_idx == -1) return false;
  hash_map_remove_entry(row, remove_idx);
  return true;
}

