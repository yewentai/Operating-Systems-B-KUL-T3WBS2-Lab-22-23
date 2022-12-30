#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include <unistd.h>
#include<string.h>

#include "sensor_db.h"
#include "config.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define length 100
#define READ_END 0
#define WRITE_END 1

FILE * open_db(char * filename, bool append)
{
    FILE *fp;
    char log[length];

    if(filename == NULL)
    {
        strcpy(log, "An error occurred when writing to the csv file.");
        write(fd[WRITE_END], log, strlen(log)+1);
        return NULL;
    }
    else
    {
        if(append)
            fp = fopen(filename, "a");
        else
            fp = fopen(filename, "w");
        
        if (fp != NULL)
            strcpy(log, "A new csv file is created or an existing file has been opened.");
        else
            strcpy(log, "An error occurred when writing to the csv file.");
        
        write(fd[WRITE_END], log, strlen(log)+1);
    }

    return fp;    
}



#if 0
int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts)
{
    char log[length];

    if(f == NULL)
    {
        strcpy(log, "An error occurred when writing to the csv file.");
        write(fd[WRITE_END], log, strlen(log)+1);
        return -1;
    }       
    else
    {
        int i = fprintf(f, "%d\t%f\t%ld\n", id, value, ts);

        if(i <= 0)
            strcpy(log, "An error occurred when writing to the csv file.");
        else
            sprintf(log,"Data insertion from sensor %d sensor.",id);  

        write(fd[WRITE_END], log, strlen(log)+1);
    }

    return 0;
}
#endif




int close_db(FILE *f)
{
    char log[length];

    if(f == NULL)
    {
        strcpy(log, "An error occurred when writing to the csv file.");
        write(fd[WRITE_END], log, strlen(log)+1);
        return -1;
    }       
    else
    {
        fclose(f);
        strcpy(log, "The data.csv file has been closed.");
        write(fd[WRITE_END], log, strlen(log)+1);        
    }

    return 0;
}




int insert_log(FILE *fp, int num)
{
    if(fp == NULL)
        return -1;
    else
    {
        char buffer[length];
        //printf("&&&&&&&&&&&\n");
        //open((void *)&(fd[READ_END]),O_NONBLOCK);
        int i = read(fd[READ_END], buffer, length);
        //printf("——————————————%d——————————\n",i);
        if(i > 1)
        {
            time_t ts = time(NULL);
            fprintf(fp, "%d\t%ld\t%s\n", num,ts,buffer);
        }
        else
            return -1;
        return i;
    }
}