#include "client.h"
#include "handlers/messages.h"
#include "helpers/bytes.h"
#include "helpers/socket.h"
#include "types/array_types.h"
#include "types/client.h"
#include "types/message.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

bool client_init(struct client *c, struct sockaddr_in *addr) {
  if (addr == NULL) {
    c->addr = create_default_addr();
  } else {
    c->addr = *addr;
  }
  c->socket = socket(c->addr.sin_family, SOCK_STREAM, 0);
  printf("client sock %d\n", c->socket);
  if (c->socket == -1) {
    fprintf(stderr, "client socket failed.\n");
    return false;
  }
  return true;
}

bool client_connect(struct client *c) {
  if (connect(c->socket, (struct sockaddr *)&c->addr, sizeof(c->addr)) != 0) {
    fprintf(stderr, "client connection failed.\n");
    return false;
  }
  // TODO validate connection response.
  return true;
}

bool client_subscribe(struct client *c, char *topic) {
  struct message msg = {
    .type = SUBSCRIBE,
  };
  if (!str_to_byte_array(topic, &msg.topic)) {
    fprintf(stderr, "str_to_byte_array in client_subscribe failed.\n");
    return false;
  }
  if (!prepare_and_send_message(c->socket, &msg)) {
    fprintf(stderr, "subscribe prepare_and_send_message failed.\n");
    return false;
  }
  message_free(&msg);
  return true;
}

bool client_publish(struct client *c, char *topic, byte_array msg) {
  struct message buf = {
    .type = PUBLISH,
    .body = msg,
  };
  if (!str_to_byte_array(topic, &buf.topic)) {
    fprintf(stderr, "str_to_byte_array in client_subscribe failed.\n");
    return false;
  }
  if (!prepare_and_send_message(c->socket, &buf)) {
    fprintf(stderr, "publish prepare_and_send_message failed.\n");
    free_byte_array(&buf.topic);
    return false;
  }
  free_byte_array(&buf.topic);
  return true;
}

message_array client_read_messages(struct client *c) {
  return message_array_generate_from_client(c->socket);
}

bool client_free(struct client *c) {
  close(c->socket);
  return true;
}

