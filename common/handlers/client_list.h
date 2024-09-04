#ifndef JM_BUS_CLIENT_LIST_HANDLER_H
#define JM_BUS_CLIENT_LIST_HANDLER_H

#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

// forward declarations
struct client_list;
struct client_metadata;
struct pollfd;

/**
 * Initialize a client list.
 *
 * @param[out] cl The client list.
 * @return True if successful, false otherwise.
 */
bool client_list_init(struct client_list *cl) __THROWNL __nonnull((1));

/**
 * Add a new client to the client list given its socket.
 *
 * @param[in] cl The client list.
 * @param[in] client_socket The client's socket.
 * @return True for success, false otherwise.
 */
bool client_list_add(struct client_list *cl, int client_socket) __THROWNL __nonnull ((1));

/**
 * Remove a client from the client list given its socket.
 *
 * @param[in] cl The client list.
 * @param[in] client_socket The client's socket.
 * @return True for success, false otherwise.
  */
bool client_list_remove(struct client_list *cl, int client_socket) __THROWNL __nonnull ((1));

/**
 * Get the pollfd of a given client socket.
 *
 * @param[in] cl The client list.
 * @param[in] client_socket The client's socket.
 * @param[out] fd The poll file descriptor to populate.
 * @return True for success, False otherwise.
 */
bool client_list_get(struct client_list *cl, int client_socket, struct pollfd *fd) __THROWNL __nonnull ((1));

/**
 * Get the pollfd of a given an index.
 *
 * @param[in] cl The client list.
 * @param[in] idx The index value.
 * @param[out] fd The poll file descriptor to populate.
 * @return True for success, False otherwise.
 */
bool client_list_get_by_idx(struct client_list *cl, int idx, struct pollfd *fd);

/**
 * Get the index value of the client in the list given the client's socket.
 *
 * @param[in] cl The client list.
 * @param[in] client_socket The client's socket.
 * @return The index of the client or -1 if not found.
  */
int client_list_get_idx(struct client_list *cl, int client_socket) __THROWNL __nonnull((1));

/**
 * Close all client connections.
 *
 * @param[in] cl The client list.
 * @return True for success, false otherwise.
 */
bool client_list_close_connections(struct client_list *cl) __THROWNL __nonnull((1));

/**
 * Free internal client list data.
 * This function does not close connections, you must call
 * client_list_close_connections beforehand.
 *
 * @param[out] cl The client list.
 */
void client_list_free(struct client_list *cl) __THROWNL __nonnull ((1));

/**
 * Initialize client metadata.
 *
 * @param[out] metadata The client metadata.
 * @return True for success, False otherwise.
 */
bool client_metadata_init(struct client_metadata *metadata) __THROWNL __nonnull((1));

/**
 * Free client metadata.
 *
 * @param[out] metadata The client metadata.
 */
void client_metadata_free(struct client_metadata *metadata) __THROWNL __nonnull((1));

__END_DECLS

#endif
