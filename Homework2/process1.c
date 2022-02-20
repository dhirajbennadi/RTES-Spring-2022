#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

#include <fcntl.h>

/*Number of Threads to be created*/
#define THREAD_NUM 1
/*Count of Semaphore for synchronization*/
/*Binary Semaphore Count is 1*/
#define SEMAPHORE_NUMBER 1

/*Named Semaphore*/
#define SEMAPHORE_NAME "semProcess"   

sem_t *semaphore = NULL;

void *routine(void *args)
{
    sem_wait(semaphore);
    sleep(50);
    printf("Hello from Process 1 Thread number = %d\n", *(int*)args);
    sem_post(semaphore);

    free(args);
}


int main()
{
    pthread_t threads[THREAD_NUM];

    /*Depending on the initialization of the semaphore, multiple threads can acquire the semaphore and start executing*/
    /*If the semaphore count is 0 , the thread will have to wait until other threads release the semaphore*/
    semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0644, SEMAPHORE_NUMBER);

    /*Creation of threads*/
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

    printf("Process 1 completed\n");

    exit(0);

    return 0;
    
}
