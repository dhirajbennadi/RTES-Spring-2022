#include <stdio.h>

#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>


#define MESSAGE_QUEUE_NAME "/sendRecvQueuePosix"

//#define MAX_SIZE 4096

//static char imagebuff[MAX_SIZE];
//struct mq_attr mq_attr;
//static mqd_t mymq;



typedef struct{
    char msgText[100];
} message;

void *senderThread(void* param)
{
    if(mq_unlink(MESSAGE_QUEUE_NAME) == -1)
    {
        printf("SenderThread: Failed to unlink the message queue\n");
    }
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = sizeof(message)
    };

    mqd_t queue = mq_open(MESSAGE_QUEUE_NAME , O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attributes);

    if(queue == -1)
    {
        perror("SenderThread: Failed to create queue");
    }


    message message_to_send;
    strcpy(message_to_send.msgText , "Dhiraj is working on message Queues");
    //memcpy(message_to_send.msgText , imagebuff , sizeof(message_to_send.msgText));

    printf("SenderThread: Sender Queue Message = %s\n", message_to_send.msgText);

    if(mq_send(queue , (char*) &message_to_send , sizeof(message_to_send) , 1) == -1)
    {
        printf("SenderThread: Failed to send message\n");
    }


    //printf("Type any key to finish\n");
    //getchar();
    

    if(mq_close(queue) == -1)
    {
        printf("SenderThread: Failed to close the message queue\n");
    }
    // if(mq_unlink(MESSAGE_QUEUE_NAME) == -1)
    // {
    //     printf("SenderThread: Failed to unlink the message queue\n");
    // }
}

void *receiverThread(void* param)
{
    struct mq_attr attributes = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_curmsgs = 0,
        .mq_msgsize = sizeof(message)
    };


    sleep(2);
    mqd_t queue = mq_open(MESSAGE_QUEUE_NAME , O_CREAT | O_RDONLY /*| O_NONBLOCK*/, S_IRUSR | S_IWUSR, &attributes);

    if(queue == -1)
    {
        perror("Receiver Thread: Failed to create queue\n");
    }

    message message_to_receive;
    //strcpy(&message_to_receive.msgText[0] , "Dhiraj is working on message queues");

    if(mq_receive(queue , (char*) &message_to_receive , sizeof(message_to_receive) , NULL ) == -1)
    {
        perror("Receiver Thread: Failed to receive message\n");
    }

    printf("Receiver Thread: Receiver Queue Message = %s\n", message_to_receive.msgText);

    //printf("Type any key to finish\n");
    //getchar();

    if(mq_close(queue) == -1)
    {
        printf("Receiver Thread: Failed to close the queue\n");
    }

    if(mq_unlink(MESSAGE_QUEUE_NAME) == -1)
    {
        printf("Receiver: Failed to unlink the message queue\n");
    }
}

#define THREAD_NUM 2


int main()
{
    pthread_t threads[THREAD_NUM];

    //pthread_create(&threads[0] , NULL , heap_mq , NULL);

    pthread_create(&threads[0] , NULL , senderThread , NULL);
  
    //pthread_join(threads[0] , NULL);

    // for(int i = 0; i < 10000; i++)
    // {
    //   ;
    // }


    pthread_create(&threads[1] , NULL , receiverThread , NULL);
    //pthread_join(threads[1] , NULL);

    for (size_t i = 0; i < THREAD_NUM; i++)
    {
        pthread_join(threads[i] , NULL);
    }


    // (void)senderThread(NULL);
    // (void)receiverThread(NULL);

    printf("Threads execution completion\n");

    return 0;
}