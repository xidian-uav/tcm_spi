/**
 ****************************************************************************************************
 * @file        usmart_port.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V3.5
 * @date        2020-12-20
 * @brief       USMART 移植文件
 *
 *              通过修改该文件,可以方便的将USMART移植到其他工程
 *              当:USMART_ENTIMX_SCAN == 0时,仅需要实现: usmart_get_input_string函数.
 *              当:USMART_ENTIMX_SCAN == 1时,需要多实现4个函数:
 *              usmart_timx_reset_time
 *              usmart_timx_get_time
 *              usmart_timx_init
 *              USMART_TIMX_IRQHandler
 *
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 *
 * V3.4之前版本详细修改说明见USMART文件夹下的:readme.txt
 *
 * V3.4 20200324
 * 1, 新增usmart_port.c和usmart_port.h,用于管理USMART的移植,方便修改
 * 2, 修改变量命名方式为: uint8_t, uint16_t, uint32_t
 * 3, 修改usmart_reset_runtime为usmart_timx_reset_time
 * 4, 修改usmart_get_runtime为usmart_timx_get_time
 * 5, 修改usmart_scan函数实现方式,改成由usmart_get_input_string获取数据流
 * 6, 修改printf函数为USMART_PRINTF宏定义
 * 7, 修改定时扫描相关函数,改用宏定义方式,方便移植
 *
 * V3.5 20201220
 * 1，修改部分代码以支持AC6编译器
 *
 ****************************************************************************************************
 */

#include "./USMART/usmart.h"
#include "./USMART/usmart_port.h"

TIM_HandleTypeDef g_timx_usmart_handle;      /* 定时器句柄 */

/**
 * @brief       获取输入数据流(字符串)
 *   @note      USMART通过解析该函数返回的字符串以获取函数名及参数等信息
 * @param       无
 * @retval
 *   @arg       0,  没有接收到数据
 *   @arg       其他,数据流首地址(不能是0)
 */
char *usmart_get_input_string(void)
{
    uint8_t len;
    char *pbuf = 0;

    if (g_usart_rx_sta & 0x8000)        /* 串口接收完成？ */
    {
        len = g_usart_rx_sta & 0x3fff;  /* 得到此次接收到的数据长度 */
        g_usart_rx_buf[len] = '\0';     /* 在末尾加入结束符. */
        pbuf = (char*)g_usart_rx_buf;
        g_usart_rx_sta = 0;             /* 开启下一次接收 */
    }

    return pbuf;
}

/* 如果使能了定时器扫描, 则需要定义如下函数 */
#if USMART_ENTIMX_SCAN == 1

/**
 * 移植注意:本例是以stm32为例,如果要移植到其他mcu,请做相应修改.
 * usmart_reset_runtime,清除函数运行时间,连同定时器的计数寄存器以及标志位一起清零.并设置重装载值为最大,以最大限度的延长计时时间.
 * usmart_get_runtime,获取函数运行时间,通过读取CNT值获取,由于usmart是通过中断调用的函数,所以定时器中断不再有效,此时最大限度
 * 只能统计2次CNT的值,也就是清零后+溢出一次,当溢出超过2次,没法处理,所以最大延时,控制在:2*计数器CNT*0.1ms.对STM32来说,是:13.1s左右
 * 其他的:USMART_TIMX_IRQHandler和Timer4_Init,需要根据MCU特点自行修改.确保计数器计数频率为:10Khz即可.另外,定时器不要开启自动重装载功能!!
 */

/**
 * @brief       复位runtime
 *   @note      需要根据所移植到的MCU的定时器参数进行修改
 * @param       无
 * @retval      无
 */
void usmart_timx_reset_time(void)
{
    __HAL_TIM_CLEAR_FLAG(&g_timx_usmart_handle, TIM_FLAG_UPDATE); /* 清除中断标志位 */
    __HAL_TIM_SET_AUTORELOAD(&g_timx_usmart_handle, 0XFFFF);      /* 将重装载值设置到最大 */
    __HAL_TIM_SET_COUNTER(&g_timx_usmart_handle, 0);              /* 清空定时器的CNT */
    usmart_dev.runtime = 0;
}

/**
 * @brief       获得runtime时间
 *   @note      需要根据所移植到的MCU的定时器参数进行修改
 * @param       无
 * @retval      执行时间,单位:0.1ms,最大延时时间为定时器CNT值的2倍*0.1ms
 */
uint32_t usmart_timx_get_time(void)
{
    if (__HAL_TIM_GET_FLAG(&g_timx_usmart_handle, TIM_FLAG_UPDATE) == SET)  /* 在运行期间,产生了定时器溢出 */
    {
        usmart_dev.runtime += 0XFFFF;
    }
    usmart_dev.runtime += __HAL_TIM_GET_COUNTER(&g_timx_usmart_handle);
    return usmart_dev.runtime;                                 /* 返回计数值 */
}

/**
 * @brief       定时器初始化函数
 * @param       arr:自动重装载值
 *              psc:定时器分频系数
 * @retval      无
 */ 
void usmart_timx_init(uint16_t arr, uint16_t psc)
{ 
    USMART_TIMX_CLK_ENABLE();
    
    g_timx_usmart_handle.Instance = USMART_TIMX;                 /* 通用定时器4 */
    g_timx_usmart_handle.Init.Prescaler = psc;                   /* 分频系数 */
    g_timx_usmart_handle.Init.CounterMode = TIM_COUNTERMODE_UP;  /* 向上计数器 */
    g_timx_usmart_handle.Init.Period = arr;                      /* 自动装载值 */
    g_timx_usmart_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&g_timx_usmart_handle);
    HAL_TIM_Base_Start_IT(&g_timx_usmart_handle);                /* 使能定时器和定时器中断 */
    HAL_NVIC_SetPriority(USMART_TIMX_IRQn,3,3);                  /* 设置中断优先级，抢占优先级3，子优先级3 */
    HAL_NVIC_EnableIRQ(USMART_TIMX_IRQn);                        /* 开启ITM中断 */ 
}

/**
 * @brief       USMART定时器中断服务函数
 * @param       无
 * @retval      无
 */
void USMART_TIMX_IRQHandler(void)
{
    if(__HAL_TIM_GET_IT_SOURCE(&g_timx_usmart_handle,TIM_IT_UPDATE)==SET)/* 溢出中断 */
    {
        usmart_dev.scan();                                   /* 执行usmart扫描 */
        __HAL_TIM_SET_COUNTER(&g_timx_usmart_handle, 0);;    /* 清空定时器的CNT */
        __HAL_TIM_SET_AUTORELOAD(&g_timx_usmart_handle, 100);/* 恢复原来的设置 */
    }
    
    __HAL_TIM_CLEAR_IT(&g_timx_usmart_handle, TIM_IT_UPDATE);/* 清除中断标志位 */
}

#endif
















