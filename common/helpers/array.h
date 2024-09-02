#ifndef JM_BUS_ARRAY_HELPERS_H
#define JM_BUS_ARRAY_HELPERS_H

#include "types/array_types.h"
#include "types/client.h"
#include <stdbool.h>
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS


/**
 * Remove an item from an array.
 * This function does a simple remove.
 * Which is to move the item at idx to the end of the array and decrement the length.
 *
 * @param[in] fds The file descriptor array.
 * @param[in] idx The index to remove.
 * @return True if successful, False otherwise.
 */
bool array_remove_pollfd_item(pollfd_array *fds, size_t idx) __THROWNL __nonnull((1));

bool array_remove_int_item(int_array *arr, size_t idx) __THROWNL __nonnull((1));

bool array_remove_client_metadata_item(client_metadata_array *arr, size_t idx) __THROWNL __nonnull((1));

/**
 * Resize the array to the given size.
 * @param[in] arr The array.
 * @param[in] n The new size.
 * @return True if successful, false otherwise.
 */
bool array_pollfd_resize(pollfd_array *arr, size_t n) __THROWNL __nonnull((1));
bool array_client_metadata_resize(client_metadata_array *arr, size_t n) __THROWNL __nonnull((1));

#define array_remove_item(X, idx) _Generic((X),                                \
  pollfd_array*          : array_remove_pollfd_item,                           \
  int_array*             : array_remove_int_item,                              \
  client_metadata_array* : array_remove_client_metadata_item                   \
)(X, idx)

#define array_resize(X, idx) _Generic((X),                                     \
  pollfd_array*          : array_pollfd_resize,                                \
  client_metadata_array* : array_client_metadata_resize                        \
)(X, idx)

__END_DECLS

#endif
