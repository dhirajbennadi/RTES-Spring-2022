/* ========================================================================== */
/*
    Code Source: Adapted from http://mercury.pr.erau.edu/~siewerts/cec450/code/
    Code Modified by: Dhiraj Bennadi
    Date: May 3rd, 2022

/****************/
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>

#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>

#include <syslog.h>
#include <sys/time.h>
#include <sys/sysinfo.h>

#include <errno.h>

/*CPP Headers*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <chrono>

#include <fstream>


/*Canny CPP Needed Stuff*/
using namespace cv;
using namespace std;

#define HRES 640
#define VRES 480

#define IMG_SIZE (VRES*HRES*3)

pthread_mutex_t globalImageBufferImageLock;
char globalImageBuffer[40][IMG_SIZE];

// Transform display window
char timg_window_name[] = "Edge Detector Transform";

int lowThreshold=0;
int const max_lowThreshold = 100;
int kernel_size = 3;
int edgeThresh = 1;
int ratio = 3;
Mat canny_frame, cdst, timg_gray, timg_grad;

IplImage* frame[40];
CvCapture* capture;

int globalStartFlag = 0;
int countToSaveImage = 0;

std::chrono::duration<double, std::milli> elapsedSeconds;

std::chrono::duration<double, std::milli> executionTimeCounter1;
std::chrono::duration<double, std::milli> executionTimeCounter2;
std::chrono::duration<double, std::milli> executionTimeChronoService2;

std::chrono::duration<double, std::milli> executionTimeChronoSequencer;

std::chrono::duration<double, std::milli> averageTimeSequencer;
std::chrono::duration<double, std::milli> averageTimeService1;
std::chrono::duration<double, std::milli> averageTimeService2;

#define TEN_HZ 1
//#define ONE_HZ 1

/*Macros to differentiate between 1 Hz and 10 Hz*/
#ifdef ONE_HZ
int ScenarioSelected = 1;
#define IMAGE_COUNT 1800
//delay for 50 msec, 20 Hz
#define DELAY_VALUE 50000000
#define SERVICE_1_MOD_VAL 5
#define SERVICE_2_MOD_VAL 20
#endif

#ifdef TEN_HZ
int ScenarioSelected = 10;
#define IMAGE_COUNT 1800
//delay for 100 msec, 100 Hz
#define DELAY_VALUE 10000000
#define SERVICE_1_MOD_VAL 8
#define SERVICE_2_MOD_VAL 10
#endif

#define USEC_PER_MSEC (1000)
#define USEC_PER_SEC (1000000)
#define NANOSEC_PER_SEC (1000000000)
#define NUM_CPU_CORES (1)
#define TRUE (1)
#define FALSE (0)

#define NUM_THREADS (2+1)

int abortTest=FALSE;
int abortS1=FALSE, abortS2=FALSE;
sem_t semS1, semS2;
struct timeval start_time_val;


typedef struct
{
    int threadIdx;
    unsigned long long sequencePeriods;
} threadParams_t;


void *Sequencer(void *threadp);

void *Service_1(void *threadp);
void *Service_2(void *threadp);
double getTimeMsec(void);
void print_scheduler(void);


int main(int argc, char** argv )
{
    struct timeval programStartTime;
    gettimeofday(&programStartTime, (struct timezone *)0);

    struct timeval current_time_val;
    int i, rc, scope;
    cpu_set_t threadcpu;
    pthread_t threads[NUM_THREADS];
    threadParams_t threadParams[NUM_THREADS];
    pthread_attr_t rt_sched_attr[NUM_THREADS];
    int rt_max_prio, rt_min_prio;
    struct sched_param rt_param[NUM_THREADS];
    struct sched_param main_param;
    pthread_attr_t main_attr;
    pid_t mainpid;
    cpu_set_t allcpuset;

    printf("RTES Course Project\n");
    gettimeofday(&start_time_val, (struct timezone *)0);
    gettimeofday(&current_time_val, (struct timezone *)0);
    syslog(LOG_CRIT, "Sequencer @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);

   printf("System has %d processors configured and %d available.\n", get_nprocs_conf(), get_nprocs());

   CPU_ZERO(&allcpuset);

   for(i=0; i < NUM_CPU_CORES; i++)
       CPU_SET(i, &allcpuset);

   printf("Using CPUS=%d from total available.\n", CPU_COUNT(&allcpuset));


    // initialize the sequencer semaphores
    //
    if (sem_init (&semS1, 0, 0)) { printf ("Failed to initialize S1 semaphore\n"); exit (-1); }
    if (sem_init (&semS2, 0, 0)) { printf ("Failed to initialize S2 semaphore\n"); exit (-1); }

    mainpid=getpid();

    rt_max_prio = sched_get_priority_max(SCHED_FIFO);
    rt_min_prio = sched_get_priority_min(SCHED_FIFO);

    rc=sched_getparam(mainpid, &main_param);
    main_param.sched_priority=rt_max_prio;
    rc=sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    if(rc < 0) perror("main_param");
    print_scheduler();


    pthread_attr_getscope(&main_attr, &scope);

    if(scope == PTHREAD_SCOPE_SYSTEM)
      printf("PTHREAD SCOPE SYSTEM\n");
    else if (scope == PTHREAD_SCOPE_PROCESS)
      printf("PTHREAD SCOPE PROCESS\n");
    else
      printf("PTHREAD SCOPE UNKNOWN\n");

    printf("rt_max_prio=%d\n", rt_max_prio);
    printf("rt_min_prio=%d\n", rt_min_prio);

    for(i=0; i < NUM_THREADS; i++)
    {

      CPU_ZERO(&threadcpu);
      CPU_SET(3, &threadcpu);

      rc=pthread_attr_init(&rt_sched_attr[i]);
      rc=pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
      rc=pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
      //rc=pthread_attr_setaffinity_np(&rt_sched_attr[i], sizeof(cpu_set_t), &threadcpu);

      rt_param[i].sched_priority=rt_max_prio-i;
      pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);

      threadParams[i].threadIdx=i;
    }


    if(pthread_mutex_init(&globalImageBufferImageLock, NULL) != 0)
    {
        cout << "Mutex Initiialization failed" << endl;
        return -1;

    }
   
    printf("Service threads will run on %d CPU cores\n", CPU_COUNT(&threadcpu));

    // Create Service threads which will block awaiting release for:
    //

    // Servcie_1 = RT_MAX-1	@ 3 Hz
    //
    rt_param[1].sched_priority=rt_max_prio-1;
    pthread_attr_setschedparam(&rt_sched_attr[1], &rt_param[1]);
    rc=pthread_create(&threads[1],               // pointer to thread descriptor
                      &rt_sched_attr[1],         // use specific attributes
                      //(void *)0,               // default attributes
                      Service_1,                 // thread function entry point
                      (void *)&(threadParams[1]) // parameters to pass in
                     );
    if(rc < 0)
        perror("pthread_create for service 1");
    else
        printf("pthread_create successful for service 1\n");


    // Service_2 = RT_MAX-2	@ 1 Hz
    //
    rt_param[2].sched_priority=rt_max_prio-2;
    pthread_attr_setschedparam(&rt_sched_attr[2], &rt_param[2]);
    rc=pthread_create(&threads[2], &rt_sched_attr[2], Service_2, (void *)&(threadParams[2]));
    if(rc < 0)
        perror("pthread_create for service 2");
    else
        printf("pthread_create successful for service 2\n");



    // Wait for service threads to initialize and await release by sequencer.
    //
    // Note that the sleep is not necessary of RT service threads are created wtih 
    // correct POSIX SCHED_FIFO priorities compared to non-RT priority of this main
    // program.
    //
    // usleep(1000000);
 
    // Create Sequencer thread, which like a cyclic executive, is highest prio
    printf("Start sequencer\n");
    threadParams[0].sequencePeriods=1800;

    // Sequencer = RT_MAX	@ 30 Hz
    //
    rt_param[0].sched_priority=rt_max_prio;
    pthread_attr_setschedparam(&rt_sched_attr[0], &rt_param[0]);
    rc=pthread_create(&threads[0], &rt_sched_attr[0], Sequencer, (void *)&(threadParams[0]));
    if(rc < 0)
        perror("pthread_create for sequencer service 0");
    else
        printf("pthread_create successful for sequeencer service 0\n");



    /*Canny Related Code*/
    //CvCapture* capture;
    int dev=0;

    if(argc > 1)
    {
        sscanf(argv[1], "%d", &dev);
        printf("using %s\n", argv[1]);
    }
    else if(argc == 1)
        printf("using default\n");

    else
    {
        printf("usage: capture [dev]\n");
        exit(-1);
    }

    capture = (CvCapture *)cvCreateCameraCapture(dev);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);


    for(int i = 0; i < 0xFFFF; i++)
    {
        ;
    }

    printf("*********************\n");
    printf("Scenario Selected = %d Hz\n\n", ScenarioSelected);
    printf("*********************\n");


   for(i=0;i<NUM_THREADS;i++)
       pthread_join(threads[i], NULL);

   printf("\nTEST COMPLETE\n");


    struct timeval programendTime;
    gettimeofday(&programendTime, (struct timezone *)0);

    printf("Program Start Time = %d and %d\n", (int)(programendTime.tv_sec-programStartTime.tv_sec), (int)programendTime.tv_usec/USEC_PER_MSEC);

   return 0;
}


void *Sequencer(void *threadp)
{
    struct timeval current_time_val;
    struct timespec delay_time = {0,DELAY_VALUE};
    struct timespec remaining_time;
    double current_time;
    double residual;
    int rc, delay_cnt=0;
    unsigned long long seqCnt=0;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    std::chrono::system_clock::time_point startTimeChronoSequencer;
    std::chrono::system_clock::time_point endTimeChronoSequencer;
    

    gettimeofday(&current_time_val, (struct timezone *)0);
    syslog(LOG_CRIT, "Sequencer thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
    printf("Sequencer thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);

    do
    {
        startTimeChronoSequencer = std::chrono::system_clock::now();
        delay_cnt=0; residual=0.0;

        //gettimeofday(&current_time_val, (struct timezone *)0);
        //syslog(LOG_CRIT, "Sequencer thread prior to delay @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
        do
        {
            rc=nanosleep(&delay_time, &remaining_time);

            if(rc == EINTR)
            { 
                residual = remaining_time.tv_sec + ((double)remaining_time.tv_nsec / (double)NANOSEC_PER_SEC);

                if(residual > 0.0) printf("residual=%lf, sec=%d, nsec=%d\n", residual, (int)remaining_time.tv_sec, (int)remaining_time.tv_nsec);
 
                delay_cnt++;
            }
            else if(rc < 0)
            {
                perror("Sequencer nanosleep");
                exit(-1);
            }
           
        } while((residual > 0.0) && (delay_cnt < 100));

        seqCnt++;
        gettimeofday(&current_time_val, (struct timezone *)0);
        //syslog(LOG_CRIT, "Sequencer cycle %llu @ sec=%d, msec=%d\n", seqCnt, (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);


        if(delay_cnt > 1) printf("Sequencer looping delay %d\n", delay_cnt);


        // Release each service at a sub-rate of the generic sequencer rate

        // Servcie_1 = RT_MAX-1	@ 3 Hz
        if((seqCnt % SERVICE_1_MOD_VAL) == 0) sem_post(&semS1);


        // Service_2 = RT_MAX-2	@ 0.5 Hz
        if((seqCnt % SERVICE_2_MOD_VAL) == 0) sem_post(&semS2);

        endTimeChronoSequencer = std::chrono::system_clock::now();

        if(endTimeChronoSequencer - startTimeChronoSequencer > executionTimeChronoSequencer)
        {
            executionTimeChronoSequencer = endTimeChronoSequencer - startTimeChronoSequencer;
        }

        averageTimeSequencer += endTimeChronoSequencer - startTimeChronoSequencer;


        //gettimeofday(&current_time_val, (struct timezone *)0);
        //syslog(LOG_CRIT, "Sequencer release all sub-services @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);

    } while(!abortTest && countToSaveImage < IMAGE_COUNT /*(seqCnt < threadParams->sequencePeriods)*/);

    sem_post(&semS1); sem_post(&semS2);
    abortS1=TRUE; abortS2=TRUE;

    printf("Worst Case Chrono Sequencer = %lf ms\n", executionTimeChronoSequencer.count());
    printf("Average Time Sequencer = %lf ms\n", averageTimeSequencer.count() / seqCnt);

    pthread_exit((void *)0);
}



void *Service_1(void *threadp)
{
    struct timeval current_time_val;
    struct timespec current_time_val_counter1;
    struct timespec current_time_val_counter2;
    struct timeval end_time_val;
    double current_time;
    struct timespec startTime;
    struct timespec endTime;
    long executionTime = 0;
    long executionTimeCounter1SystemClock = 0;
    long executionTimeCounter2SystemClock = 0;

    std::chrono::system_clock::time_point endTimeChronoCounter1;
    std::chrono::system_clock::time_point endTimeChronoCounter2;

    std::chrono::system_clock::time_point averageStartTime;

    
    unsigned long long S1Cnt=0;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    gettimeofday(&current_time_val, (struct timezone *)0);
    //gettimeofday(&lastExecutionTime, (struct timezone *)0);
    syslog(LOG_CRIT, "Frame Sampler thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
    printf("Frame Sampler thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);


    int countToSaveImage = 0;
    string name = "frame";
    string number = "";
    string extension = ".jpeg";
    ofstream myfile;

    ofstream jitterAnalysisIOBound;
    ofstream jitterAnalysisCPUBound;

    int imageCount = 0;
    int imageCounter1 = 0;
    int imageCounter2 = 0;
    int IOBoundStartFlag = 0;

    while(!abortS1)
    {
        
        sem_wait(&semS1);
        clock_gettime(CLOCK_REALTIME, &startTime);
        auto startTimeChrono = std::chrono::system_clock::now();

        S1Cnt++;

        /*Synthetic Load*/
        frame[imageCount]=cvQueryFrame(capture);

        if(frame[imageCount] == 0)
        {
            
            auto endTimeChronoIOBound = std::chrono::system_clock::now();
            IOBoundStartFlag = true;
            globalStartFlag = false;

            elapsedSeconds = endTimeChronoIOBound - startTimeChrono;
            imageCounter1++;

            endTimeChronoCounter1 = std::chrono::system_clock::now();
            clock_gettime(CLOCK_REALTIME, &current_time_val_counter1);
        }
        else
        {
            auto endTimeChronoCPUBound = std::chrono::system_clock::now();

            elapsedSeconds = endTimeChronoCPUBound - startTimeChrono;

            pthread_mutex_lock(&globalImageBufferImageLock);
            memcpy(globalImageBuffer[imageCount], frame[imageCount]->imageData, IMG_SIZE);
            globalStartFlag = true;
            IOBoundStartFlag = false;
            imageCounter2++;
            pthread_mutex_unlock(&globalImageBufferImageLock);


            myfile.open("Service1.txt", ios::app);
            number = to_string(imageCounter2);
            myfile << "Capturing Image with ImageCounter2 = " << number << "\n";
            myfile.close();

            
            imageCount++;
            if(imageCount == 40)
            {
                imageCount = 0;
            }
            

            jitterAnalysisCPUBound.open("jitterAnalysisCPUBound.txt", ios::app);
            jitterAnalysisCPUBound << "Frame Number = " << number << " " <<
            "Duration of Capture CPU Bound = " << elapsedSeconds.count() << "\n";
            jitterAnalysisCPUBound.close();

            endTimeChronoCounter2 = std::chrono::system_clock::now();
            clock_gettime(CLOCK_REALTIME, &current_time_val_counter2);

            //syslog(LOG_NOTICE, "Image Counter 2 = %d complete in %lf ms \n", imageCounter2, elapsedSeconds);
        }

        if(IOBoundStartFlag)
        {
            if((current_time_val_counter1.tv_nsec - startTime.tv_nsec) > executionTimeCounter1SystemClock)
            {
                //pthread_mutex_lock(&globalImageBufferImageLock);
                executionTimeCounter1SystemClock = current_time_val_counter1.tv_nsec - startTime.tv_nsec;
                //pthread_mutex_unlock(&globalImageBufferImageLock);
            }

            if(endTimeChronoCounter1 - startTimeChrono > executionTimeCounter1)
            {
                //pthread_mutex_lock(&globalImageBufferImageLock);
                executionTimeCounter1 = endTimeChronoCounter1 - startTimeChrono;
                //pthread_mutex_unlock(&globalImageBufferImageLock);
            }


        }


        if(globalStartFlag)
        {
            if((current_time_val_counter2.tv_nsec - startTime.tv_nsec) > executionTimeCounter2SystemClock)
            {
                executionTimeCounter2SystemClock = current_time_val_counter2.tv_nsec - startTime.tv_nsec;
            }

            if(endTimeChronoCounter2 - startTimeChrono > executionTimeCounter2)
            {
                executionTimeCounter2 = endTimeChronoCounter2 - startTimeChrono;
            }


            averageTimeService1 += endTimeChronoCounter2 - startTimeChrono;

        }

        

    }
    
    //printf("Frame Sampler thread End Time @ sec=%d, msec=%d\n", (int)(end_time_val.tv_sec-start_time_val.tv_sec), (int)end_time_val.tv_usec/USEC_PER_MSEC);
    //printf("Worst Case Execution Time Service = %d = %ld\n", threadParams->threadIdx, executionTime/USEC_PER_SEC);
    printf("Worst Case Execution Time Thread = %d IO Bound  = %ld\n", threadParams->threadIdx, executionTimeCounter1SystemClock/USEC_PER_SEC);
    printf("Worst Case Chrono IO Bound = %lf ms\n\n", executionTimeCounter1.count());

    printf("Worst Case Execution Time Thread = %d CPU Bound = %ld\n", threadParams->threadIdx, executionTimeCounter2SystemClock/USEC_PER_SEC);
    printf("Worst Case Chrono CPU Bound = %lf ms\n", executionTimeCounter2.count());

    printf("Service 1 Count = %lld\n",S1Cnt);
    //syslog(LOG_CRIT, "Worst Execution Time = %ld", executionTime/USEC_PER_SEC);
    cvReleaseCapture(&capture);


    
    printf("Average Time Service 1 = %lf ms\n", averageTimeService1.count() / S1Cnt);

    pthread_exit((void *)0);
}

void *Service_2(void *threadp)
{
    struct timeval current_time_val;
    double current_time;
    unsigned long long S2Cnt=0;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    struct timespec startTime;
    struct timespec endTime;
    struct timespec imageTimeStamp;
    long executionTime = 0;

    //int countToSaveImage = 0;
    string name = "frame";
    string number = "";
    string extension = ".jpeg";
    string extension2 = ".ppm";

    gettimeofday(&current_time_val, (struct timezone *)0);
    syslog(LOG_CRIT, "Time-stamp with Image Analysis thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
    printf("Time-stamp with Image Analysis thread @ sec=%d, msec=%d\n", (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);

    ofstream myfile;
    int byteCount = 0;
    int imageCount = 0;
    bool startTimeRecorded = false;


    while(!abortS2)
    {
        sem_wait(&semS2);
        // if(startTimeRecorded == false)
        // {
        //     startTimeRecorded = true;
        //     averageStartTime = std::chrono::system_clock::now();
        // }

        clock_gettime(CLOCK_REALTIME, &startTime);
        auto startTimeChronoService2 = std::chrono::system_clock::now();
        S2Cnt++;


        if(globalStartFlag)
        {

            Mat mat_frame = cv::cvarrToMat(frame[imageCount]);

            number = to_string(countToSaveImage);
            name = name + number + extension;

            imwrite(name, mat_frame , {CV_IMWRITE_JPEG_QUALITY , 90});
            //imwrite(name, mat_frame);
            name = "frame";

            myfile.open("Service2.txt", ios::app);
            myfile << "Storing Image = " << number << "\n";
            time_t now = time(0);
            char *date = ctime(&now);
            myfile << date << "\n"; 
            myfile << "\n";
            myfile.close();

            pthread_mutex_lock(&globalImageBufferImageLock);
            countToSaveImage++;
            pthread_mutex_unlock(&globalImageBufferImageLock);

            imageCount++;
            if(imageCount == 40)
            {
                imageCount = 0;
            }
        }
        
        gettimeofday(&current_time_val, (struct timezone *)0);
        //syslog(LOG_CRIT, "Time-stamp with Image Analysis release %llu @ sec=%d, msec=%d\n", S2Cnt, (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
        syslog(LOG_CRIT, "Thread Num = %d Frame Sampler release %llu @ sec=%d, msec=%d\n", threadParams->threadIdx, S2Cnt, (int)(current_time_val.tv_sec-start_time_val.tv_sec), (int)current_time_val.tv_usec/USEC_PER_MSEC);
        clock_gettime(CLOCK_REALTIME, &endTime);
        auto endTimeChronoService2 = std::chrono::system_clock::now();

        if((endTime.tv_nsec - startTime.tv_nsec) > executionTime)
        {
            executionTime = endTime.tv_nsec - startTime.tv_nsec;
        }

        if(endTimeChronoService2 - startTimeChronoService2 > executionTimeChronoService2)
        {
            executionTimeChronoService2 = endTimeChronoService2 - startTimeChronoService2;
        }

        averageTimeService2 += endTimeChronoService2 - startTimeChronoService2;
    }
    printf("Worst Case Execution Time System Service = %d = %ld\n", threadParams->threadIdx, executionTime/USEC_PER_SEC);

    printf("Worst Case Execution Time Chrono Service = %d = %lf\n", threadParams->threadIdx, executionTimeChronoService2.count());
    printf("Service 2 Count = %lld\n",S2Cnt);


    //auto averageEndTime = std::chrono::system_clock::now();

    //averageTimeService2 = averageEndTime - averageStartTime;
    printf("Average Time Service 2 = %lf ms\n", averageTimeService2.count() / S2Cnt);

    pthread_exit((void *)0);
}


double getTimeMsec(void)
{
  struct timespec event_ts = {0, 0};

  clock_gettime(CLOCK_MONOTONIC, &event_ts);
  return ((event_ts.tv_sec)*1000.0) + ((event_ts.tv_nsec)/1000000.0);
}


void print_scheduler(void)
{
   int schedType;

   schedType = sched_getscheduler(getpid());

   switch(schedType)
   {
       case SCHED_FIFO:
           printf("Pthread Policy is SCHED_FIFO\n");
           break;
       case SCHED_OTHER:
           printf("Pthread Policy is SCHED_OTHER\n"); exit(-1);
         break;
       case SCHED_RR:
           printf("Pthread Policy is SCHED_RR\n"); exit(-1);
           break;
       default:
           printf("Pthread Policy is UNKNOWN\n"); exit(-1);
   }
}

