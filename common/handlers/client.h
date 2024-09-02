#ifndef JM_BUS_CLIENT_CONNECTION_HANDLER_H
#define JM_BUS_CLIENT_CONNECTION_HANDLER_H

#include "types/array_types.h"
#include "types/message.h"
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct client;
struct message;

bool client_init(struct client *c, struct sockaddr_in *addr) __THROWNL __nonnull((1));
bool client_connect(struct client *c) __THROWNL __nonnull((1));
bool client_subscribe(struct client *c, char *topic) __THROWNL __nonnull((1));
bool client_publish(struct client *c, char *topic, byte_array msg) __THROWNL __nonnull((1,2));
message_array client_read_messages(struct client *c) __THROWNL __nonnull((1));
bool client_free(struct client *c) __THROWNL __nonnull((1));

__END_DECLS

#endif
