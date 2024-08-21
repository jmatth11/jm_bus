#include "bytes.h"
#include <stdio.h>

void convert_64bit_to_utf8(size_t n, uint8_t *out) {
  for (int i = 0; i < 5; ++i) {
    out[i] = (uint8_t)(n>>(i*8));
  }
}

void convert_utf8_to_64bit(uint8_t *buf, size_t *out) {
  size_t local = 0;
  for (int i = 0; i < 5; ++i) {
    local += buf[i]<<(i*8);
  }
  *out = local;
}

void read_byte_array(uint8_t *buf, byte_array *out) {
  for (int i = 0; i < out->cap; ++i) {
    if (!insert_byte_array(out, buf[i])) {
      fprintf(stderr, "error inserting byte.\n");
    }
  }
}

