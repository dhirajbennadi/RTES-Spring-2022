#include <stdio.h>

#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>


#define MESSAGE_QUEUE_NAME "/sendRecvQueueNew"

#define MAX_SIZE 4096

static char imagebuff[MAX_SIZE];
struct mq_attr mq_attr;
static mqd_t mymq;

void* heap_mq(void* param)
{
  if(mq_unlink(MESSAGE_QUEUE_NAME) == -1)
  {
    printf("Heap: Failed to unlink the message queue\n");
  }

  int i, j;
  char pixel = 'A';

  for(i=0;i<4096;i+=64) {
    pixel = 'A';
    for(j=i;j<i+64;j++) {
      imagebuff[j] = (char)pixel++;
    }
    imagebuff[j-1] = '\n';
  }
  imagebuff[4095] = '\0';
  imagebuff[63] = '\0';

  printf("buffer = %s\n", imagebuff);

  /* setup common message q attributes */
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = sizeof(void *)+sizeof(int);

  mq_attr.mq_flags = 0;

  mymq = mq_open(MESSAGE_QUEUE_NAME, O_CREAT|O_RDWR, S_IRUSR | S_IWUSR, &mq_attr);

  if(mymq == -1)
    perror("mq_open");

}


void *newSenderThread(void *param)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr;
  int prio;
  int nbytes;
  int id = 999;

  if(mq_unlink(MESSAGE_QUEUE_NAME) == -1)
  {
    printf("SenderThread: Failed to unlink the message queue\n");
  }

  mqd_t queue = mq_open(MESSAGE_QUEUE_NAME , O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, &mq_attr);
  if(queue == -1)
  {
      perror("Sender Thread: Failed to open queue");
  }



    /* send malloc'd message with priority=30 */

    buffptr = (void *)malloc( sizeof(imagebuff));
    strcpy(buffptr, imagebuff);
    printf("Message to send = %s\n", (char *)buffptr);

    printf("Sending %ld bytes\n", sizeof(buffptr));

    memcpy(buffer, &buffptr, sizeof(void *));
    memcpy(&(buffer[sizeof(void *)]), (void *)&id, sizeof(int));

    if(mq_send(queue, buffer, (size_t)(sizeof(void *)+sizeof(int)), 30) == -1)
    {
      perror("mq_send");
    }
    else
    {
      printf("send: message ptr 0x%p successfully sent\n", buffptr);
    }

    //taskDelay(3000);

}


void *newreceiverThread(void *param)
{
  char buffer[sizeof(void *)+sizeof(int)];
  void *buffptr; 
  int prio;
  int nbytes;
  int count = 0;
  int id;

  sleep(2);
  mqd_t queue = mq_open(MESSAGE_QUEUE_NAME , O_CREAT | O_RDONLY | O_NONBLOCK, S_IRUSR | S_IWUSR, &mq_attr);
  if(queue == -1)
  {
    perror("Failed to open queue\n");
  }
 


    /* read oldest, highest priority msg from the message queue */

    printf("Reading %ld bytes\n", sizeof(void *));
  
    if((nbytes = mq_receive(queue, buffer, (size_t)(sizeof(void *)+sizeof(int)), &prio)) == -1)
/*
    if((nbytes = mq_receive(mymq, (void *)&buffptr, (size_t)sizeof(void *), &prio)) == ERROR)
*/
    {
      perror("mq_receive");
    }
    else
    {
      memcpy(&buffptr, buffer, sizeof(void *));
      memcpy((void *)&id, &(buffer[sizeof(void *)]), sizeof(int));
      printf("receive: ptr msg 0x%p received with priority = %d, length = %d, id = %d\n", buffptr, prio, nbytes, id);

      printf("contents of ptr = \n%s\n", (char *)buffptr);

      free(buffptr);

      printf("heap space memory freed\n");

      if(mq_close(queue) == -1)
      {
        printf("Receiver Thread: Failed to close the queue\n");
      }

      if(mq_unlink(MESSAGE_QUEUE_NAME) == -1)
      {
        printf("Receiver: Failed to unlink the message queue\n");
      }

    }

}


#define THREAD_NUM 3


int main()
{
    pthread_t threads[THREAD_NUM];

    pthread_create(&threads[0] , NULL , heap_mq , NULL);

    pthread_join(threads[0] , NULL);

    pthread_create(&threads[1] , NULL , newSenderThread , NULL);

    pthread_join(threads[1] , NULL);

    pthread_create(&threads[2] , NULL , newreceiverThread , NULL);

    pthread_join(threads[2] , NULL);

    // for (size_t i = 0; i < THREAD_NUM; i++)
    // {
    //     pthread_join(threads[i] , NULL);
    // }

    printf("Threads execution completion\n");

    return 0;
}