#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, const char **argv) {
  int serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(9901);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if (connect(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0) {
    fprintf(stderr, "connect failed.\n");
    exit(1);
  }
  sleep(1);
  char init[50];
  int n = recv(serv_sock, init, sizeof(init), 0);
  if (n == 0) {
    fprintf(stderr, "recv nothing.\n");
    exit(1);
  }
  fprintf(stdout, "%s", init);

  char msg[] = "Hello, Server!\n";
  send(serv_sock, msg, sizeof(msg), 0);
  sleep(1);
  n = recv(serv_sock, msg, sizeof(msg), 0);
  if (n == 0) {
    fprintf(stderr, "recv nothing.\n");
    exit(1);
  }
  fprintf(stdout, "%s", msg);
  return 0;
}
