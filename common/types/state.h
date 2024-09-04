#ifndef JM_BUS_STATE_H
#define JM_BUS_STATE_H

#include "types/client.h"
#include "types/server.h"
#include <stdatomic.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

// forward declare
struct hash_map;

/**
 * The main structure to manage server state.
 */
struct server_state {
  /* the list of clients and their info */
  struct client_list clients;
  /* the server info */
  struct server_info server;
  /* the registered topics as a Key(topic) Value(array of client sockets) pair. */
  struct hash_map *topics;
  /* the thread pool */
  struct thread_pool *pool;
  /* atomic flag for the running process. */
  atomic_bool running;
};

__END_DECLS

#endif
