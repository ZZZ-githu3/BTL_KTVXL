#include "ina226.h"

// C?u hình I2C1 ph?n c?ng (Chân PB6: SCL, PB7: SDA)
static void INA226_I2C_Config(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // Chân SCL (PB6) và SDA (PB7) ki?u Alternate Function Open-Drain
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000; // 100kHz

    I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStructure);
}

// Hàm ghi d? li?u vào thanh ghi INA226
void INA226_WriteReg(uint8_t reg, uint16_t val) {
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, (INA226_ADDR << 1), I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

    I2C_SendData(I2C1, (val >> 8) & 0xFF);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

    I2C_SendData(I2C1, val & 0xFF);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));

    I2C_GenerateSTOP(I2C1, ENABLE);
}

// Hàm d?c d? li?u t? thanh ghi INA226
uint16_t INA226_ReadReg(uint8_t reg) {
    uint8_t msb, lsb;
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, (INA226_ADDR << 1), I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, reg);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, (INA226_ADDR << 1), I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    msb = I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, DISABLE);
    I2C_GenerateSTOP(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
    lsb = I2C_ReceiveData(I2C1);

    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return ((uint16_t)msb << 8) | lsb;
}

void INA226_Init(void) {
    INA226_I2C_Config();
    INA226_WriteReg(INA226_REG_CONFIG, 0x4127);      // C?u hình m?u do
    INA226_WriteReg(INA226_REG_CALIBRATION, 0x0A00); // H? s? hi?u chu?n dòng di?n
}

// L?y di?n áp ngõ vào (Bus Voltage) tính b?ng Volt
float INA226_GetBusVoltage_V(void) {
    uint16_t val = INA226_ReadReg(INA226_REG_BUSVOLT);
    return (float)val * 0.00125f; // 1 LSB = 1.25mV
}

// L?y dòng di?n tính b?ng mA
float INA226_GetCurrent_mA(void) {
    int16_t val = (int16_t)INA226_ReadReg(INA226_REG_CURRENT);
    return (float)val * 1.0f;
}

