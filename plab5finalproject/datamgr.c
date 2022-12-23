#include "datamgr.h"

void datamgr(FILE *fp_sensor_map)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp_sensor_map)) != -1)
    {
        char *token = strtok(line, ",");
        sensor_id_t sensor_id = atoi(token);
        token = strtok(NULL, ",");
        room_id_t room_id = atoi(token);
        datamgr_parse_sensor_data(sensor_id, room_id);
    }
    free(line);
}

void datamgr_free()
{
    dpl_free(&list, true);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id)
{
    FILE *fp = fopen("room_sensor.map", "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
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

void *element_copy(void *element)
{
    my_element_t *copy = malloc(sizeof(my_element_t));
    assert(copy != NULL);
    copy->sensor_id = ((my_element_t *)element)->sensor_id;
    copy->room_id = ((my_element_t *)element)->room_id;
    copy->avg = ((my_element_t *)element)->avg;
    copy->last_modified = ((my_element_t *)element)->last_modified;
    return (void *)copy;
}

void element_free(void **element)
{
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((my_element_t *)x)->sensor_id < ((my_element_t *)y)->sensor_id) ? -1 : (((my_element_t *)x)->sensor_id == ((my_element_t *)y)->sensor_id) ? 0
                                                                                                                                                        : 1);
}
