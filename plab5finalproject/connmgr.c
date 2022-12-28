/**
 * \author Wentai Ye
 */

#include "connmgr.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static char log_msg[SIZE]; // Message to be sent to the child process

void *connmgr_listen(void *p_client);

void *connmgr(void *port_void)
{
        int port = *(int *)port_void; // Port number
        pthread_t tid[MAX_CONN];      // Thread ID
        tcpsock_t *server, *client;   // Server and client socket

        if (tcp_passive_open(&server, port) != TCP_NO_ERROR)
                exit(EXIT_FAILURE);
        puts("[Connection manager] Connection manager Started!");
        do
        {
                if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR)
                        exit(EXIT_FAILURE);
                num_conn++; // the number of connections (also the number of corresponding threads)
                puts("[Connection manager] A new connection is established.");

                if (pthread_create(tid + num_conn, NULL, connmgr_listen, client) != 0)
                {
                        perror("pthread_create()");
                        exit(EXIT_FAILURE);
                }
        } while (num_conn < MAX_CONN);

        if (tcp_close(&server) != TCP_NO_ERROR) // Close the server socket
                exit(EXIT_FAILURE);
        puts("[Connection manager] Connection manager is terminated.");

        for (int i = 0; i < num_conn; i++) // Wait for all threads to finish
                pthread_join(tid[i], NULL);
        return NULL;
}

void *connmgr_listen(void *p_client)
{
        tcpsock_t *client = (tcpsock_t *)p_client; // Client socket
        sensor_data_t data;                        // Data to be received from the child process
        int bytes = 0;                             // Number of bytes received
        int result = TCP_NO_ERROR;                 // Result of the tcp_receive function

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
                sprintf(log_msg, "Sensor node %d has opened a new connection.", data.id);
                write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);
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
                        sbuffer_insert(sbuffer, &data);
                        if (result == TCP_NO_ERROR)
                                sbuffer_insert(sbuffer, &data);
                        else
                                break;
                }
        }

        if (result == TCP_SOCKET_ERROR)
        {
                pthread_mutex_lock(&mutex_pipe);
                sprintf(log_msg, "Error occured on connection to peer!");
                write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);
                pthread_mutex_unlock(&mutex_pipe);
        }
        else if (result == TCP_CONNECTION_CLOSED)
        {
                time_t tik;
                time(&tik);
                while (1)
                {
                        if (time(NULL) - tik > TIMEOUT)
                        {
                                num_conn--;
                                pthread_mutex_lock(&mutex_pipe);
                                sprintf(log_msg, "Sensor node %d has closed the connection.", data.id);
                                write(fd[WRITE_END], log_msg, strlen(log_msg) + 1);
                                pthread_mutex_unlock(&mutex_pipe);
                                tcp_close(&client);
                                puts("[Connection manager] A connection is closed.");
                                break;
                        }
                }
        }
        pthread_exit(NULL);
}