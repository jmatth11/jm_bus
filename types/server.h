#ifndef JM_BUS_SERVER_H
#define JM_BUS_SERVER_H

#include <netinet/in.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct server_info {
  struct sockaddr_in addr;
  int socket;
};

__END_DECLS

#endif
