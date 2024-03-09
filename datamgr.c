#define _XOPEN_SOURCE 600
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <assert.h>
#include <pthread.h>
#include <sys/select.h>
#include <unistd.h> 

#include "config.h"
#include "datamgr.h"
#include "./lib/dplist.h"
#include "connmgr.h"
#include "sbuffer.h"


#define SET_MIN_TEMP 10
#define SET_MAX_TEMP 20

#define READ_END 0
#define WRITE_END 1
#define length 100
#define TIME_OUT 5
extern int conn_counter;

#define SENSOR_ERROR_INVALID_SENSOR_ID 11
#define SENSOR_ERROR_NON_EXISTING_FILES 12

extern int token;
extern pthread_mutex_t mut;
extern pthread_cond_t cond;
extern int fd[2];



dplist_t *list = NULL;

void err_handler_sensor_node_t(int error)
{
	switch(error)
	{
		case SENSOR_ERROR_INVALID_SENSOR_ID:
		printf("\n Program execution stopped after invalid sensor id given");
		assert(1==0);
		break;

		case SENSOR_ERROR_NON_EXISTING_FILES:
		printf("\n Program execution stopped after non existing file given");
		assert(1==0);
		break;

    	default: 
    	break;
	}
}

//#if 0
void print_list(dplist_t* tmp)
{

	sensor_node_t *node;
	int i, num;

	num = dpl_size(tmp);
	for ( i = 0; i < num; i++)
	{
		node=(sensor_node_t*) dpl_get_element_at_index(tmp, i );
		printf("In node %d, room_id %u, sensor_id %u, running_avg%lf, last_modified %ld\n",i,node->room_id,node->sensor_id,node->running_avg,node->last_modified);
	}
}
//#endif




void datamgr_parse_sensor_files(FILE * fp_sensor_map, sbuffer_t *sbuffer)
{
	uint16_t room_id, sensor_id; 
	int i, num;
	char log[length];
	
	if( fp_sensor_map == NULL)
		fprintf(stdout, " Files Reading Error1: %s\n", strerror(errno));

	list = dpl_create(element_copy, element_free, element_compare);
	sensor_node_t * node;
	node = malloc(sizeof(sensor_node_t));	
			
	while(!feof(fp_sensor_map))
	{
		// start to get the room_id and sensor_id from 'fp_sensor_map'	
		fscanf(fp_sensor_map,"%" SCNd16 "%" SCNd16, &room_id, &sensor_id);
		// test whether the sensor_id already exists.

		i = dpl_get_index_of_element(list, (void*)&sensor_id);

		if(i != -1)
			continue;    

		node->sensor_id = sensor_id;
		node->room_id = room_id;
		node->running_avg = 0;	

		for(num=0; num<RUN_AVG_LENGTH; num++)
			node->data_list[num]=0;	//set all 0 node->data_list[RUN_AVG_LENGTH]={0};
		dpl_insert_at_index(list, (void*)node, 0, true);    
	}
	//print_list(list);

	
	sensor_data_t data;
	data.id = 1;

    time_t start = time(NULL);
    time_t end = time(NULL);

	while(difftime(end,start) < TIME_OUT)
	{
		if((data.id != 0)||(conn_counter != 0))
		{
			start = time(NULL);	
			pthread_mutex_lock(&mut);

			while(token == 0)
			{
				pthread_cond_wait(&cond, &mut);
			}

			i = sbuffer_get(sbuffer, &data);

			if(i != 0)
			{
				//token = 0;
				pthread_mutex_unlock(&mut);
				//pthread_cond_signal(&cond);			
			}
				//pthread_mutex_unlock(&mut);
			else
			{	
				if(data.id == 0)
				{
					token = 0;
					pthread_mutex_unlock(&mut);
					pthread_cond_signal(&cond);

					//if(conn_counter != 0)
					//	continue;
					//else
					//	break;
					end = time(NULL);
					continue;

				}	

				i = dpl_get_index_of_element(list,(void*)&(data.id));
				if((i == -1)&&(data.id != 0))
				{
					//err_handler_sensor_node_t(SENSOR_ERROR_INVALID_SENSOR_ID);
					sprintf(log, "Received sensor data with invalid sensor node ID <%d>",data.id);
					write(fd[WRITE_END], log, strlen(log)+1);

					token = 0;
					pthread_cond_signal(&cond);	
					pthread_mutex_unlock(&mut);
					end = time(NULL);
					continue;
				}
				
				sensor_node_t *tmp = dpl_get_element_at_index(list,i);
				tmp->last_modified = data.ts;

				int size = RUN_AVG_LENGTH;
				for(num=0; num<RUN_AVG_LENGTH; num++)
				{
					if(tmp->data_list[num]==0)
					{
						size = num;
						break;
					}
				}

				if(size<RUN_AVG_LENGTH)   //datalist still has space
					tmp->data_list[size]= data.value;
				else	//data_list is  full
				{
					//update the data_list
					for(num=1; num<RUN_AVG_LENGTH; num++)//move
						tmp->data_list[num-1] = tmp->data_list[num];
					tmp->data_list[RUN_AVG_LENGTH-1]=data.value;//give the fifth value
				}
				
				sensor_value_t running_sum = 0;

				for(num=0; num < RUN_AVG_LENGTH; num++)
					running_sum += tmp->data_list[num];
					//get the average value
				tmp->running_avg = running_sum/(size+1);
				//printf("%f\n",tmp->running_avg);
					
					//check the MAX and MIN


				if(tmp->running_avg > SET_MAX_TEMP)
				{
					//fprintf(stderr,"In room %u sensor %u temperature %lf is too high!\n",tmp->room_id,tmp->sensor_id,tmp->running_avg);
					sprintf(log, "Sensor node %d reports it's too hot (avg temp = %f)",tmp->sensor_id,tmp->running_avg);//fork
					write(fd[WRITE_END], log, strlen(log)+1);//fork
				}

				if(tmp->running_avg < SET_MIN_TEMP)
				{
					//fprintf(stderr,"The room %u sensor %u temperature %lf is too low!\n",tmp->room_id,tmp->sensor_id,tmp->running_avg);
					sprintf(log, "Sensor node %d reports it's too cold (avg temp = %f)",tmp->sensor_id,tmp->running_avg);//fork
					write(fd[WRITE_END], log, strlen(log)+1);//fork		
				}

				token = 0;
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mut);
			}
			//start = time(NULL);
		}	
		end = time(NULL);
	}
}
#if 0
	while ((data.id != 0)||(conn_counter != 0))
	{	
		pthread_mutex_lock(&mut);

		while(token == 0)
		{
			pthread_cond_wait(&cond, &mut);
		}

		i = sbuffer_get(sbuffer, &data);

		if(i != 0)
		{
			//token = 0;
			pthread_mutex_unlock(&mut);
			//pthread_cond_signal(&cond);			
		}
			//pthread_mutex_unlock(&mut);
		else
		{	
			if(data.id == 0)
			{
				token = 0;
				pthread_mutex_unlock(&mut);
				pthread_cond_signal(&cond);

				if(conn_counter != 0)
					continue;
				else
					break;
			}	

			i = dpl_get_index_of_element(list,(void*)&(data.id));
			if((i == -1)&&(data.id != 0))
			{
				//err_handler_sensor_node_t(SENSOR_ERROR_INVALID_SENSOR_ID);
				sprintf(log, "Received sensor data with invalid sensor node ID <%d>",data.id);
				write(fd[WRITE_END], log, strlen(log)+1);

				token = 0;
				pthread_cond_signal(&cond);	
				pthread_mutex_unlock(&mut);
				continue;
			}
			
			sensor_node_t *tmp = dpl_get_element_at_index(list,i);
			tmp->last_modified = data.ts;

			int size = RUN_AVG_LENGTH;
			for(num=0; num<RUN_AVG_LENGTH; num++)
			{
				if(tmp->data_list[num]==0)
				{
					size = num;
					break;
				}
			}

			if(size<RUN_AVG_LENGTH)   //datalist still has space
				tmp->data_list[size]= data.value;
			else	//data_list is  full
			{
				//update the data_list
				for(num=1; num<RUN_AVG_LENGTH; num++)//move
					tmp->data_list[num-1] = tmp->data_list[num];
				tmp->data_list[RUN_AVG_LENGTH-1]=data.value;//give the fifth value
			}
			
			sensor_value_t running_sum = 0;

			for(num=0; num < RUN_AVG_LENGTH; num++)
				running_sum += tmp->data_list[num];
				//get the average value
			tmp->running_avg = running_sum/(size+1);
			//printf("%f\n",tmp->running_avg);
				
				//check the MAX and MIN


			if(tmp->running_avg > SET_MAX_TEMP)
			{
				//fprintf(stderr,"In room %u sensor %u temperature %lf is too high!\n",tmp->room_id,tmp->sensor_id,tmp->running_avg);
				sprintf(log, "Sensor node %d reports it's too hot (avg temp = %f)",tmp->sensor_id,tmp->running_avg);//fork
				write(fd[WRITE_END], log, strlen(log)+1);//fork
			}

			if(tmp->running_avg < SET_MIN_TEMP)
			{
				//fprintf(stderr,"The room %u sensor %u temperature %lf is too low!\n",tmp->room_id,tmp->sensor_id,tmp->running_avg);
				sprintf(log, "Sensor node %d reports it's too cold (avg temp = %f)",tmp->sensor_id,tmp->running_avg);//fork
				write(fd[WRITE_END], log, strlen(log)+1);//fork		
			}

			token = 0;
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mut);
		}
	}	
}
#endif
	//return;



void datamgr_free()
{
  dpl_free( &list, true);
  list=NULL;
}


uint16_t datamgr_get_room_id(sensor_id_t sensor_id)
{
	int i = dpl_get_index_of_element(list, (void*)&sensor_id);
	if(i == -1)
		return -1;

	sensor_node_t *tmp = dpl_get_element_at_index(list, i);
	return tmp->room_id;
}


sensor_value_t datamgr_get_avg(sensor_id_t sensor_id)
{
	int i = dpl_get_index_of_element(list, (void*)&sensor_id);
	if(i == -1)
		return -1;

	sensor_node_t *tmp = dpl_get_element_at_index(list, i);
	return tmp->running_avg;
}


time_t datamgr_get_last_modified(sensor_id_t sensor_id)
{
	int i = dpl_get_index_of_element(list, (void*)&sensor_id);
	if(i == -1)
		return -1;

	sensor_node_t *tmp = dpl_get_element_at_index(list, i);
	return tmp->last_modified;
}



int datamgr_get_total_sensors()
{
	return dpl_size(list);
}