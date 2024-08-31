#include "messages.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "helpers/bytes.h"
#include "types/array_types.h"
#include "types/message.h"

#ifndef JM_BUS_UTF8_BYTE_LEN
#define JM_BUS_UTF8_BYTE_LEN 4
#endif

static uint8_t read_buffer[BUFSIZ];

size_t messages_read(const byte_array msg, struct message *out) {
  // TODO probably add some way to debug when we hit an error
  struct message local;
  size_t cur_idx = 0;
  if (msg.len <= 0) return 0;
  out->type = msg.byte_data[cur_idx];
  ++cur_idx;
  // check to make sure there is a size value in the next few bytes
  if (msg.len <= (cur_idx + (JM_BUS_UTF8_BYTE_LEN - 1))) return 0;
  size_t topic_len = 0;
  convert_n_utf8_to_64bit(&msg.byte_data[cur_idx], JM_BUS_UTF8_BYTE_LEN, &topic_len);
  if (!init_byte_array(&local.topic, topic_len)) {
    fprintf(stderr, "error initializing topic.\n");
    cur_idx = 0;
    goto clean_up;
  }
  cur_idx += JM_BUS_UTF8_BYTE_LEN;
  if (msg.len <= cur_idx) return 0;
  read_byte_array(&msg.byte_data[cur_idx], &local.topic);
  cur_idx += topic_len;
  // check to make sure there is a size value in the next few bytes
  if (msg.len <= (cur_idx + (JM_BUS_UTF8_BYTE_LEN - 1))) return 0;
  size_t body_len = 0;
  convert_n_utf8_to_64bit(&msg.byte_data[cur_idx], JM_BUS_UTF8_BYTE_LEN, &body_len);
  if (!init_byte_array(&local.body, body_len)) {
    fprintf(stderr, "error initializing body.\n");
    cur_idx = 0;
    goto clean_up;
  }
  cur_idx += JM_BUS_UTF8_BYTE_LEN;
  if (msg.len <= cur_idx) return 0;
  read_byte_array(&msg.byte_data[cur_idx], &local.body);
  cur_idx += body_len;
  *out = local;

clean_up:
  if (local.topic.byte_data != NULL) {
    free_byte_array(&local.topic);
  }
  if (local.body.byte_data != NULL) {
    free_byte_array(&local.body);
  }
  return cur_idx;
}

size_t messages_write(const struct message *msg, byte_array *out) {
  size_t msg_len = 11 + msg->topic.len + msg->body.len;
  byte_array local;
  size_t cur_idx = 0;
  if (!init_byte_array(&local, msg_len)) {
    fprintf(stderr, "error initializing message.\n");
    return 0;
  }
  if (!insert_byte_array(&local, msg->type)) {
    fprintf(stderr, "error inserting type byte.\n");
    free_byte_array(&local);
    return 0;
  }
  ++cur_idx;
  convert_64bit_to_n_utf8(msg->topic.len, JM_BUS_UTF8_BYTE_LEN, &local.byte_data[cur_idx]);
  cur_idx += JM_BUS_UTF8_BYTE_LEN;
  if (memcpy(&local.byte_data[cur_idx], msg->topic.byte_data, msg->topic.len) != NULL) {
    fprintf(stderr, "error with memcpy for topic.\n");
    free_byte_array(&local);
    return 0;
  }
  cur_idx += msg->topic.len;
  convert_64bit_to_n_utf8(msg->body.len, JM_BUS_UTF8_BYTE_LEN, &local.byte_data[cur_idx]);
  cur_idx += JM_BUS_UTF8_BYTE_LEN;
  if (memcpy(&local.byte_data[cur_idx], msg->body.byte_data, msg->body.len) != NULL) {
    fprintf(stderr, "error with memcpy for body.\n");
    free_byte_array(&local);
    return 0;
  }
  cur_idx += msg->body.len;
  *out = local;
  return cur_idx;
}

size_t messages_gen_connection(byte_array *out) {
  struct message msg = {
    .type = CONNECTION,
  };
  return messages_write(&msg, out);
}

size_t messages_gen_error(char *err, byte_array topic, byte_array *out) {
  struct message msg = {
    .type = ERROR,
  };
  if (!init_byte_array(&msg.body, 20)) {
    fprintf(stderr, "error generating body for error message.\n");
    return 0;
  }
  size_t n = strlen(err);
  for (int i = 0; i < n; ++i) {
    if (!insert_byte_array(&msg.body, err[i])) {
      fprintf(stderr, "error inserting character for error message.\n");
    }
  }
  return messages_write(&msg, out);
}

char * message_get_topic(const struct message *msg) {
  char *topic = malloc(sizeof(char)*msg->topic.len);
  memcpy(topic, msg->topic.byte_data, msg->topic.len);
  return topic;
}

message_array message_array_generate_from_client(int client_sock) {
  byte_array byte_buffer;
  message_array result;
  if (!init_message_array(&result, 2)) {
    fprintf(stderr, "init message array from client failed.\n");
    return result;
  }
  if (!init_byte_array(&byte_buffer, BUFSIZ)) {
    fprintf(stderr, "init byte array from client failed.\n");
    return result;
  }
  size_t n = 0;
  do {
    n = recv(client_sock, read_buffer, BUFSIZ, 0);
    for (int i = 0; i < n; ++i) {
      if (!insert_byte_array(&byte_buffer, read_buffer[i])) {
        fprintf(stderr, "generating byte array from client failed.\n");
        free_byte_array(&byte_buffer);
        return result;
      }
    }
  } while (n == BUFSIZ);
  n = 0;
  while (n < byte_buffer.len) {
    struct message local_c;
    n += messages_read(byte_buffer, &local_c);
    if (!insert_message_array(&result, local_c)) {
      fprintf(stderr, "failed to insert message from client.\n");
      free_message_array(&result);
      return result;
    }
  }
  return result;
}
