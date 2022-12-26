#include "sensor_db.h"

void *storagemgr()
{
    FILE *csv = open_db("sensor_data.csv", false); // A new, empty data.csv should be created when the server starts up. It should not be deleted when the server stops.

    static char log_msg[SIZE]; // Message to be received from the child process
    strcpy(log_msg, "A new data.csv file has been created.");
    write(fd[WRITE_END], log_msg, SIZE);

    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    while (sbuffer_get_head(sbuffer, data) == SBUFFER_SUCCESS)
    {
        insert_sensor(csv, data);
        sleep(1);
    }

    close_db(csv); // Close sensor_data.csv
    strcpy(log_msg, "The data.csv file has been closed.");
    write(fd[WRITE_END], log_msg, SIZE);

    free(data);
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

    static char log_msg[SIZE]; // Message to be received from the child process
    sprintf(log_msg, "Data insertion from sensor %d succeeded.", data->id);
    write(fd[WRITE_END], log_msg, SIZE);
}

int close_db(FILE *csv)
{
    if (csv == NULL)
    {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }
    fclose(csv);

    static char log_msg[SIZE]; // Message to be received from the child process
    strcpy(log_msg, "Successfully closed the sensor_date file");
    write(fd[WRITE_END], log_msg, SIZE);

    exit(EXIT_SUCCESS);
}
