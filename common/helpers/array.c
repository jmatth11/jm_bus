#include "array.h"
#include <stdlib.h>

bool array_remove_pollfd_item(pollfd_array *fds, size_t idx) {
  if (idx >= fds->len || idx < 0) return false;
  // move item at index to end of array
  struct pollfd local = fds->pollfd_data[fds->len - 1];
  fds->pollfd_data[fds->len - 1] = fds->pollfd_data[idx];
  fds->pollfd_data[idx] = local;
  // decrement the length to "remove" it
  --fds->len;
  return true;
}

bool array_remove_int_item(int_array *arr, size_t idx) {
  if (idx >= arr->len || idx < 0) return false;
  // move item at index to end of array
  int local = arr->int_data[arr->len - 1];
  arr->int_data[arr->len - 1] = arr->int_data[idx];
  arr->int_data[idx] = local;
  // decrement the length to "remove" it
  --arr->len;
  return true;
}

bool array_remove_client_metadata_item(client_metadata_array *arr, size_t idx) {
  if (idx >= arr->len || idx < 0) return false;
  // move item at index to end of array
  struct client_metadata local = arr->client_metadata_data[arr->len - 1];
  arr->client_metadata_data[arr->len - 1] = arr->client_metadata_data[idx];
  arr->client_metadata_data[idx] = local;
  // decrement the length to "remove" it
  --arr->len;
  return true;
}

bool array_pollfd_resize(pollfd_array *arr, size_t n) {
  if (n < 0) n = 1;
  struct pollfd *local = realloc(arr->pollfd_data, n);
  if (local == NULL) return false;
  free(arr->pollfd_data);
  arr->pollfd_data = local;
  return true;
}

bool array_client_metadata_resize(client_metadata_array *arr, size_t n) {
  if (n < 0) n = 1;
  struct client_metadata *local = realloc(arr->client_metadata_data, n);
  if (local == NULL) return false;
  free(arr->client_metadata_data);
  arr->client_metadata_data = local;
  return true;
}
