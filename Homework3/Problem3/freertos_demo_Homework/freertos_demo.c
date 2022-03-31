//*****************************************************************************
//
// freertos_demo.c - Simple FreeRTOS example.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "utils/uartstdio.h"
#include "led_task.h"
#include "switch_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "projdefs.h"

#include <time.h>
#include <stdio.h>
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>FreeRTOS Example (freertos_demo)</h1>
//!
//! This application demonstrates the use of FreeRTOS on Launchpad.
//!
//! The application blinks the user-selected LED at a user-selected frequency.
//! To select the LED press the left button and to select the frequency press
//! the right button.  The UART outputs the application status at 115,200 baud,
//! 8-n-1 mode.
//!
//! This application utilizes FreeRTOS to perform the tasks in a concurrent
//! fashion.  The following tasks are created:
//!
//! - An LED task, which blinks the user-selected on-board LED at a
//!   user-selected rate (changed via the buttons).
//!
//! - A Switch task, which monitors the buttons pressed and passes the
//!   information to LED task.
//!
//! In addition to the tasks, this application also uses the following FreeRTOS
//! resources:
//!
//! - A Queue to enable information transfer between tasks.
//!
//! - A Semaphore to guard the resource, UART, from access by multiple tasks at
//!   the same time.
//!
//! - A non-blocking FreeRTOS Delay to put the tasks in blocked state when they
//!   have nothing to do.
//!
//! For additional details on FreeRTOS, refer to the FreeRTOS web page at:
//! http://www.freertos.org/
//
//*****************************************************************************


//*****************************************************************************
//
// The mutex that protects concurrent access of UART from multiple tasks.
//
//*****************************************************************************
xSemaphoreHandle g_pUARTSemaphore;

SemaphoreHandle_t xSemaphoreMutex;
SemaphoreHandle_t xSemaphoreMutex2;

/* Task priorities. */
#define hello_task_PRIORITY (configTIMER_TASK_PRIORITY - 1)
#define hello_task_PRIORITY2 (configTIMER_TASK_PRIORITY - 2)


static TimerHandle_t one_shot_timer;

static TimerHandle_t try_this_timer;

volatile static bool signalTask1 = false;
volatile static int signalTask1Count = 0;
volatile static int signalTask2Count = 0;
volatile static bool signalTask2 = false;

portTickType ui32WakeTime;
portTickType ui32WakeTime2;


portTickType start_time;
portTickType end_time;
portTickType start_time1;
portTickType end_time1;

unsigned int idx = 0, jdx = 1;
unsigned int seqIterations = 50;
//unsigned int seqIterations = 20;
//unsigned int reqIterations = 10000000;
unsigned int reqIterations = 8000;
volatile unsigned int fib = 0, fib0 = 0, fib1 = 1;

int FIB_TEST(unsigned int seqCnt, unsigned int iterCnt)
{
   for(idx=0; idx < iterCnt; idx++)
   {
      fib = fib0 + fib1;
      while(jdx < seqCnt)
      {
         fib0 = fib1;
         fib1 = fib;
         fib = fib0 + fib1;
         jdx++;
      }

      jdx=0;
   }

   return idx;
}


void myTimerCallback(TimerHandle_t xTimer)
{

    signalTask1Count++;
    signalTask2Count++;


    if(signalTask1Count == 3)
    {
        signalTask1Count = 0;
        xSemaphoreTake( xSemaphoreMutex, portMAX_DELAY );
        signalTask1 = true;
        xSemaphoreGive(xSemaphoreMutex);
        //signalTask1Count = 0;
        //signalTask1 = true;
        UARTprintf("Signalling Task 1: time: %d\n\r", xTaskGetTickCount()/portTICK_PERIOD_MS);
    }

    if(signalTask2Count == 8)
    {
        signalTask2Count = 0;
        xSemaphoreTake( xSemaphoreMutex2, portMAX_DELAY);
        signalTask2 = true;
        xSemaphoreGive(xSemaphoreMutex2);


        UARTprintf("Signalling Task 2: time: %d\n\r", xTaskGetTickCount()/portTICK_PERIOD_MS);
    }

}

void myTimerCallbackNew(TimerHandle_t xTimer)
{
    TickType_t startTime;
    startTime = xTaskGetTickCount();


    UARTprintf("Time elapsed = %d\n\r", startTime / portTICK_PERIOD_MS);



}
#define DELAY_LOOP 65535

unsigned int syncVariableFromTask1toTask2 = 0;

static void task1(void *pvParameters);
static void task2(void *pvParameters);
static void task1Problem3(void *pvParameters);
static void task2Problem3(void *pvParameters);
static void timing(void *pvParameters);
void TimerStart(void);
void TimerStart2(void);

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// Initialize FreeRTOS and start the initial set of tasks.
//
//*****************************************************************************
int main(void)
{
    //
    // Set the clocking to run at 50 MHz from the PLL.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ConfigureUART();
    //
    // Print demo introduction.
    //
    UARTprintf("\n\nWelcome to the EK-TM4C123GXL FreeRTOS Demo!\n");

    //
    // Create a mutex to guard the UART.
    //
    g_pUARTSemaphore = xSemaphoreCreateMutex();

    xSemaphoreMutex = xSemaphoreCreateMutex();

    xSemaphoreMutex2 = xSemaphoreCreateMutex();

    if(xSemaphoreMutex != NULL)
    {
        UARTprintf("Mutex was successfully created\n\r");
    }
    else
    {
        UARTprintf("Mutex creation failed\n\r");
    }

    if(xSemaphoreMutex2 != NULL)
    {
        UARTprintf("Mutex2 was successfully created\n\r");
    }
    else
    {
        UARTprintf("Mutex2 creation failed\n\r");
    }


    one_shot_timer = xTimerCreate("One Shot Timer", 10 /*pdMS_TO_TICKS(10) / portTICK_PERIOD_MS*/, pdTRUE, (void*)0 , myTimerCallback);



    /*Creation of Tasks*/
//    xTaskCreate(task1, "Task1", configMINIMAL_STACK_SIZE + 10, "Task 1 Executing",
//            hello_task_PRIORITY, NULL);
//    xTaskCreate(task2, "Task2", configMINIMAL_STACK_SIZE + 10, "Task 2 Executing",
//            hello_task_PRIORITY, NULL);

    if(xTaskCreate(task1Problem3, "Task1", configMINIMAL_STACK_SIZE + 1000, "Task 1 Executing",
            hello_task_PRIORITY, NULL) != pdTRUE)
    {
        return 1;
    }

    if(xTaskCreate(task2Problem3, "Task2", configMINIMAL_STACK_SIZE + 1000, "Task 2 Executing",
            hello_task_PRIORITY2, NULL) != pdTRUE)
    {
        return 1;
    }


//    xTaskCreate(timing, "timing", configMINIMAL_STACK_SIZE + 1000, "fibonacci",
//            hello_task_PRIORITY, NULL);

//        xTaskCreate(timing, "timing", configMINIMAL_STACK_SIZE + 100, "fibonacci",
//                hello_task_PRIORITY, NULL);

    TimerStart();

    //TimerStart2();

    //
    // Start the scheduler.  This should not return.
    //
    vTaskStartScheduler();

    //
    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    //

    while(1)
    {
    }
}


/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void task1(void *pvParameters)
{
    //uint8_t *str = pvParameters;
    int i = 0;
    //struct timespec start_time = {0, 0};
    time_t start;
    time_t end;

    clock_t startTime;
    clock_t endTime;


    for (;;)
    {


//        if( xSemaphoreTake( xSemaphoreMutex, ( TickType_t ) 10000 ) == pdTRUE )
//        {
//            vTaskDelay(10);
//            UARTprintf("Task 1 Ended\n\r");
//            //syncVariableFromTask1toTask2 = 1;
//            xSemaphoreGive(xSemaphoreMutex);
//            //vTaskDelay(1000);
//        }
//        else
//        {
//            UARTprintf("Mutex could not be acquired by Task 1\n\r");
//        }

        if(syncVariableFromTask1toTask2 == 0)
        {
            startTime = clock();
            start = time(0);
            //UARTprintf("Task 1 Started\n\r");
            for(i = 0; i < 0xFFFFFFFF; i++)
            {
                ;
            }

            if( xSemaphoreTake( xSemaphoreMutex, ( TickType_t ) 10000 ) == pdTRUE )
            {
                //clock_gettime(CLOCK_MONOTONIC, &start_time);
                //time(&seconds);
                syncVariableFromTask1toTask2 = 1;
                xSemaphoreGive(xSemaphoreMutex);
                endTime = clock();
                end = time(0);
                double time_taken = (double)(endTime - startTime);
                double elapsedTime = end - start;

                //UARTprintf("Task 1 Time = %f\n\r", startTime / CLOCKS_PER_SEC);
                //UARTprintf("Task 1 Time = %f\n\r", start / CLOCKS_PER_SEC);


                //UARTprintf("Task 1 Ended at time = %f\n\r", time_taken / CLOCKS_PER_SEC);
                //UARTprintf("Task 1 Ended at time = %f\n\r", elapsedTime / CLOCKS_PER_SEC);
                vTaskDelay(1000);
            }
            else
            {
                UARTprintf("Task 1 could not acquire the mutex\n\r");
            }


        }

    }
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void task2(void *pvParameters)
{
    //uint8_t *str = pvParameters;

    time_t seconds;

    vTaskDelay(40);
    int i = 0;

    clock_t startTime;
    clock_t endTime;

    for (;;)
    {
        if(syncVariableFromTask1toTask2 == 1)
        {
            //UARTprintf("Task 2 Started\n\r");
            startTime = clock();
            for(i = 0; i < 0xFFFFFFFF; i++)
            {
                ;
            }

            if( xSemaphoreTake( xSemaphoreMutex, ( TickType_t ) 10000 ) == pdTRUE )
            {
                //time(&seconds);
                syncVariableFromTask1toTask2 = 0;
                xSemaphoreGive(xSemaphoreMutex);
                endTime = clock();

                //UARTprintf("Task 2 Time = %f\n\r", startTime);


                double time_taken = (double)(endTime - startTime);
                //UARTprintf("Task 2 Ended at time = %f\n\r", time_taken);
                vTaskDelay(1000);
            }
            else
             {
                 UARTprintf("Task 2 could not acquire the mutex\n\r");
             }

            //vTaskDelay(10);

        }


    }
}

void TimerStart(void)
{
    if( xTimerStart( one_shot_timer, 0 ) != pdPASS )
    {
        /* The timer could not be set into the Active
        state. */
        UARTprintf("Timer could not be started\n\r");
    }

}

void TimerStart2(void)
{
    if( xTimerStart( try_this_timer, 0 ) != pdPASS )
    {
        /* The timer could not be set into the Active
        state. */
        UARTprintf("Timer could not be started\n\r");
    }


}


static void task1Problem3(void *pvParameters)
{


    for (;;)
    {
        if(signalTask1 == true)
        {
            //UARTprintf("****Task of 10ms Started****\n\r");
            start_time=xTaskGetTickCount();
            FIB_TEST(50, 700);
            xSemaphoreTake( xSemaphoreMutex, portMAX_DELAY);

            signalTask1 = false;
            xSemaphoreGive(xSemaphoreMutex);

            //FIB_TEST(50, 750);
            end_time=xTaskGetTickCount();
            int32_t elapsed=end_time-start_time;
            UARTprintf("%d Elapsed time TAsk 1 \n\r",elapsed/portTICK_PERIOD_MS);
            vTaskDelay(1000);

        }

    }
}

static void task2Problem3(void *pvParameters)
{
    int i = 0;


    for (;;)
    {

        if(signalTask2 == true)
        {
            //UARTprintf("****Task of 10ms Started****\n\r");
            start_time1=xTaskGetTickCount();
            FIB_TEST(50, 2675);
            xSemaphoreTake( xSemaphoreMutex, portMAX_DELAY);

            signalTask2 = false;
            xSemaphoreGive(xSemaphoreMutex);

            //FIB_TEST(50, 750);
            end_time1=xTaskGetTickCount();
            int32_t elapsed1=end_time1-start_time1;
            UARTprintf("%d Elapsed time Task 2 \n\r",elapsed1/portTICK_PERIOD_MS);
            vTaskDelay(1000);

        }

    }
}

static void timing(void *pvParameters)
{
    int i = 0;

    TickType_t startTime;
    TickType_t endTime;
    TickType_t elapsedTime;


    while(1)
    {
        startTime = xTaskGetTickCount();
        FIB_TEST(50, 750);
        endTime = xTaskGetTickCount();

        elapsedTime = endTime - startTime;

        UARTprintf("Time elapsed = %d\n\r", elapsedTime / portTICK_PERIOD_MS);
        startTime = 0;
        endTime = 0;
    }
}
