#ifndef JM_BUS_CLIENT_H
#define JM_BUS_CLIENT_H

#include "types/array_types.h"

#include <stdatomic.h>
#include <sys/poll.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct client_list {
  pollfd_array fds;
  atomic_bool modified;
};

struct client {
  int client_sock;
};

__END_DECLS

#endif
