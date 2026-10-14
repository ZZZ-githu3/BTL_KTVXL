#ifndef __INA226_H
#define __INA226_H

#include "stm32f10x.h"

#define INA226_ADDR 0x40

// Các thanh ghi co b?n c?a INA226
#define INA226_REG_CONFIG      0x00
#define INA226_REG_SHUNTVOLT   0x01
#define INA226_REG_BUSVOLT     0x02
#define INA226_REG_CURRENT     0x04
#define INA226_REG_CALIBRATION 0x05

void INA226_Init(void);
float INA226_GetBusVoltage_V(void);
float INA226_GetCurrent_mA(void);

#endif


