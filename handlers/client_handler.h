#ifndef JM_BUS_CLIENT_HANDLER_H
#define JM_BUS_CLIENT_HANDLER_H

#include "types/client.h"

#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

bool client_list_init(struct client_list *cl) __THROWNL __nonnull((1));
bool client_list_add(struct client_list *cl, int client_socket) __THROWNL __nonnull ((1));
bool client_list_remove(struct client_list *cl, int client_socket) __THROWNL __nonnull ((1));
bool client_list_remove_by_idx(struct client_list *cl, int idx) __THROWNL __nonnull ((1));
bool client_list_get(struct client_list *cl, int client_socket, struct pollfd *ci) __THROWNL __nonnull ((1));
bool client_list_get_by_idx(struct client_list *cl, int idx, struct pollfd *ci);
void client_list_free(struct client_list *cl) __THROWNL __nonnull ((1));

__END_DECLS

#endif
