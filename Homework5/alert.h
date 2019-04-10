/*****************************************************************************
 *   @File          alert.h
 *   @Brief         Contains dependencies pertaining to alert.c
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#ifndef ALERT_H_
#define ALERT_H_

#define ULONG_MAX   (0xFFFFFFFF)

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

/* Task reference handler for alert thread to notify it on the
 * event to superfluous temperature reading from the sensor */
TaskHandle_t xHandleAlert;

/**
* @Brief  The callback function for the Alert task
* @Param  Thread Parameters Typically, although I'm passing nothing here
* @Return void
**/
void _alert_handler(void *pvParameters);

#endif /* ALERT_H_ */
