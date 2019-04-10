/*****************************************************************************
 *   @File          i2c.c
 *   @Brief         Contains function definitions for i2c.h
 *   @Tools_Used    FreeRTOS
 *   @Author(s)     Souvik De
 *   @Date          April 7th, 2019
 *   @version       1.0
*****************************************************************************/

#include "i2c.h"

void init_I2C(uint32_t g_ui32SysClock)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);

    GPIOPinConfigure(GPIO_PN4_I2C2SDA);
    GPIOPinConfigure(GPIO_PN5_I2C2SCL);

    GPIOPinTypeI2CSCL(GPIO_PORTN_BASE, GPIO_PIN_5);
    GPIOPinTypeI2C(GPIO_PORTN_BASE, GPIO_PIN_4);

    I2CMasterInitExpClk(I2C2_BASE, g_ui32SysClock, false);
}

uint8_t* I2CGet2Byte(uint8_t dev_id, uint8_t register_address)
{
    static uint8_t data[2] = {0};

    I2CMasterSlaveAddrSet(I2C2_BASE, dev_id, false);
    I2CMasterDataPut(I2C2_BASE, register_address);
    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));

    I2CMasterSlaveAddrSet(I2C2_BASE, dev_id, true);

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));
    data[0] = I2CMasterDataGet(I2C2_BASE);

    I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(!I2CMasterBusy(I2C2_BASE));
    while(I2CMasterBusy(I2C2_BASE));
    data[1] = I2CMasterDataGet(I2C2_BASE);

    return data;
}

