/**
 * \author Wentai Ye
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <sys/wait.h>
#include "config.h"
#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"
#include "sbuffer.h"
#include "lib/tcpsock.h"

int num_conn = 0;           // Number of connections
int fd[2];                  // File descriptor for the pipe
sbuffer_t *sbuffer = NULL;  // Shared buffer
static int seq = 0;         // Sequence number of the log file
pthread_mutex_t mutex_pipe; // Mutex for the log file

void append_log(char *msg);

int main(int argc, char *argv[])
{
    /**************************************************************
     * The port of this TCP connection is given as a command line
     * argument at start-up of the main process, e.g. ./server 1234.
     **************************************************************/
    if (argc != 2)
    { // Check the number of arguments
        perror("Usage: ./main port");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);
    if (port < MIN_PORT || port > MAX_PORT)
    { // Check the port number
        perror("Port number should be between 1024 and 65535");
        exit(EXIT_FAILURE);
    }

    /******************************************************************************
     * Each time theserver is started, a new empty gateway.log file should be created.
     ******************************************************************************/
    FILE *fp = fopen("gateway.log", "w");
    if (fp == NULL)
    {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    /**********************************************************************
     *A shared data structure is used for communication between all threads.
     ***********************************************************************/
    sbuffer_init(&sbuffer); // Initialize the shared buffer

    /****************************************************************************************
     * The sensor gateway consists of a main process and a log process.
     * The log process receives log-events from the main process using a pipe.
     * All threads of the server process can generate log-events and write these to the pipe.
     ****************************************************************************************/
    if (pipe(fd) == -1) // Create a pipe between parent and child process(logger)
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&mutex_pipe, NULL); // Initialize the mutex for the log file

    /************************************************************************
     * The main process runs three threads at startup: the connection manager,
     * the datamanager, and the storage manager thread.
     ************************************************************************/
    pthread_t tid_connmgr, tid_datamgr, tid_storagemgr;
    if (pthread_create(&tid_connmgr, NULL, connmgr, &port) != 0)
    {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }

    sleep(5); // Wait for the connection manager and stotage manager to start
    if (pthread_create(&tid_storagemgr, NULL, storagemgr, NULL) != 0)
    {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&tid_datamgr, NULL, datamgr, NULL) != 0)
    {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }

    pid_t pid; // Process ID
    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0)
    { // The log process is started (with fork) as a child process of the main process.
        close(fd[WRITE_END]);
        static char log_msg[SIZE]; // Message to be received from the child process
        while (read(fd[READ_END], log_msg, SIZE) > 0)
        {
            append_log(log_msg);
        }
        close(fd[READ_END]);
        exit(EXIT_SUCCESS);
    }
    else
    { // Parent process
        close(fd[READ_END]);
        pthread_join(tid_connmgr, NULL);
        pthread_join(tid_storagemgr, NULL);
        pthread_join(tid_datamgr, NULL);
        close(fd[WRITE_END]);
        wait(NULL);
    }
    exit(EXIT_SUCCESS);
}

/*************************************************************************
 * A log-event contains an ASCII info message describing the type of event.
 * For eachlog-event received, the log process writes an ASCII message
 * to a new line in a log file called “gateway.log”.
 * \param msg The log message
 *************************************************************************/
void append_log(char *msg)
{
    FILE *fp;
    fp = fopen("gateway.log", "a");
    if (fp == NULL)
    {
        perror("fopen()");
        exit(EXIT_FAILURE);
    }

    char time_buffer[128];
    time_t rawtime; // time_t is a long int
    time(&rawtime); // get current time
    strftime(time_buffer, sizeof(time_buffer), "%d/%m/%Y %H:%M:%S", localtime(&rawtime));
    fprintf(fp, "%d %s %s\n", seq++, time_buffer, msg);
    fclose(fp);
}
