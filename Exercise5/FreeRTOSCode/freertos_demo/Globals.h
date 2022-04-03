/*
 * Globals.h
 *
 *  Created on: Apr 2, 2022
 *      Author: dhira
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

//#define seqgen
//#define seqgen2x
#define seqgen100x // For Question 3

#ifdef seqgen
#define freq (30)
#define SEQUENCER_PERIODS (900)
#endif

#ifdef seqgen2x
#define freq (60)
#define SEQUENCER_PERIODS (1800)
#endif

#ifdef seqgen100x
#define freq (3000)
#define SEQUENCER_PERIODS (900)
#endif

extern int abortS1;
extern int abortS2;
extern int abortS3;
extern int abortS4;
extern int abortS5;
extern int abortS6;
extern int abortS7;
extern unsigned long long seqCnt;

extern xSemaphoreHandle g_pUARTSemaphore;

extern SemaphoreHandle_t g_pUARTSemaphore1;
extern SemaphoreHandle_t g_pUARTSemaphore2;
extern SemaphoreHandle_t g_pUARTSemaphore3;
extern SemaphoreHandle_t g_pUARTSemaphore4;
extern SemaphoreHandle_t g_pUARTSemaphore5;
extern SemaphoreHandle_t g_pUARTSemaphore6;
extern SemaphoreHandle_t g_pUARTSemaphore7;


typedef struct worstCaseExecutionTimes{
    TickType_t wcetService1;
    TickType_t wcetService2;
    TickType_t wcetService3;
    TickType_t wcetService4;
    TickType_t wcetService5;
    TickType_t wcetService6;
    TickType_t wcetService7;
}worstCaseExecutionTimes_t;


extern worstCaseExecutionTimes_t wcetStats;
#endif /* GLOBALS_H_ */
