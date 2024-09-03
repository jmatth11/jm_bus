#include "../deps/array_template/array_template.h"

#include "hash_map.h"
#include "helpers/array.h"
#include "helpers/log.h"
#include "helpers/strings.h"
#include "types/array_types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct hash_map_entry {
  char *key;
  int_array value;
};

generate_array_template(map_entry, struct hash_map_entry *)

generate_array_template(map, map_entry_array)

struct hash_map {
  map_array entries;
  pthread_mutex_t mutex;
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
  if (idx == (arr->len - 1)) {
    --arr->len;
    return true;
  }
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
    error_log("error hash map init.\n");
    return NULL;
  }
  if (!init_map_array(&hm->entries, N)) {
    free(hm);
    error_log("error hash map init entries.\n");
    return NULL;
  }
  if (pthread_mutex_init(&hm->mutex, NULL) < 0) {
    hash_map_destroy(hm);
    error_log("hash map mutex failed to initialize.\n");
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
          free_int_array(&entry->value);
          free(entry);
        }
      }
      free_map_entry_array(&map_entry);
    }
  }
  free_map_array(&hm->entries);
  pthread_mutex_destroy(&hm->mutex);
  free(hm);
}

bool hash_map_get(struct hash_map *hm, const char *key, int_array *out) {
  pthread_mutex_lock(&hm->mutex);
  int hash = hash_from_str(key);
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    pthread_mutex_unlock(&hm->mutex);
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
  pthread_mutex_unlock(&hm->mutex);
  return true;
}

bool hash_map_set(struct hash_map *hm, const char *key, int value) {
  pthread_mutex_lock(&hm->mutex);
  bool result = true;
  int hash = hash_from_str(key);
  int idx = fast_mod(hash, hm->entries.cap);
  bool exists = false;
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    if (!init_map_entry_array(row, 10)) {
      error_log("error init map entry in hash map get.\n");
      result = false;
      goto clean_up;
    }
  } else {
    for (int i = 0; i < row->len; ++i) {
      struct hash_map_entry *existing_entry = NULL;
      get_map_entry_array(row, i, &existing_entry);
      if (existing_entry != NULL && strcmp(existing_entry->key, key) == 0) {
        exists = true;
        if (!insert_int_array(&existing_entry->value, value)) {
          error_log("insert value in existing hash map failed.\n");
          result = false;
          goto clean_up;
        }
        break;
      }
    }
  }
  if (!exists) {
    struct hash_map_entry *entry = malloc(sizeof(struct hash_map_entry));
    size_t key_len = strlen(key);
    entry->key = malloc(sizeof(char) * key_len);
    strncpy(entry->key, key, key_len);
    if (!init_int_array(&entry->value, 3)) {
      error_log("hash map entry array failed.\n");
      free(entry->key);
      free(entry);
      result = false;
      goto clean_up;
    }
    if (!insert_int_array(&entry->value, value)) {
      error_log("insert value in new hash map failed.\n");
      free_int_array(&entry->value);
      free(entry->key);
      free(entry);
      result = false;
      goto clean_up;
    }
    if (!insert_map_entry_array(row, entry)) {
      error_log("inserting new entry in hash map failed.\n");
      free_int_array(&entry->value);
      free(entry->key);
      free(entry);
      result = false;
      goto clean_up;
    }
  }
clean_up:
  pthread_mutex_unlock(&hm->mutex);
  return result;
}

bool hash_map_remove(struct hash_map* hm, const char *key) {
  pthread_mutex_lock(&hm->mutex);
  int hash = hash_from_str(key);
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    pthread_mutex_unlock(&hm->mutex);
    return false;
  }
  int remove_idx = -1;
  for (int i = 0; i < row->len; ++i) {
    struct hash_map_entry *existing_entry = NULL;
    get_map_entry_array(row, i, &existing_entry);
    if (existing_entry != NULL && strcmp(existing_entry->key, key) == 0) {
      remove_idx = i;
      free(existing_entry->key);
      free_int_array(&existing_entry->value);
      free(existing_entry);
      break;
    }
  }
  if (remove_idx == -1) {
    pthread_mutex_unlock(&hm->mutex);
    return false;
  }
  hash_map_remove_entry(row, remove_idx);
  pthread_mutex_unlock(&hm->mutex);
  return true;
}

bool hash_map_remove_value(struct hash_map* hm, const char *key, const int value) {
  pthread_mutex_lock(&hm->mutex);
  int hash = hash_from_str(key);
  int idx = fast_mod(hash, hm->entries.cap);
  map_entry_array *row = &hm->entries.map_data[idx];
  if (row->map_entry_data == NULL) {
    error_log("key does not exist.\n");
    pthread_mutex_unlock(&hm->mutex);
    return false;
  }
  int remove_idx = -1;
  for (int i = 0; i < row->len; ++i) {
    struct hash_map_entry *existing_entry = NULL;
    get_map_entry_array(row, i, &existing_entry);
    if (existing_entry != NULL && strcmp(existing_entry->key, key) == 0) {
      for (int entry_idx = 0; entry_idx < existing_entry->value.len; ++entry_idx) {
        int local_val = existing_entry->value.int_data[entry_idx];
        if (local_val == value) {
          remove_idx = entry_idx;
          break;
        }
      }
      if (remove_idx == -1) {
        error_log("hash map remove_value idx not found.\n");
        pthread_mutex_unlock(&hm->mutex);
        return false;
      }
      if (!array_remove_item(&existing_entry->value, remove_idx)) {
        error_log("failed to remove int from hash map entry.\n");
        pthread_mutex_unlock(&hm->mutex);
        return false;
      }
      break;
    }
  }
  pthread_mutex_unlock(&hm->mutex);
  return true;
}

