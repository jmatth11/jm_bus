#ifndef JM_BUS_CLIENT_H
#define JM_BUS_CLIENT_H

#include "types/array_types.h"

#include <netinet/in.h>
#include <stdatomic.h>
#include <sys/poll.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct client_metadata {
  str_array topics;
};

generate_array_template(client_metadata, struct client_metadata);

struct client_list {
  pollfd_array fds;
  client_metadata_array metadata;
  atomic_bool modified;
};

struct client {
  int socket;
  struct sockaddr_in addr;
};

__END_DECLS

#endif
