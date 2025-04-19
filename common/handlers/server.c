#include "server.h"
#include "helpers/log.h"
#include "types/server.h"

#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

bool server_handler_init(struct server_info *si, int __type, int __protocol) {
  si->socket = socket(si->addr.sin_family, __type, __protocol);
  if (si->socket == -1) {
    error_log("server socket failed to initialize.\n");
    return false;
  }
  int opt = 1;
  if (setsockopt(si->socket,
                 SOL_SOCKET,
                 SO_REUSEADDR,
                 (char*)&opt,
                 sizeof(opt)) < 0) {
    error_log("set socket options failed.\n");
    return false;
  }
  if (ioctl(si->socket, FIONBIO, &opt) < 0) {
    error_log("server socket failed to be set to non-blocking.\n");
    return false;
  }
  return true;
}

bool server_handler_bind_and_listen(struct server_info *si) {
  if (bind(si->socket, (struct sockaddr *)&si->addr, sizeof(si->addr)) != 0) {
    error_log("bind failed. Error: %s\n", strerror(errno));
    return false;
  }
  if (listen(si->socket, 10) != 0) {
    error_log("listen failed.\n");
    return false;
  }
  return true;
}

void server_handler_close(struct server_info *si) {
  shutdown(si->socket, SHUT_RDWR);
  close(si->socket);
}
