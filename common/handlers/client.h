#ifndef JM_BUS_CLIENT_CONNECTION_HANDLER_H
#define JM_BUS_CLIENT_CONNECTION_HANDLER_H

#include "types/array_types.h"
#include "types/message.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

// forward declarations
struct client;
struct message;

/**
 * Initialize a client with a given address.
 *
 * @param[out] c The client.
 * @param[in] addr The address to use. Pass NULL for default values. (AF_INET, 9901, INADDR_ANY)
 * @return True if successful, False otherwise.
 */
bool client_init(struct client *c, struct sockaddr_in *addr) __THROWNL __nonnull((1));

/**
 * Connect to the server with the info in addr.
 *
 * @param[in] c The client.
 * @return True for successful, False otherwise.
 */
bool client_connect(struct client *c) __THROWNL __nonnull((1));

/**
 * Subscribe to a given topic.
 *
 * @param[in] c The client.
 * @param[in] topic The topic's name.
 * @return True if successful, False otherwise.
 */
bool client_subscribe(struct client *c, char *topic) __THROWNL __nonnull((1));

/**
 * Publish a message to a topic.
 *
 * @param[in] c The client.
 * @param[in] topic The topic's name.
 * @param[in] msg The message to send.
 * @return True if successful, False otherwise.
 */
bool client_publish(struct client *c, char *topic, byte_array msg) __THROWNL __nonnull((1,2));

/**
 * Read messages sent to the client.
 * This call will block until data is received.
 *
 * @param[in] c The client.
 * @return All messages from the received data.
 */
message_array client_read_messages(struct client *c) __THROWNL __nonnull((1));

/**
 * Free the client and close the connection.
 *
 * @param[in] c The client.
 * @return True if successful, False otherwise.
 */
bool client_free(struct client *c) __THROWNL __nonnull((1));

__END_DECLS

#endif
