#ifndef JM_BUS_STATE_H
#define JM_BUS_STATE_H

#include "types/client.h"
#include "types/server.h"
#include <stdatomic.h>

struct server_state {
  struct client_list clients;
  struct server_info server;
  atomic_int action;
  atomic_bool running;
};

#endif
