/**
 * \author Wentai Ye
 */

#ifndef CONNMGR_H_
#define CONNMGR_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include "config.h"
#include "sbuffer.h"
#include "lib/tcpsock.h"

extern char rmsg[SIZE]; // Message to be received from the child process
extern int seq;         // Sequence number of the log file
extern int fd[2];       // File descriptor for the pipe

extern sbuffer_t *sbuffer;

void *connmgr(void *port);

void *connmgr_listen(void *p);

#endif // CONNMGR_H_