#include "server_states.h"
#include "client_list.h"
#include "helpers/socket.h"
#include "server.h"
#include "structures/hash_map.h"
#include "structures/thread_pool.h"
#include "types/array_types.h"
#include "types/client.h"
#include "types/state.h"
#include <stdio.h>
#include <string.h>


bool server_state_init(struct server_state *s, struct sockaddr_in *addr) {
  s->running = true;
  if (addr == NULL) {
    s->server.addr = create_default_addr();
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
  printf("CALLING ADD TOPIC client %d\n", client_sock);
  int idx = client_list_get_idx(&s->clients, client_sock);
  printf("idx=%d\n", idx);
  struct client_metadata *metadata = &s->clients.metadata.client_metadata_data[idx];
  size_t topic_len = strlen(topic);
  for (int i = 0; i < metadata->topics.len; ++i) {
    const char *md_topic = metadata->topics.str_data[i];
    printf("STRNCMP: md_topic=%s, topic=%s, topic_len=%zu\n", md_topic, topic, topic_len);
    if (strncmp(md_topic, topic, topic_len) == 0) {
      fprintf(stderr, "client %d, is already subscribed to topic \"%s\".\n", client_sock, topic);
      return true;
    }
  }
  char *md_topic = malloc(sizeof(char)*topic_len);
  strncpy(md_topic, topic, topic_len);
  if (!insert_str_array(&metadata->topics, md_topic)) {
    fprintf(stderr, "copying topic to metadata failed.\n");
    return false;
  }
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
  struct pollfd local_c;
  for (int i = 0; i < s->clients.fds.len; ++i) {
    if (client_list_get_by_idx(&s->clients, i, &local_c)) {
      if (local_c.fd == client_sock) {
        found_idx = i;
        break;
      }
    }
  }
  if (found_idx == -1) {
    fprintf(stderr, "could not find client socket.\n");
    return false;
  }
  struct client_metadata *metadata = &s->clients.metadata.client_metadata_data[found_idx];
  for (int topic_idx = 0; topic_idx < metadata->topics.len; ++topic_idx) {
    const char *topic_name = metadata->topics.str_data[topic_idx];
    printf("handling topic_name \"%s\" for client %d.\n", topic_name, client_sock);
    if (!hash_map_remove_value(s->topics, topic_name, client_sock)) {
      fprintf(stderr, "remove topics for removed client failed.\n");
      return false;
    }
  }
  close(client_sock);
  if (!client_list_remove(&s->clients, client_sock)) {
    fprintf(stderr, "client list remove by idx in server state remove client failed.\n");
    return false;
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
