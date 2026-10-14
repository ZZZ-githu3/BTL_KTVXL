#include "stm32f10x.h"
#include "ina226.h"

float voltage = 0;
float current = 0;

int main(void) {
    volatile uint32_t i; // Khai báo bi?n i ? d?u hàm
    
    SystemInit();
    INA226_Init(); // Kh?i t?o I2C và INA226

    while(1) {
        voltage = INA226_GetBusVoltage_V(); // Ð?c di?n áp pin
        current = INA226_GetCurrent_mA(); // Ð?c dòng di?n
        
        // Vòng l?p tr?
        for(i = 0; i < 500000; i++);
    }
}

