#include "../deps/array_template/array_template.h"

#include "client_list.h"
#include "connections.h"
#include "handlers/messages.h"
#include "server_states.h"
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
#include "structures/hash_map.h"

static bool send_client_message(int client_sock, byte_array buf) {
  if (send(client_sock, buf.byte_data, buf.len, 0) != 0) {
    fprintf(stderr, "failed to send message to client %d\n", client_sock);
    return false;
  }
  return true;
}

static bool publish_message(struct server_state *s, int from, struct message *msg) {
  bool result = false;
  byte_array buf;
  char *topic_name = NULL;
  if (!init_byte_array(&buf, 20)) {
    fprintf(stderr, "Client %d: initializing byte array for message.\n", from);
    goto clean_up;
  }
  size_t n = messages_write(msg, &buf);
  if (n == 0) {
    fprintf(stderr, "Client %d: message could not be generated.\n", from);
    goto clean_up;
  }
  topic_name = message_get_topic(msg);
  if (topic_name == NULL) {
    fprintf(stderr, "topic name was null in publish message.\n");
    goto clean_up;
  }
  int_array clients;
  if (!hash_map_get(s->topics, topic_name, &clients)) {
    fprintf(stderr, "error getting clients for topic %s.\n", topic_name);
    goto clean_up;
  }
  for (int i = 0; i < clients.len; ++i) {
    int client_sock = clients.int_data[i];
    if (client_sock != from) {
      if (!send_client_message(client_sock, buf)) {
        fprintf(stderr, "error publishing message on topic \"%s\" from %d, to %d.\n", topic_name, from, client_sock);
      }
    }
  }
  result = true;
clean_up:
  if (buf.byte_data != NULL) {
    free_byte_array(&buf);
  }
  if (topic_name != NULL) {
    free(topic_name);
  }
  return result;
}

void* accept_messages(void *ctx) {
  struct server_state *state = (struct server_state*)ctx;
  int client_sock = accept(state->server.socket, NULL, NULL);
  if (client_sock != -1) {
    fprintf(stdout, "adding client %d\n", client_sock);
    byte_array buf;
    if (!init_byte_array(&buf, 20)) {
      fprintf(stderr, "connection message for client %d failed.\n", client_sock);
      return NULL;
    }
    if (messages_gen_connection(&buf) == 0) {
      fprintf(stderr, "create connection message for client %d failed.\n", client_sock);
      return NULL;
    }
    if (!send_client_message(client_sock, buf)) {
      fprintf(stderr, "sending connection message for client %d failed.\n", client_sock);
      return NULL;
    }
    if (!server_state_add_client(state, client_sock)) {
      fprintf(stderr, "server state add client failed.\n");
    }
  } else {
    fprintf(stderr, "socket accept failed: %s\n", strerror(errno));
  }
  return NULL;
}

void* process_poll_events(void *ctx) {
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
            struct message_event event = {
              .state = state,
              .from = local_c.fd,
              .msgs = msgs,
            };
            thread_pool_start_job(state->pool, process_messages, event);
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

void* process_messages(void *ctx) {
  struct thread_job *job = (struct thread_job*)ctx;
  struct server_state *state = job->event.state;
  while (true) {
    pthread_mutex_lock(&job->mutex);

    if (pthread_cond_wait(&job->cond, &job->mutex) != 0) {
      fprintf(stderr, "pthread conditional wait failed.\n");
      continue;
    }
    if (job->event.from == -1) {
      pthread_mutex_unlock(&job->mutex);
      break;
    }

    for (int i = 0; i < job->event.msgs.len; ++i) {
      struct message *msg = &job->event.msgs.message_data[i];
      char *topic_name = message_get_topic(msg);
      switch (msg->type) {
        case SUBSCRIBE: {
          if (topic_name != NULL) {
            if (!server_state_add_client_topic(state, topic_name, job->event.from)) {
              fprintf(stderr, "Client %d: error subsribing to topic\n", job->event.from);
              byte_array err_msg;
              if (messages_gen_error("error subscribing to topic", msg->topic, &err_msg) != 0) {
                send_client_message(job->event.from, err_msg);
              } else {
                fprintf(stderr, "critical error in publish.\n");
              }
            }
          } else {
            fprintf(stderr, "Client %d: topic name was null in subscribe.\n", job->event.from);
          }
          break;
        }
        case PUBLISH: {
          if (!publish_message(state, job->event.from, msg)) {
            fprintf(stderr, "Client %d: error publishing message to topic %s.\n", job->event.from, topic_name);
            byte_array err_msg;
            if (messages_gen_error("error publishing message to topic", msg->topic, &err_msg) != 0) {
              send_client_message(job->event.from, err_msg);
            } else {
              fprintf(stderr, "critical error in publish.\n");
            }
          }
          break;
        }
        case ERROR: {
          server_state_remove_client(job->event.state, job->event.from);
          break;
        }
      }
      free(topic_name);
    }
    pthread_mutex_unlock(&job->mutex);
  }
  return job;
}
