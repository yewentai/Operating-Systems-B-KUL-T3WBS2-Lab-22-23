/**
 * \author Wentai Ye
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "config.h"
#include "lib/dplist.h"
#include "sbuffer.h"

#ifndef DATAMGR_H_
#define DATAMGR_H_

#ifndef SET_MAX_TEMP
#error SET_MAX_TEMP not set
#endif

#ifndef SET_MIN_TEMP
#error SET_MIN_TEMP not set
#endif

#define RUN_AVG_LENGTH 5

static dplist_t *list = NULL; // Pointer to the list
extern sbuffer_t *sbuffer;    // Pointer to the shared buffer
extern char rmsg[SIZE];       // Message to be received from the child process

/*
 * Use ERROR_HANDLER() for handling memory allocation problems, invalid sensor IDs, non-existing files, etc.
 */
#define ERROR_HANDLER(condition, ...)                                                                     \
  do                                                                                                      \
  {                                                                                                       \
    if (condition)                                                                                        \
    {                                                                                                     \
      printf("\nError: in %s - function %s at line %d: %s\n", __FILE__, __func__, __LINE__, __VA_ARGS__); \
      exit(EXIT_FAILURE);                                                                                 \
    }                                                                                                     \
  } while (0)

/**
 * The data manager thread implements the server intelligence as defined in plab1.
 * The room-sensor mapping is read from a text file “room_sensor.map”.
 * The data manager reads sensor measurements from the shared in-memory data buffer.
 */
void *datamgr();

/**
 * This method should be called to initialize the datamgr, and to read the room-sensor mapping from the file room_sensor.map
 * Use ERROR_HANDLER() if the file cannot be opened
 * \return a pointer to the list
 */
int **get_map_array();

/**
 * This method should be called to clean up the datamgr, and to free all used memory.
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free();

/**
 * Gets the room ID for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the corresponding room id
 */
uint16_t datamgr_get_room_id(sensor_id_t sensor_id, int **map, int m);

/**
 * Gets the running AVG of a certain senor ID (if less then RUN_AVG_LENGTH measurements are recorded the avg is 0)
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the running AVG of the given sensor
 */
sensor_value_t datamgr_get_avg(sensor_id_t sensor_id);

/**
 * Returns the time of the last reading for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the last modified timestamp for the given sensor
 */
time_t datamgr_get_last_modified(sensor_id_t sensor_id);

/**
 *  Return the total amount of unique sensor ID's recorded by the datamgr
 *  \return the total amount of sensors
 */
int datamgr_get_total_sensors();

static void *element_copy(void *element);

static void element_free(void **element);

static int element_compare(void *x, void *y);

#endif // DATAMGR_H_
