#include "bytes.h"
#include "types/array_types.h"
#include <stdio.h>
#include <string.h>

void convert_64bit_to_n_utf8(size_t val, size_t n, uint8_t *out) {
  for (int i = 0; i < n; ++i) {
    out[i] = (uint8_t)(val>>(i*8));
  }
}

void convert_n_utf8_to_64bit(uint8_t *buf, size_t n, size_t *out) {
  size_t local = 0;
  for (int i = 0; i < n; ++i) {
    local += buf[i]<<(i*8);
  }
  *out = local;
}

void read_byte_array(uint8_t *buf, byte_array *arr) {
  for (int i = 0; i < arr->cap; ++i) {
    if (!insert_byte_array(arr, buf[i])) {
      fprintf(stderr, "error inserting byte.\n");
    }
  }
}

bool str_to_byte_array(const char *str, byte_array *out) {
  byte_array local;
  size_t str_len = strlen(str);
  if (!init_byte_array(&local, str_len)) {
    fprintf(stderr, "str_to_byte_array init_byte_array failed.\n");
    return false;
  }
  for (int i = 0; i < str_len; ++i) {
    if (!insert_byte_array(&local, str[i])) {
      fprintf(stderr, "str_to_byte_array insert byte failed.\n");
      return false;
    }
  }
  *out = local;
  return true;
}

char* byte_array_to_str(byte_array *arr) {
  char *result = malloc(sizeof(char)*arr->len);
  if (result == NULL) return result;
  memcpy(result, arr->byte_data, arr->len);
  return result;
}

void print_byte_array(byte_array arr) {
  for (int str_idx = 0; str_idx < arr.len; ++str_idx) {
    printf("%x ",arr.byte_data[str_idx]);
  }
}

