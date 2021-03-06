#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#include <fcntl.h>


#define THREAD_NUM 6
#define SEMAPHORE_NUMBER 1
#define SEMAPHORE_NAME "semProcess"   

sem_t *semaphore = NULL;

void *routine(void *args)
{
    sem_wait(semaphore);
    sleep(5);
    printf("Hello from Process 2 Thread number = %d\n", *(int*)args);
    sem_post(semaphore);

    free(args);
}


int main()
{
    pthread_t threads[THREAD_NUM];

    /*Depending on the initialization of the semaphore, multiple threads can acquire the semaphore and start executing*/
    /*If the semaphore count is 0 , the thread will have to wait until other threads release the semaphore*/
    //sem_init(&semaphore , 1 , 3);
    semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, SEMAPHORE_NUMBER);

    for(int i = 0; i < THREAD_NUM; i++)
    {
        int *a = malloc(sizeof(int));
        *a = i;

        pthread_create(&threads[i] , NULL , &routine , a);

    }

    for (size_t i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(threads[i] , NULL);
    }

    sem_close(semaphore);          // frees resources
    sem_unlink(SEMAPHORE_NAME); // removes named semaphore

    printf("Process 2 completed\n");

    exit(0);
    return 0;
    
}