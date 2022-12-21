#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include <sys/wait.h>

#include"config.h"
#include"logger.h"
#include"sensor_db.h"

int seq = 0;
int fd[2];

int main(void) {
    // Set the correct time zone
    // system("timedatectl set-timezone Europe/Brussels");    

    /**
     * Create the log file
     */
    if (access("gateway.log", F_OK) != -1) // file exists
        puts("gateway.log already exists");
    else {// file doesn't exist
        create_log_file();
        puts("gateway.log created");
    }

    /**
     *  First create a pipe
     */
    pid_t pid;
    if (pipe(fd) == -1) {
        perror("pipe()");
        exit(1);
    }
    char rmsg[SIZE]; // Message to be received from the child process

    /**
     *  Then fork a child process
     */
    fflush(NULL);
    pid = fork();
    if (pid < 0) {
        perror("fork()");
        exit(1);
    }
    if (pid == 0) {//parent process
        close(fd[READ_END]);

        // Open sensor_data.csv with append mode
        FILE* f = open_db("sensor_data.csv", true);

        // insert 10 random data into sensor_data.csv
        for (int i = 0; i < 10; i++) {
            sensor_id_t id = rand() % 10;
            sensor_value_t value = rand() % 100;
            sensor_ts_t ts = time(NULL);
            insert_sensor(f, id, value, ts);
        }

        // Close sensor_data.csv
        close_db(f);

        close(fd[WRITE_END]);
        wait(NULL);
    }
    if (pid > 0) {//child process
        close(fd[WRITE_END]);
        while (read(fd[READ_END], rmsg, SIZE) > 0) {
            append_log(rmsg);
        }
        close(fd[READ_END]);
        exit(0);
    }
    exit(0);
}