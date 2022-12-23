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
#include "config.h"
#include "sbuffer.h"
#include "lib/tcpsock.h"

#ifndef CONNMGR_H_
#define CONNMGR_H_

#ifndef TIMEOUT
#error TIMEOUT not set
#endif

#define MAX_CONN 3 // state the max. number of connections the server will handle before exiting

extern int num_conn; // Number of connections
extern int fd[2];    // File descriptor for the pipe
extern sbuffer_t *sbuffer;

/**
 * The connection manager listens on a TCP socket for incoming connection requests
from new sensor nodes.
 * \param port The port number
 */
void *connmgr(void *port);

/**
 * \brief This function is used to listen to the port
 * \param p The port number
 */
void *connmgr_listen(void *port_void);

#endif // CONNMGR_H_