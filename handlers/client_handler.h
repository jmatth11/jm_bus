#ifndef JM_BUS_CLIENT_HANDLER_H
#define JM_BUS_CLIENT_HANDLER_H

#include "types/client.h"

#include <stdbool.h>
#include <sys/cdefs.h>

struct client_info client_from_socket(int client_socket);

bool client_list_init(struct client_list *cl) __THROWNL __nonnull((1));
bool client_list_add(struct client_list *cl, int client_socket) __THROWNL __nonnull ((1));
bool client_list_get(struct client_list *cl, int client_socket, struct client_info *ci) __THROWNL __nonnull ((1));
bool client_list_get_by_idx(struct client_list *cl, int idx, struct client_info *ci);
void client_list_free(struct client_list *cl) __THROWNL __nonnull ((1));

#endif
