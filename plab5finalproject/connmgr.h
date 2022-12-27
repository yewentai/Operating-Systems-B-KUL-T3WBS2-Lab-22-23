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

#define MAX_CONN 99 // state the max number of connections the server will handle

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
 * \brief This is a thread created by datamgr and used to listen to the port,
 * each client will correspond to a thread.
 * \param p_client The pointer to client socket
 */
void *thread_listen(void *p_client);

/**
 * \brief This is a fuction used to receive and read data from the client.
 * \param client The pointer to client socket
 * \param data The pointer to the data structure
 * \return The result of the tcp_receive function
 */
int receive_one_data(tcpsock_t *client, sensor_data_t *data);
#endif // CONNMGR_H_