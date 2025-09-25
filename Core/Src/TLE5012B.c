#include "TLE5012B.h"

/* 外部SPI句柄 */
extern SPI_HandleTypeDef hspi1;

/**
 * @brief  TLE5012B初始化函数 (硬件SPI模式)
 * @param  无
 * @retval 无
 */
void TLE5012B_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置CS引脚为推挽输出 (手动控制) */
    GPIO_InitStruct.Pin = TLE5012B_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TLE5012B_CS_PORT, &GPIO_InitStruct);
    
    /* 初始化CS引脚状态 */
    TLE5012B_CS_HIGH();     /* CS空闲时为高 */
    
    /* 等待传感器稳定 */
    HAL_Delay(10);
}

/**
 * @brief  读取TLE5012B寄存器数据 (硬件SPI SSC模式)
 * @param  reg_addr: 寄存器地址
 * @retval 读取到的16位数据
 */
uint16_t TLE5012B_ReadRegister(uint16_t reg_addr)
{
    uint8_t tx_cmd[2] = {0};
    uint8_t rx_data[2] = {0};
    uint16_t result = 0;
    HAL_StatusTypeDef status;
    
    /* 准备要发送的命令（寄存器地址） */
    tx_cmd[0] = (reg_addr >> 8) & 0xFF;    /* 高字节 */
    tx_cmd[1] = reg_addr & 0xFF; 
    
    /* 拉低CS开始通信 */
    TLE5012B_CS_LOW();
    
    /* 在CS拉低后增加延时，确保从机准备好 */
    HAL_Delay(1);
    
    /* 步骤1: 使用硬件SPI发送命令 (2字节) */
    HAL_SPI_Transmit(&hspi1, tx_cmd, 2, HAL_MAX_DELAY);

    
    /* 延时，确保从机有时间准备好数据 */
    for(volatile int i = 0; i < 100; i++);
    
    /* 步骤2: 使用硬件SPI接收数据 (2字节) */
    HAL_SPI_Receive(&hspi1, rx_data, 2, HAL_MAX_DELAY);

    
    /* 拉高CS结束通信 */
    TLE5012B_CS_HIGH();
    
    /* 组合接收到的数据 */
    result = (rx_data[0] << 8) | rx_data[1];
    
    return result;
}

/**
 * @brief  读取角度值
 * @param  无
 * @retval 角度值（度）
 */
float TLE5012B_ReadAngle(void)
{
    uint16_t raw_angle;
    float angle_deg;
    
    /* 读取角度寄存器 */
    raw_angle = TLE5012B_ReadRegister(TLE5012B_READ_ANGLE_VALUE);
    
    /* 检查是否读取失败 */
    if (raw_angle == 0xFFFF) {
        return -1.0f; // 返回错误值
    }
    
    /* 转换为角度值（0-360度） */
    /* TLE5012B的角度分辨率是15位，即0-32767对应0-360度 */
    angle_deg = (float)(raw_angle & 0x7FFF) * 360.0f / 32768.0f;
    
    return angle_deg;
}

/**
 * @brief  读取转速值
 * @param  无
 * @retval 转速值（原始数据）
 */
int16_t TLE5012B_ReadSpeed(void)
{   
        uint16_t raw_angle;

        raw_angle = TLE5012B_ReadRegister(TLE5012B_READ_SPEED_VALUE);
        

        raw_angle = raw_angle & 0x7FFF;
        uint16_t sign = raw_angle & 0x4000;
        if (sign == 0x4000) { // 负数
           raw_angle=raw_angle|0x8000;
        }

        int16_t speed = (int16_t)raw_angle;
        speed = speed * 1000;
        float tupd = 2.0 * 42.7 / 1000;
        float speed_v  = (float)(speed * 2 * 3.14) / 0x7fff;   //rad/s
        speed = speed_v * tupd * 60 / (2 * 3.14);

        return speed;

}

/**
 * @brief  读取状态寄存器
 * @param  无
 * @retval 状态值
 */
uint16_t TLE5012B_ReadStatus(void)
{
    return TLE5012B_ReadRegister(TLE5012B_READ_STATUS);
}