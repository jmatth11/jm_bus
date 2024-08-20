#include "../deps/array_template/array_template.h"

#include "connections.h"
#include "client_handler.h"
#include "types/state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

generate_array_template(int, int)

void* accept_messages(void *ctx) {
  struct server_state *state = (struct server_state*)ctx;
  int client_sock = accept(state->server.socket, NULL, NULL);
  if (client_sock != -1) {
    fprintf(stdout, "adding client %d\n", client_sock);
    // TODO send acceptance packet
    send(client_sock, "T", 1, 0);
    if (!client_list_add(&state->clients, client_sock)) {
      fprintf(stderr, "client failed to be added: %d\n", client_sock);
    }
  } else {
    fprintf(stderr, "socket accept failed: %s\n", strerror(errno));
  }
  return NULL;
}

void* process_messages(void *ctx) {
  struct server_state *state = (struct server_state*)ctx;
  int_array marked;
  init_int_array(&marked, 10);
  char recv_msg[512];
  for (int i = 0; i < state->clients.fds.len; ++i) {
      struct pollfd local_c;
      // set the client connections to the read listener.
      if (client_list_get_by_idx(&state->clients, i, &local_c)) {
        if (local_c.fd == state->server.socket) {
          if (local_c.revents & POLLIN) {
            accept_messages(state);
          } else {
            fprintf(stdout, "server return event: %d\n", local_c.revents);
          }
        } else {
          if (local_c.revents & POLLIN) {
            int n = recv(local_c.fd, recv_msg, 512, 0);
            if (n == 0) {
              fprintf(stdout, "client %d disconnected with 0 recv\n", local_c.fd);
              close(local_c.fd);
              insert_int_array(&marked, i);
            }
            send(local_c.fd, recv_msg, n, 0);
          } else if (local_c.revents & (POLLHUP | POLLERR)) {
              fprintf(stdout, "client %d disconnected\n", local_c.fd);
              close(local_c.fd);
              insert_int_array(&marked, i);
          }
        }
      }
  }
  // remove the marked clients.
  for (int marked_idx = 0; marked_idx < marked.len; ++marked_idx) {
    client_list_remove_by_idx(&state->clients, marked.int_data[marked_idx]);
  }
  return NULL;
}
