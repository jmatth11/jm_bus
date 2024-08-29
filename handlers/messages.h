#ifndef JM_BUS_MESSAGES_HANLDER_H
#define JM_BUS_MESSAGES_HANLDER_H

#include <stdbool.h>
#include "types/array_types.h"
#include "types/message.h"
#include <sys/cdefs.h>

__BEGIN_DECLS

size_t messages_read(const byte_array msg, struct message *out) __THROWNL;
size_t messages_write(const struct message *msg, byte_array *out) __THROWNL __nonnull((1));

message_array message_array_generate_from_client(int client_sock) __THROWNL;

__END_DECLS

#endif
