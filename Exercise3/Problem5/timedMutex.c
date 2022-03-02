#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include <stdio.h>

#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define NSEC_PER_SEC (1000000000)
#define NSEC_PER_MSEC (1000000)
#define NSEC_PER_MICROSEC (1000)
#define DELAY_TICKS (1)
#define ERROR (-1)
#define OK (0)

pthread_mutex_t mutex1,mutex2;
#define perror(s) printf("\n" s "\n");
int criticalSectionVariable;


int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t);


void * func1(void *param)
{
    struct timespec start_time = {0, 0};
    struct timespec finish_time = {0, 0};
    struct timespec thread_dt = {0, 0};
    //printf("Thread =%d\n", *(int*)param);
    time_t rawtime;
    struct tm * timeinfo;
        typedef struct data{
        float X;
        float Y;
        float Z;
        struct timespec t_val;
    }data_t;


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
        clock_gettime(CLOCK_MONOTONIC, &start_time);
        pthread_mutex_lock(&mutex1);
        printf("Thread = %d in Critical Section\n", *(int*)param);
        int fd = open("/var/tmp/rtes",O_WRONLY|O_APPEND);
        if(fd < 0)
        {
            perror("File Descriptor failed\n");
        }
        write(fd,str,strlen(str));
        close(fd);
        sleep(11);
        clock_gettime(CLOCK_MONOTONIC, &finish_time);
        delta_t(&finish_time, &start_time, &thread_dt);

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        printf ( "Data updated in file at time: %s", asctime (timeinfo) );
        printf("Data = %s\n", str);
        pthread_mutex_unlock(&mutex1);
    }

}
void * func2(void *param) 
{
    struct timespec start_time = {0, 0};
    struct timespec finish_time = {0, 0};
    struct timespec thread_dt = {0, 0};
    int retVal = 0;
    time_t rawtime;
    struct tm * timeinfo;
    sleep(0.5);

    while (1)
    {

        clock_gettime(CLOCK_MONOTONIC, &start_time);
        struct timespec *wait;
        struct timespec newWait;
        int ret = 0;
        wait =(struct timespec *)(malloc(sizeof(struct timespec)));

        clock_gettime(CLOCK_REALTIME, wait);
        newWait.tv_sec = wait->tv_sec + 10;
        newWait.tv_nsec = wait->tv_nsec + 0;

        ret = pthread_mutex_timedlock(&mutex1,&newWait);
        if(ret != 0) 
        {
            switch(ret) 
            {
                case EINVAL: 
                    printf("EINVAL\n");
                break;
                case EAGAIN: 
                    printf("EAGAIN\n");
                break;
                case ETIMEDOUT: 
                    printf("ETIMEDOUT\n");
                break;
                case EDEADLK: 
                    printf("EDEADLK\n");
                break;
                default: 
                    printf("unknown\n");
                break;

            }
        }

        if(ret!=0)
        {
            printf("Thread = %d did not get to lock the mutex , Error Code = %d\n", *(int*)param, ret);

        }
        else
        {
            printf("Thread = %d in Critical Section\n", *(int*)param);
            retVal = pthread_mutex_unlock(&mutex1);
            if(retVal != 0)
            {
                printf("Did not unlock the mutex\n");
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &finish_time);

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        printf ( "No new data available at time : %s", asctime (timeinfo) );

    }
}





int main() {

    int ret;
    int fd = open("/var/tmp/rtes",O_RDWR|O_CREAT|O_TRUNC, 0666);
    if(fd<0)
        perror("File Descriptor Issue\n");

    pthread_t thread1,thread2;
    int a = 1;
    int b = 2;

    ret=pthread_create(&thread1,NULL,&func1,&a);

    if(ret!=0)
    {
        perror("Unable to create thread");
    }

    ret=pthread_create(&thread2,NULL,&func2,&b);
    if(ret!=0)
    {
        perror("Unable to  create thread");
    }

    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);

    return 0;
}


int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  //printf("\ndt calcuation\n");

  // case 1 - less than a second of change
  if(dt_sec == 0)
  {
	  //printf("dt less than 1 second\n");

	  if(dt_nsec >= 0 && dt_nsec < NSEC_PER_SEC)
	  {
	          //printf("nanosec greater at stop than start\n");
		  delta_t->tv_sec = 0;
		  delta_t->tv_nsec = dt_nsec;
	  }

	  else if(dt_nsec > NSEC_PER_SEC)
	  {
	          //printf("nanosec overflow\n");
		  delta_t->tv_sec = 1;
		  delta_t->tv_nsec = dt_nsec-NSEC_PER_SEC;
	  }

	  else // dt_nsec < 0 means stop is earlier than start
	  {
	         printf("stop is earlier than start\n");
		 return(ERROR);  
	  }
  }

  // case 2 - more than a second of change, check for roll-over
  else if(dt_sec > 0)
  {
	  //printf("dt more than 1 second\n");

	  if(dt_nsec >= 0 && dt_nsec < NSEC_PER_SEC)
	  {
	          //printf("nanosec greater at stop than start\n");
		  delta_t->tv_sec = dt_sec;
		  delta_t->tv_nsec = dt_nsec;
	  }

	  else if(dt_nsec > NSEC_PER_SEC)
	  {
	          //printf("nanosec overflow\n");
		  delta_t->tv_sec = delta_t->tv_sec + 1;
		  delta_t->tv_nsec = dt_nsec-NSEC_PER_SEC;
	  }

	  else // dt_nsec < 0 means roll over
	  {
	          //printf("nanosec roll over\n");
		  delta_t->tv_sec = dt_sec-1;
		  delta_t->tv_nsec = NSEC_PER_SEC + dt_nsec;
	  }
  }

  return(OK);
}