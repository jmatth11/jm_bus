#include "messages.h"
#include <stdio.h>
#include <string.h>
#include "helpers/bytes.h"

bool messages_read(const byte_array msg, struct message *out) {
  if (msg.len <= 0) return false;
  size_t cur_idx = 0;
  out->type = msg.byte_data[cur_idx];
  ++cur_idx;
  size_t topic_len = 0;
  convert_utf8_to_64bit(&msg.byte_data[cur_idx], &topic_len);
  if (!init_byte_array(&out->topic, topic_len)) {
    fprintf(stderr, "error initializing topic.\n");
    return false;
  }
  cur_idx += 5;
  read_byte_array(&msg.byte_data[cur_idx], &out->topic);
  cur_idx += topic_len;
  size_t body_len = 0;
  convert_utf8_to_64bit(&msg.byte_data[cur_idx], &body_len);
  if (!init_byte_array(&out->body, body_len)) {
    fprintf(stderr, "error initializing body.\n");
    return false;
  }
  cur_idx += 5;
  read_byte_array(&msg.byte_data[cur_idx], &out->body);
  return true;
}

bool messages_write(const struct message *msg, byte_array *out) {
  size_t msg_len = 11 + msg->topic.len + msg->body.len;
  byte_array local;
  if (!init_byte_array(&local, msg_len)) {
    fprintf(stderr, "error initializing message.\n");
    return false;
  }
  size_t cur_idx = 0;
  if (!insert_byte_array(&local, msg->type)) {
    fprintf(stderr, "error inserting type byte.\n");
    return false;
  }
  ++cur_idx;
  convert_64bit_to_utf8(msg->topic.len, &local.byte_data[cur_idx]);
  cur_idx += 5;
  if (memcpy(&local.byte_data[cur_idx], msg->topic.byte_data, msg->topic.len) != NULL) {
    fprintf(stderr, "error with memcpy for topic.\n");
    return false;
  }
  cur_idx += msg->topic.len;
  convert_64bit_to_utf8(msg->body.len, &local.byte_data[cur_idx]);
  cur_idx += 5;
  if (memcpy(&local.byte_data[cur_idx], msg->body.byte_data, msg->body.len) != NULL) {
    fprintf(stderr, "error with memcpy for body.\n");
    return false;
  }
  return true;
}
