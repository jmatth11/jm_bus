#include "socket.h"
#include "helpers/log.h"
#include "handlers/messages.h"

struct sockaddr_in create_default_addr() {
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(9901);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  return serv_addr;
}

struct sockaddr_in create_addr(sa_family_t __type, uint16_t port, int32_t addr) {
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = __type;
  serv_addr.sin_port = port;
  serv_addr.sin_addr.s_addr = addr;
  return serv_addr;
}

bool prepare_and_send_message(int socket, struct message *msg) {
  byte_array data;
  if (messages_write(msg, &data) == 0) {
    error_log("prepare_and_send_message generated zero bytes.\n");
    return false;
  }
  bool result = true;
  if (!send_message(socket, data)) {
    error_log("prepare_and_send_message send message failed.\n");
    exit(1);
    result = false;
  }
  byte_array_free(&data);
  return result;
}

bool send_message(int socket, byte_array buf) {
  if (send(socket, buf.byte_data, buf.len, 0) == -1) {
    error_log("failed to send message to socket %d\n", socket);
    return false;
  }
  return true;
}
