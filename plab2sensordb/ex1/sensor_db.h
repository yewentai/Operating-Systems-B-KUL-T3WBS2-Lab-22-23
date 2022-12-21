#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <stdbool.h>

/*
an operation to open a text file with a given name, and providing an indication if the file should be overwritten if the file already exists or if the data should be appended to the existing file.
*/
FILE * open_db(char * filename, bool append);

/*
an operation to append a single sensor reading to the csv file
*/
int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts);

/*
an operation to close the csv file.
*/
int close_db(FILE * f);



