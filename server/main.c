#include "handlers/connections.h"
#include "handlers/server_states.h"
#include "helpers/log.h"
#include "types/state.h"
#include <stdlib.h>

int main(int argc, const char **argv) {
  struct server_state state;
  if (!server_state_init(&state, NULL)) {
    error_log("server state init failed.\n");
    exit(1);
  }
  // start main loop
  while (state.running) {
    // wait for activity on the connections.
    int activity = poll(state.clients.fds.pollfd_data, state.clients.fds.len, -1);

    if (activity < 0) {
      error_log("poll over file descriptors failed.\n");
      exit(2);
    }
    // change function name
    process_poll_events(&state);
  }
  if (!server_state_free(&state)) {
    error_log("server state free failed.\n");
    exit(3);
  }
  return 0;
}
