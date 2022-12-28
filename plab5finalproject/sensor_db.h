/**
 * \author Wentai Ye
 */

#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "config.h"
#include "sbuffer.h"

extern sbuffer_t *sbuffer;
extern int fd[2]; // pipe file descriptor
extern pthread_mutex_t mutex_pipe;

/**
 * The storage manager thread reads sensor measurements from the shared data buffer
 * and inserts them into a csv-file “data.csv”
 */
void *storagemgr();

/*
open a text file with a given name, and select open mode.
param filename: the name of the file to open
param append: if true, the data should be appended to the existing file, if false, the file should be overwritten
*/
FILE *open_db(char *filename, bool append);

/*
append a single sensor reading to the csv file.
param f: the file pointer to the csv file
param data: the sensor reading to append
*/
void insert_sensor(FILE *f, sensor_data_t *data);

/*
close the csv file.
param f: the file pointer to the csv file
*/
int close_db(FILE *f);

#endif /* _SENSOR_DB_H_ */
