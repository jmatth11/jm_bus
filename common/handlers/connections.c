#include "../deps/array_template/array_template.h"

#include "helpers/log.h"

#include "client_list.h"
#include "connections.h"
#include "handlers/messages.h"
#include "helpers/socket.h"
#include "server_states.h"
#include "structures/thread_pool.h"
#include "types/message.h"
#include "types/state.h"
#include "types/array_types.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include "structures/hash_map.h"

static bool publish_message(struct server_state *s, int from, struct message *msg) {
  char *topic_name = message_get_topic(msg);
  if (topic_name == NULL) {
    error_log("topic name was null in publish message.\n");
    return false;
  }
  int_array clients;
  if (!hash_map_get(s->topics, topic_name, &clients)) {
    error_detailed("error getting clients for topic %s.\n", topic_name);
    free(topic_name);
    return false;
  }
  for (int i = 0; i < clients.len; ++i) {
    int client_sock = clients.int_data[i];
    if (client_sock != from) {
      info_log("sending message to %d\n", client_sock);
      if (!prepare_and_send_message(client_sock, msg)) {
        error_detailed("error publishing message on topic \"%s\" from %d, to %d.\n", topic_name, from, client_sock);
      }
    }
  }
  free(topic_name);
  return true;
}

void accept_messages(struct server_state *state) {
  int client_sock = accept(state->server.socket, NULL, NULL);
  if (client_sock != -1) {
    info_log("adding client %d\n", client_sock);
    byte_array buf;
    if (!init_byte_array(&buf, 20)) {
      error_detailed("connection message for client %d failed.\n", client_sock);
      return;
    }
    if (messages_gen_connection(&buf) == 0) {
      error_log("create connection message for client %d failed.\n", client_sock);
      return;
    }
    if (!send_message(client_sock, buf)) {
      error_log("sending connection message for client %d failed.\n", client_sock);
      return;
    }
    if (!server_state_add_client(state, client_sock)) {
      error_log("server state add client failed.\n");
    }
  } else {
    error_log("socket accept failed: %s\n", strerror(errno));
  }
}

void process_poll_events(struct server_state *state) {
  int_array marked;
  init_int_array(&marked, 10);
  for (int i = 0; i < state->clients.fds.len; ++i) {
    struct pollfd *local_c = &state->clients.fds.pollfd_data[i];
#ifdef DEBUG
    debug_detailed("client %d, local_c.revents: %04x\n", local_c->fd, local_c->revents);
#endif
    if (local_c->fd == state->server.socket) {
      if (local_c->revents & POLLIN) {
        accept_messages(state);
      }
    } else {
      if (local_c->revents & (POLLHUP | POLLERR)) {
          info_log("client %d disconnected\n", local_c->fd);
          insert_int_array(&marked, local_c->fd);
      } else if (local_c->revents & POLLIN) {
        message_array msgs = message_array_generate_from_client(local_c->fd);
        if (msgs.len == 0) {
          info_log("client %d messaged with 0 recv -- disconnecting\n", local_c->fd);
          message_array_free_list(msgs);
          free_message_array(&msgs);
          insert_int_array(&marked, local_c->fd);
          continue;
        }
        struct message_event event = {
          .state = state,
          .from = local_c->fd,
          .msgs = msgs,
        };
        thread_pool_start_job(state->pool, process_messages, event);
      } else if (local_c->revents & POLLNVAL) {
        error_log("client %d had an invalid polling state -- disconnecting.\n", local_c->fd);
        insert_int_array(&marked, local_c->fd);
      }
    }
  }
  // remove the marked clients.
  for (int marked_idx = 0; marked_idx < marked.len; ++marked_idx) {
    if (!server_state_remove_client(state, marked.int_data[marked_idx])) {
      error_log("server state failed to remove clients.\n");
      exit(1);
    }
  }
}

void* process_messages(void *ctx) {
  struct thread_job *job = (struct thread_job*)ctx;
  struct server_state *state = job->event.state;
  pthread_cond_signal(&job->cond);
  while (true) {
    pthread_mutex_lock(&job->mutex);

    if (pthread_cond_wait(&job->cond, &job->mutex) != 0) {
      error_log("pthread conditional wait failed.\n");
      continue;
    }
    if (job->event.from == -1) {
      pthread_mutex_unlock(&job->mutex);
      break;
    }

    for (int i = 0; i < job->event.msgs.len; ++i) {
      struct message *msg = &job->event.msgs.message_data[i];
      char *topic_name = message_get_topic(msg);
#ifdef DEBUG
      debug_log("DEBUG: process messages msg\n");
      message_print(msg);
      debug_log("DEBUG: process messages msg end\n");
      debug_detailed("client %d, topic_name=%s, msg->type=%d\n", job->event.from, topic_name, msg->type);
#endif
      switch (msg->type) {
        case SUBSCRIBE: {
          if (topic_name != NULL) {
            if (!server_state_add_client_topic(state, topic_name, job->event.from)) {
              error_log("Client %d: error subsribing to topic\n", job->event.from);
              byte_array err_msg;
              if (messages_gen_error("error subscribing to topic", &err_msg) != 0) {
                send_message(job->event.from, err_msg);
                free_byte_array(&err_msg);
              } else {
                error_log("critical error in publish.\n");
              }
            }
          } else {
            error_log("Client %d: topic name was null in subscribe.\n", job->event.from);
          }
          break;
        }
        case PUBLISH: {
          if (!publish_message(state, job->event.from, msg)) {
            error_log("Client %d: error publishing message to topic %s.\n", job->event.from, topic_name);
            byte_array err_msg;
            if (messages_gen_error("error publishing message to topic", &err_msg) != 0) {
              send_message(job->event.from, err_msg);
              free_byte_array(&err_msg);
            } else {
              error_log("critical error in publish.\n");
            }
          }
          break;
        }
        case ERROR: {
          server_state_remove_client(job->event.state, job->event.from);
          break;
        }
        case CONNECTION:
          break;
      }
      free(topic_name);
    }
    message_array_free_list(job->event.msgs);
    free_message_array(&job->event.msgs);
    pthread_mutex_unlock(&job->mutex);
    atomic_store(&job->occupied, false);
  }
  return job;
}
