
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

#define TRUE (1)
#define FALSE (0)





#define SEQUENCER_FREQ (freq)



#include "Services.h"
#include "Globals.h"


//*****************************************************************************
//
// The mutex that protects concurrent access of UART from multiple tasks.
//
//*****************************************************************************

   xSemaphoreHandle g_pUARTSemaphore;
   SemaphoreHandle_t g_pUARTSemaphore1,g_pUARTSemaphore2,g_pUARTSemaphore3,g_pUARTSemaphore4,g_pUARTSemaphore5,g_pUARTSemaphore6,g_pUARTSemaphore7;

   //*****************************************************************************
   //
   // Timer variables.
   //
   //*****************************************************************************

   TickType_t Start, Wcet1=0, Wcet2=0, Wcet3=0, Wcet4=0, Wcet5=0, Wcet6=0,Wcet7=0;



void
Timer0IntHandler(void)
{
   // UARTprintf("\n Entered ISR!\n");
    /* xHigherPriorityTaskWoken must be set to pdFALSE
       so it can later be detected if it was set to pdTRUE by any of the functions called within the interrupt. */
       BaseType_t xHigherPriorityTaskWoken = pdFALSE;
       ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);  // Clear the timer interrupt.
       ROM_IntMasterDisable();

       seqCnt++;
       #ifdef seqgen
       if((seqCnt % 10) == 0) // Service_1 = RT_MAX-1   @ 3 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore1, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive1 failed!\n");
           }
        }
       if((seqCnt % 30) == 0) // Service_2 = RT_MAX-2   @ 1 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore2, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive2 failed!\n");
           }
       }
       if((seqCnt % 60) == 0) // Service_3 = RT_MAX-3   @ 0.5 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore3, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive3 failed!\n");
           }
       }
       if((seqCnt % 30) == 0) // Service_4 = RT_M AX-2   @ 1 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore4, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive4 failed!\n");
           }
       }
       if((seqCnt % 60) == 0) // Service_5 = RT_MAX-3   @ 0.5 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore5, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive5 failed!\n");
           }
       }
       if((seqCnt % 30) == 0) // Service_6 = RT_MAX-2   @ 1 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore6, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive6 failed!\n");
           }
       }
       if((seqCnt % 300) == 0) // Service_7 = RT_MIN   @0.1 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore7, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive7 failed!\n");
           }
       }
       #endif

       #ifdef seqgen2x
       if((seqCnt % 2) == 0) // Servcie_1 = RT_MAX-1   @ 30 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore1, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive1 failed!\n");
           }
       }
       if((seqCnt % 6) == 0) // Service_2 = RT_MAX-2   @ 10 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore2, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive2 failed!\n");
           }
       }
       if((seqCnt % 12) == 0)// Service_3 = RT_MAX-3   @ 5 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore3, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive3 failed!\n");
           }
       }
       if((seqCnt % 6) == 0) // Service_4 = RT_MAX-2   @ 10 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore4, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive4 failed!\n");
           }
       }
       if((seqCnt % 12) == 0)// Service_5 = RT_MAX-3   @ 5 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore5, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive5 failed!\n");
           }
       }
       if((seqCnt % 6) == 0) // Service_6 = RT_MAX-2   @ 10 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore6, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive6 failed!\n");
           }
       }
       if((seqCnt % 60) == 0)// Service_7 = RT_MIN   @ 1 Hz
       {
           if(xSemaphoreGiveFromISR(g_pUARTSemaphore7, &xHigherPriorityTaskWoken)!= pdTRUE)
           {
               UARTprintf("\n SemaphoreGive7 failed!\n");
           }
       }
       #endif

   #ifdef seqgen100x
     if((seqCnt % 100) == 0) // Servcie_1 = RT_MAX-1   @ 30 Hz
     {
         if(xSemaphoreGiveFromISR(g_pUARTSemaphore1, &xHigherPriorityTaskWoken)!= pdTRUE)
         {
             UARTprintf("\n SemaphoreGive1 failed!\n");
         }
     }
     if((seqCnt % 300) == 0) // Service_2 = RT_MAX-2   @ 10 Hz
     {
         if(xSemaphoreGiveFromISR(g_pUARTSemaphore2, &xHigherPriorityTaskWoken)!= pdTRUE)
         {
             UARTprintf("\n SemaphoreGive2 failed!\n");
         }
     }
     if((seqCnt % 600) == 0)// Service_3 = RT_MAX-3   @ 5 Hz
     {
         if(xSemaphoreGiveFromISR(g_pUARTSemaphore3, &xHigherPriorityTaskWoken)!= pdTRUE)
         {
             UARTprintf("\n SemaphoreGive3 failed!\n");
         }
     }
     if((seqCnt % 300) == 0) // Service_4 = RT_MAX-2   @ 10 Hz
     {
         if(xSemaphoreGiveFromISR(g_pUARTSemaphore4, &xHigherPriorityTaskWoken)!= pdTRUE)
         {
             UARTprintf("\n SemaphoreGive4 failed!\n");
         }
     }
     if((seqCnt % 600) == 0)// Service_5 = RT_MAX-3   @ 5 Hz
     {
         if(xSemaphoreGiveFromISR(g_pUARTSemaphore5, &xHigherPriorityTaskWoken)!= pdTRUE)
         {
             UARTprintf("\n SemaphoreGive5 failed!\n");
         }
     }
     if((seqCnt % 300) == 0) // Service_6 = RT_MAX-2   @ 10 Hz
     {
         if(xSemaphoreGiveFromISR(g_pUARTSemaphore6, &xHigherPriorityTaskWoken)!= pdTRUE)
         {
             UARTprintf("\n SemaphoreGive6 failed!\n");
         }
     }
     if((seqCnt % 3000) == 0)// Service_7 = RT_MIN   @ 1 Hz
     {
         if(xSemaphoreGiveFromISR(g_pUARTSemaphore7, &xHigherPriorityTaskWoken)!= pdTRUE)
         {
             UARTprintf("\n SemaphoreGive7 failed!\n");
         }
     }
     #endif
           if(seqCnt >= SEQUENCER_PERIODS)
           {
               xSemaphoreGiveFromISR(g_pUARTSemaphore1, &xHigherPriorityTaskWoken);
               xSemaphoreGiveFromISR(g_pUARTSemaphore2, &xHigherPriorityTaskWoken);
               xSemaphoreGiveFromISR(g_pUARTSemaphore3, &xHigherPriorityTaskWoken);
               xSemaphoreGiveFromISR(g_pUARTSemaphore4, &xHigherPriorityTaskWoken);
               xSemaphoreGiveFromISR(g_pUARTSemaphore5, &xHigherPriorityTaskWoken);
               xSemaphoreGiveFromISR(g_pUARTSemaphore6, &xHigherPriorityTaskWoken);
               xSemaphoreGiveFromISR(g_pUARTSemaphore7, &xHigherPriorityTaskWoken);

               abortS1 = TRUE;
               abortS2 = TRUE;
               abortS3 = TRUE;
               abortS4 = TRUE;
               abortS5 = TRUE;
               abortS6 = TRUE;
               abortS7 = TRUE;
               ROM_IntDisable(INT_TIMER0A);
               ROM_TimerIntDisable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
               ROM_TimerDisable(TIMER0_BASE, TIMER_A);
               UARTprintf("\nTEST COMPLETE!!!\n");
               UARTprintf("\nWorst Case Execution Time Service 1= %d\n", wcetStats.wcetService1);
               UARTprintf("\nWorst Case Execution Time Service 2= %d\n", wcetStats.wcetService2);
               UARTprintf("\nWorst Case Execution Time Service 3= %d\n", wcetStats.wcetService3);
               UARTprintf("\nWorst Case Execution Time Service 4= %d\n", wcetStats.wcetService4);
               UARTprintf("\nWorst Case Execution Time Service 5= %d\n", wcetStats.wcetService5);
               UARTprintf("\nWorst Case Execution Time Service 6= %d\n", wcetStats.wcetService6);
               UARTprintf("\nWorst Case Execution Time Service 7= %d\n", wcetStats.wcetService7);

               UARTprintf("\n");
       }

       ROM_IntMasterEnable();

 }

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

int
main(void)
{
    ROM_FPULazyStackingEnable();
    //
    // Set the clocking to run at 50 MHz from the PLL.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    //Tick count gets incremented at every tick interrupt

    //
    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    //
    ConfigureUART();

    //
    // Enable processor interrupts.
    //
    ROM_IntMasterEnable();

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    // Configure the one 32-bit periodic timer.
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A,ROM_SysCtlClockGet()/SEQUENCER_FREQ);

    // Setup the interrupts for the timer timeouts.
    ROM_IntEnable(INT_TIMER0A);

    // Enable interrupt
    ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Enable the timer 0.
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);

    UARTprintf("\nWelcome!\n");
    Start = xTaskGetTickCount();

    //
    // Create a mutex to guard the UART.
    //
    g_pUARTSemaphore = xSemaphoreCreateMutex();

    //For all the seven tasks
    g_pUARTSemaphore1 = xSemaphoreCreateBinary();
    g_pUARTSemaphore2 = xSemaphoreCreateBinary();
    g_pUARTSemaphore3 = xSemaphoreCreateBinary();
    g_pUARTSemaphore4 = xSemaphoreCreateBinary();
    g_pUARTSemaphore5 = xSemaphoreCreateBinary();
    g_pUARTSemaphore6 = xSemaphoreCreateBinary();
    g_pUARTSemaphore7 = xSemaphoreCreateBinary();



    if(xTaskCreate(Service1, (const char *)"Service1", 100, NULL, configMAX_PRIORITIES, NULL) != pdTRUE)
    {
        while(1)
        {

        }
     }
    if(xTaskCreate(Service2, (const char *)"Service2", 100, NULL, configMAX_PRIORITIES-1, NULL) != pdTRUE)
       {
           while(1)
           {

           }
        }
    if(xTaskCreate(Service3, (const char *)"Service3", 100, NULL, configMAX_PRIORITIES-2, NULL) != pdTRUE)
        {
            while(1)
            {

            }
         }
    if(xTaskCreate(Service4, (const char *)"Service4", 100, NULL, configMAX_PRIORITIES-1, NULL) != pdTRUE)
        {
            while(1)
            {

            }
         }
    if(xTaskCreate(Service5, (const char *)"Service5", 100, NULL, configMAX_PRIORITIES-2, NULL) != pdTRUE)
        {
            while(1)
            {

            }
         }
    if(xTaskCreate(Service6, (const char *)"Service6", 100, NULL, configMAX_PRIORITIES-1, NULL) != pdTRUE)
        {
            while(1)
            {

            }
         }
    if(xTaskCreate(Service7, (const char *)"Service7", 100, NULL, configMAX_PRIORITIES-3, NULL) != pdTRUE)
        {
            while(1)
            {

            }
         }

    vTaskStartScheduler();

    //
    // In case the scheduler returns for some reason, print an error and loop
    // forever.
    //

    while(1)
    {
    }
}


