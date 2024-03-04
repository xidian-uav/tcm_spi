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
