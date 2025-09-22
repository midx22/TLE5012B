#include "TLE5012B.h"

/**
 * @brief  TLE5012B初始化函数 (软件SPI模式)
 * @param  无
 * @retval 无
 */
void TLE5012B_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 使能GPIOA时钟 */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /* 配置CS引脚为推挽输出 */
    GPIO_InitStruct.Pin = TLE5012B_CS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TLE5012B_CS_PORT, &GPIO_InitStruct);
    
    /* 配置SCK引脚为推挽输出 */
    GPIO_InitStruct.Pin = TLE5012B_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TLE5012B_SCK_PORT, &GPIO_InitStruct);
    
    /* 配置DATA引脚为推挽输出 (初始状态) */
    GPIO_InitStruct.Pin = TLE5012B_DATA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TLE5012B_DATA_PORT, &GPIO_InitStruct);
    
    /* 初始化SPI引脚状态 (SPI Mode 1: CPOL=0, CPHA=1) */
    TLE5012B_CS_HIGH();     /* CS空闲时为高 */
    TLE5012B_SCK_LOW();     /* SCK空闲时为低 (CPOL=0) */
    TLE5012B_DATA_WRITE(0); /* DATA空闲时为低 */
    
    /* 等待传感器稳定 */
    HAL_Delay(10);
}

/**
 * @brief  软件SPI发送一个字节数据
 * @param  data: 要发送的字节数据
 * @retval 无
 */
uint8_t SoftSPI_SendByte(uint8_t data)
{
    uint8_t i;
    
    /* 设置DATA引脚为输出模式 */
    TLE5012B_DATA_OUT();
    
    /* 发送8位数据 (MSB先发送) */
    for (i = 0; i < 8; i++) {
        /* 在时钟上升沿前设置数据 (CPHA=1) */
        TLE5012B_DATA_WRITE((data & 0x80) ? 1 : 0);
        data <<= 1;
        
        /* 微秒级延时确保建立时间 */
        for (volatile int j = 0; j < 10; j++);
        
        /* 产生时钟上升沿 */
        TLE5012B_SCK_HIGH();
        
        /* 时钟高电平保持时间 */
        for (volatile int j = 0; j < 10; j++);
        
        /* 产生时钟下降沿 */
        TLE5012B_SCK_LOW();
        
        /* 时钟低电平保持时间 */
        for (volatile int j = 0; j < 10; j++);
    }
    
    return 0;
}

/**
 * @brief  软件SPI接收一个字节数据
 * @param  无
 * @retval 接收到的字节数据
 */
uint8_t SoftSPI_ReceiveByte(void)
{
    uint8_t data = 0;
    uint8_t i;
    
    /* 设置DATA引脚为输入模式 */
    TLE5012B_DATA_IN();
    
    /* 接收8位数据 (MSB先接收) */
    for (i = 0; i < 8; i++) {
        data <<= 1;
        
        /* 微秒级延时 */
        for (volatile int j = 0; j < 10; j++);
        
        /* 产生时钟上升沿 */
        TLE5012B_SCK_HIGH();
        
        /* 在时钟上升沿读取数据 (CPHA=1) */
        if (TLE5012B_DATA_READ()) {
            data |= 0x01;
        }
        
        /* 时钟高电平保持时间 */
        for (volatile int j = 0; j < 10; j++);
        
        /* 产生时钟下降沿 */
        TLE5012B_SCK_LOW();
        
        /* 时钟低电平保持时间 */
        for (volatile int j = 0; j < 10; j++);
    }
    
    return data;
}

/**
 * @brief  读取TLE5012B寄存器数据 (软件SPI SSC模式)
 * @param  reg_addr: 寄存器地址
 * @retval 读取到的16位数据
 */
uint16_t TLE5012B_ReadRegister(uint16_t reg_addr)
{
    uint8_t tx_high, tx_low;
    uint8_t rx_high, rx_low;
    uint16_t result = 0;
    
    /* 准备要发送的命令（寄存器地址） */
    tx_high = (reg_addr >> 8) & 0xFF;    /* 高字节 */
    tx_low = reg_addr & 0xFF;            /* 低字节 */
    
    /* 拉低CS开始通信 */
    TLE5012B_CS_LOW();
    
    /* 短延时，确保CS有效 */
    for (volatile int i = 0; i < 100; i++);
    
    /* 步骤1: 发送命令 (2字节) */
    SoftSPI_SendByte(tx_high);
    SoftSPI_SendByte(tx_low);
    
    /* 延时，确保从机有时间准备好数据 */
    for (volatile int i = 0; i < 1000; i++);
    
    /* 步骤2: 接收数据 (2字节) */
    rx_high = SoftSPI_ReceiveByte();
    rx_low = SoftSPI_ReceiveByte();
    
    /* 拉高CS结束通信 */
    TLE5012B_CS_HIGH();
    
    /* 组合接收到的数据 */
    result = (rx_high << 8) | rx_low;
    
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
uint16_t TLE5012B_ReadSpeed(void)
{
    return TLE5012B_ReadRegister(TLE5012B_READ_SPEED_VALUE);
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