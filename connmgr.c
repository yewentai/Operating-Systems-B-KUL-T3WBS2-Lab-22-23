/**
 * \author Wentai Ye
 */

#include "connmgr.h"

static char tmsg[SIZE];        // Message to be sent to the child process
static struct timeval timeout; // Timeout for select()

void *connmgr(void *port_void)
{
        int tol_conn = 0;             // Number of total connections
        tcpsock_t *server, *client;   // Server and client socket
        int port = *(int *)port_void; // Port number
        pthread_t tid[MAX_CONN];      // Thread ID
        timeout.tv_sec = TIMEOUT;     // Timeout for tcp_receive()
        timeout.tv_usec = 0;          // Timeout for tcp_receive()
        if (tcp_passive_open(&server, port) != TCP_NO_ERROR)
                exit(EXIT_FAILURE);
        puts("[Connection manager] Connection manager Started!");

        while (tol_conn <= MAX_CONN)
        {
                if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR)
                        exit(EXIT_FAILURE);
                cur_conn++; // The number of connections
                tol_conn++; // The total number of connections
                puts("[Connection manager] A new connection is established.");

                if (pthread_create(tid + cur_conn, NULL, connmgr_listen, client) != 0)
                {
                        perror("pthread_create()");
                        exit(EXIT_FAILURE);
                }
        }

        if (tcp_close(&server) != TCP_NO_ERROR) // Close the server socket
                exit(EXIT_FAILURE);
        puts("[Connection manager] Connection manager is terminated.");
        for (int i = 0; i < tol_conn; i++) // Wait for all threads to finish
                pthread_join(tid[i], NULL);
        pthread_exit(NULL);
}

void *connmgr_listen(void *p_client)
{
        tcpsock_t *client = (tcpsock_t *)p_client;                                  // Client socket
        int sd;                                                                     // Socket descriptor
        tcp_get_sd(client, &sd);                                                    // Socket descriptor
        setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)); // Set timeout for tcp_receive()
        sensor_data_t data;                                                         // Data to be received from the client
        int bytes = 0;                                                              // Number of bytes received
        int result = TCP_NO_ERROR;                                                  // Result of tcp_receive()

        /*****************************************
         * Read first data from the client socket
         *****************************************/
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *)&data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *)&data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *)&data.ts, &bytes);
        if (result == TCP_NO_ERROR)
        {
                // write data to sbuffer
                pthread_mutex_lock(&mutex_pipe);
                sprintf(tmsg, "Sensor node %d has opened a new connection.", data.id);
                write(fd[WRITE_END], tmsg, SIZE);
                pthread_mutex_unlock(&mutex_pipe);
                sbuffer_insert(sbuffer, &data);

                /********************************************************
                 * Read the following data from the client socket
                 * until the connection is closed and timeout is reached
                 * or an error occurs
                 ********************************************************/
                while (1)
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
                                sbuffer_insert(sbuffer, &data);
                        else
                                break;
                }
        }

        if (result == TCP_SOCKET_ERROR)
        {
                pthread_mutex_lock(&mutex_pipe);
                sprintf(tmsg, "Error occured on connection to peer!");
                write(fd[WRITE_END], tmsg, SIZE);
                pthread_mutex_unlock(&mutex_pipe);
        }
        else if (result == TCP_CONNECTION_CLOSED)
        {
                cur_conn--;
                pthread_mutex_lock(&mutex_pipe);
                sprintf(tmsg, "Sensor node %d has closed the connection.", data.id);
                write(fd[WRITE_END], tmsg, SIZE);
                pthread_mutex_unlock(&mutex_pipe);
                tcp_close(&client);
                puts("[Connection manager] A connection is closed.");
        }
        pthread_exit(NULL);
}