#ifndef JM_BUS_SOCKEY_HELPERS_H
#define JM_BUS_SOCKEY_HELPERS_H

#include "types/array_types.h"
#include "types/message.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * Create sockaddr_in with default values.
 * (AF_INET, 9901, INADDR_ANY)
 *
 * @return The sockaddr_in.
 */
struct sockaddr_in create_default_addr() __THROWNL;

/**
 * Create a sockaddr_in with the given type, port, and address.
 *
 * @param[in] __type The address type. see Address families in socket.h
 * @param[in] port The port number.
 * @param[in] addr The IP address. In hex format.
 * @return The sockaddr_in.
 */
struct sockaddr_in create_addr(sa_family_t __type, uint16_t port, int32_t addr) __THROWNL;

/**
 * Convenience function to prepare and send a given message to a given socket.
 *
 * @param[in] socket The target socket.
 * @param[in] msg The message to send.
 * @return True on success, false otherwise.
 */
bool prepare_and_send_message(int socket, struct message *msg) __THROWNL __nonnull((2));

/**
 * Send a message to a given socket.
 *
 * @param[in] socket The target socket.
 * @param[in] buf The raw message to send.
 * @return True on success, false otherwise.
 */
bool send_message(int socket, byte_array buf) __THROWNL;

__END_DECLS

#endif
