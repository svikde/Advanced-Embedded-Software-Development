/*****************************************************************************
 *   @File          log.h
 *   @Brief         Contains dependencies pertaining to log.c
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "temperature.h"
#include "led.h"

/* Logger Queue Handler */
QueueHandle_t logger_queue;

/* Structure for passing messages using queues */
typedef struct Message
{
    tempt_t temperature;
    led_t led;
    char str[100];
    uint8_t IsTemperature;
    uint8_t IsLed;
}mesg_t;

/**
* @Brief  The callback function for the Logger task
* @Param  Thread Parameters Typically, although I'm passing nothing here
* @Return void
**/
void _log_handler(void *pvParameters);

/**
* @Brief  Converts Ticks to MS Second with Clock rate normalization
* @Param  Ticks to be converted
* @Return Converted MS Value
**/
inline uint32_t tick2ms(TickType_t tick);

#endif /* LOG_H_ */
