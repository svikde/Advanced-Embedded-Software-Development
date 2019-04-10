/*****************************************************************************
 *   @File          alert.c
 *   @Brief         Contains function definitions for alert.h
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#include "alert.h"
#include "log.h"
#include "temperature.h"

void _alert_handler(void *pvParameters)
{
    uint32_t temperature = 0;
    mesg_t message;

    message.IsTemperature = 0;
    message.IsLed = 0;

    while(1)
    {
        /* Wait indefinitely until notified */
        xTaskNotifyWait( 0x00, ULONG_MAX, &temperature, portMAX_DELAY);
        message.temperature.celcius = (double)(temperature);
        sprintf(message.str,"Alert : Temperature > %d C, Reduce Temperature",TEMP_THRESHOLD);
        xQueueSend(logger_queue, &message, 0);
    }
}


