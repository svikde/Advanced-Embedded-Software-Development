/*****************************************************************************
 *   @File          temperature.c
 *   @Brief         Contains function definitions for temperature.h.
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#include "temperature.h"
#include "log.h"
#include "alert.h"

#include <stdint.h>
#include <stdbool.h>


tempt_t read_temperature(void)
{
    uint8_t* data;
    int16_t temperature_hex = 0;
    tempt_t temperature;

    data = I2CGet2Byte(TMP102_DEV_ID, 0);
    temperature_hex = ((*data << 8) | *(data + 1)) >> 4;

    if((temperature_hex & 0x0800) == 0x0800)
    {
        temperature_hex ^= (0x0FFF);
        temperature_hex += 1;
        temperature_hex = temperature_hex * (-1);
    }

    temperature.celcius = temperature_hex * SCALING_FACTOR;
    temperature.farenheit = (temperature.celcius * 1.8) + 32;
    temperature.kelvin = temperature.celcius + 273.15;
    temperature.timestamp = xTaskGetTickCount();

    return temperature;
}

void _temperature_handler(void *pvParameters)
{
    TickType_t xLastWakeTime;
    mesg_t message;
    uint32_t tempunsg = 0;

    message.IsTemperature = 1;
    message.IsLed = 0;

    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
        message.temperature = read_temperature();
        message.temperature.timestamp = xTaskGetTickCount();

        /* Send alert to the alert task if temperature exceeds threshold */
        if(message.temperature.celcius >= TEMP_THRESHOLD)
        {
            tempunsg = (uint32_t)(message.temperature.celcius * 1000);
            /* Reference with Alert task handler to notify the alert task */
            xTaskNotify(xHandleAlert, tempunsg, eSetValueWithOverwrite);
        }
        else
        {
            xQueueSend(logger_queue, &message, 0);
        }
    }
}

