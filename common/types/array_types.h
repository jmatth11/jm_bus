#ifndef JM_BUS_ARRAY_TYPES_H
#define JM_BUS_ARRAY_TYPES_H

#include "../deps/array_template/array_template.h"

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/poll.h>

__BEGIN_DECLS

generate_array_template(int, int)

generate_array_template(pollfd, struct pollfd)

generate_array_template(byte, uint8_t)

__END_DECLS

#endif
