#ifndef JM_BUS_MESSAGES_HANLDER_H
#define JM_BUS_MESSAGES_HANLDER_H

#include <stdbool.h>
#include "types/array_types.h"
#include "types/message.h"
#include <stddef.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

size_t messages_read(const byte_array msg, struct message *out) __THROWNL;
size_t messages_write(const struct message *msg, byte_array *out) __THROWNL __nonnull((1));
size_t messages_gen_connection(byte_array *out) __THROWNL;
size_t messages_gen_error(char *err, byte_array topic, byte_array *out) __THROWNL;
void message_free(struct message *msg) __THROWNL __nonnull((1));
void message_print(struct message *msg) __THROWNL __nonnull((1));

char * message_get_topic(const struct message *msg) __THROWNL __nonnull((1));

message_array message_array_generate_from_client(int client_sock) __THROWNL;
void message_array_free_list(message_array msgs) __THROWNL;

__END_DECLS

#endif
