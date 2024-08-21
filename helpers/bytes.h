#ifndef JM_BUS_BYTES_HELPER_H
#define JM_BUS_BYTES_HELPER_H

#include <stddef.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include "types/message.h"

__BEGIN_DECLS

void convert_64bit_to_utf8(size_t n, uint8_t *out) __THROWNL __nonnull((2));
void convert_utf8_to_64bit(uint8_t *buf, size_t *out) __THROWNL __nonnull((1,2));
void read_byte_array(uint8_t *buf, byte_array *out) __THROWNL __nonnull((1,2));

__END_DECLS

#endif
