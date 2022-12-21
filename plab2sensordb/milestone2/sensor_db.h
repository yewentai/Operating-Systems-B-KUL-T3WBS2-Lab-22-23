/**
 * \author Wentai Ye
 */

#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <stdbool.h>

#define READ_END 0 
#define WRITE_END 1
extern int fd[2];   // pipe file descriptor
#define SIZE 50

/*
open a text file with a given name, and select open mode.
param filename: the name of the file to open
param append: if true, the data should be appended to the existing file, if false, the file should be overwritten
*/
FILE* open_db(char* filename, bool append);

/*
append a single sensor reading to the csv file.
param f: the file pointer to the csv file
param id: the sensor id
param value: the sensor value
param ts: the sensor timestamp
*/
int insert_sensor(FILE* f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts);

/*
close the csv file.
param f: the file pointer to the csv file
*/
int close_db(FILE* f);

#endif /* _SENSOR_DB_H_ */

