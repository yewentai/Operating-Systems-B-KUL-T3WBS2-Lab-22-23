#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<time.h>
#include<unistd.h>

#include"sensor_db.h"
#include"config.h"

int main(void) {
    system("timedatectl set-timezone Europe/Brussels");    //set local time
    time_t rawtime;

    if (access("./sensor_date.csv", 0)) {
        printf("File does not exist, create one!\n");
        //create a sensor_db file
        FILE* sensor_data = open_db("sensor_date.csv", "w");
        if (sensor_data == NULL) {
            perror("fopen()");
            exit(1);
        }

        //append a single sensor reading to the csv file
        sensor_id_t id = 1;
        sensor_value_t value = 2.0;
        sensor_ts_t ts = time(&rawtime);
        insert_sensor(sensor_data, id, value, ts);

        //close the file
        close_db(sensor_data);
    }
    else {
        printf("File exists\n");
        //append one data to the file
        FILE* sensor_data = fopen("sensor_date.csv", "a");
        if (sensor_data == NULL) {
            perror("fopen()");
            exit(1);
        }

        //append a single sensor reading to the csv file
        sensor_id_t id = 1;
        sensor_value_t value = 2.0;
        sensor_ts_t ts = time(&rawtime);
        insert_sensor(sensor_data, id, value, ts);

        //close the file
        close_db(sensor_data);
    }

    exit(0);
}
