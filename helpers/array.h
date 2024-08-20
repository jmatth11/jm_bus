#ifndef JM_BUS_ARRAY_HELPERS_H
#define JM_BUS_ARRAY_HELPERS_H

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
bool array_remove_item(pollfd_array *fds, size_t idx) __THROWNL __nonnull((1));

__END_DECLS

#endif
