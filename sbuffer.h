/**
 * \author Wentai Ye
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "config.h"

#ifndef _SBUFFER_H_
#define _SBUFFER_H_

#define SBUFFER_FAILURE -1
#define SBUFFER_SUCCESS 0
#define SBUFFER_NO_DATA 1

typedef struct sbuffer sbuffer_t;
typedef struct sbuffer_node sbuffer_node_t;

/**
 * Allocates and initializes a new shared sbuffer
 * \param sbuffer a double pointer to the sbuffer that needs to be initialized
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_init(sbuffer_t **sbuffer);

/**
 * All allocated resources are freed and cleaned up
 * \param sbuffer a double pointer to the sbuffer that needs to be freed
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_free(sbuffer_t **sbuffer);

/**
 * Removes the first sensor data in 'sbuffer' (at the 'head') and returns this sensor data as '*data'
 * If 'sbuffer' is empty, the function doesn't block until new sensor data becomes available but returns SBUFFER_NO_DATA
 * \param sbuffer a pointer to the sbuffer that is used
 * \param data a pointer to pre-allocated sensor_data_t space, the data will be copied into this structure. No new memory is allocated for 'data' in this function.
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_remove(sbuffer_t *sbuffer, sensor_data_t *data);

/**
 * Removes the first sensor data in 'sbuffer' (at the 'head') and returns this sensor data as '*data'
 * If 'sbuffer' is empty, the function blocks until new sensor data becomes available
 * \param sbuffer a pointer to the sbuffer that is used
 * \param data a pointer to pre-allocated sensor_data_t space, the data will be copied into this structure. No new memory is allocated for 'data' in this function.
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_read(sbuffer_t *sbuffer, sensor_data_t *data);

/**
 * Inserts the sensor data in 'data' at the end of 'sbuffer' (at the 'tail')
 * \param sbuffer a pointer to the sbuffer that is used
 * \param data a pointer to sensor_data_t data, that will be copied into the sbuffer
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occured
 */
int sbuffer_insert(sbuffer_t *sbuffer, sensor_data_t *data);

#endif //_SBUFFER_H_
