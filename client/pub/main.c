#include "handlers/client.h"
#include "helpers/bytes.h"
#include "types/array_types.h"
#include "types/client.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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
  sleep(1);
  if (!client_subscribe(&client, "test_topic")) {
    fprintf(stderr, "client subscription failed.\n");
    exit(1);
  }
  sleep(1);
  byte_array msg;
  const char *msg_str = "This is a test message";
  if (!str_to_byte_array(msg_str, &msg)) {
    fprintf(stderr, "client init byte array failed.\n");
    exit(1);
  }
  printf("publish to topic \"%s\"\nmessage: \"%s\"\n", "test_topic", msg_str);
  if (!client_publish(&client, "test_topic", msg)) {
    fprintf(stderr, "client publish failed.\n");
    exit(1);
  }
  sleep(1);
  free_byte_array(&msg);
  client_free(&client);
  return 0;
}
