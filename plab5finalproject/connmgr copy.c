/**
 * \author Wentai Ye
 */

#include "connmgr.h"

static char log_msg[SIZE]; // Message to be received from the child process
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *connmgr(void *port_void)
{
        puts("Connection manager is starting up");

        int port = *(int *)port_void; // Port number
        pthread_t tid[MAX_CONN];      // Thread ID
        tcpsock_t *server, *client;   // Server and client socket

        if (tcp_passive_open(&server, port) != TCP_NO_ERROR)
                exit(EXIT_FAILURE);
        do
        {
                if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR)
                        exit(EXIT_FAILURE);
                num_conn++; // the number of connections (also the number of threads)

                // For each client-side node communicating with the server, there is a dedicated thread to process incoming data at the server.int ret_create_thread;
                if (pthread_create(tid + num_conn, NULL, thread_listen, client) != 0)
                {
                        perror("pthread_create()");
                        exit(EXIT_FAILURE);
                }

        } while (num_conn < MAX_CONN);

        if (tcp_close(&server) != TCP_NO_ERROR)
                exit(EXIT_FAILURE);

        for (int i = 0; i < num_conn; i++)
                pthread_join(tid[i], NULL);

        puts("connection manager is shutting down");
        return NULL;
}

void *thread_listen(void *p_client)
{
        tcpsock_t *client = (tcpsock_t *)p_client; // Client socket
        sensor_data_t data;                        // Data to be received from the child process
        int bytes = 0;                             // Number of bytes received
        int result = TCP_NO_ERROR;                 // Result of the tcp_receive function

        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *)&data.id, &bytes);
        error_handling(result, data, client);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *)&data.value, &bytes);
        error_handling(result, data, client);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *)&data.ts, &bytes);
        error_handling(result, data, client);
        // write data to sbuffer
        sprintf(log_msg, "Sensor node %d has opened a new connection.", data.id);
        write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);
        puts("Sensor node has opened a new connection!");
        sbuffer_insert(sbuffer, &data);

        while (result == TCP_NO_ERROR)
        {
                // read sensor ID
                bytes = sizeof(data.id);
                result = tcp_receive(client, (void *)&data.id, &bytes);
                error_handling(result, data, client);
                // read temperature
                bytes = sizeof(data.value);
                result = tcp_receive(client, (void *)&data.value, &bytes);
                error_handling(result, data, client);
                // read timestamp
                bytes = sizeof(data.ts);
                result = tcp_receive(client, (void *)&data.ts, &bytes);
                error_handling(result, data, client);
                // write data to sbuffer
                sbuffer_insert(sbuffer, &data);
        };
        pthread_exit(NULL);
}

void error_handling(int result, sensor_data_t data, tcpsock_t *client)
{
        if (result == TCP_CONNECTION_CLOSED)
        {
                puts("Sensor node has closed the connection!");
                sprintf(log_msg, "Sensor node %d has closed the connection.", data.id);
                write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);
                num_conn--;
                tcp_close(&client);
                pthread_exit(NULL);
        }
        else if (result == TCP_SOCKET_ERROR)
        {
                puts("Error occured on connection to peer!");
                sprintf(log_msg, "Error occured on connection to sensor node %d.", data.id);
                write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);
                num_conn--;
                tcp_close(&client);
                pthread_exit(NULL);
        }
}
