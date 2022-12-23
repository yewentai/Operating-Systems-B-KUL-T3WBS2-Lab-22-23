#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/wait.h>
#include "config.h"
#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"
#include "lib/tcpsock.h"

int seq = 0;               // Sequence number of the log file
int fd[2];                 // File descriptor for the pipe
char log_msg[SIZE];        // Message to be received from the child process
sbuffer_t *sbuffer = NULL; // Shared buffer

void append_log(char *msg);

int main(int argc, char *argv[])
{
    /******************************************************
     * Check if the user has provided the right port number
     ******************************************************/
    if (argc != 2)
    {
        perror("Usage: ./main port");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    if (port < MIN_PORT || port > MAX_PORT)
    {
        perror("Port number should be between 1024 and 65535");
        exit(EXIT_FAILURE);
    }

    // system("timedatectl set-timezone Europe/Brussels"); // Set the correct time zone

    pthread_t tid_connmgr, tid_datamgr, tid_storagemgr; // Thread ID of the connection manager, data manager and storage manager

    sbuffer_init(&sbuffer); // Initialize the shared buffer

    /********************************************************
     * Create a pipe between parent and child process(logger)
     ********************************************************/
    pid_t pid;
    if (pipe(fd) == -1)
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }

    /**********************
     * Fork a child process
     **********************/
    fflush(NULL); // Flush all open streams
    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // child process
        close(fd[WRITE_END]);
        while (read(fd[READ_END], log_msg, SIZE) > 0)
        {
            append_log(log_msg);
        }
        close(fd[READ_END]);

        exit(EXIT_SUCCESS);
    }
    else
    { // parent process
        close(fd[READ_END]);
        strcpy(log_msg, "Gateway started");
        write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);

        /********************************************
         * Create a thread for the connection manager
         ********************************************/
        if (pthread_create(&tid_connmgr, NULL, connmgr, &port) != 0)
        {
            perror("pthread_create()");
            exit(EXIT_FAILURE);
        }

        /*****************************************
         * Create a thread for the storage manager
         *****************************************/
        if (pthread_create(&tid_storagemgr, NULL, storagemgr, NULL) != 0)
        {
            perror("pthread_create()");
            exit(EXIT_FAILURE);
        }

        // /**************************************
        //  * Create a thread for the data manager
        //  **************************************/
        // if (pthread_create(&tid_datamgr, NULL, datamgr, NULL) != 0)
        // {
        //     perror("pthread_create()");
        //     exit(EXIT_FAILURE);
        // }

        close(fd[WRITE_END]);

        wait(NULL);
    }

    pthread_join(tid_connmgr, NULL);
    pthread_join(tid_storagemgr, NULL);
    // pthread_join(tid_datamgr, NULL);

    exit(EXIT_SUCCESS);
}

void append_log(char *msg)
{
    FILE *fp;
    fp = fopen("gateway.log", "a");
    if (fp == NULL)
    {
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
