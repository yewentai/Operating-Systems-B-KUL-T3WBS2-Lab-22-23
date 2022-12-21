#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<time.h>

#include"config.h"
#include"logger.h"

void create_log_file(void) {
    FILE* fp;
    fp = fopen("gateway.log", "w");
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }
    fclose(fp);
}

void append_log(char* msg) {
    FILE* fp;
    fp = fopen("gateway.log", "a");
    if (fp == NULL) {
        perror("fopen()");
        exit(1);
    }
    char time_buffer[128];
    time_t rawtime; // time_t is a long int
    time(&rawtime); // get current time
    strftime(time_buffer, sizeof(time_buffer), "%d/%m/%Y %H:%M:%S", localtime(&rawtime));
    fprintf(fp, "<%d><%s><%s>\n", seq++, time_buffer, msg);
    fclose(fp);
}


