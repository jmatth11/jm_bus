#include "handlers/client.h"
#include "handlers/messages.h"
#include "helpers/bytes.h"
#include "types/array_types.h"
#include "types/client.h"
#include "types/message.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <errno.h>

int main(int argc, const char **argv) {
  struct client client;
  if (!client_init(&client, NULL)) {
    fprintf(stderr, "client init failed.\n");
    exit(1);
  }
  if (!client_connect(&client)) {
    fprintf(stderr, "connect failed.\n");
    exit(1);
  }
  client_read_messages(&client);
  sleep(1);
  if (!client_subscribe(&client, "test_topic")) {
    fprintf(stderr, "client subscription failed.\n");
    exit(1);
  }
  sleep(1);
  message_array msgs = client_read_messages(&client);
  for (int i = 0; i < msgs.len; ++i) {
    struct message *local_msg = &msgs.message_data[i];
    printf("Type: %d\n", local_msg->type);
    printf("Topic:");
    char *topic_name = byte_array_to_str(&local_msg->topic);
    printf("%s\n", topic_name);
    printf("Body:");
    char *body = byte_array_to_str(&local_msg->body);
    printf("%s\n", body);
    free(topic_name);
    free(body);
    // done with message, free it
    message_free(local_msg);
  }
  free_message_array(&msgs);
  client_free(&client);
  return 0;
}
