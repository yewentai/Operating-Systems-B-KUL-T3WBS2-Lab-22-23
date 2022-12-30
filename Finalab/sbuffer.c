/**
 * \Juncheng Zhu
 */

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include "config.h"


int sbuffer_init(sbuffer_t **buffer) 
{
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) 
{
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) 
        return SBUFFER_FAILURE;

    while (((*buffer)->head) != NULL) 
    {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) 
{
    sbuffer_node_t *dummy;
    if (buffer == NULL) 
        return SBUFFER_FAILURE;
    if (buffer->head == NULL) 
        return SBUFFER_NO_DATA;
    *data = buffer->head->data;
    dummy = buffer->head;
    if (buffer->head == buffer->tail) // buffer has only one node
        buffer->head = buffer->tail = NULL;
    else  // buffer has many nodes empty
        buffer->head = buffer->head->next;
    free(dummy);
    return SBUFFER_SUCCESS;
}

int sbuffer_get(sbuffer_t *buffer, sensor_data_t *data) 
{
    if (buffer == NULL) 
        return SBUFFER_FAILURE;
    if (buffer->head == NULL) 
        return SBUFFER_NO_DATA;
    *data = buffer->head->data;
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
        buffer->head = buffer->tail = dummy;
    else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    return SBUFFER_SUCCESS;
}