#ifndef JM_BUS_SERVER_HANDLER_H
#define JM_BUS_SERVER_HANDLER_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

// forward declarations.
struct server_info;

/**
 * Initialize the server info with the given type and protocol.
 *
 * @param[out] si The server info.
 * @param[in] __type The socket type. see enum __socket_type for options.
 * @param[in] __protocol The socket protocol. see Protocol families in socket.h.
 * @return True for success, false otherwise.
 */
bool server_handler_init(struct server_info *si, int __type, int __protocol) __THROWNL __nonnull((1));

/**
 * Bind to and listen to socket initialized in the server_info object.
 *
 * @param[in] si The server_info.
 * @return True for success, false otherwise.
 */
bool server_handler_bind_and_listen(struct server_info *si) __THROWNL __nonnull((1));

/**
 * Close and shutdown the server connection.
 *
 * @param[in] si The server_info.
 */
void server_handler_close(struct server_info *si) __THROWNL __nonnull((1));

__END_DECLS

#endif
