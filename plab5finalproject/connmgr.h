/**
 * \author Wentai Ye
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "config.h"
#include "sbuffer.h"
#include "lib/tcpsock.h"

#ifndef CONNMGR_H_
#define CONNMGR_H_

#ifndef TIMEOUT
#error TIMEOUT not set
#endif

#ifndef MAX_CONN
#define MAX_CONN 3 // state the max number of connections the server will handle
#endif

extern int num_conn;               // Number of connections
extern int fd[2];                  // File descriptor for the pipe
extern sbuffer_t *sbuffer;         // Shared buffer
extern pthread_mutex_t mutex_pipe; // Mutex for the log file
extern bool quit;                  // Flag to quit

/**
 * The connection manager listens on a TCP socket for incoming connection requests
from new sensor nodes.
 * \param port The port number
 */
void *connmgr(void *port);

/**
 * \brief This is a thread created by datamgr and used to listen to the port,
 * each client will correspond to a thread.
 * \param p_client The pointer to client socket
 */
void *connmgr_listen(void *p_client);

#endif // CONNMGR_H_