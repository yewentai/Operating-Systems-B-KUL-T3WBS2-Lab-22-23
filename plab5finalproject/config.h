/**
 * \author Wentai Ye
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>
#include <stdbool.h>

#define READ_END 0  // The read end of the pipe
#define WRITE_END 1 // The write end of the pipe
#define SIZE 1024   // The size of the pipe buffer

typedef uint16_t sensor_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t; // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine
typedef uint16_t sensor_room_t;

typedef struct
{
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
} sensor_data_t;

typedef struct
{
    sensor_id_t sensor_id;
    sensor_value_t value;
    sensor_value_t avg;
    sensor_ts_t ts;
    bool valid;
} my_element_t;

#endif /* _CONFIG_H_ */
