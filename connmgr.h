#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <pthread.h>
#include "sbuffer.h"

#define PORT 5678
//#define MAX_CONN 10  // state the max. number of connections the server will handle before exiting
//int conn_counter = 0;
//sbuffer_t sbuffer;

/**
 * Implements a sequential test server (only one connection at the same time)
 */

void * pthread_receive(void *p);
