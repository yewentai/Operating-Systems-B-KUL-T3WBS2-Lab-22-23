#include "datamgr.h"

void *datamgr()
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
        perror("fopen()");
        exit(EXIT_FAILURE);
    }
    int m = 0;
    int n = 2;
    char mid;
    while (!feof(fp))
    {
        mid = fgetc(fp); // get the character
        if (mid == '\n') // if the character is a new line
            m++;         // increase the line count
    }
    rewind(fp);    // set the file pointer to the beginning of the file
    int map[m][n]; // create a 2D array with the size of the file
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &map[i][j]);
        }
    }
    fclose(fp);
    for (int i = 0; i < m; i++)
    {
        if (map[i][1] == sensor_id)
        {
            return map[i][0];
        }
    }
    return 0;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id)
{
    sensor_value_t avg = 0;
    int count = 0;
    if (dpl_size(list) <= 5)
    {
        return 0;
    }
    for (int i = 0; i < dpl_size(list); i++)
    {
        if (((my_element_t *)dpl_get_element_at_index(list, i))->sensor_id == sensor_id)
        {
            avg += ((my_element_t *)dpl_get_element_at_index(list, i))->avg;
            count++;
        }
    }
    return avg / count;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id)
{
    time_t last_modified = 0;
    for (int i = 0; i < dpl_size(list); i++)
    {
        if (((my_element_t *)dpl_get_element_at_index(list, i))->sensor_id == sensor_id)
        {
            if (((my_element_t *)dpl_get_element_at_index(list, i))->last_modified > last_modified)
            {
                last_modified = ((my_element_t *)dpl_get_element_at_index(list, i))->last_modified;
            }
        }
    }
    return last_modified;
}

int datamgr_get_total_sensors()
{
    int count = 0;
    for (int i = 0; i < dpl_size(list); i++)
    {
        if (((my_element_t *)dpl_get_element_at_index(list, i))->sensor_id != ((my_element_t *)dpl_get_element_at_index(list, i + 1))->sensor_id)
        {
            count++;
        }
    }
    return count;
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
