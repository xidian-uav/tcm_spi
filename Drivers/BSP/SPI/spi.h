#ifndef __SPI_H
#define __SPI_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* SPI1 引脚 定义 */

#define SPI1_SCK_GPIO_PORT              GPIOA
#define SPI1_SCK_GPIO_PIN               GPIO_PIN_5
#define SPI1_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define SPI1_MISO_GPIO_PORT             GPIOA
#define SPI1_MISO_GPIO_PIN              GPIO_PIN_6
#define SPI1_MISO_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define SPI1_MOSI_GPIO_PORT             GPIOA
#define SPI1_MOSI_GPIO_PIN              GPIO_PIN_7
#define SPI1_MOSI_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

/* SPI2相关定义 */
#define SPI1_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI1_CLK_ENABLE(); }while(0)    /* SPI1时钟使能 */

/******************************************************************************************/


/******************************************************************************************/
/* SPI2 引脚 定义 */

#define SPI2_SCK_GPIO_PORT              GPIOB
#define SPI2_SCK_GPIO_PIN               GPIO_PIN_13
#define SPI2_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define SPI2_MISO_GPIO_PORT             GPIOB
#define SPI2_MISO_GPIO_PIN              GPIO_PIN_14
#define SPI2_MISO_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define SPI2_MOSI_GPIO_PORT             GPIOB
#define SPI2_MOSI_GPIO_PIN              GPIO_PIN_15
#define SPI2_MOSI_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

/* SPI2相关定义 */
#define SPI2_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI2_CLK_ENABLE(); }while(0)    /* SPI2时钟使能 */

/******************************************************************************************/


/******************************************************************************************/
/* SPI3 引脚 定义 */

#define SPI3_SCK_GPIO_PORT              GPIOB
#define SPI3_SCK_GPIO_PIN               GPIO_PIN_3
#define SPI3_SCK_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define SPI3_MISO_GPIO_PORT             GPIOB
#define SPI3_MISO_GPIO_PIN              GPIO_PIN_4
#define SPI3_MISO_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

#define SPI3_MOSI_GPIO_PORT             GPIOB
#define SPI3_MOSI_GPIO_PIN              GPIO_PIN_5
#define SPI3_MOSI_GPIO_CLK_ENABLE()     do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

/* SPI3相关定义 */
#define SPI3_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI3_CLK_ENABLE(); }while(0)    /* SPI3时钟使能 */

/******************************************************************************************/


/* SPI总线速度设置 */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7


#define SPI_RX_BUFFER_SIZE 256         /* SPI接收多个字节时的缓冲区大小 */

void spi_init(SPI_HandleTypeDef* hspi);
void spi_set_speed(SPI_HandleTypeDef* hspi, uint8_t speed);
uint8_t spi_read_write_byte(SPI_HandleTypeDef* hspi, uint8_t tx_data);
void spi_read_write_data(SPI_HandleTypeDef* hspi, uint8_t* tx_data, uint16_t tx_data_size, uint8_t* rx_data_buff);
#endif
























