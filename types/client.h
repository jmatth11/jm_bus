#ifndef JM_BUS_CLIENT_H
#define JM_BUS_CLIENT_H

#include "../deps/array_template/array_template.h"

#include <stdlib.h>
#include <stdint.h>

struct client_info {
  int socket;
  uint8_t flags;
};

generate_array_template(client_info, struct client_info)

struct client_list {
  client_info_array clients;
};

#endif
