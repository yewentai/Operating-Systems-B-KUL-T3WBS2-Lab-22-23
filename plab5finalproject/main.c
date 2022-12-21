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
#include "logger.h"
#include "sensor_db.h"

void *element_copy(void *element);
void element_free(void **element);
int element_compare(void *x, void *y);

int seq = 0;
int fd[2];
char rmsg[SIZE]; // Message to be received from the child process

typedef struct
{
    int id;
    char *name;
} my_element_t;

sbuffer_t *sbuffer = NULL;

int main(int argc, char *argv[])
{
    /************************************************
     * Check if the user has provided the port number
     ************************************************/
    if (argc != 2)
    {
        perror("Usage: ./main port");
        exit(EXIT_FAILURE);
    }

    /***********************************
     * Check if the port number is valid
     ***********************************/
    int port = atoi(argv[1]);
    if (port < 1024 || port > 65535)
    {
        perror("Port number should be between 1024 and 65535");
        exit(EXIT_FAILURE);
    }

    // system("timedatectl set-timezone Europe/Brussels"); // Set the correct time zone

    sbuffer_init(&sbuffer); // Create a sbuffer

    if (access("gateway.log", F_OK) == -1) // Create the log file
        create_log_file();

    /*******************************************************
     * Create a pipe between parent and child process(logger)
     ********************************************************/
    pid_t pid;
    if (pipe(fd) == -1)
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }

    /***************************
     * Then fork a child process
     ***************************/
    fflush(NULL);
    pid = fork();
    if (pid < 0)
    {
        perror("fork()");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    { // parent process
        close(fd[READ_END]);
        puts("Gateway is running...");
        connmgr(port); // Create a connection manager and start the gateway
        puts("storage manager is running...");
        storagemgr(); // Create a storagemgr
        puts("data manager is running...");
        datamgr(); // Create a datamgr

        close(fd[WRITE_END]);
        wait(NULL);
    }
    if (pid > 0)
    { // child process
        close(fd[WRITE_END]);
        while (read(fd[READ_END], rmsg, SIZE) > 0)
        {
            append_log(rmsg);
        }
        close(fd[READ_END]);

        exit(EXIT_SUCCESS);
    }
    exit(EXIT_SUCCESS);
}

void *element_copy(void *element)
{
    my_element_t *copy = malloc(sizeof(my_element_t));
    assert(copy != NULL);
    char *new_name;
    asprintf(&new_name, "%s", ((my_element_t *)element)->name); // asprintf requires _GNU_SOURCE
    copy->id = ((my_element_t *)element)->id;
    copy->name = new_name;
    return (void *)copy;
}

void element_free(void **element)
{
    free((((my_element_t *)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((my_element_t *)x)->id < ((my_element_t *)y)->id) ? -1 : (((my_element_t *)x)->id == ((my_element_t *)y)->id) ? 0
                                                                                                                            : 1);
}
