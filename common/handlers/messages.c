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

static bool read_all_raw_bytes(int socket, byte_array *out) {
  byte_array byte_buffer;
  if (!init_byte_array(&byte_buffer, BUFSIZ)) {
    fprintf(stderr, "init byte array from client failed.\n");
    return false;
  }
  size_t n = 0;
  do {
    n = recv(socket, read_buffer, BUFSIZ, 0);
    for (int i = 0; i < n; ++i) {
      if (!insert_byte_array(&byte_buffer, read_buffer[i])) {
        fprintf(stderr, "generating byte array from client failed.\n");
        free_byte_array(&byte_buffer);
        return false;
      }
    }
  } while (n == BUFSIZ);
  *out = byte_buffer;
  return true;
}

size_t messages_read(const byte_array msg, struct message *out) {
  // TODO probably add some way to debug when we hit an error
  printf("DEBUG: message_read\n");
  print_byte_array(msg);
  printf("DEBUG: message_read end\n");
  struct message local;
  size_t cur_idx = 0;
  if (msg.len <= 0) return 0;
  local.type = msg.byte_data[cur_idx];
  ++cur_idx;
  // check to make sure there is a size value in the next few bytes
  if (msg.len <= (cur_idx + (JM_BUS_UTF8_BYTE_LEN - 1))) return 0;
  size_t topic_len = 0;
  convert_n_utf8_to_64bit(&msg.byte_data[cur_idx], JM_BUS_UTF8_BYTE_LEN, &topic_len);
  if (!init_byte_array(&local.topic, topic_len)) {
    fprintf(stderr, "error initializing topic.\n");
    message_free(&local);
    return 0;
  }
  cur_idx += JM_BUS_UTF8_BYTE_LEN;
  if (msg.len <= cur_idx) {
    message_free(&local);
    return 0;
  }
  read_byte_array(&msg.byte_data[cur_idx], &local.topic);
  cur_idx += topic_len;
  // check to make sure there is a size value in the next few bytes
  if (msg.len <= (cur_idx + (JM_BUS_UTF8_BYTE_LEN - 1))) {
    message_free(&local);
    return 0;
  }
  size_t body_len = 0;
  convert_n_utf8_to_64bit(&msg.byte_data[cur_idx], JM_BUS_UTF8_BYTE_LEN, &body_len);
  if (!init_byte_array(&local.body, body_len)) {
    fprintf(stderr, "error initializing body.\n");
    message_free(&local);
    return 0;
  }
  cur_idx += JM_BUS_UTF8_BYTE_LEN;
  if (msg.len <= cur_idx && body_len != 0) {
    message_free(&local);
    return 0;
  }
  if (body_len != 0) {
    read_byte_array(&msg.byte_data[cur_idx], &local.body);
  }
  cur_idx += body_len;
  size_t total_len = (1 + (JM_BUS_UTF8_BYTE_LEN *2) + topic_len + body_len);
  if (cur_idx != total_len) {
    fprintf(stderr, "cur_idx != local.len -- %zu != %zu\n", cur_idx, total_len);
  }
  *out = local;
  return cur_idx;
}

size_t messages_write(const struct message *msg, byte_array *out) {
  // 1 + (size byte length * 2) + topic_len + body_len
  size_t msg_len = (1 + (JM_BUS_UTF8_BYTE_LEN * 2)) + msg->topic.len + msg->body.len;
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
  local.len = cur_idx;
  for (int i = 0; i < msg->topic.len; ++i) {
    if (!insert_byte_array(&local, msg->topic.byte_data[i])) {
      fprintf(stderr, "error with insert_byte_array for topic.\n");
      free_byte_array(&local);
      return 0;
    }
  }
  cur_idx += msg->topic.len;
  convert_64bit_to_n_utf8(msg->body.len, JM_BUS_UTF8_BYTE_LEN, &local.byte_data[cur_idx]);
  cur_idx += JM_BUS_UTF8_BYTE_LEN;
  local.len = cur_idx;
  for (int i = 0; i < msg->body.len; ++i) {
    if (!insert_byte_array(&local, msg->body.byte_data[i])) {
      fprintf(stderr, "error with insert_byte_array for body.\n");
      free_byte_array(&local);
      return 0;
    }
  }
  cur_idx += msg->body.len;
  if (cur_idx != local.len) {
    fprintf(stderr, "cur_idx != local.len -- %zu != %zu\n", cur_idx, local.len);
  }
  *out = local;
  return cur_idx;
}

void message_print(struct message *msg) {
  printf("Message Info\n");
  printf("Type: %d\n", msg->type);
  printf("Topic:\n");
  print_byte_array(msg->topic);
  printf("\n");
  printf("Body:\n");
  print_byte_array(msg->body);
  printf("\n");
}

size_t messages_gen_connection(byte_array *out) {
  struct message msg = {
    .type = CONNECTION,
  };
  int ret = messages_write(&msg, out);
  message_free(&msg);
  return ret;
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
  int ret = messages_write(&msg, out);
  message_free(&msg);
  return ret;
}

char * message_get_topic(const struct message *msg) {
  char *topic = malloc(sizeof(char)*msg->topic.len);
  memcpy(topic, msg->topic.byte_data, msg->topic.len);
  return topic;
}

message_array message_array_generate_from_client(int client_sock) {
  message_array result;
  if (!init_message_array(&result, 2)) {
    fprintf(stderr, "init message array from client failed.\n");
    return result;
  }
  byte_array byte_buffer;
  if (!read_all_raw_bytes(client_sock, &byte_buffer)) {
    fprintf(stderr, "message_array_generate_from_client error reading all bytes.\n");
    free_message_array(&result);
    return result;
  }
  size_t n = 0;
  while (n < byte_buffer.len) {
    struct message local_c;
    int read_n = messages_read(byte_buffer, &local_c);
    if (read_n == 0) {
      fprintf(stderr, "message_array_generate_from_client messages_read returned 0\n");
      n += byte_buffer.len;
      continue;
    }
    n += read_n;
    if (n < byte_buffer.len) {
      byte_buffer.byte_data = &byte_buffer.byte_data[read_n];
      byte_buffer.len -= read_n;
    }
    if (!insert_message_array(&result, local_c)) {
      fprintf(stderr, "failed to insert message from client.\n");
      free_byte_array(&byte_buffer);
      message_array_free_list(result);
      free_message_array(&result);
      return result;
    }
  }
  free_byte_array(&byte_buffer);
  return result;
}

void message_array_free_list(message_array msgs) {
  for (int i = 0; i<msgs.len; ++i) {
    struct message *tmp = &msgs.message_data[i];
    message_free(tmp);
  }
}

void message_free(struct message *msg) {
  if (msg->topic.byte_data != NULL) {
    free_byte_array(&msg->topic);
  }
  if (msg->body.byte_data != NULL) {
    free_byte_array(&msg->body);
  }
}
