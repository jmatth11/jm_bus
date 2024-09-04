#ifndef JM_BUS_MESSAGES_HANDLER_H
#define JM_BUS_MESSAGES_HANDLER_H

#include <sys/cdefs.h>

__BEGIN_DECLS

// forward declarations.
struct server_state;

/**
 * Process new connections.
 * Sends back a connection message to the connecting client if the client
 * connects successfully.
 *
 * @param[in] state The server state.
 */
void accept_messages(struct server_state *state) __THROWNL __nonnull((1));

/**
 * Process the poll events from the file descriptors.
 * Handles checking for new clients, removing clients that have disconnected, and
 * spawning/reusing threads to handle incoming messages.
 *
 * @param[in] state The server state.
  */
void process_poll_events(struct server_state *state) __THROWNL __nonnull((1));

/**
 * Process messages on a thread.
 * This function is meant to be called on a new thread and handles executing
 * the events from the received messages.
 *
 * @param[in] ctx A thread_job structure.
 * @return The thread_job on success, NULL otherwise.
  */
void* process_messages(void *ctx) __THROWNL __nonnull((1));

__END_DECLS

#endif
