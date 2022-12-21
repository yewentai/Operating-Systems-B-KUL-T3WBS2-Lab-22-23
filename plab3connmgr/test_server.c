/**
 * \author Wentai Ye
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <pthread.h>
#include <string.h>

#define PORT 5678
#define MAX_CONN 3 // state the max. number of connections the server will handle before exiting

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *sockedThread(void *p)
{
    tcpsock_t *client = (tcpsock_t *)p;
    int bytes, result;
    do
    {
        sensor_data_t data;
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *)&data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *)&data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *)&data.ts, &bytes);
        if ((result == TCP_NO_ERROR) && bytes)
        {
            printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                   (long int)data.ts);
        }
    } while (result == TCP_NO_ERROR);

    if (result == TCP_CONNECTION_CLOSED)
        printf("Peer has closed connection\n");
    else
        printf("Error occured on connection to peer\n");
    tcp_close(&client);
    pthread_exit(NULL);
}

/**
 * Implements a sequential test server
 */

int main(void)
{
    pthread_t tid[MAX_CONN];

    tcpsock_t *server, *client;

    int conn_counter = 0;

    printf("Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR)
        exit(EXIT_FAILURE);
    do
    {
        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR)
            exit(EXIT_FAILURE);
        printf("Incoming client connection\n");
        conn_counter++; // the number of connections (also the number of threads)

        int ret_create_thread;
        ret_create_thread = pthread_create(tid + conn_counter, NULL, sockedThread, client);
        if (ret_create_thread != 0)
        {
            fprintf(stderr, "can't create thread: %s\n", strerror(ret_create_thread));
            exit(EXIT_FAILURE);
        }

    } while (conn_counter < MAX_CONN);

    if (tcp_close(&server) != TCP_NO_ERROR)
        exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");

    for (int i = 0; i < conn_counter; i++)
        pthread_join(tid[i], NULL);

    exit(EXIT_SUCCESS);
}
