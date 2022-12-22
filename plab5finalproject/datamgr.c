#include "datamgr.h"

#define SET_MAX_TEMP 40
#define SET_MIN_TEMP -10
#define RUN_AVG_LENGTH 5

void *datamgr(void *)
{
    FILE *fp = fopen("room_sensor.map", "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    while (!feof(fp))
    {
        char line[100];
        fgets(line, 100, fp);
        char *token = strtok(line, " ");
        printf("%s", token);
    }

    void datamgr_parse_sensor_files(FILE * fp_sensor_map, FILE * fp_sensor_data)
    {
    }

    void datamgr_free()
    {
    }

    uint16_t datamgr_get_room_id(sensor_id_t sensor_id)
    {
        FILE *fp = fopen("room_sensor.map", "r");
        if (fp == NULL)
        {
            perror("fopen");
            exit(EXIT_FAILURE);
        }
        // while (!feof(fp))
        // {
        //     char line[100];
        //     fgets(line, 100, fp);
        //     char *token = strtok(line, " ");
        //     if (strcmp(token, sensor_id) == 0)
        //     {
        //         token = strtok(NULL, " ");
        //         return atoi(token);
        //     }
    }
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id)
{
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id)
{
}

int datamgr_get_total_sensors()
{
}