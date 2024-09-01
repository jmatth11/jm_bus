#ifndef JM_BUS_BYTES_HELPER_H
#define JM_BUS_BYTES_HELPER_H

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include "types/message.h"

__BEGIN_DECLS

/**
 * Convert 64 bit number to utf8 format.
 * @param[in] val The 64 bit number.
 * @param[in] n The number of bytes to pull out.
 * @param[out] out The uint8_t array.
 */
void convert_64bit_to_n_utf8(size_t val, size_t n, uint8_t *out) __THROWNL __nonnull((3));

/**
 * Convert utf8 array into a 64 bit number.
 * @param[in] buf The uint8_t array.
 * @param[in] n The number of bytes to pull out.
 * @param[out] out The 64 bit number.
 */
void convert_n_utf8_to_64bit(uint8_t *buf, size_t n, size_t *out) __THROWNL __nonnull((1,3));

/**
 * Read utf8 array into a byte_array.
 * @param[in] buf The uint8_t array.
 * @param[out] out The byte_array to populate.
 */
void read_byte_array(uint8_t *buf, byte_array *out) __THROWNL __nonnull((1,2));

__END_DECLS

#endif
