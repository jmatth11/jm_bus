#ifndef JM_BUS_STATE_HANDLER_H
#define JM_BUS_STATE_HANDLER_H

#include "types/array_types.h"

#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct server_state;

bool server_state_init(struct server_state *) __THROWNL __nonnull((1));
bool server_state_add_client(struct server_state *, int client_sock) __THROWNL __nonnull((1));
bool server_state_remove_client(struct server_state *, int client_sock) __THROWNL __nonnull((1));
bool server_state_remove_clients(struct server_state *, int_array client_idxs) __THROWNL __nonnull((1));
bool server_state_free(struct server_state *) __THROWNL __nonnull((1));

__END_DECLS

#endif
