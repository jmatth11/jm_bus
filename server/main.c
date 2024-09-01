#include "handlers/connections.h"
#include "handlers/server_states.h"
#include "types/state.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv) {
  struct server_state state;
  if (!server_state_init(&state, NULL)) {
    fprintf(stderr, "server state init failed.\n");
    exit(1);
  }
  // start main loop
  while (state.running) {
    // wait for activity on the connections.
    int activity = poll(state.clients.fds.pollfd_data, state.clients.fds.len, -1);

    if (activity < 0) {
      fprintf(stderr, "poll over file descriptors failed.\n");
      exit(2);
    }
    // change function name
    process_poll_events(&state);
  }
  if (!server_state_free(&state)) {
    fprintf(stderr, "server state free failed.\n");
    exit(3);
  }
  return 0;
}
