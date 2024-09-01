#ifndef JM_BUS_SERVER_HANDLER_H
#define JM_BUS_SERVER_HANDLER_H

#include <netinet/in.h>
#include <stdbool.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

struct server_info;

bool server_handler_init(struct server_info *si, int __type, int __protocol) __THROWNL __nonnull((1));
struct sockaddr_in server_handler_default_addr() __THROWNL;
bool server_handler_bind_and_listen(struct server_info *si) __THROWNL __nonnull((1));
void server_handler_close(struct server_info *si) __THROWNL __nonnull((1));

__END_DECLS

#endif
