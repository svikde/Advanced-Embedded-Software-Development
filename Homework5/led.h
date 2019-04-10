/*****************************************************************************
 *   @File          led.h
 *   @Brief         Contains dependencies pertaining to led.c
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#ifndef LED_H_
#define LED_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "inc/hw_types.h"
#include "drivers/pinout.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "FreeRTOS.h"
#include "task.h"

/* Structure containing for sending led data to the logger task */
typedef struct Led
{
    uint32_t togglecount;
    char name[10];
    TickType_t timestamp;
}led_t;

/**
* @Brief  Initializes GPIO's for LEDs
* @Param  void
* @Return void
**/
void init_led(void);

/**
* @Brief  The callback function for the Led task
* @Param  Thread Parameters Typically, although I'm passing nothing here
* @Return void
**/
void _led_handler(void *pvParameters);

#endif /* LED_H_ */
