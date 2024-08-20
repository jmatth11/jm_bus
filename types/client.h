#ifndef JM_BUS_CLIENT_H
#define JM_BUS_CLIENT_H

#include "../deps/array_template/array_template.h"

#include <stdatomic.h>
#include <sys/poll.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

generate_array_template(pollfd, struct pollfd)

struct client_list {
  pollfd_array fds;
  atomic_bool modified;
};

__END_DECLS

#endif
