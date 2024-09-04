#ifndef JM_BUS_SERVER_H
#define JM_BUS_SERVER_H

#include <netinet/in.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * The basic server info structure.
 */
struct server_info {
  /* server connection info */
  struct sockaddr_in addr;
  /* the server's socket */
  int socket;
};

__END_DECLS

#endif
