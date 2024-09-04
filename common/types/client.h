#ifndef JM_BUS_CLIENT_H
#define JM_BUS_CLIENT_H

#include "types/array_types.h"

#include <netinet/in.h>
#include <pthread.h>
#include <stdatomic.h>
#include <sys/poll.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * The client's metadata.
 * Currently used to store the topics a user is subscribed to.
 */
struct client_metadata {
  str_array topics;
};

/**
 * Generate Array type for client_metadata.
 */
generate_array_template(client_metadata, struct client_metadata);

/**
 * The client list used by the server to manage connection clients.
 */
struct client_list {
  pollfd_array fds;
  client_metadata_array metadata;
  pthread_mutex_t mutex;
};

/**
 * The client structure for a client application to use.
 */
struct client {
  int socket;
  struct sockaddr_in addr;
};

__END_DECLS

#endif
