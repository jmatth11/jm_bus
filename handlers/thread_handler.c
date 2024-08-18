#include "thread_handler.h"
#include "handlers/client_handler.h"
#include "types/state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

void* accept_loop(void *ctx) {
  struct server_state *state = (struct server_state*)ctx;
  int client_sock = accept(state->server.socket, NULL, NULL);
  if (client_sock != -1) {
    fprintf(stdout, "adding client %d\n", client_sock);
    send(client_sock, "T", 1, 0);
    if (!client_list_add(&state->clients, client_sock)) {
      fprintf(stderr, "client failed to be added: %d\n", client_sock);
    }
  } else {
    fprintf(stderr, "socket accept failed: %s\n", strerror(errno));
  }
  return NULL;
}

void* process_loop(void *ctx) {
  struct server_state *state = (struct server_state*)ctx;
  char recv_msg[512];
  for (int i = 0; i < state->clients.clients.len; ++i) {
      struct client_info local_c;
      // set the client connections to the read listener.
      if (client_list_get_by_idx(&state->clients, i, &local_c)) {
        if (FD_ISSET(local_c.socket, &state->readfs)) {
          int n = recv(local_c.socket, recv_msg, 512, 0);
          if (n == 0) {
            fprintf(stdout, "client %d disconnected\n", local_c.socket);
            close(local_c.socket);
            // TODO properly handle removing client from list
            break;
          }
          send(local_c.socket, recv_msg, n, 0);
        }
      }
  }
  return NULL;
}
