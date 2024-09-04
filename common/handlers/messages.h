#ifndef JM_BUS_MESSAGES_HANLDER_H
#define JM_BUS_MESSAGES_HANLDER_H

#include <stdbool.h>
#include "types/array_types.h"
#include "types/message.h"
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * Parse a byte_array message into message structure.
 *
 * @param[in] msg The raw message.
 * @param[out] out The message structure to populate.
 * @return The number of bytes read, 0 for error.
 */
size_t messages_read(const byte_array msg, struct message *out) __THROWNL;

/**
 * Write a message structure out to a byte array.
 *
 * @param[in] msg The message structure.
 * @param[out] out The raw message.
 * @return The number of bytes written, 0 for error.
 */
size_t messages_write(const struct message *msg, byte_array *out) __THROWNL __nonnull((1));

/**
 * Generate a connection message and write it out to a byte_array.
 *
 * @param[out] out The raw message.
 * @return The number of bytes written, 0 for errors.
 */
size_t messages_gen_connection(byte_array *out) __THROWNL;

/**
 * Generate an error message and write it out to a byte_array.
 *
 * @param[in] err The error message.
 * @param[out] out The raw message.
 * @return The number of bytes written, 0 for errors.
 */
size_t messages_gen_error(char *err, byte_array *out) __THROWNL;

/**
 * Free message internals.
 *
 * @param[out] msg The message.
 */
void message_free(struct message *msg) __THROWNL __nonnull((1));

/**
 * Print the message out. Used for debugging purposes mostly.
 *
 * @param[in] msg The message.
 */
void message_print(struct message *msg) __THROWNL __nonnull((1));

/**
 * Get the topic name from a given message.
 * The caller is responsible for freeing the returned string.
 *
 * @param[in] msg The message.
 * @return The topic name, NULL for error.
 */
char * message_get_topic(const struct message *msg) __THROWNL __nonnull((1));

/**
 * Generate an array of parsed messages from the data received on a given client socket.
 *
 * @param[in] client_sock The client's socket.
 * @return A populated message_array, an empty message_array on error.
 */
message_array message_array_generate_from_client(int client_sock) __THROWNL;

/**
 * Free message_array internals.
 *
 * @param[in] msgs The message array.
 */
void message_array_free_list(message_array msgs) __THROWNL;

__END_DECLS

#endif
