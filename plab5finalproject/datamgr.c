#include "datamgr.h"

void *datamgr()
{
    FILE *map = fopen("room_sensor.map", "r");
    if (map == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    FILE *csv = fopen("sensor_data.csv", "r");
    if (csv == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    datamgr_parse_sensor_files(map, csv);
}

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data)
{
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
    free((((my_element_t *)*element))->sensor_id);
    free((((my_element_t *)*element))->room_id);
    free((((my_element_t *)*element))->avg);
    free((((my_element_t *)*element))->last_modified);
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((my_element_t *)x)->sensor_id < ((my_element_t *)y)->sensor_id) ? -1 : (((my_element_t *)x)->sensor_id == ((my_element_t *)y)->sensor_id) ? 0
                                                                                                                                                        : 1);
}
