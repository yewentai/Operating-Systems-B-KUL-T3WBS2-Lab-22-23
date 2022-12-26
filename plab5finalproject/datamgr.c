#include "datamgr.h"

void *datamgr()
{
    /******************************************************
    ** open "room_sensor.map" and get the lines of the file
    *******************************************************/
    FILE *fp = fopen("room_sensor.map", "r");
    int m = 0;
    int n = 2;
    char mid;
    while (!feof(fp))
    {
        mid = fgetc(fp); // get the character
        if (mid == '\n') // if the character is map new line
            m++;         // increase the line count
    }
    rewind(fp); // set the file pointer to the beginning of the file

    /***************************************
    ** allocate the memory for the map array
    ****************************************/
    int **map;
    map = (int **)malloc(sizeof(int *) * m);
    for (int i = 0; i < m; i++)
        map[i] = (int *)malloc(sizeof(int) * n);
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &map[i][j]);
        }
    }
    fclose(fp);
    // check the map array
    puts("The map is:");
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            printf("%d ", map[i][j]);
        }
        printf("\n");
    }

    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    sensor_data_t *data = malloc(sizeof(sensor_data_t));
    while (sbuffer_remove(sbuffer, data) == SBUFFER_SUCCESS)
    {
        element->sensor_id = data->id;
        element->room_id = datamgr_get_room_id(data->id, map, m);
        element->value = data->value;
        element->avg = datamgr_get_avg(data->id);
        element->last_modified = data->ts;
        dpl_insert_at_index(list, element, 0, true);
        if (element->avg > SET_MAX_TEMP)
        {
            printf("The room %d is too hot!\n", element->room_id);
        }
        else if (element->avg < SET_MIN_TEMP)
        {
            printf("The room %d is too cold!\n", element->room_id);
        }
    }
    free(element);
    datamgr_free();
}

void datamgr_free()
{
    dpl_free(&list, true);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id, int **map, int m)
{

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
            count++;
            avg += ((my_element_t *)dpl_get_element_at_index(list, i))->value;
        }
        if (count == 5)
            break;
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
