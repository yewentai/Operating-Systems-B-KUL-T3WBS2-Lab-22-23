/**
 * \author Wentai Ye
 */

#include "sensor_db.h"

static char tmsg[SIZE]; // Message to be received from the child process

void *storagemgr()
{
    puts("[Storage manager] Storage manager Started!");

    FILE *fp = fopen("sensor_data.csv", "w");
    if (fp == NULL)
    {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    pthread_mutex_lock(&mutex_pipe);
    strcpy(tmsg, "A new data.csv file has been created.");
    write(fd[WRITE_END], tmsg, SIZE);
    pthread_mutex_unlock(&mutex_pipe);
    puts("[Storage manager] A new data.csv file has been created.");

    FILE *csv = fopen("sensor_data.csv", "a"); // Open the file in append mode
    if (csv == NULL)
    {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    while (1)
    {
        int ret_remove = sbuffer_remove(sbuffer, data);
        if (ret_remove == SBUFFER_SUCCESS)
        {
            insert_sensor(csv, data);
            pthread_mutex_lock(&mutex_pipe);
            sprintf(tmsg, "Data insertion from sensor %d succeeded.", data->id);
            write(fd[WRITE_END], tmsg, SIZE);
            pthread_mutex_unlock(&mutex_pipe);
            puts("[Storage manager] Data insertion from sensor succeeded.");
        }
        else if (ret_remove == SBUFFER_NO_DATA)
        {
            // pthread_cond_wait(&cond_signal_tail, &mutex_sbuffer_head);
            // sleep(1);
        }
        else
        {
            pthread_mutex_lock(&mutex_pipe);
            sprintf(tmsg, "Data insertion from sensor failed.");
            write(fd[WRITE_END], tmsg, SIZE);
            pthread_mutex_unlock(&mutex_pipe);
            break;
        }
        if (quit == true)
            break;
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
    fprintf(csv, "%ld,%" PRIu16 ",%.1lf\n", data->ts, data->id, data->value);
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
    strcpy(tmsg, "The data.csv file has been closed.");
    write(fd[WRITE_END], tmsg, SIZE);
    pthread_mutex_unlock(&mutex_pipe);
    puts("[Storage manager] The data.csv file has been closed.");
    return 0;
}
