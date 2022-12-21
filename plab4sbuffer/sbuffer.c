/**
 * \author Wentai Ye
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "sbuffer.h"

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
struct sbuffer_node
{
    struct sbuffer_node *next; /**< a pointer to the next node*/
    sensor_data_t data;        /**< a structure containing the data */
};

/**
 * a structure to keep track of the buffer
 */
struct sbuffer
{
    sbuffer_node_t *head; /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail; /**< a pointer to the last node in the buffer */
};

int sbuffer_init(sbuffer_t **buffer)
{
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL)
        return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer)
{
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL))
    {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head)
    {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

// int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data)
// {
//     sbuffer_node_t *dummy;
//     if (buffer == NULL)
//         return SBUFFER_FAILURE;
//     if (buffer->head == NULL)
//         return SBUFFER_NO_DATA;
//     *data = buffer->head->data;
//     dummy = buffer->head;
//     if (buffer->head == buffer->tail) // buffer has only one node
//     {
//         buffer->head = buffer->tail = NULL;
//     }
//     else // buffer has many nodes empty
//     {
//         buffer->head = buffer->head->next;
//     }
//     free(dummy);
//     return SBUFFER_SUCCESS;
// }

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data)
{
    sbuffer_node_t *dummy;
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;       // create a mutex
    static pthread_cond_t dataavailable = PTHREAD_COND_INITIALIZER; // create a condition variable
    bool done = false;
    pthread_mutex_lock(&mutex);
    if (buffer->head == NULL)
    {
        while (buffer->head == NULL && !done)
            pthread_cond_wait(&dataavailable, &mutex);
        if (done)
        {
            pthread_cond_signal(&dataavailable); // wake up other reading threads if any
            pthread_mutex_unlock(&mutex);
            return SBUFFER_NO_DATA;
        }
    }
    // setting data and removing node happens here (see original code)
    if (data->id == 0)
    {
        done = true;
        pthread_cond_signal(&dataavailable); // other threads might be waiting ->next !
        pthread_mutex_unlock(&mutex);
        return SBUFFER_NO_DATA;
    }
    pthread_cond_signal(&dataavailable); // other threads might be waiting ->next !
    pthread_mutex_unlock(&mutex);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data)
{
    sbuffer_node_t *dummy;
    if (buffer == NULL)
        return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL)
        return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    }
    else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    return SBUFFER_SUCCESS;
}
