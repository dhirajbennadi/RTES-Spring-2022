/*
 * Services.h
 *
 *  Created on: Apr 2, 2022
 *      Author: dhira
 */

#ifndef SERVICES_H_
#define SERVICES_H_

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

void Service1(void* pvParameters);
void Service2(void* pvParameters);
void Service3(void* pvParameters);
void Service4(void* pvParameters);
void Service5(void* pvParameters);
void Service6(void* pvParameters);
void Service7(void* pvParameters);

#endif /* SERVICES_H_ */
