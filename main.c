#define _XOPEN_SOURCE 600
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <inttypes.h>
#include <assert.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdint.h>
#include <time.h>

#include "config.h"
#include "datamgr.h"
#include "connmgr.h"
#include "./lib/dplist.h"
#include "./lib/tcpsock.h"
#include "sbuffer.h"
#include "sensor_db.h"


#define FILE_ERROR(fp, error_msg)    do {               \
                      if ((fp)==NULL) {                 \
                        printf("%s\n",(error_msg));     \
                        exit(EXIT_FAILURE);             \
                      }                                 \
                    } while(0)

sbuffer_t sbuffer;
pthread_mutex_t mut;
pthread_cond_t cond;
#define PORT 5678
#define MAX_CONN 10  // state the max. number of connections the server will handle before exiting
int conn_counter = 0;
#define length 100
#define READ_END 0
#define WRITE_END 1
#define TIME_OUT 5
int fd[2];
int token = 1;
pthread_t tid[3];



static void * pthread_connmgr(void *p) 
{
    tcpsock_t *server;
    tcpsock_t *client;
    sensor_data_t data;
    pthread_t tid[MAX_CONN];
    int err;
    //char log[length];//fork

    printf("Test server is started\n");
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) 
        exit(EXIT_FAILURE);
    
    time_t start = time(NULL);
    time_t end = time(NULL);
#if 0
    while(difftime(end,start) < TIME_OUT)
    {
        if(conn_counter < MAX_CONN) 
        {
            if (tcp_wait_for_connection(server, &client) == TCP_NO_ERROR) 
                printf("Incoming client connection\n");            
            else
                exit(EXIT_FAILURE);

            err = pthread_create(&tid[conn_counter],NULL,pthread_receive,(void *)client);
            if (err)
            {
                perror("pthread_create()\n");
                exit(1);              
            }
            if(conn_counter != 0)
                start = time(NULL);
        }
        end = time(NULL);
    }
#endif
#if 1
    while(conn_counter < MAX_CONN) 
    {
        if (tcp_wait_for_connection(server, &client) == TCP_NO_ERROR) 
            printf("Incoming client connection\n");            
        else
            exit(EXIT_FAILURE);

        err = pthread_create(&tid[conn_counter],NULL,pthread_receive,(void *)client);
        if (err)
        {
            perror("pthread_create()\n");
            exit(1);              
        }
    }
#endif
    if (tcp_close(&server) != TCP_NO_ERROR) 
        exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");
    return 0;
}





static void * pthread_stormgr(void * p)
{
    sensor_data_t *data;
    data = malloc(sizeof(sensor_data_t));   
    data->id = 1;
    char log[length];
    int signal = 1;
    
    //pthread_mutex_lock(&mut);
    //printf("******************\n");


    time_t start = time(NULL);
    time_t end = time(NULL);       

    while(difftime(end,start) < TIME_OUT)
    {
        if((data->id != 0)||(conn_counter != 0))
        {
            start = time(NULL);
            //printf("********%d**********",data->id);
            pthread_mutex_lock(&mut);

            if(sbuffer.head == NULL)
                pthread_mutex_unlock(&mut);
            else
            {
                while(token == 1)
                {
                    pthread_cond_wait(&cond,&mut);
                }
                
                int remove = sbuffer_remove(&sbuffer,data);

                if(remove<0)
                {
                    perror("sbuffer_remove()\n");
                    exit(1);
                }  

                //FILE * fp_text = fopen("sensor_data_out_text", "a"); 
                FILE * fp_csv = fopen("data.csv","a");                 

                if((signal == 1) && (fp_csv != NULL))
                {
                    strcpy(log,"A new data.csv file has been created.");  
                    write(fd[WRITE_END], log, strlen(log)+1);
                    signal = 0;
                }

                if(data->id != 0)
                {
                    printf("<READ> id:%d\ttemp:%f\tts:%ld\n",data->id,data->value,data->ts);
                    //fprintf(fp_text,"%" PRIu16 " %g %ld\n", data->id,data->value,data->ts);
                    fprintf(fp_csv,"%d,%f,%ld\n", data->id,data->value,data->ts);

                    sprintf(log,"Data insertion from sensor %d succeeded.", data->id);  
                    write(fd[WRITE_END], log, strlen(log)+1);
                }


                //fclose(fp_text); 
                fclose(fp_csv);             

                token = 1;
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&mut);     
            }
        }   
        end = time(NULL);    
    }

    strcpy(log,"The data.csv file has been closed.");  
    write(fd[WRITE_END], log, strlen(log)+1);

    //printf("pthread_stormgr is closed\n");
    pthread_cancel(tid[2]);

    pthread_exit(NULL);
}





static void * pthread_datamgr(void * p)
{
    FILE *fp = fopen("room_sensor.map","r");
    datamgr_parse_sensor_files(fp, &sbuffer);
    //printf("pthread_datamgr is closed\n");
    fclose(fp);
    datamgr_free();
    pthread_exit(NULL);
}



int main()
{
    pid_t pid;
    if(pipe(fd)<0)
    {
        perror("pipe()");
        return -1;       
    }

    fflush(NULL);
    pid = fork();

    if(pid < 0)
    {
        perror("fork()");
        return -2;       
    }

    
    if(pid == 0)
    {
        int num = 1;        
        close(fd[WRITE_END]);
#if 0
        clock_t start_time = clock();
        clock_t end_time = clock();
        //printf("%ld\n",(end_time-start_time));
        //printf("%ld\n",CLOCKS_PER_SEC);

        while(((double)(end_time - start_time)/CLOCKS_PER_SEC) < (double)TIME_OUT)
        {  
            FILE *fp = fopen("gateway.log", "a"); 
            int ref = insert_log(fp,num); 
            //printf("%d\n",ref);     
            if (ref > 0)
            {
                num++;
                start_time = clock();
                //fclose(fp);
            }
            fclose(fp);
            end_time = clock();
            printf("_________%f________\n",(double)(end_time - start_time));
            printf("~~~~~~~~~%f~~~~~~~~\n",(double)(end_time - start_time)/CLOCKS_PER_SEC);
        }
#endif
#if 1
        time_t start = time(NULL);
        time_t end = time(NULL);
        char buff[100];       

        while(difftime(end,start) < TIME_OUT)
        {
            FILE *fp = fopen("gateway.log", "a");
            int ref = insert_log(fp,num);
            if (ref > 1)
            {
                insert_log(fp,num);
                num++;
                start = time(NULL);
            }
            fclose(fp);
            end = time(NULL);
        }
#endif  
        close(fd[READ_END]);
        return 0;       
    }
    else
    {
        close(fd[READ_END]);

        sbuffer_t *sbuffer_p = &sbuffer;
        sensor_data_t data;
        //pthread_t tid[3];//0 is write, 1&2 is read
        
        int init = sbuffer_init(&sbuffer_p);
        if(init < 0)
        {
            perror("sbuffer_init()\n");
            exit(1);
        }

        pthread_mutex_init(&mut, NULL);
        pthread_cond_init(&cond, NULL);
  
        int err[3];
        err[2] = pthread_create(&tid[2],NULL,pthread_connmgr,NULL);
        err[1] = pthread_create(&tid[1],NULL,pthread_stormgr,NULL);
        err[0] = pthread_create(&tid[0],NULL,pthread_datamgr,NULL); 

        for(int i = 0;i < 3;i++)
        {   
            if (err[i])
            {
                perror("pthread_create()\n");
                exit(1);              
            }
        }

        //sleep(1);
#if 0
        int free = sbuffer_free(&sbuffer_p);
        //printf("&&&&&&&&&&&&&\n");

        if(free < 0)
        {
            perror("sbuffer_free()\n");
            exit(1);
        }
#endif
        for(int i = 0;i < 3;i++)
        {
            pthread_join(tid[i],NULL);
            //sleep(1);
        }


        int free = sbuffer_free(&sbuffer_p);
        //printf("&&&&&&&&&&&&&\n");

        if(free < 0)
        {
            perror("sbuffer_free()\n");
            exit(1);
        }

        pthread_mutex_destroy(&mut);
        pthread_cond_destroy(&cond);

        close(fd[WRITE_END]);

        wait(NULL);
        return 0;
    }
}


