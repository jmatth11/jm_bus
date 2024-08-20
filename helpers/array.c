#include "array.h"

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
