#ifndef JM_BUS_SERVER_H
#define JM_BUS_SERVER_H

#include <netinet/in.h>

struct server_info {
  int socket;
  struct sockaddr_in addr;
};

#endif
