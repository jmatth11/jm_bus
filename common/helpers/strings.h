#ifndef JM_BUS_STRINGS_HELPERS_H
#define JM_BUS_STRINGS_HELPERS_H

#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * Generate a hash representation of the given string.
 *
 * @param[in] str The string.
 * @return The hash value.
 */
unsigned long hash_from_str(const char *str) __THROWNL __nonnull((1));

__END_DECLS

#endif
