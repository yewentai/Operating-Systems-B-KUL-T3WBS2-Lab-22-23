#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<time.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

#include"config.h"
#include"sensor_db.h"
#include"logger.h"

char buffer[SIZE];

FILE* open_db(char* filename, bool append) {
    FILE* fp;
    if (append)
        fp = fopen(filename, "a");
    else
        fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen()");
        strcpy(buffer, "Failed to open the sensor_date file");
        write(fd[WRITE_END], buffer, SIZE);
        exit(1);
    }
    else {
        strcpy(buffer, "Successfully opened the sensor_date file");
        write(fd[WRITE_END], buffer, SIZE);
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
    fprintf(f, "%s,%"PRIu16",%.1lf\n", time_buffer, id, value);
    strcpy(buffer, "Successfully inserted the sensor data");
    write(fd[WRITE_END], buffer, SIZE);
    return 0;
}

int close_db(FILE* f) {
    if (f == NULL) {
        perror("fopen()");
        exit(1);
    }
    fclose(f);
    strcpy(buffer, "Successfully closed the sensor_date file");
    write(fd[WRITE_END], buffer, SIZE);
    return 0;
}

