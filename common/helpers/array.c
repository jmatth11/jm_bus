#include "array.h"
#include <stdlib.h>

bool array_remove_item(pollfd_array *fds, size_t idx) {
  if (idx >= fds->len || idx < 0) return false;
  // move item at index to end of array
  struct pollfd local = fds->pollfd_data[fds->len - 1];
  fds->pollfd_data[fds->len - 1] = fds->pollfd_data[idx];
  fds->pollfd_data[idx] = local;
  // decrement the length to "remove" it
  --fds->len;
  return true;
}

bool array_resize(pollfd_array *fds, size_t n) {
  if (n < 0) n = 1;
  struct pollfd *local = realloc(fds->pollfd_data, n);
  if (local == NULL) return false;
  free(fds->pollfd_data);
  fds->pollfd_data = local;
  return true;
}
