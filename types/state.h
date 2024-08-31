#ifndef JM_BUS_STATE_H
#define JM_BUS_STATE_H

#include "types/client.h"
#include "types/server.h"
#include <stdatomic.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct hash_map;

struct server_state {
  struct client_list clients;
  struct server_info server;
  struct hash_map *topics;
  struct thread_pool *pool;
  atomic_int action;
  atomic_bool running;
};

__END_DECLS

#endif
