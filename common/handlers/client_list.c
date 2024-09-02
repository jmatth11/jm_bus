#include "client_list.h"
#include "helpers/array.h"
#include "types/array_types.h"
#include "types/client.h"
#include <stdio.h>
#include <stdlib.h>

static bool client_metadata_array_remove(struct client_list *cl, int idx) {
  bool result = array_remove_item(&cl->metadata, idx);
  int gap_size = cl->metadata.cap - cl->metadata.len;
  if (gap_size > 100) {
    // cut gap size in half to reclaim memory
    if (!array_resize(&cl->metadata, (gap_size/2))) {
      return false;
    }
  }
  return result;
}

bool client_list_init(struct client_list *cl) {
  if (!init_pollfd_array(&cl->fds, 100)) {
    fprintf(stderr, "error initializing poll file descriptors.\n");
    return false;
  }
  if (!init_client_metadata_array(&cl->metadata, 100)) {
    fprintf(stderr, "error initializing client metadata.\n");
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
  struct client_metadata metadata;
  if (!client_metadata_init(&metadata)) {
    fprintf(stderr, "error initializing client metadata.\n");
    return false;
  }
  if (!insert_client_metadata_array(&cl->metadata, metadata)) {
    fprintf(stderr, "error adding metadata to client list.\n");
    return false;
  }
  return true;
}

bool client_list_remove(struct client_list *cl, int client_socket) {
  for (int i = 0; i < cl->fds.len; ++i) {
    struct pollfd *local_fd = &cl->fds.pollfd_data[i];
    if (local_fd->fd == client_socket) {
      bool fds_result = client_list_remove_by_idx(cl, i);
      struct client_metadata *md = &cl->metadata.client_metadata_data[i];
      client_metadata_free(md);
      bool metadata_result = client_metadata_array_remove(cl, i);
      if (!fds_result || metadata_result) {
        fprintf(stderr, "client list remove is out of synch.\n");
        exit(1);
      }
      break;
    }
  }
  return true;
}

bool client_list_remove_by_idx(struct client_list *cl, int idx) {
  bool result = array_remove_item(&cl->fds, idx);
  int gap_size = cl->fds.cap - cl->fds.len;
  if (gap_size > 100) {
    // cut gap size in half to reclaim memory
    if (!array_resize(&cl->fds, (gap_size/2))) {
      return false;
    }
  }
  return result;
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

int client_list_get_idx(struct client_list *cl, int client_socket) {
  struct pollfd local;
  int idx = -1;
  for (int i = 0; i < cl->fds.len; ++i) {
    get_pollfd_array(&cl->fds, i, &local);
    if (local.fd == client_socket) {
      idx = i;
      break;
    }
  }
  return idx;
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

bool client_metadata_init(struct client_metadata *metadata) {
  return init_str_array(&metadata->topics, 2);
}

void client_metadata_free(struct client_metadata *metadata) {
  for (int i = 0; i < metadata->topics.len; ++i) {
    free(metadata->topics.str_data[i]);
  }
  free_str_array(&metadata->topics);
}

