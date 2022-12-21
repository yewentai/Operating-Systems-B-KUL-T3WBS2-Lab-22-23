#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "config.h"

#define SET_MAX_TEMP 40
#define SET_MIN_TEMP -10
#define RUN_AVG_LENGTH 5

/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them.
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */
void datamgr_parse_sensor_files(FILE* fp_sensor_map, FILE* fp_sensor_data) {
    