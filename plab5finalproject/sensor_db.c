/**
 * \author Wentai Ye
 */

#include "sensor_db.h"

static char log_msg[SIZE]; // Message to be received from the child process

void *storagemgr()
{
    puts("Storage manager is up and running.");
    FILE *csv = open_db("sensor_data.csv", false); // A new, empty data.csv should be created when the server starts up. It should not be deleted when the server stops.

    strcpy(log_msg, "A new data.csv file has been created.");
    write(fd[WRITE_END], log_msg, SIZE);
    puts("A new data.csv file has been created.");

    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    if (sbuffer_get_head(sbuffer, data) == SBUFFER_SUCCESS)
    {
        insert_sensor(csv, data);
        sprintf(log_msg, "Data insertion from sensor %d succeeded.", data->id);
        write(fd[WRITE_END], log_msg, SIZE);
    }
    else
    {
        sprintf(log_msg, "Data insertion from sensor failed.");
        write(fd[WRITE_END], log_msg, SIZE);
    }
    while (sbuffer_get_head(sbuffer, data) == SBUFFER_SUCCESS)
    {
        insert_sensor(csv, data);
    }

    close_db(csv); // Close sensor_data.csv

    free(data);
    return NULL;
}

FILE *open_db(char *filename, bool append)
{
    FILE *fp;
    if (append)
        fp = fopen(filename, "a");
    else
        fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }
    return fp;
}

void insert_sensor(FILE *csv, sensor_data_t *data)
{
    if (csv == NULL)
    {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    char time_buffer[128];
    strftime(time_buffer, sizeof(time_buffer), "%d/%m/%Y %H:%M:%S", localtime(&data->ts));
    fprintf(csv, "%s,%" PRIu16 ",%.1lf\n", time_buffer, data->id, data->value);
}

int close_db(FILE *csv)
{
    if (csv == NULL)
    {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }
    fclose(csv);
    strcpy(log_msg, "The data.csv file has been closed.");
    write(fd[WRITE_END], log_msg, SIZE);
    return 0;
}
