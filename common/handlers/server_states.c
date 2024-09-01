#include "server_states.h"
#include "client_list.h"
#include "server.h"
#include "structures/hash_map.h"
#include "structures/thread_pool.h"
#include "types/state.h"
#include <stdio.h>


bool server_state_init(struct server_state *s, struct sockaddr_in *addr) {
  s->running = true;
  if (addr == NULL) {
    s->server.addr = server_handler_default_addr();
  } else {
    s->server.addr = *addr;
  }
  if (!server_handler_init(&s->server, SOCK_STREAM, 0)) {
    fprintf(stderr, "server_info failed.\n");
    return false;
  }
  if (!server_handler_bind_and_listen(&s->server)) {
    fprintf(stderr, "bind and listen failed.\n");
    return false;
  }
  if (!client_list_init(&s->clients)) {
    fprintf(stderr, "error client list initialization.\n");
    return false;
  }
  if (!client_list_add(&s->clients, s->server.socket)) {
    fprintf(stderr, "error client list add.\n");
    return false;
  }
  s->topics = hash_map_create(1000);
  if (s->topics == NULL) {
    fprintf(stderr, "hash map create failed.\n");
    return false;
  }
  s->pool = thread_pool_create(100);
  if (s->pool == NULL) {
    fprintf(stderr, "thread pool create failed.\n");
    return false;
  }
  return true;
}

bool server_state_add_client_topic(struct server_state *s, const char *topic, int client_sock) {
  return hash_map_set(s->topics, topic, client_sock);
}

bool server_state_add_client(struct server_state *s, int client_sock) {
  if (!client_list_add(&s->clients, client_sock)) {
    fprintf(stderr, "client failed to be added: %d\n", client_sock);
    return false;
  }
  return true;
}

bool server_state_remove_client(struct server_state *s, int client_sock) {
  int found_idx = -1;
  for (int i = 0; i < s->clients.fds.len; ++i) {
    struct pollfd local_c;
    if (client_list_get_by_idx(&s->clients, i, &local_c)) {
      if (local_c.fd == client_sock) {
        found_idx = i;
        break;
      }
    }
  }
  close(client_sock);
  if (!client_list_remove_by_idx(&s->clients, found_idx)) {
    fprintf(stderr, "client list remove by idx in server state remove client failed.\n");
    return false;
  }
  return true;
}

bool server_state_remove_clients(struct server_state *s, int_array client_idxs) {
  for (int idx = 0; idx < client_idxs.len; ++idx) {
    int remove_idx = client_idxs.int_data[idx];
    struct pollfd *local_c = &s->clients.fds.pollfd_data[remove_idx];
    close(local_c->fd);
    if (!client_list_remove_by_idx(&s->clients, remove_idx)) {
      fprintf(stderr, "remove clients with client remove by idx failed.\n");
      return false;
    }
  }
  return true;
}

bool server_state_free(struct server_state *s) {
  if (!client_list_close_connections(&s->clients)) {
    fprintf(stderr, "client list close connections failed.\n");
  }
  client_list_free(&s->clients);
  server_handler_close(&s->server);
  hash_map_destroy(s->topics);
  thread_pool_destroy(s->pool);
  return true;
}
