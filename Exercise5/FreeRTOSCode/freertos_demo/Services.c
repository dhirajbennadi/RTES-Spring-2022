/*
 * Services.c
 *
 *  Created on: Apr 2, 2022
 *      Author: dhira
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

#include "Services.h"
#include "Globals.h"


void Service1(void* pvParameters)
{
    volatile TickType_t tickCount= xTaskGetTickCount();
        TickType_t start_tick,stop_tick,diff_ticks = 0;
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n<TIME: %d seconds; %d milliseconds>   Service[1]", tickCount/1000, tickCount%1000);
        xSemaphoreGive(g_pUARTSemaphore);
        unsigned long S1Cnt=0;
        while(!abortS1)
        {
            if(xSemaphoreTake(g_pUARTSemaphore1, portMAX_DELAY) != pdTRUE)
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nERROR: Unable to obtain semaphore 1.\n");
                xSemaphoreGive(g_pUARTSemaphore);
            }
            else
            {
                start_tick = xTaskGetTickCount();
                tickCount = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\n Service 1 started at : %d ms", tickCount);
                xSemaphoreGive(g_pUARTSemaphore);
                S1Cnt++;
                stop_tick = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nExecution time of Service [1]: %d msecs\n", stop_tick - start_tick);
                xSemaphoreGive(g_pUARTSemaphore);
                if((stop_tick - start_tick) > diff_ticks)
                {
                    diff_ticks = stop_tick - start_tick;
                    wcetStats.wcetService1 = stop_tick - start_tick;
                }
            }
        }
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n");
        UARTprintf("\nService [1] ran %d ", S1Cnt);
        //UARTprintf("\n WCET for Service 1 = %d", wcetStats.wcetService1);
        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelete(NULL);
}

void Service2(void* pvParameters)
{
    volatile TickType_t tickCount= xTaskGetTickCount();
        TickType_t start_tick,stop_tick,diff_ticks = 0;
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n<TIME: %d seconds; %d milliseconds>   Service[2]", tickCount/1000, tickCount%1000);
        xSemaphoreGive(g_pUARTSemaphore);
        unsigned long S2Cnt=0;
        while(!abortS2)
        {
            if(xSemaphoreTake(g_pUARTSemaphore2, portMAX_DELAY) != pdTRUE)
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nERROR: Unable to obtain semaphore 2.\n");
                xSemaphoreGive(g_pUARTSemaphore);
            }
            else
            {
                start_tick = xTaskGetTickCount();
                tickCount = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\n Service 2 started at : %d ms", tickCount);
                xSemaphoreGive(g_pUARTSemaphore);
                S2Cnt++;
                stop_tick = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nExecution time of Service [2]: %d msecs\n", stop_tick - start_tick);
                xSemaphoreGive(g_pUARTSemaphore);
                if((stop_tick - start_tick) > diff_ticks)
                {
                    diff_ticks = stop_tick - start_tick;
                    wcetStats.wcetService2 = stop_tick - start_tick;
                }
            }
        }
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n");
        UARTprintf("\nService [2] ran %d ", S2Cnt);
        //UARTprintf("\n WCET for Service 2 = %d", wcetStats.wcetService2);
        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelete(NULL);
}
void Service3(void* pvParameters)
{
    volatile TickType_t tickCount= xTaskGetTickCount();
        TickType_t start_tick,stop_tick,diff_ticks = 0;
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n<TIME: %d seconds; %d milliseconds>   Service[3]", tickCount/1000, tickCount%1000);
        xSemaphoreGive(g_pUARTSemaphore);
        unsigned long S3Cnt=0;
        while(!abortS3)
        {
            if(xSemaphoreTake(g_pUARTSemaphore3, portMAX_DELAY) != pdTRUE)
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nERROR: Unable to obtain semaphore 3.\n");
                xSemaphoreGive(g_pUARTSemaphore);
            }
            else
            {
                start_tick = xTaskGetTickCount();
                tickCount = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\n Service 3 started at : %d ms", tickCount);
                xSemaphoreGive(g_pUARTSemaphore);
                S3Cnt++;
                stop_tick = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nExecution time of Service [3]: %d msecs\n", stop_tick - start_tick);
                xSemaphoreGive(g_pUARTSemaphore);
                if((stop_tick - start_tick) > diff_ticks)
                {
                    diff_ticks = stop_tick - start_tick;
                    wcetStats.wcetService3 = stop_tick - start_tick;
                }
            }
        }
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n");
        UARTprintf("\nService [3] ran %d ", S3Cnt);
        //UARTprintf("\n WCET for Service 3 = %d", wcetStats.wcetService3);
        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelete(NULL);
}
void Service4(void* pvParameters)
{
    volatile TickType_t tickCount= xTaskGetTickCount();
        TickType_t start_tick,stop_tick,diff_ticks = 0;
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n<TIME: %d seconds; %d milliseconds>   Service[4]", tickCount/1000, tickCount%1000);
        xSemaphoreGive(g_pUARTSemaphore);
        unsigned long S4Cnt=0;
        while(!abortS4)
        {
            if(xSemaphoreTake(g_pUARTSemaphore4, portMAX_DELAY) != pdTRUE)
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nERROR: Unable to obtain semaphore 4.\n");
                xSemaphoreGive(g_pUARTSemaphore);
            }
            else
            {
                start_tick = xTaskGetTickCount();
                tickCount = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\n Service 4 started at : %d ms", tickCount);
                xSemaphoreGive(g_pUARTSemaphore);
                S4Cnt++;
                stop_tick = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nExecution time of Service [4]: %d msecs\n", stop_tick - start_tick);
                xSemaphoreGive(g_pUARTSemaphore);
                if((stop_tick - start_tick) > diff_ticks)
                {
                    diff_ticks = stop_tick - start_tick;
                    wcetStats.wcetService4 = stop_tick - start_tick;
                }
            }
        }
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n");
        UARTprintf("\nService [4] ran %d ", S4Cnt);
        //UARTprintf("\n WCET for Service 4 = %d", wcetStats.wcetService4);
        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelete(NULL);
}
void Service5(void* pvParameters)
{
    volatile TickType_t tickCount= xTaskGetTickCount();
        TickType_t start_tick,stop_tick,diff_ticks = 0;
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n<TIME: %d seconds; %d milliseconds>   Service[5]", tickCount/1000, tickCount%1000);
        xSemaphoreGive(g_pUARTSemaphore);
        unsigned long S5Cnt=0;
        while(!abortS5)
        {
            if(xSemaphoreTake(g_pUARTSemaphore5, portMAX_DELAY) != pdTRUE)
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nERROR: Unable to obtain semaphore 5.\n");
                xSemaphoreGive(g_pUARTSemaphore);
            }
            else
            {
                start_tick = xTaskGetTickCount();
                tickCount = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\n Service 5 started at : %d ms", tickCount);
                xSemaphoreGive(g_pUARTSemaphore);
                S5Cnt++;
                stop_tick = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nExecution time of Service [5]: %d msecs\n", stop_tick - start_tick);
                xSemaphoreGive(g_pUARTSemaphore);
                if((stop_tick - start_tick) > diff_ticks)
                {
                    diff_ticks = stop_tick - start_tick;
                    wcetStats.wcetService5 = stop_tick - start_tick;
                }
            }
        }
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n");
        UARTprintf("\nService [5] ran %d ", S5Cnt);
        //UARTprintf("\n WCET for Service 5 = %d", wcetStats.wcetService5);
        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelete(NULL);
}
void Service6(void* pvParameters)
{
    volatile TickType_t tickCount= xTaskGetTickCount();
        TickType_t start_tick,stop_tick,diff_ticks = 0;
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n<TIME: %d seconds; %d milliseconds>   Service[6]", tickCount/1000, tickCount%1000);
        xSemaphoreGive(g_pUARTSemaphore);
        unsigned long S6Cnt=0;
        while(!abortS6)
        {
            if(xSemaphoreTake(g_pUARTSemaphore6, portMAX_DELAY) != pdTRUE)
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nERROR: Unable to obtain semaphore 6.\n");
                xSemaphoreGive(g_pUARTSemaphore);
            }
            else
            {
                start_tick = xTaskGetTickCount();
                tickCount = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\n Service 6 started at : %d ms", tickCount);
                xSemaphoreGive(g_pUARTSemaphore);
                S6Cnt++;
                stop_tick = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nExecution time of Service [6]: %d msecs\n", stop_tick - start_tick);
                xSemaphoreGive(g_pUARTSemaphore);
                if((stop_tick - start_tick) > diff_ticks)
                {
                    diff_ticks = stop_tick - start_tick;
                    wcetStats.wcetService6 = stop_tick - start_tick;
                }
            }
        }
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n");
        UARTprintf("\nService [6] ran %d ", S6Cnt);
        //UARTprintf("\n WCET for Service 6 = %d", wcetStats.wcetService6);
        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelete(NULL);
}
void Service7(void* pvParameters)
{
    volatile TickType_t tickCount= xTaskGetTickCount();
        TickType_t start_tick,stop_tick,diff_ticks = 0;
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n<TIME: %d seconds; %d milliseconds>   Service[7]", tickCount/1000, tickCount%1000);
        xSemaphoreGive(g_pUARTSemaphore);
        unsigned long S7Cnt=0;
        while(!abortS7)
        {
            if(xSemaphoreTake(g_pUARTSemaphore7, portMAX_DELAY) != pdTRUE)
            {
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nERROR: Unable to obtain semaphore 7.\n");
                xSemaphoreGive(g_pUARTSemaphore);
            }
            else
            {
                start_tick = xTaskGetTickCount();
                tickCount = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\n Service 7 started at : %d ms", tickCount);
                xSemaphoreGive(g_pUARTSemaphore);
                S7Cnt++;
                stop_tick = xTaskGetTickCount();
                xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
                UARTprintf("\nExecution time of Service [7]: %d msecs\n", stop_tick - start_tick);
                xSemaphoreGive(g_pUARTSemaphore);
                if((stop_tick - start_tick) > diff_ticks)
                {
                    diff_ticks = stop_tick - start_tick;
                    wcetStats.wcetService7 = stop_tick - start_tick;
                }
            }
        }
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\n");
        UARTprintf("\n Service [7] ran %d times", S7Cnt);
        //UARTprintf("\n WCET for Service 7 = %d", wcetStats.wcetService7);
        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelete(NULL);
}
