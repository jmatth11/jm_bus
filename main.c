#include "handlers/client_handler.h"
#include "handlers/server_handler.h"
#include "handlers/connections.h"
#include "types/client.h"
#include "types/server.h"
#include "types/state.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {
  struct server_state state;
  state.running = true;
  state.server.addr = server_handler_default_addr();
  if (!server_handler_init(&state.server, SOCK_STREAM, 0)) {
    fprintf(stderr, "server_info failed.\n");
    exit(1);
  }
  if (!server_handler_bind_and_listen(&state.server)) {
    fprintf(stderr, "bind and listen failed.\n");
    exit(2);
  }
  if (!client_list_init(&state.clients)) {
    fprintf(stderr, "error client list initialization.\n");
    exit(3);
  }
  if (!client_list_add(&state.clients, state.server.socket)) {
    fprintf(stderr, "error client list add.\n");
    exit(3);
  }
  // spawn acceptence thread
  while (state.running) {
    // wait for activity on the connections.
    int activity = poll(state.clients.fds.pollfd_data, state.clients.fds.len, -1);

    if (activity < 0) {
      fprintf(stderr, "poll over file descriptors failed.\n");
      break;
    }
    // change function name
    process_messages(&state);
  }
  return 0;
}
