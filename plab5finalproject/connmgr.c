#include "connmgr.h"

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
                if (pthread_create(tid + num_conn, NULL, connmgr_listen, client) != 0)
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
}

void *connmgr_listen(void *p)
{
        tcpsock_t *client = (tcpsock_t *)p; // Client socket
        sensor_data_t data;                 // Data to be received from the child process
        int bytes = 0;                      // Number of bytes received
        int result = TCP_NO_ERROR;          // Result of the tcp_receive function
        static char log_msg[SIZE];          // Message to be received from the child process

        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *)&data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *)&data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *)&data.ts, &bytes);
        // write data to sbuffer

        if (result == TCP_NO_ERROR)
        {
                sprintf(log_msg, "Sensor node %d has opened a new connection.", data.id);
                write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);

                puts("Sensor node has opened a new connection!");

                sbuffer_insert(sbuffer, &data);
        }

        while (result == TCP_NO_ERROR)
        {
                // read sensor ID
                bytes = sizeof(data.id);
                result = tcp_receive(client, (void *)&data.id, &bytes);
                // read temperature
                bytes = sizeof(data.value);
                result = tcp_receive(client, (void *)&data.value, &bytes);
                // read timestamp
                bytes = sizeof(data.ts);
                result = tcp_receive(client, (void *)&data.ts, &bytes);
                // write data to sbuffer

                if (result == TCP_NO_ERROR)
                {
                        sbuffer_insert(sbuffer, &data);
                }
        };

        if (result == TCP_CONNECTION_CLOSED)
        {
                sprintf(log_msg, "Sensor node %d has closed the connection.", data.id);
                write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);
                num_conn--;

                puts("Sensor node has closed the connection!");
        }
        else
        {
                puts("Error occured on connection to peer!");
        }
        tcp_close(&client);
        pthread_exit(NULL);
}
