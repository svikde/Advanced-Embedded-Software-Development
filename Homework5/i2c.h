/*****************************************************************************
 *   @File          i2c.h
 *   @Brief         Contains dependencies pertaining to i2c.c
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#ifndef I2C_H_
#define I2C_H_

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

#define TMP102_DEV_ID   (0x48)      //Temperature sensor Device ID

/**
* @Brief  Reads two bytes from the I2C Bus
* @Param  (1)Device ID, (2) Register Address
* @Return Data
**/
uint8_t* I2CGet2Byte(uint8_t dev_id, uint8_t register_address);

/**
* @Brief  Initlializes the I2C bus
* @Param  System Clock
* @Return void
**/
void init_I2C(uint32_t g_ui32SysClock);

#endif /* I2C_H_ */
