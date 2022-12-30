/**
 * \author Wentai Ye
 */

#include "sensor_db.h"

static char log_msg[SIZE]; // Message to be received from the child process

void *storagemgr()
{
    puts("[Storage manager] Storage manager Started!");
    FILE *csv = open_db("sensor_data.csv", false); // A new, empty data.csv should be created when the server starts up. It should not be deleted when the server stops.

    pthread_mutex_lock(&mutex_pipe);
    strcpy(log_msg, "A new data.csv file has been created.");
    write(fd[WRITE_END], log_msg, SIZE);
    pthread_mutex_unlock(&mutex_pipe);
    puts("[Storage manager] A new data.csv file has been created.");

    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    while (1)
    {
        int ret_read = sbuffer_read(sbuffer, data);
        if (ret_read == SBUFFER_SUCCESS)
        {
            insert_sensor(csv, data);

            pthread_mutex_lock(&mutex_pipe);
            sprintf(log_msg, "Data insertion from sensor %d succeeded.", data->id);
            write(fd[WRITE_END], log_msg, SIZE);
            pthread_mutex_unlock(&mutex_pipe);
            puts("[Storage manager] Data insertion from sensor succeeded.");
        }
        else if (ret_read == SBUFFER_NO_DATA)
        {
            puts("[Storage manager] No data to be read from the sbuffer.");
            pthread_cond_wait(&cond_signal, &mutex_sbuffer);
        }
        else
        {
            pthread_mutex_lock(&mutex_pipe);
            sprintf(log_msg, "Data insertion from sensor failed.");
            write(fd[WRITE_END], log_msg, SIZE);
            pthread_mutex_unlock(&mutex_pipe);
            break;
        }
    }
    close_db(csv); // Close sensor_data.csv
    free(data);    // Free the memory
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
    pthread_mutex_lock(&mutex_pipe);
    strcpy(log_msg, "The data.csv file has been closed.");
    write(fd[WRITE_END], log_msg, SIZE);
    pthread_mutex_unlock(&mutex_pipe);
    puts("[Storage manager] The data.csv file has been closed.");
    return 0;
}
