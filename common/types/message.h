#ifndef JM_BUS_MESSAGE_H
#define JM_BUS_MESSAGE_H

#include "types/array_types.h"

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

enum message_type {
  ERROR = 0,
  CONNECTION = 1,
  SUBSCRIBE = 2,
  PUBLISH = 3,
};

struct message {
  byte_array topic;
  byte_array body;
  uint8_t type;
};

generate_array_template(message, struct message)

struct server_state;

struct message_event {
  struct server_state* state;
  int from;
  message_array msgs;
};

__END_DECLS

#endif
