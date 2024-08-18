#include "client_handler.h"
#include "types/client.h"

struct client_info client_from_socket(int client_socket) {
  return (struct client_info){
    .socket = client_socket,
    .flags = 0,
  };
}

bool client_list_init(struct client_list *cl) {
  return init_client_info_array(&cl->clients, 1000);
}

bool client_list_add(struct client_list *cl, int client_socket) {
  return insert_client_info_array(&cl->clients, client_from_socket(client_socket));
}

bool client_list_get(struct client_list *cl, int client_socket, struct client_info *ci) {
  bool found = false;
  struct client_info local;
  for (int i = 0; i < cl->clients.len; ++i) {
    get_client_info_array(&cl->clients, i, &local);
    if (local.socket == client_socket) {
      *ci = local;
      found = true;
      break;
    }
  }
  return found;
}

bool client_list_get_by_idx(struct client_list *cl, int idx, struct client_info *ci) {
  if (idx >= cl->clients.len) return false;
  *ci = cl->clients.client_info_data[idx];
  return true;
}

void client_list_free(struct client_list *cl) {
  free_client_info_array(&cl->clients);
}

