/**
 * \author Wentai Ye
 */

#include "dplist.h"

// definition of error codes
#define DPLIST_NO_ERROR 0      // no error
#define DPLIST_MEMORY_ERROR 1  // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 // error due to a list operation applied on a NULL list

// definition of error messages
#ifdef DEBUG
#define DEBUG_PRINTF(...)                                                                   \
    do                                                                                      \
    {                                                                                       \
        fprintf(stderr, "\nIn %s - function %s at line %d:", __FILE__, __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                                       \
        fflush(stderr);                                                                     \
    } while (0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif

// definition of error handling
#define DPLIST_ERR_HANDLER(condition, err_code)   \
    do                                            \
    {                                             \
        if ((condition))                          \
            DEBUG_PRINTF(#condition " failed\n"); \
        assert(!(condition));                     \
    } while (0)

// The real definition of struct node
struct dplist_node
{
    dplist_node_t *prev, *next;
    void *element;
};

// The real definition of struct list
struct dplist
{
    dplist_node_t *head;
    void *(*element_copy)(void *src_element);
    void (*element_free)(void **element);
    int (*element_compare)(void *x, void *y);
};

dplist_t *dpl_create( // callback functions
    void *(*element_copy)(void *src_element),
    void (*element_free)(void **element),
    int (*element_compare)(void *x, void *y))
{
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element)
{
    if (*list == NULL)
        return;
    dplist_node_t *current = (*list)->head;
    dplist_node_t *next = NULL;
    while (current != NULL)
    {
        next = current->next;
        if (free_element)
            (*list)->element_free(&current->element);
        free(current);
        current = next;
    }
    free(*list);
    *list = NULL;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy)
{
    if (list == NULL)
        return NULL;
    dplist_node_t *new_node = malloc(sizeof(dplist_node_t));
    DPLIST_ERR_HANDLER(new_node == NULL, DPLIST_MEMORY_ERROR);
    if (insert_copy)
        new_node->element = list->element_copy(element);
    else
        new_node->element = element;
    if (index == 0)
    {
        new_node->next = list->head;
        new_node->prev = NULL;
        if (list->head != NULL)
            list->head->prev = new_node;
        list->head = new_node;
    }
    else
    {
        dplist_node_t *current = list->head;
        int i = 0;
        while (i < index - 1 && current != NULL)
        {
            current = current->next;
            i++;
        }
        if (current == NULL)
        {
            free(new_node);
            return NULL;
        }
        new_node->next = current->next;
        new_node->prev = current;
        if (current->next != NULL)
            current->next->prev = new_node;
        current->next = new_node;
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element)
{
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    dplist_node_t *current = list->head;
    int i = 0;
    while (i < index && current != NULL)
    {
        current = current->next;
        i++;
    }
    if (current == NULL)
        return NULL;
    if (current->prev == NULL)
        list->head = current->next;
    else
        current->prev->next = current->next;
    if (current->next != NULL)
        current->next->prev = current->prev;
    if (free_element)
        list->element_free(&current->element);
    free(current);
    return list;
}

int dpl_size(dplist_t *list)
{
    if (list == NULL)
        return -1;
    dplist_node_t *current = list->head;
    int size = 0;
    while (current != NULL)
    {
        size++;
        current = current->next;
    }
    return size;
}

void *dpl_get_element_at_index(dplist_t *list, int index)
{
    if (list == NULL)
        return NULL;
    dplist_node_t *current = list->head;
    int i = 0;
    while (i < index && current != NULL)
    {
        current = current->next;
        i++;
    }
    if (current == NULL)
        return NULL;
    return current->element;
}

int dpl_get_index_of_element(dplist_t *list, void *element)
{
    if (list == NULL)
        return -1;
    dplist_node_t *current = list->head;
    int i = 0;
    while (current != NULL)
    {
        if (list->element_compare(current->element, element) == 0)
            return i;
        current = current->next;
        i++;
    }
    return -1;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index)
{
    if (list == NULL)
        return NULL;
    dplist_node_t *current = list->head;
    int i = 0;
    while (i < index && current != NULL)
    {
        current = current->next;
        i++;
    }
    if (current == NULL)
        return NULL;
    return current;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference)
{
    if (list == NULL || reference == NULL)
        return NULL;
    DPLIST_ERR_HANDLER(reference == NULL, DPLIST_INVALID_ERROR);
    dplist_node_t *current = list->head;
    while (current != NULL)
    {
        if (current == reference)
            return current->element;
        current = current->next;
    }
    return NULL;
}