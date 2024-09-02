#ifndef JM_BUS_BYTES_HELPER_H
#define JM_BUS_BYTES_HELPER_H

#include <stdbool.h>
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
 * Read utf8 array into an initialized byte_array.
 *
 * @param[in] buf The uint8_t array.
 * @param[in/out] arr The initialized byte_array to populate.
 */
void read_byte_array(uint8_t *buf, byte_array *arr) __THROWNL __nonnull((1,2));

/**
 * Convert a string to a byte_array type.
 *
 * @param[in] str The string.
 * @param[out] out The byte array.
 * @return True if successful, false otherwise.
 */
bool str_to_byte_array(const char *str, byte_array *out) __THROWNL __nonnull((1,2));

/**
 * Convert a byte_array type into a string.
 *
 * @param[in] arr The byte array.
 * @return The string version of the byte array, NULL if failed.
 */
char* byte_array_to_str(byte_array *arr) __THROWNL __nonnull((1));

void print_byte_array(byte_array arr);

__END_DECLS

#endif
