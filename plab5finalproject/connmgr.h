/**
 * \author Wentai Ye
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include "config.h"
#include "sbuffer.h"
#include "lib/tcpsock.h"

#ifndef CONNMGR_H_
#define CONNMGR_H_

#define MAX_CONN 3 // state the max. number of connections the server will handle before exiting

extern char log_msg[SIZE]; // Message to be received from the child process
extern int seq;            // Sequence number of the log file
extern int fd[2];          // File descriptor for the pipe
extern sbuffer_t *sbuffer;

/**
 * \brief This function is used to handle the connection with the client
 * \param port The port number
 */
void *connmgr(void *port);

/**
 * \brief This function is used to listen to the port
 * \param p The port number
 */
void *connmgr_listen(void *p);

#endif // CONNMGR_H_