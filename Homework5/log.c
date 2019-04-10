/*****************************************************************************
 *   @File          log.c
 *   @Brief         Contains function definitions for log.h
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#include "log.h"

void _log_handler(void *pvParameters)
{
    mesg_t message;
    uint16_t tempdec;

    while(1)
    {
        xQueueReceive(logger_queue, &message, portMAX_DELAY);

        if(!message.IsLed && !message.IsTemperature)
        {
           tempdec = (int)message.temperature.celcius % 1000;
           UARTprintf("%s, Temperature : %d.", message.str,(int)message.temperature.celcius/1000);
           UARTprintf("%d\n",tempdec);
        }

        if(message.IsTemperature)
        {
            tempdec = 1000 * message.temperature.celcius;
            UARTprintf("[%u] Temperature : %u.",tick2ms(message.temperature.timestamp),(int)message.temperature.celcius);
            UARTprintf("%u\n",tempdec % 1000);
        }

        if(message.IsLed)
        {
            UARTprintf("[%u] Toggle Count : %u",tick2ms(message.led.timestamp),(int)message.led.togglecount);
            UARTprintf(" Name : %s\n",message.led.name);
        }
    }
}

inline uint32_t tick2ms(TickType_t tick)
{
    return ((tick * 1000)/ configTICK_RATE_HZ);
}


