/*
@file: thread-safe.c
@desc: this code illustrates the different methods to achieve thread-safe code namely
       1) use of local variables
       2) thread-indexed global variables
       3) mutexes
@author: Jahnavi Pinnamaneni; japi8358@colorado.edu
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <threads.h>
#include <pthread.h>

#include <time.h>

#define handle_err(err_no, arg) \
        do{errno = err_no; perror("arg"), exit(EXIT_FAILURE);}while(0);

//this is a thread_indexed global variable i.e., each thread would have a separate copy of the variable
static __thread int thread_num = 0;

int global_int = 0;

pthread_mutex_t mutex;

bool read_flag = false;

typedef struct thread_info{
    pthread_t thread_id;
}thread_info_t;


/*
The variable "thread_local_num" is initialised and updated to illustrate the usage of local variable inside a function
The variable "thread_num" is thread indexed, another global variable "global_int" is used to show the difference by comparision.

This thread then writes the updates values of the timestamp and attitude status to a temporary file and mutexes are used to avoid
data corruption.
*/
static void *thread_func_1(void * arg)
{
    thread_info_t *t_info = (thread_info_t *)arg;
    int thread_local_num = 10;
    thread_num = 2;
    global_int = 2;
    printf("thread id %ld, thread_num %d global_int %d thread_local_variable %d\n",t_info->thread_id,thread_num, global_int, thread_local_num);

    thread_num += 4;
    global_int += 4;
    thread_local_num++;
    printf("thread id %ld, thread_num %d global_int %d thread_local_variable %d\n",t_info->thread_id,thread_num, global_int, thread_local_num);

    typedef struct data{
        float X;
        float Y;
        float Z;
        struct timespec t_val;
    }data_t;

    printf("Entering while loop\n");
    data_t *att_data = calloc(1,sizeof(struct data));
    while(1)
    {
        att_data->X += 0.01;
        att_data->Y += 0.02;
        att_data->Z += 0.03;
        int clock_ret = clock_gettime(CLOCK_REALTIME,&att_data->t_val);

        if( att_data->Z>= 100.00)
        {
            att_data->X = 0.00;
            att_data->Y = 0.00;
            att_data->Z = 0.00;
        }
         
        char *str;
        asprintf(&str,"Timestamp: %d:%ld    X:%f, Y:%f, Z:%f\n",att_data->t_val.tv_sec, att_data->t_val.tv_nsec,att_data->X,att_data->Y,att_data->Z);
        pthread_mutex_lock(&mutex);
        int fd = open("/var/tmp/rtes",O_WRONLY|O_APPEND);
        if(fd < 0)
        handle_err(fd, "open");
        write(fd,str,strlen(str));
        close(fd);
        pthread_mutex_unlock(&mutex);
        read_flag = true;
        
        sleep(1);
    }

}

/*
This function reads the data from the file from the beginning and prints it out on the terminal.
*/
static void *thread_func_2(void * arg)
{
    thread_info_t *t_info = (thread_info_t *)arg;
    int thread_local_num = 100;
    thread_num = 1;
    global_int = 1;
    printf("thread id %ld, thread_num %d global_int %d thread_local_variable %d\n",t_info->thread_id,thread_num, global_int, thread_local_num);

    thread_num += 3;
    global_int += 3;
    thread_local_num++;
    printf("thread id %ld, thread_num %d global_int %d thread_local_variable %d\n",t_info->thread_id,thread_num, global_int, thread_local_num);

    while(1)
    {
        sleep(1);
        if(read_flag)
        {
            pthread_mutex_lock(&mutex);
            int fd = open("/var/tmp/rtes",O_RDONLY);
            if(fd < 0)
                handle_err(fd, "open");
            lseek(fd,0,SEEK_SET);
            int read_ret = 0;
            char s;
            do{
                read_ret = read(fd,&s,1);
                printf("%c",s);
            }while(read_ret);
            pthread_mutex_unlock(&mutex);
        }
        else
            continue;
    }

}

//Application entry point
/*
Two threads are created along with a mutex. These thread are then joined before the process exits. The mutex is destroyed.
A temporary file is opened or a new file is created if it does not already exist so that the threads can perform read and 
write operations.
*/
int main()
{
    int mut_ret = pthread_mutex_init(&mutex,NULL);
    if(mut_ret <0)
        handle_err(mut_ret,"mutex");
    
    int fd = open("/var/tmp/rtes",O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(fd<0)
        handle_err(fd,"open");

    thread_info_t *t_info_1 = (thread_info_t *)calloc(1,sizeof(struct thread_info));
    int ret = pthread_create(&(t_info_1->thread_id),NULL, thread_func_1,t_info_1);
    if(ret != 0)
        handle_err(ret, "pthread_create");
    
    thread_info_t *t_info_2 = (thread_info_t *)calloc(1,sizeof(struct thread_info));
    ret = pthread_create(&(t_info_2->thread_id),NULL, thread_func_2,t_info_2);
    if(ret != 0)
        handle_err(ret, "pthread_create");

    pthread_join(t_info_1->thread_id,NULL);
    free(t_info_1);

    pthread_join(t_info_2->thread_id,NULL);

    if(pthread_mutex_destroy(&mutex) != 0)
     perror("mutex destroy");

    free(t_info_2);
    close(fd);
    return 0;
}