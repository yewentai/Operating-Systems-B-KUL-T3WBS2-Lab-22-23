/**
 * \author Juncheng Zhu
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "dplist.h"
#include "../datamgr.h"

/*
 * definition of error codes
 */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 

#ifdef DEBUG
#define DEBUG_PRINTF(...) 									                                        \
        do {											                                            \
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	    \
            fprintf(stderr,__VA_ARGS__);								                            \
            fflush(stderr);                                                                         \
                } while(0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition, err_code)                         \
    do {                                                                \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");      \
            assert(!(condition));                                       \
        } while(0)


/*
 * The real definition of struct list / struct node
 */




void* element_copy(void* element)
{
	sensor_node_t *node = (sensor_node_t *)malloc( sizeof(sensor_node_t));
	
	//*node = *(sensor_node_t *) element;  
	node->room_id = ((sensor_node_t *)element)->room_id;
	node->sensor_id = ((sensor_node_t *)element)->sensor_id;
	node->running_avg = ((sensor_node_t *)element)->running_avg;
	node->last_modified = ((sensor_node_t *)element)->last_modified;
	for(int i = 0; i < RUN_AVG_LENGTH; i++)
    {
		node->data_list[i] = ((sensor_node_t *)element)->data_list[i];
	}		
	return (void*) node;
}

void element_free(void** element)
{
	//free(((sensor_node_t *)element)->data_list);
	free( *element);
	*element = NULL;
}

int element_compare(void *x, void* y)
{
    if(((sensor_node_t *)x)->sensor_id < (*((sensor_id_t *)y)))
    {
        return -1;
    }

	if(((sensor_node_t *)x)->sensor_id == (*((sensor_id_t *)y)))
    {
        return 0;
    }
      
	if(((sensor_node_t *)x)->sensor_id > (*((sensor_id_t *)y)))
        return 1;  
    
    return 0;
}


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(dplist_t));
    assert(list!= NULL);

    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}




void dpl_free(dplist_t **list, bool free_element) 
    {
        if((*list == NULL) || (list == NULL))
            return;
        else
        {
            dplist_node_t *cur = NULL, *next = NULL;

            for(cur = (*list)->head; cur != NULL; cur = next)
            {
                next = cur->next;
                if(free_element == true)
                    (*list)->element_free(&(cur->element));
                else;

                free(cur);             
            }

            //free((*list)->head);
            free(*list);
            *list = NULL;
            return;
        }

    }





dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) 
{
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));
    assert(list_node != NULL);

    if(insert_copy == true)
        list_node->element = list->element_copy(element);
    else
        list_node->element = element;

    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } 
    else 
    {
        if (index <= 0) 
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
                ref_at_index->prev->next = list_node;
                ref_at_index->prev = list_node;
                // pointer drawing breakpoint
            } 
            else 
            { // covers case 3
                list_node->next = NULL;
                list_node->prev = ref_at_index;
                ref_at_index->next = list_node;
                // pointer drawing breakpoint
            }
        }
    }

    //if(insert_copy == true)
    //    free(list_node->element);
    //free(list_node);

    return list;
}







dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) 
{
    if(list == NULL)
        return NULL;

    if(list->head == NULL)
        return list;

    if(list->head->next == NULL)
    {
        if(free_element == true)
           list->element_free(&(list->head->element)); 
        free(list->head);
        list->head = NULL;
        return list;
    }

    dplist_node_t *cur;
    cur = malloc(sizeof(struct dplist_node));

    if(index <= 0)
    {
        cur = list->head;
        list->head = cur->next;
        list->head->prev = NULL;

        if(free_element == true)
           list->element_free(&(cur->element)); 
        free(cur);
        cur = NULL;
        return list;
    }

    //dplist_node_t *cur;
    //cur = malloc(sizeof(struct dplist_node));
    //printf("#######\n");
    cur = dpl_get_reference_at_index(list, index);
    //printf("%d\n",((sensor_node_t *)cur->element)->room_id);
    assert(cur != NULL); 

    if(index >= (dpl_size(list)-1))
        cur->prev->next = NULL;
    else
    {
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
    }

    if(free_element == true)
        list->element_free(&(cur->element)); 
    free(cur);
    cur = NULL;
    return list;
}





int dpl_size(dplist_t *list)
{
    if(list == NULL)
        return -1;
    else
    {
        dplist_node_t *cur;

        int i = 0;
        for(cur = list->head; cur != NULL; i++)
            cur = cur->next;
        
        return i;
    }
}





void *dpl_get_element_at_index(dplist_t *list, int index) 
{
    if(list == NULL || list->head == NULL)
        return NULL;
    else
    {
        dplist_node_t *cur;   
        cur = dpl_get_reference_at_index(list,index);
        assert(cur != NULL);  
        
        return cur->element;  
    }
}





int dpl_get_index_of_element(dplist_t *list, void *element) 
{
    if((list == NULL)||(list->head == NULL))
        return -1; //NULL is not int
    else
    {
        dplist_node_t *cur;
        cur = malloc(sizeof(dplist_node_t));
        assert(cur != NULL);   
        
        int index = 0;
#if 0
        for(cur = list->head; (list->element_compare(cur->element,element) != 0) && (cur != NULL); index++)
        {
            cur = cur->next;
            printf("%d\n",index);
        }
#endif
        for(cur = list->head; cur != NULL; index++)
        {
            if(list->element_compare(cur->element,element) == 0)
                break;
            cur = cur->next;
        }

        if(cur == NULL)
        {
            //free(cur);
            return -1;
        }
        else
        {
            //free(cur);
            return index;
        }
    }    
}



dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) 
{
    int count = 0;
    int num = index;
    dplist_node_t *dummy;
    if(list == NULL)
        return NULL;
    if (list->head == NULL) 
        return NULL;
    if(num <= 0)
        return list->head;

    if(num >= sizeof(list))
        num = (sizeof(list)-1);

    for (dummy = list->head; dummy->next != NULL; dummy = dummy->next) 
    {
        if (count >= num) 
            return dummy;
        count++;
    }
    return dummy;
}




void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) 
{
    if((list == NULL) || (list->head == NULL) || (reference == NULL))
        return NULL;
    else
    {
        dplist_node_t *cur;
        cur = malloc(sizeof(struct dplist_node));
        assert(cur != NULL);

        for(cur = list->head; ((cur->prev != reference->prev) || (cur->next != reference->next)) && cur != NULL; cur = cur->next);
        if(cur == NULL)
            return NULL;
        else
            return cur->element;
    }
}
