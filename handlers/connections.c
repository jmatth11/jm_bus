#include "../deps/array_template/array_template.h"

#include "connections.h"
#include "client_handler.h"
#include "handlers/messages.h"
#include "states.h"
#include "structures/thread_pool.h"
#include "types/message.h"
#include "types/state.h"
#include "types/array_types.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>

void* accept_messages(void *ctx) {
  struct server_state *state = (struct server_state*)ctx;
  int client_sock = accept(state->server.socket, NULL, NULL);
  if (client_sock != -1) {
    fprintf(stdout, "adding client %d\n", client_sock);
    // TODO send acceptance packet
    send(client_sock, "T", 1, 0);
    if (!server_state_add_client(state, client_sock)) {
      fprintf(stderr, "server state add client failed.\n");
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
            message_array msgs = message_array_generate_from_client(local_c.fd);
            if (msgs.len == 0) {
              fprintf(stdout, "client %d disconnected with 0 recv\n", local_c.fd);
              insert_int_array(&marked, i);
            }
          } else if (local_c.revents & (POLLHUP | POLLERR)) {
              fprintf(stdout, "client %d disconnected\n", local_c.fd);
              insert_int_array(&marked, i);
          }
        }
      }
  }
  // remove the marked clients.
  for (int marked_idx = 0; marked_idx < marked.len; ++marked_idx) {
    server_state_remove_clients(state, marked);
  }
  return NULL;
}

void* send_messages(void *ctx) {
  struct thread_job *job = (struct thread_job*)ctx;
  bool running = true;
  while (running) {
    pthread_mutex_lock(&job->mutex);

    if (pthread_cond_wait(&job->cond, &job->mutex) != 0) {
      fprintf(stderr, "pthread conditional wait failed.\n");
      continue;
    }

    // TODO send messages out to clients

    pthread_mutex_unlock(&job->mutex);
  }
  return job;
}
