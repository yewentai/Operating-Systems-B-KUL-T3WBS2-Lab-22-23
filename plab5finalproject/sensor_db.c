#include "sensor_db.h"

char buffer[SIZE];
FILE *open_db(char *filename, bool append);
int insert_sensor(FILE *f, sensor_data_t *data);
int close_db(FILE *f);

void *storagemgr()
{
    FILE *f = open_db("sensor_data.csv", false); // Create sensor_data.csv
    strcpy(buffer, "A new data.csv file has been created.");
    write(fd[WRITE_END], buffer, SIZE);
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    while (sbuffer_remove(sbuffer, data) != SBUFFER_NO_DATA)
    {
        insert_sensor(f, data);
    }
    close_db(f); // Close sensor_data.csv
    strcpy(buffer, "The data.csv file has been closed.");
    write(fd[WRITE_END], buffer, SIZE);
    free(data);

    exit(EXIT_SUCCESS);
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
        strcpy(buffer, "Failed to open the sensor_date file");
        write(fd[WRITE_END], buffer, SIZE);
        exit(1);
    }
    else
    {
        strcpy(buffer, "Successfully opened the sensor_date file");
        write(fd[WRITE_END], buffer, SIZE);
    }
    return fp;
}

int insert_sensor(FILE *f, sensor_data_t *data)
{
    if (f == NULL)
    {
        perror("fopen()");
        exit(1);
    }

    char time_buffer[128];

    strftime(time_buffer, sizeof(time_buffer), "%d/%m/%Y %H:%M:%S", localtime(&data->ts));
    fprintf(f, "%s,%" PRIu16 ",%.1lf\n", time_buffer, data->id, data->value);
    sprintf(buffer, "Data insertion from sensor %d succeeded.", data->id);
    write(fd[WRITE_END], buffer, SIZE);
    exit(EXIT_SUCCESS);
}

int close_db(FILE *f)
{
    if (f == NULL)
    {
        perror("fopen()");
        exit(1);
    }
    fclose(f);
    strcpy(buffer, "Successfully closed the sensor_date file");
    write(fd[WRITE_END], buffer, SIZE);
    exit(EXIT_SUCCESS);
}
