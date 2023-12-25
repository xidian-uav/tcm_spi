/**
 ****************************************************************************************************
 * @file        spi.h
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

#ifndef __SPI_H
#define __SPI_H

#include "./SYSTEM/sys/sys.h"


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
#define SPI2_SPI                        SPI2
#define SPI2_SPI_CLK_ENABLE()           do{ __HAL_RCC_SPI2_CLK_ENABLE(); }while(0)    /* SPI2时钟使能 */

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


void spi2_init(void);
void spi2_set_speed(uint8_t speed);
uint8_t spi2_read_write_byte(uint8_t txdata);

#endif
























