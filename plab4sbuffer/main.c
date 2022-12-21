#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "config.h"
#include "sbuffer.h"
#include "file_creator.h"

#define THREAD_NUM 3
#define SRC_FILE "./sensor_data"
#define DST_FILE "./sensor_data_out"

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // create a mutex
static void *srctobuf(void *buffer);
static void *buftodst(void *buffer);

int main()
{
    system("timedatectl set-timezone Europe/Brussels"); // Set the correct time zone

    /**
     * Create the source file: sensor_data and
     * the destination file: sensor_data_out
     **/
    srccreate();
    dstcreate();

    /**
     * Initialize the buffer
     */
    sbuffer_t *buffer = NULL;
    sbuffer_init(&buffer);

    /**
     * Create the threads
     */
    int i, ret;
    pthread_t tid[THREAD_NUM];
    ret = pthread_create(tid + 0, NULL, srctobuf, buffer);
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create: %s\n", strerror(ret));
        exit(1);
    }
    ret = pthread_create(tid + 1, NULL, buftodst, buffer);
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create: %s\n", strerror(ret));
        exit(1);
    }
    ret = pthread_create(tid + 2, NULL, buftodst, buffer);
    if (ret != 0)
    {
        fprintf(stderr, "pthread_create: %s\n", strerror(ret));
        exit(1);
    }

    for (i = 0; i < THREAD_NUM; i++)
    { // Make calling thread wait for termination of the thread
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&mutex); // Destroy the mutex.
    sbuffer_free(&buffer);         // Free the buffer
    exit(0);
}

static void *srctobuf(void *buffer)
{
    FILE *fp = fopen(SRC_FILE, "r");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    sensor_data_t *data = malloc(sizeof(sensor_data_t));

    while (!feof(fp))
    {
        fread(&data->id, sizeof(sensor_id_t), 1, fp);
        fread(&data->value, sizeof(sensor_value_t), 1, fp);
        fread(&data->ts, sizeof(sensor_ts_t), 1, fp);
        // printf("id: %hu,value: %.1lf,time: %ld\n", data->id, data->value, data->ts);
        sbuffer_insert(buffer, data);
    }

    // while (fread(&data, sizeof(sensor_data_t), 1, fp) == 1)
    // {
    //     pthread_mutex_lock(&mutex);
    //     printf("id: %hu,value: %.1lf,time: %ld\n", data.id, data.value, data.ts);
    //     sbuffer_insert(buffer, &data);
    //     pthread_mutex_unlock(&mutex);
    // }

    fclose(fp);

    pthread_exit(NULL);
}

static void *buftodst(void *buffer)
{

    FILE *fp = fopen(DST_FILE, "a");
    if (fp == NULL)
    {
        perror("fopen");
        exit(1);
    }

    sensor_data_t data;
    while (sbuffer_remove(buffer, &data) == SBUFFER_SUCCESS)
    {
        pthread_mutex_lock(&mutex);
        fwrite(&data, sizeof(sensor_data_t), 1, fp);
        // printf("id: %hu,value: %.1lf,time: %ld\n", data.id, data.value, data.ts);
        pthread_mutex_unlock(&mutex);
    }

    fclose(fp);
    pthread_exit(NULL);
}