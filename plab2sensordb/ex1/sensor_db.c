#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<time.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include"sensor_db.h"
#include"config.h"

FILE* open_db(char* filename, bool append) {
    FILE* fp;
    if (append) {
        fp = fopen(filename, "a");
    }
    else {
        fp = fopen(filename, "w");
    }
    return fp;
}

int insert_sensor(FILE* f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts) {
    if (f == NULL) {
        perror("fopen()");
        exit(1);
    }

    char time_buffer[128];
    strftime(time_buffer, sizeof(time_buffer), "%d/%m/%Y %H:%M:%S", localtime(&ts));
    fprintf(f, "%s,%"PRIu16",%lf\n", time_buffer,id,value);
    return 0;
}

int close_db(FILE* f) {
    fclose(f);
}

