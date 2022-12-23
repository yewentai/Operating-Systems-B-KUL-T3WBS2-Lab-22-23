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
    char *new_name;
    asprintf(&new_name, "%s", ((my_element_t *)element)->name); // asprintf requires _GNU_SOURCE
    copy->id = ((my_element_t *)element)->id;
    copy->name = new_name;
    return (void *)copy;
}

void element_free(void **element)
{
    free((((my_element_t *)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((my_element_t *)x)->id < ((my_element_t *)y)->id) ? -1 : (((my_element_t *)x)->id == ((my_element_t *)y)->id) ? 0
                                                                                                                            : 1);
}
