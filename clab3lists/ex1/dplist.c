/**
 * \author Wentai Ye
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dplist.h"


 /*
 * definition of error codes
 **/
#define DPLIST_NO_ERROR 0      // no error
#define DPLIST_MEMORY_ERROR 1  // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 // error due to a list operation applied on a NULL list

#ifdef DEBUG
#define DEBUG_PRINTF(...)                                                                    \
    do                                                                                       \
    {                                                                                        \
        fprintf(stderr, "\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                                                        \
        fflush(stderr);                                                                      \
    } while (0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif

#define DPLIST_ERR_HANDLER(condition, err_code)   \
    do                                            \
    {                                             \
        if ((condition))                          \
            DEBUG_PRINTF(#condition " failed\n"); \
        assert(!(condition));                     \
    } while (0)

 /**
  * The real definition of struct list / struct node
  */
struct dplist_node
{
    dplist_node_t* prev, * next;
    element_t element;
};

struct dplist
{
    dplist_node_t* head;
    // more fields will be added later
};

dplist_t* dpl_create()
{
    dplist_t* list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    // pointer drawing breakpoint
    list->head = NULL;
    list->head->next = NULL;
    return list;
}

void dpl_free(dplist_t** list)
{
    // TODO: add your code here
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    dplist_node_t* node = (*list)->head;
    while (node != NULL)
    {
        dplist_node_t* temp = node;
        node = node->next;
        free(temp);
    }
    free(*list);
    list = NULL;
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/

dplist_t* dpl_insert_at_index(dplist_t* list, element_t element, int index)
{
    dplist_node_t* ref_at_index, * list_node;
    if (list == NULL)
        return NULL;
    list_node = malloc(sizeof(dplist_node_t));
    DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    list_node->element = element;
    // pointer drawing breakpoint
    if (list->head == NULL)
    { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    }
    else if (index <= 0)
    { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    }
    else
    {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list))
        { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            // ref_at_index->prev->next = list_node;
            list_node->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        }
        else
        { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t* dpl_remove_at_index(dplist_t* list, int index)
{
    // TODO: add your code here
    dplist_node_t* ref_at_index;
    if (list == NULL)
        return NULL;
    if (list->head == NULL)
        return list;
}

int dpl_size(dplist_t* list)
{
    // TODO: add your code here
    if (list == NULL)
        return -1;
    if (list->head == NULL)
        return 0;
    dplist_node_t* node = list->head;
    int count = 0;
    while (node != NULL)
    {
        count++;
        node = node->next;
    }
    return count;
}

dplist_node_t* dpl_get_reference_at_index(dplist_t* list, int index)
{
    int count;
    dplist_node_t* dummy;
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    if (list->head == NULL)
        return NULL;
    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++)
    {
        if (count >= index)
            return dummy;
    }
    return dummy;
}

element_t dpl_get_element_at_index(dplist_t* list, int index)
{
    // TODO: add your code here
    dplist_node_t* ref_at_index;
    if (list == NULL)
        return -1;
    if (list->head == NULL)
        return -1;
    ref_at_index = dpl_get_reference_at_index(list, index);
    return ref_at_index->element;
}

int dpl_get_index_of_element(dplist_t* list, element_t element)
{
    // TODO: add your code here
    dplist_node_t* dummy;
    int count;
    if (list == NULL)
        return -1;
    if (list->head == NULL)
        return -1;
    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++)
    {
        if (dummy->element == element)
            return count;
    }
}
