#ifndef __TLE5012B_H
#define __TLE5012B_H

#include "stm32f1xx_hal.h"
#include "spi.h"

/* TLE5012B寄存器地址定义 */
#define TLE5012B_READ_STATUS         0x8001
#define TLE5012B_READ_ANGLE_VALUE    0x8021
#define TLE5012B_READ_SPEED_VALUE    0x8031

/* CS引脚定义 (手动控制) */
#define TLE5012B_CS_PIN              GPIO_PIN_4
#define TLE5012B_CS_PORT             GPIOA

/* CS引脚控制宏 */
#define TLE5012B_CS_LOW()            HAL_GPIO_WritePin(TLE5012B_CS_PORT, TLE5012B_CS_PIN, GPIO_PIN_RESET)
#define TLE5012B_CS_HIGH()           HAL_GPIO_WritePin(TLE5012B_CS_PORT, TLE5012B_CS_PIN, GPIO_PIN_SET)

/* 函数声明 */
void TLE5012B_Init(void);
uint16_t TLE5012B_ReadRegister(uint16_t reg_addr);
float TLE5012B_ReadAngle(void);
uint16_t TLE5012B_ReadSpeed(void);
uint16_t TLE5012B_ReadStatus(void);

#endif /* __TLE5012B_H */