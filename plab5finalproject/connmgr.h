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

extern sbuffer_t *sbuffer;

void connmgr(int port);

void *connmgr_listen(void *p);

#endif // CONNMGR_H_