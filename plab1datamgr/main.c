#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "lib/dplist.h"
#include "datamgr.h"
#include <time.h>

int main() {
    FILE* map = fopen("room_sensor.map", "r");
    if (map == NULL) return -1;
    FILE* data = fopen("sensor_data", "rb");
    if (data == NULL) return -1;

    datamgr_parse_sensor_files(map, data);

    datamgr_free();

    fclose(map);
    fclose(data);
}