/**
 * \author Luc Vandeurzen
 */

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "config.h"

#define NUM_MEASUREMENTS 100
#define SLEEP_TIME 30 // every SLEEP_TIME seconds, sensors wake up and measure temperature
#define NUM_SENSORS 8 // also defines number of rooms (currently 1 room = 1 sensor)
#define TEMP_DEV 5    // max deviation from previous temperature in 0.1 centigrade
#include "file_creator.h"

uint16_t sensor_id[NUM_SENSORS] = {15, 21, 37, 49, 112, 129, 132, 142};
double sensor_temperature[NUM_SENSORS] = {15, 17, 18, 19, 20, 23, 24, 25}; // starting temperatures

void srccreate(void)
{
    FILE *fp_text, *fp_bin;
    int i, j;
    time_t starttime = time(&starttime);
    srand48(time(NULL));

    // generate binary file sensor_data and corresponding log file
    fp_bin = fopen("sensor_data", "wb");
    if (fp_bin == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    for (i = 0; i < NUM_MEASUREMENTS; i++, starttime += SLEEP_TIME)
    {
        for (j = 0; j < NUM_SENSORS; j++)
        {
            // write current temperatures to file
            fwrite(sensor_id + j, sizeof(sensor_id[0]), 1, fp_bin);
            fwrite(&(sensor_temperature[j]), sizeof(sensor_temperature[0]), 1, fp_bin);
            fwrite(&starttime, sizeof(time_t), 1, fp_bin);

            // get new temperature: still needs some fine-tuning ...
            sensor_temperature[j] = sensor_temperature[j] + TEMP_DEV * ((drand48() - 0.5) / 10);
        }
    }

    fclose(fp_bin);
}

void dstcreate(void)
{
    FILE *fp_text, *fp_bin;

    // generate binary file sensor_data and corresponding log file
    fp_bin = fopen("sensor_data_out", "wb");
    if (fp_bin == NULL)
    {
        perror("fopen()");
        exit(1);
    }
}
