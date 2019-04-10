/*****************************************************************************
 *   @File          temperature.h
 *   @Brief         Contains dependencies pertaining to temperature.c and
 *                  the temperature sensor TMP102.
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#include "FreeRTOS.h"
#include "task.h"
#include "i2c.h"

/* Scaling factor for the temperature sensor */
#define SCALING_FACTOR  (0.0625)

/* Temperature threshold beyond which alert is sent to the logger task */
#define TEMP_THRESHOLD  (28)

/* Structure containing data relevant to temperature sensor */
typedef struct Temperature
{
    float celcius;
    float farenheit;
    float kelvin;
    TickType_t timestamp;
}tempt_t;

/**
* @Brief  This function reads the temperature register of the temperature sensor
* @Param  void
* @Return the temperature structure object
**/
tempt_t read_temperature(void);

/**
* @Brief  The temperature task callback function
* @Param  Thread Parameters Typically, although I'm passing nothing here
* @Return void
**/
void _temperature_handler(void *pvParameters);

#endif /* TEMPERATURE_H_ */
