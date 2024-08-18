#include "handlers/client_handler.h"
#include "handlers/server_handler.h"
#include "handlers/thread_handler.h"
#include "helpers/time.h"
#include "types/client.h"
#include "types/server.h"
#include "types/state.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>

int main(int argc, const char **argv) {
  struct server_state state;
  state.running = true;
  state.accepting = true;
  state.server.addr = server_handler_default_addr();
  if (!server_handler_init(&state.server, SOCK_STREAM, 0)) {
    fprintf(stderr, "server_info failed.\n");
    exit(1);
  }
  if (!server_handler_bind_and_listen(&state.server, 1000)) {
    fprintf(stderr, "bind and listen failed.\n");
    exit(2);
  }
  while (state.running) {
    // TODO rework this setup of select FDs so we don't have to
    // repopulate this list every time
    // clear set
    FD_ZERO(&state.readfs);
    // add server socket to set
    FD_SET(state.server.socket, &state.readfs);
    int max_fd = state.server.socket;
    for (int i = 0; i < state.clients.clients.len; ++i) {
      struct client_info local_c;
      // set the client connections to the read listener.
      if (client_list_get_by_idx(&state.clients, i, &local_c)) {
        FD_SET(local_c.socket, &state.readfs);
        if (max_fd < local_c.socket) max_fd = local_c.socket;
      }
    }

    // wait for activity on the connections.
    int activity = select(max_fd + 1,
                          &state.readfs, NULL, NULL, NULL);

    if (activity < 0) {
      fprintf(stderr, "select over file descriptors failed.\n");
      break;
    }
    // if server socket was set, it's a new connection.
    if (FD_ISSET(state.server.socket, &state.readfs)) {
      // change function name
      accept_loop(&state);
    } else {
      // change function name
      process_loop(&state);
    }
  }
  return 0;
}
