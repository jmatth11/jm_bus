#ifndef JM_BUS_MESSAGE_H
#define JM_BUS_MESSAGE_H

#include "types/array_types.h"

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS


enum message_type {
  ERROR = 0,
  CONNECTION,
  SUBSCRIBE,
  PUBLISH,
};

struct message {
  byte_array topic;
  byte_array body;
  uint8_t type;
};

generate_array_template(message, struct message)

__END_DECLS

#endif
