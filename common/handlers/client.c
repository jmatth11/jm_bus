#include "client.h"
#include "handlers/messages.h"
#include "helpers/bytes.h"
#include "helpers/log.h"
#include "helpers/socket.h"
#include "types/array_types.h"
#include "types/client.h"
#include "types/message.h"
#include <string.h>
#include <sys/socket.h>

bool client_init(struct client *c, struct sockaddr_in *addr) {
  if (addr == NULL) {
    c->addr = create_default_addr();
  } else {
    c->addr = *addr;
  }
  c->socket = socket(c->addr.sin_family, SOCK_STREAM, 0);
  if (c->socket == -1) {
    error_log("client socket failed.\n");
    return false;
  }
  return true;
}

bool client_connect(struct client *c) {
  if (connect(c->socket, (struct sockaddr *)&c->addr, sizeof(c->addr)) != 0) {
    error_log("client connection failed.\n");
    return false;
  }
  message_array msgs = client_read_messages(c);
  if (msgs.len == 0) {
    error_log("error in retrieving connection reciept from server.\n");
    return false;
  }
  bool result = false;
  for (int i = 0; i < msgs.len; ++i) {
    struct message *m = &msgs.message_data[i];
    if (m->type == CONNECTION) {
      result = true;
    }
  }
  return result;
}

bool client_subscribe(struct client *c, char *topic) {
  struct message msg = {
    .type = SUBSCRIBE,
  };
  if (!str_to_byte_array(topic, &msg.topic)) {
    error_log("str_to_byte_array in client_subscribe failed.\n");
    return false;
  }
  if (!prepare_and_send_message(c->socket, &msg)) {
    error_log("subscribe prepare_and_send_message failed.\n");
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
    error_log("str_to_byte_array in client_subscribe failed.\n");
    return false;
  }
  if (!prepare_and_send_message(c->socket, &buf)) {
    error_log("publish prepare_and_send_message failed.\n");
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

