/**
 ****************************************************************************************************
 * @file        led.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       LED 驱动代码
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
 * V1.0 20200417
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef _LED_H
#define _LED_H
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */

#define LED0_GPIO_PORT                  GPIOB
#define LED0_GPIO_PIN                   GPIO_PIN_5
#define LED0_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)             /* PB口时钟使能 */

#define LED1_GPIO_PORT                  GPIOE
#define LED1_GPIO_PIN                   GPIO_PIN_5
#define LED1_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOE_CLK_ENABLE(); }while(0)             /* PE口时钟使能 */

/******************************************************************************************/
/* LED端口定义 */
#define LED0(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED0_GPIO_PORT, LED0_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)      /* LED0翻转 */

#define LED1(x)   do{ x ? \
                      HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_SET) : \
                      HAL_GPIO_WritePin(LED1_GPIO_PORT, LED1_GPIO_PIN, GPIO_PIN_RESET); \
                  }while(0)      /* LED1翻转 */

/* LED取反定义 */
#define LED0_TOGGLE()   do{ HAL_GPIO_TogglePin(LED0_GPIO_PORT, LED0_GPIO_PIN); }while(0)        /* 翻转LED0 */
#define LED1_TOGGLE()   do{ HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_GPIO_PIN); }while(0)        /* 翻转LED1 */

/******************************************************************************************/
/* 外部接口函数*/
void led_init(void);                                                                            /* 初始化 */

#endif
