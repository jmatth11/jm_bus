#include "client_handler.h"
#include "helpers/array.h"
#include "types/client.h"
#include <stdio.h>

bool client_list_init(struct client_list *cl) {
  if (!init_pollfd_array(&cl->fds, 100)) {
    fprintf(stderr, "error initializing poll file descriptors.\n");
    return false;
  }
  return true;
}

bool client_list_add(struct client_list *cl, int client_socket) {
  struct pollfd local_pollfd = {
    .fd = client_socket,
    .events = POLLIN,
    .revents = 0,
  };
  if (!insert_pollfd_array(&cl->fds, local_pollfd)) {
    fprintf(stderr, "error added poll file descriptor to array.\n");
    return false;
  }
  return true;
}

bool client_list_remove(struct client_list *cl, int client_socket) {
  bool result = true;
  for (int i = 0; i < cl->fds.len; ++i) {
    struct pollfd *local_fd = &cl->fds.pollfd_data[i];
    if (local_fd->fd == client_socket) {
      result = array_remove_item(&cl->fds, i);
      break;
    }
  }
  int gap_size = cl->fds.cap - cl->fds.len;
  if (gap_size > 100) {
    // cut gap size in half to reclaim memory
    if (!array_resize(&cl->fds, (gap_size/2))) {
      return false;
    }
  }
  return result;
}

bool client_list_remove_by_idx(struct client_list *cl, int idx) {
  return array_remove_item(&cl->fds, idx);
}

bool client_list_get(struct client_list *cl, int client_socket, struct pollfd *fd) {
  bool found = false;
  struct pollfd local;
  for (int i = 0; i < cl->fds.len; ++i) {
    get_pollfd_array(&cl->fds, i, &local);
    if (local.fd == client_socket) {
      *fd = local;
      found = true;
      break;
    }
  }
  return found;
}

bool client_list_get_by_idx(struct client_list *cl, int idx, struct pollfd *fd) {
  if (idx >= cl->fds.len) return false;
  *fd = cl->fds.pollfd_data[idx];
  return true;
}

bool client_list_close_connections(struct client_list *cl) {
  for (int i = 0; i < cl->fds.len; ++i) {
    struct pollfd *client = &cl->fds.pollfd_data[i];
    close(client->fd);
  }
  return true;
}

void client_list_free(struct client_list *cl) {
  free_pollfd_array(&cl->fds);
}

