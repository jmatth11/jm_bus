#ifndef JM_BUS_STATE_HANDLER_H
#define JM_BUS_STATE_HANDLER_H

#include "types/array_types.h"

#include <netinet/in.h>
#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct server_state;

/**
 * Initialize server state object and all internal structures.
 *
 * @param[out] s The server state structure to populate.
 * @param[in] addr Optional param to set the server's address.
 *   Set to NULL to use default IPv4 0.0.0.0:9901.
 * @return True if successful, False otherwise.
 */
bool server_state_init(
  struct server_state *s,
  struct sockaddr_in *addr
) __THROWNL __nonnull((1));

/**
 * Add or set topic for a client to subscribe to.
 *
 * @param[in] s The server state.
 * @param[in] topic The topic's name.
 * @param[in] client_sock The client's socket fd.
 * @return True if successful, False otherwise.
 */
bool server_state_add_client_topic(
  struct server_state *s,
  const char *topic,
  int client_sock
) __THROWNL __nonnull((1,2));

/**
 * Add a newly connected client to server client list.
 *
 * @param[out] s The server state.
 * @param[in] client_sock The client's socket fd.
 * @return True if successful, False otherwise.
 */
bool server_state_add_client(
  struct server_state *s,
  int client_sock
) __THROWNL __nonnull((1));

/**
 * Remove client to server client list.
 *
 * @param[out] s The server state.
 * @param[in] client_sock The client's socket fd.
 * @return True if successful, False otherwise.
 */
bool server_state_remove_client(
  struct server_state *s,
  int client_sock
) __THROWNL __nonnull((1));

/**
 * Remove a list of clients to server client list.
 *
 * @param[out] s The server state.
 * @param[in] client_idxs The indexes of the clients to remove from the client list.
 * @return True if successful, False otherwise.
 */
bool server_state_remove_clients(
  struct server_state *s,
  int_array client_idxs
) __THROWNL __nonnull((1));

/**
 * Free the server state and it's internal structures.
 *
 * @param[out] s The server state.
 * @return True if successful, False otherwise.
 */
bool server_state_free(
  struct server_state *s
) __THROWNL __nonnull((1));

__END_DECLS

#endif
