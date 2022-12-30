/**
 * \author Wentai Ye
 */

#include "sbuffer.h"

pthread_mutex_t mutex_pipe;         // Mutex for the log file
pthread_mutex_t mutex_sbuffer_head; // Mutex for the shared buffer
pthread_mutex_t mutex_sbuffer_tail; // Mutex for the shared buffer
pthread_cond_t cond;                // Condition variable for the storage manager thread

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
    pthread_cond_init(&cond, NULL);                // Initialize the condition variable
    pthread_mutex_init(&mutex_sbuffer_head, NULL); // Initialize the mutex for the shared buffer
    pthread_mutex_init(&mutex_sbuffer_tail, NULL); // Initialize the mutex for the shared buffer
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **sbuffer)
{
    if ((sbuffer == NULL) || (*sbuffer == NULL))
    {
        return SBUFFER_FAILURE;
    }
    while ((*sbuffer)->head)
    {
        sbuffer_node_t *dummy;
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
    if (sbuffer == NULL)
        return SBUFFER_FAILURE;
    if (sbuffer->head == NULL)
    {
        return SBUFFER_NO_DATA;
    }
    *data = sbuffer->head->data;
    pthread_mutex_lock(&mutex_sbuffer_head);

    if (sbuffer->head == sbuffer->tail) // sbuffer has only one node
        sbuffer->head = sbuffer->tail = NULL;
    else // sbuffer has many nodes empty
        sbuffer->head = sbuffer->head->next;
    sbuffer->count--;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex_sbuffer_head);
    return SBUFFER_SUCCESS;
}

int sbuffer_read(sbuffer_t *sbuffer, sensor_data_t *data)
{
    if (sbuffer == NULL)
        return SBUFFER_FAILURE;
    if (sbuffer->head == NULL)
    {
        return SBUFFER_NO_DATA;
    }
    *data = sbuffer->head->data;
    pthread_cond_wait(&cond, &mutex_sbuffer_head);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *sbuffer, sensor_data_t *data)
{
    if (sbuffer == NULL)
        return SBUFFER_FAILURE;

    sbuffer_node_t *dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL)
        return SBUFFER_FAILURE;

    pthread_mutex_lock(&mutex_sbuffer_tail);
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
    pthread_mutex_unlock(&mutex_sbuffer_tail);
    sbuffer->count++;
    return SBUFFER_SUCCESS;
}
