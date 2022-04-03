/*
 * Globals.c
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
#include "Globals.h"

int abortS1= false, abortS2=false, abortS3=false, abortS4=false, abortS5=false, abortS6=false, abortS7=false;
unsigned long long seqCnt=0;

worstCaseExecutionTimes_t wcetStats;

xSemaphoreHandle g_pUARTSemaphore;
