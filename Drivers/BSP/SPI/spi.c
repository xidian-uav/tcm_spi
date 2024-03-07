#include "./SYSTEM/usart/usart.h"
#include "./BSP/SPI/spi.h"

/**
 * @brief       SPI初始化代码
 * @note        主机模式,8位数据,禁止硬件片选
 * @param       hspi  : SPI句柄
 * @retval      无
 */
void spi_init(SPI_HandleTypeDef hspi)
{
    if (hspi.Instance == SPI1)
    {
        SPI1_SPI_CLK_ENABLE();                                            /* SPI1时钟使能 */
    }
    else if(hspi.Instance == SPI2)
    {
        SPI2_SPI_CLK_ENABLE();                                            /* SPI2时钟使能 */
    }
    else if(hspi.Instance == SPI3)
    {
        SPI3_SPI_CLK_ENABLE();                                            /* SPI3时钟使能 */
    }
    else                                                                  /* 初始化错误 */;
    {
        while(1)
        {
            printf("SPI initialization error.The SPI port does not exist!");
            HAL_Delay(1000);
        };  
    }

    HAL_SPI_Init(&hspi);                                                  /* 初始化 */

    __HAL_SPI_ENABLE(&hspi);                                              /* 使能SPI */

    spi_read_write_byte(hspi, 0Xff); /* 启动传输, 实际上就是产生8个时钟脉冲, 达到清空DR的作用, 非必需 */
}

/**
 * @brief       SPI底层驱动，时钟使能，引脚配置
 * @note        此函数会被HAL_SPI_Init()调用
 * @param       hspi  : SPI句柄
 * @retval      无
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    if (hspi->Instance == SPI1)
    {
        SPI1_SCK_GPIO_CLK_ENABLE();  /* SPI1_SCK脚时钟使能 */
        SPI1_MISO_GPIO_CLK_ENABLE(); /* SPI1_MISO脚时钟使能 */
        SPI1_MOSI_GPIO_CLK_ENABLE(); /* SPI1_MOSI脚时钟使能 */

        /* SCK引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI1_SCK_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(SPI1_SCK_GPIO_PORT, &gpio_init_struct);

        /* MISO引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI1_MISO_GPIO_PIN;
        HAL_GPIO_Init(SPI1_MISO_GPIO_PORT, &gpio_init_struct);

        /* MOSI引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI1_MOSI_GPIO_PIN;
        HAL_GPIO_Init(SPI1_MOSI_GPIO_PORT, &gpio_init_struct);
    }
    else if(hspi->Instance == SPI2)
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
    else if(hspi->Instance == SPI3)
    {
        SPI3_SCK_GPIO_CLK_ENABLE();  /* SPI3_SCK脚时钟使能 */
        SPI3_MISO_GPIO_CLK_ENABLE(); /* SPI3_MISO脚时钟使能 */
        SPI3_MOSI_GPIO_CLK_ENABLE(); /* SPI3_MOSI脚时钟使能 */

        /* SCK引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI3_SCK_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(SPI3_SCK_GPIO_PORT, &gpio_init_struct);

        /* MISO引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI3_MISO_GPIO_PIN;
        HAL_GPIO_Init(SPI3_MISO_GPIO_PORT, &gpio_init_struct);

        /* MOSI引脚模式设置(复用输出) */
        gpio_init_struct.Pin = SPI3_MOSI_GPIO_PIN;
        HAL_GPIO_Init(SPI3_MOSI_GPIO_PORT, &gpio_init_struct);
    }
    else{
        while(1)
        {
            printf("SPI initialization error.The SPI port does not exist!");
            HAL_Delay(1000);
        };  
    }
}

/**
 * @brief       SPI速度设置函数
 * @note        SPI1时钟选择来自APB2, 即PCLK1, 为72Mhz
                SPI2时钟选择来自APB1, 即PCLK1, 为36Mhz
                SPI3时钟选择来自APB1, 即PCLK1, 为36Mhz
 *              SPI速度 = PCLK / 2^(speed + 1)
 * @param       speed   : SPI时钟分频系数
                          取值为SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
 * @retval      无
 */

void spi_set_speed(SPI_HandleTypeDef hspi, uint8_t speed)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(speed)); /* 判断有效性 */
    __HAL_SPI_DISABLE(&hspi);             /* 关闭SPI */
    hspi.Instance->CR1 &= 0XFFC7;         /* 位3-5清零，用来设置波特率 */
    hspi.Instance->CR1 |= speed << 3;     /* 设置SPI速度 */
    __HAL_SPI_ENABLE(&hspi);              /* 使能SPI */
}

/**
 * @brief       SPI读写一个字节数据
 * @param       hspi  : SPI句柄
 * @param       tx_data  : 要发送的数据(1字节)
 * @retval      接收到的数据(1字节)
 */
uint8_t spi_read_write_byte(SPI_HandleTypeDef hspi, uint8_t tx_data)
{
    uint8_t rx_data;
    HAL_SPI_TransmitReceive(&hspi, &tx_data, &rx_data, 1, 1000);
    return rx_data; /* 返回收到的数据 */
}

/**
 * @brief       SPI读写多个字节数据
 * @param       hspi  : SPI句柄
 * @param       tx_data  : 要发送的数据(任意字节)
 * @param       tx_data_size  : 要发送的数据大小(以字节为单位)
 * @param       rx_data_buff  : 接收数据区域的指针
 */
void spi_read_write_data(SPI_HandleTypeDef hspi, uint8_t* tx_data, uint16_t tx_data_size, uint8_t* rx_data_buff)
{
    HAL_SPI_TransmitReceive(&hspi, tx_data, rx_data_buff, tx_data_size, 1000);
}