#ifndef JM_BUS_MESSAGE_H
#define JM_BUS_MESSAGE_H

#include "types/array_types.h"

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * Message type enums.
  */
enum message_type {
  /**
   * An error occurred.
   * If on the server a message is sent to the client and nothing else changes.
   * If from the client the server disconnects the client.
   */
  ERROR = 0,
  /* Used for the server to tell the client it successfully disconnected.*/
  CONNECTION = 1,
  /* Used for a client to subscribe to a topic on the server. */
  SUBSCRIBE = 2,
  /* Used for a client to publish to a topic on the server. */
  PUBLISH = 3,
};

/**
 * The base message type.
 */
struct message {
  /* the topic name */
  byte_array topic;
  /* the raw message */
  byte_array body;
  /* the type */
  enum message_type type;
};

/**
 * Array type of messages.
 */
generate_array_template(message, struct message)

// forward declare.
struct server_state;

/**
 * Message event type to send between threads on the server.
 */
struct message_event {
  /* the current server state. */
  struct server_state* state;
  /* the from client's socket */
  int from;
  /* the array of messages to send */
  message_array msgs;
};

__END_DECLS

#endif
