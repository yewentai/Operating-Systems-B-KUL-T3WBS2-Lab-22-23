/**
 * \author {JUNCHENG ZHU}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "config.h"
#include "lib/tcpsock.h"
#include "sbuffer.h"
#include "connmgr.h"
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h>
#include <assert.h>
#include "datamgr.h"
#include "./lib/dplist.h"
#include <stdbool.h>


#define PORT 5678
#define MAX_CONN 2  // state the max. number of connections the server will handle before exiting
extern int conn_counter;
extern sbuffer_t sbuffer;
pthread_mutex_t locker;
#define length 100
extern int fd[2];
#define READ_END 0
#define WRITE_END 1
#define TIME_OUT 5

/**
 * Implements a sequential test server (only one connection at the same time)
 */

void * pthread_receive(void *p)//listens on a TCP socket and write the data to sbuffer
{
    int bytes, result;
    sensor_data_t data;
    tcpsock_t *client = (tcpsock_t *)p;
    conn_counter++;
    char log[length];
    int signal = 1;

    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);
        if ((result == TCP_NO_ERROR) && bytes) 
        {
            //printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
            //    (long int) data.ts);
            //printf("%d",conn_counter);
#if 1
            if(signal == 1)
            {
                sprintf(log,"Sensor node %d has opened a new connection",data.id);//fork
                write(fd[WRITE_END], log, strlen(log)+1);//fork
                signal = 0;
            }
#endif

            pthread_mutex_lock(&locker);
            int insert = sbuffer_insert(&sbuffer,&data);
            pthread_mutex_unlock(&locker);

            if(insert < 0)
            {
            perror("sbuffer_insert()\n");
            exit(1);
            }            
        }       
    } while (result == TCP_NO_ERROR);

    conn_counter--;

    if (result == TCP_CONNECTION_CLOSED)
    {
        printf("Peer has closed connection\n");
        
        sprintf(log,"Sensor node %d has closed the connection",data.id);//fork
        write(fd[WRITE_END], log, strlen(log)+1);//fork

        data.id = 0;
        data.ts = 0;
        data.value = 0;

        int insert = sbuffer_insert(&sbuffer,&data);
        if(insert < 0)
        {
            perror("sbuffer_insert()\n");
            exit(1);
        }

        int i = 0;
        while(i < TIME_OUT+1)
        {
            write(fd[WRITE_END], "", 1);
            i++;
            sleep(1);
        }
    }
    else
        printf("Error occured on connection to peer\n");
    tcp_close(&client);

    pthread_exit(NULL);
}




/*
int main(void) 
{
    tcpsock_t *server;
    tcpsock_t *client;
    sensor_data_t data;
    pthread_t tid[MAX_CONN];
    int err;

    printf("Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) 
        exit(EXIT_FAILURE);
*/

/*
    if (tcp_wait_for_connection(server, &client[conn_counter]) != TCP_NO_ERROR) 
        exit(EXIT_FAILURE);
    printf("Incoming client connection\n");
*/
/*
    while(conn_counter < MAX_CONN) 
    {
        printf("&&&&&&&&&&&%d&&&&&&&&&\n",conn_counter);

        if (tcp_wait_for_connection(server, &client) == TCP_NO_ERROR) 
            printf("Incoming client connection\n");            
            //exit(EXIT_FAILURE);
        else
            //printf("Incoming client connection\n");
            exit(EXIT_FAILURE);

        err = pthread_create(&tid[conn_counter],NULL,pthread_receive,(void *)client);
        if (err)
        {
            perror("pthread_create()\n");
            exit(1);              
        }
        //printf("********%d***********\n",conn_counter);
    }

    if (tcp_close(&server) != TCP_NO_ERROR) 
        exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");
    return 0;
}
*/