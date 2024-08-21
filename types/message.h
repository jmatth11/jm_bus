#ifndef JM_BUS_MESSAGE_H
#define JM_BUS_MESSAGE_H

#include "../deps/array_template/array_template.h"

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

generate_array_template(byte, uint8_t)

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

__END_DECLS

#endif
