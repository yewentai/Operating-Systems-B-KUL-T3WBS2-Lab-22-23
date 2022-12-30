/**
 * \author Wentai Ye
 */

#include "sbuffer.h"
bool read_done = false;

/**
 * basic node for the sbuffer, these nodes are linked together to create the sbuffer
 */
struct sbuffer_node
{
    struct sbuffer_node *next; /**< a pointer to the next node*/
    sensor_data_t data;        /**< a structure containing the data */
};

/**
 * a structure to keep track of the sbuffer
 */
struct sbuffer
{
    sbuffer_node_t *head; /**< a pointer to the first node in the sbuffer */
    sbuffer_node_t *tail; /**< a pointer to the last node in the sbuffer */
    int count;            /**< the number of nodes in the sbuffer */
};

int sbuffer_init(sbuffer_t **sbuffer)
{
    *sbuffer = malloc(sizeof(sbuffer_t));
    if (*sbuffer == NULL)
        return SBUFFER_FAILURE;
    (*sbuffer)->head = NULL;
    (*sbuffer)->tail = NULL;
    (*sbuffer)->count = 0;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **sbuffer)
{
    sbuffer_node_t *dummy;
    if ((sbuffer == NULL) || (*sbuffer == NULL))
    {
        return SBUFFER_FAILURE;
    }
    while ((*sbuffer)->head)
    {
        dummy = (*sbuffer)->head;
        (*sbuffer)->head = (*sbuffer)->head->next;
        free(dummy);
    }
    free(*sbuffer);
    *sbuffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *sbuffer, sensor_data_t *data)
{
    sbuffer_node_t *dummy;
    if (sbuffer == NULL)
        return SBUFFER_FAILURE;
    if (sbuffer->head == NULL)
        return SBUFFER_NO_DATA;
    *data = sbuffer->head->data;
    dummy = sbuffer->head;
    pthread_mutex_lock(&mutex_sbuffer);
    if (sbuffer->head == sbuffer->tail) // sbuffer has only one node
        sbuffer->head = sbuffer->tail = NULL;
    else // sbuffer has many nodes empty
        sbuffer->head = sbuffer->head->next;
    sbuffer->count--;
    // pthread_cond_wait(&cond_signal, &mutex_sbuffer);
    // sched_yield();
    pthread_mutex_unlock(&mutex_sbuffer);
    free(dummy);
    return SBUFFER_SUCCESS;
}

int sbuffer_read(sbuffer_t *sbuffer, sensor_data_t *data)
{
    if (sbuffer == NULL)
        return SBUFFER_FAILURE;
    if (sbuffer->head == NULL)
        return SBUFFER_NO_DATA;
    pthread_mutex_lock(&mutex_sbuffer);
    *data = sbuffer->head->data;
    // pthread_cond_signal(&cond_signal);
    sched_yield();
    pthread_mutex_unlock(&mutex_sbuffer);
    read_done = true;
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *sbuffer, sensor_data_t *data)
{
    sbuffer_node_t *dummy;
    if (sbuffer == NULL)
        return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL)
        return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    if (sbuffer->tail == NULL) // sbuffer empty (sbuffer->head should also be NULL
    {
        sbuffer->head = sbuffer->tail = dummy;
    }
    else // sbuffer not empty
    {
        sbuffer->tail->next = dummy;
        sbuffer->tail = sbuffer->tail->next;
    }
    sbuffer->count++;
    pthread_cond_signal(&cond_signal);
    pthread_mutex_unlock(&mutex_sbuffer);
    return SBUFFER_SUCCESS;
}
