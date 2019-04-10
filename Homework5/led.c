/*****************************************************************************
 *   @File          led.c
 *   @Brief         Contains function definitions for led.h
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#include "led.h"
#include "log.h"

void init_led(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_1);
}

void _led_handler(void *pvParameters)
{
    TickType_t xLastWakeTime;
    mesg_t message;
    uint32_t count = 0;

    message.IsTemperature = 0;
    message.IsLed = 1;
    strcpy(message.led.name,"Souvik");

    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));

        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, GPIO_PIN_1);

        count++;
        message.led.togglecount = count;
        message.led.timestamp = xTaskGetTickCount();
        xQueueSend(logger_queue, &message, 0);

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));

        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0x0);
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0x0);

        count++;
        message.led.togglecount = count;
        message.led.timestamp = xTaskGetTickCount();
        xQueueSend(logger_queue, &message, 0);
    }
}


