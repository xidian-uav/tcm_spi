/**
 ****************************************************************************************************
 * @file        spi.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       SPI 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200424
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/SPI/spi.h"

SPI_HandleTypeDef g_spi2_handler; /* SPI2句柄 */

/**
 * @brief       SPI初始化代码
 *   @note      主机模式,8位数据,禁止硬件片选
 * @param       无
 * @retval      无
 */
void spi2_init(void)
{
    SPI2_SPI_CLK_ENABLE(); /* SPI2时钟使能 */

    g_spi2_handler.Instance = SPI2_SPI;                                /* SPI2 */
    g_spi2_handler.Init.Mode = SPI_MODE_MASTER;                        /* 设置SPI工作模式，设置为主模式 */
    g_spi2_handler.Init.Direction = SPI_DIRECTION_2LINES;              /* 设置SPI单向或者双向的数据模式:SPI设置为双线模式 */
    g_spi2_handler.Init.DataSize = SPI_DATASIZE_8BIT;                  /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
    g_spi2_handler.Init.CLKPolarity = SPI_POLARITY_HIGH;               /* 串行同步时钟的空闲状态为高电平 */
    g_spi2_handler.Init.CLKPhase = SPI_PHASE_2EDGE;                    /* 串行同步时钟的第二个跳变沿（上升或下降）数据被采样 */
    g_spi2_handler.Init.NSS = SPI_NSS_SOFT;                            /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
    g_spi2_handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; /* 定义波特率预分频的值:波特率预分频值为256 */
    g_spi2_handler.Init.FirstBit = SPI_FIRSTBIT_MSB;                   /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
    g_spi2_handler.Init.TIMode = SPI_TIMODE_DISABLE;                   /* 关闭TI模式 */
    g_spi2_handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* 关闭硬件CRC校验 */
    g_spi2_handler.Init.CRCPolynomial = 7;                             /* CRC值计算的多项式 */
    HAL_SPI_Init(&g_spi2_handler);                                     /* 初始化 */

    __HAL_SPI_ENABLE(&g_spi2_handler); /* 使能SPI2 */

    spi2_read_write_byte(0Xff); /* 启动传输, 实际上就是产生8个时钟脉冲, 达到清空DR的作用, 非必需 */
}

/**
 * @brief       SPI底层驱动，时钟使能，引脚配置
 *   @note      此函数会被HAL_SPI_Init()调用
 * @param       hspi:SPI句柄
 * @retval      无
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    if (hspi->Instance == SPI2_SPI)
    {
        SPI2_SCK_GPIO_CLK_ENABLE();  /* SPI2_SCK脚时钟使能 */
        SPI2_MISO_GPIO_CLK_ENABLE(); /* SPI2_MISO脚时钟使能 */
        SPI2_MOSI_GPIO_CLK_ENABLE(); /* SPI2_MOSI脚时钟使能 */

        /* SCK引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI2_SCK_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(SPI2_SCK_GPIO_PORT, &gpio_init_struct);

        /* MISO引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI2_MISO_GPIO_PIN;
        HAL_GPIO_Init(SPI2_MISO_GPIO_PORT, &gpio_init_struct);

        /* MOSI引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI2_MOSI_GPIO_PIN;
        HAL_GPIO_Init(SPI2_MOSI_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief       SPI2速度设置函数
 *   @note      SPI2时钟选择来自APB1, 即PCLK1, 为36Mhz
 *              SPI速度 = PCLK1 / 2^(speed + 1)
 * @param       speed   : SPI2时钟分频系数
                        取值为SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 * @retval      无
 */
void spi2_set_speed(uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* 判断有效性 */
    __HAL_SPI_DISABLE(&g_spi2_handler);             /* 关闭SPI */
    g_spi2_handler.Instance->CR1 &= 0XFFC7;         /* 位3-5清零，用来设置波特率 */
    g_spi2_handler.Instance->CR1 |= speed << 3;     /* 设置SPI速度 */
    __HAL_SPI_ENABLE(&g_spi2_handler);              /* 使能SPI */
}

/**
 * @brief       SPI2读写一个字节数据
 * @param       txdata  : 要发送的数据(1字节)
 * @retval      接收到的数据(1字节)
 */
uint8_t spi2_read_write_byte(uint8_t txdata)
{
    uint8_t rxdata;
    HAL_SPI_TransmitReceive(&g_spi2_handler, &txdata, &rxdata, 1, 1000);
    return rxdata; /* 返回收到的数据 */
}
