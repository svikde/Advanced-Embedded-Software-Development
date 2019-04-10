/*****************************************************************************
 *   @File          main.c
 *   @Brief         Creates 3 tasks to gauge temperature through I2C bus,
 *                  blink led, log data using queues and send alert on
 *                  superflous rise of temperature.
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "grlib/grlib.h"
#include "drivers/frame.h"
#include "drivers/kentec320x240x16_ssd2119.h"
#include "drivers/pinout.h"
#include "drivers/touch.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/systick.h"
#include "utils/random.h"
#include "utils/uartstdio.c"

#include "temperature.h"
#include "led.h"
#include "log.h"
#include "alert.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

uint32_t g_ui32SysClock;

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
void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName)
{
    tContext sContext;

    //
    // A fatal FreeRTOS error was detected, so display an error message.
    //
    GrContextInit(&sContext, &g_sKentec320x240x16_SSD2119);
    GrContextForegroundSet(&sContext, ClrRed);
    GrContextBackgroundSet(&sContext, ClrBlack);
    GrContextFontSet(&sContext, g_psFontCm20);
    GrStringDrawCentered(&sContext, "Fatal FreeRTOS error!", -1,
                         GrContextDpyWidthGet(&sContext) / 2,
                         (((GrContextDpyHeightGet(&sContext) - 24) / 2) +
                          24), 1);

    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

void InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

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
    // Run from the PLL at 120 MHz.
    //
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480),
                                            configCPU_CLOCK_HZ);

    InitConsole();

    UARTprintf("Program Running \n");

    init_I2C(g_ui32SysClock);
    init_led();

    logger_queue = xQueueCreate(10, sizeof(mesg_t));

    if(xTaskCreate(_temperature_handler, "Task Temperature", 1000, NULL, 1, NULL) != pdPASS)
    {
        UARTprintf("Failed to create temperature task !\n");
        while(1);
    }

    if(xTaskCreate(_led_handler, "Task Led", 1000, NULL, 1, NULL) != pdPASS)
    {
        UARTprintf("Failed to create led task !\n");
        while(1);
    }

    if(xTaskCreate(_log_handler, "Task Logger", 1000, NULL, 1, NULL) != pdPASS)
    {
        UARTprintf("Failed to create logger task !\n");
        while(1);
    }

    if(xTaskCreate(_alert_handler, "Task Alert", 1000, NULL, 2, &xHandleAlert) != pdPASS)
    {
        UARTprintf("Failed to create Alert task !\n");
        while(1);
    }

    vTaskStartScheduler();

    while(1);
}
