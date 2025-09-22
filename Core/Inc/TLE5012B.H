#ifndef __TLE5012B_H
#define __TLE5012B_H

#include "main.h"

/* TLE5012B寄存器地址定义 */
#define TLE5012B_READ_STATUS         0x8001
#define TLE5012B_READ_ANGLE_VALUE    0x8021
#define TLE5012B_READ_SPEED_VALUE    0x8031

/* 软件SPI引脚定义 */
#define TLE5012B_CS_PIN              GPIO_PIN_4
#define TLE5012B_CS_PORT             GPIOA
#define TLE5012B_SCK_PIN             GPIO_PIN_5
#define TLE5012B_SCK_PORT            GPIOA
#define TLE5012B_DATA_PIN            GPIO_PIN_7
#define TLE5012B_DATA_PORT           GPIOA

/* 软件SPI控制宏 */
#define TLE5012B_CS_LOW()            HAL_GPIO_WritePin(TLE5012B_CS_PORT, TLE5012B_CS_PIN, GPIO_PIN_RESET)
#define TLE5012B_CS_HIGH()           HAL_GPIO_WritePin(TLE5012B_CS_PORT, TLE5012B_CS_PIN, GPIO_PIN_SET)
#define TLE5012B_SCK_LOW()           HAL_GPIO_WritePin(TLE5012B_SCK_PORT, TLE5012B_SCK_PIN, GPIO_PIN_RESET)
#define TLE5012B_SCK_HIGH()          HAL_GPIO_WritePin(TLE5012B_SCK_PORT, TLE5012B_SCK_PIN, GPIO_PIN_SET)
#define TLE5012B_DATA_OUT()          do { GPIO_InitTypeDef gpio = {TLE5012B_DATA_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH}; HAL_GPIO_Init(TLE5012B_DATA_PORT, &gpio); } while(0)
#define TLE5012B_DATA_IN()           do { GPIO_InitTypeDef gpio = {TLE5012B_DATA_PIN, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH}; HAL_GPIO_Init(TLE5012B_DATA_PORT, &gpio); } while(0)
#define TLE5012B_DATA_WRITE(x)       HAL_GPIO_WritePin(TLE5012B_DATA_PORT, TLE5012B_DATA_PIN, (x) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define TLE5012B_DATA_READ()         HAL_GPIO_ReadPin(TLE5012B_DATA_PORT, TLE5012B_DATA_PIN)

/* 函数声明 */
void TLE5012B_Init(void);
uint8_t SoftSPI_SendByte(uint8_t data);
uint8_t SoftSPI_ReceiveByte(void);
uint16_t TLE5012B_ReadRegister(uint16_t reg_addr);
float TLE5012B_ReadAngle(void);
uint16_t TLE5012B_ReadSpeed(void);
uint16_t TLE5012B_ReadStatus(void);

#endif /* __TLE5012B_H */