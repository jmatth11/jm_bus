#ifndef JM_BUS_STATE_H
#define JM_BUS_STATE_H

#include "types/client.h"
#include "types/server.h"
#include <bits/pthreadtypes.h>
#include <stdatomic.h>
#include <sys/select.h>

struct server_state {
  struct client_list clients;
  struct server_info server;
  fd_set readfs;
  pthread_t listen_thread;
  pthread_t process_thread;
  atomic_int action;
  atomic_bool running;
  atomic_bool accepting;
};

#endif
