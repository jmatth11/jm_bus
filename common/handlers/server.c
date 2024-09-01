#include "server.h"
#include "types/server.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

bool server_handler_init(struct server_info *si, int __type, int __protocol) {
  si->socket = socket(si->addr.sin_family, __type, __protocol);
  if (si->socket == -1) {
    fprintf(stderr, "server socket failed to initialize.\n");
    return false;
  }
  int opt = 1;
  if (setsockopt(si->socket,
                 SOL_SOCKET,
                 SO_REUSEADDR,
                 (char*)&opt,
                 sizeof(opt)) < 0) {
    fprintf(stderr, "set socket options failed.\n");
    return false;
  }
  if (ioctl(si->socket, FIONBIO, &opt) < 0) {
    fprintf(stderr, "server socket failed to be set to non-blocking.\n");
    return false;
  }
  return true;
}

struct sockaddr_in server_handler_default_addr() {
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(9901);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  return serv_addr;
}

struct sockaddr_in server_handler_addr(sa_family_t __type, uint16_t port, int32_t addr) {
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = __type;
  serv_addr.sin_port = port;
  serv_addr.sin_addr.s_addr = addr;
  return serv_addr;
}

bool server_handler_bind_and_listen(struct server_info *si) {
  if (bind(si->socket, (struct sockaddr *)&si->addr, sizeof(si->addr)) != 0) {
    fprintf(stderr, "bind failed. Error: %s\n", strerror(errno));
    return false;
  }
  if (listen(si->socket, 10) != 0) {
    fprintf(stderr, "listen failed.\n");
    return false;
  }
  return true;
}

void server_handler_close(struct server_info *si) {
  shutdown(si->socket, SHUT_RDWR);
  close(si->socket);
}
