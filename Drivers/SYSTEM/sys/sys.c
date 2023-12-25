/**
 ****************************************************************************************************
 * @file        sys.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       系统初始化代码(包括时钟配置/中断管理/GPIO设置等)
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
 * V1.0 20211103
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"


/**
 * @brief       设置中断向量表偏移地址
 * @param       baseaddr: 基址
 * @param       offset: 偏移量(必须是0, 或者0X100的倍数)
 * @retval      无
 */
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset)
{
    /* 设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留 */
    SCB->VTOR = baseaddr | (offset & (uint32_t)0xFFFFFE00);
}

/**
 * @brief       执行: WFI指令(执行完该指令进入低功耗状态, 等待中断唤醒)
 * @param       无
 * @retval      无
 */
void sys_wfi_set(void)
{
    __ASM volatile("wfi");
}

/**
 * @brief       关闭所有中断(但是不包括fault和NMI中断)
 * @param       无
 * @retval      无
 */
void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

/**
 * @brief       开启所有中断
 * @param       无
 * @retval      无
 */
void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

/**
 * @brief       设置栈顶地址
 * @note        左侧的红X, 属于MDK误报, 实际是没问题的
 * @param       addr: 栈顶地址
 * @retval      无
 */
void sys_msr_msp(uint32_t addr)
{
    __set_MSP(addr);    /* 设置栈顶地址 */
}

/**
 * @brief       进入待机模式
 * @param       无
 * @retval      无
 */
void sys_standby(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();    /* 使能电源时钟 */
    SET_BIT(PWR->CR, PWR_CR_PDDS); /* 进入待机模式 */
}

/**
 * @brief       系统软复位
 * @param       无
 * @retval      无
 */
void sys_soft_reset(void)
{
    NVIC_SystemReset();
}

/**
 * @brief       系统时钟初始化函数
 * @param       plln: PLL倍频系数(PLL倍频), 取值范围: 2~16
                中断向量表位置在启动时已经在SystemInit()中初始化
 * @retval      无
 */
void sys_stm32_clock_init(uint32_t plln)
{
    HAL_StatusTypeDef ret = HAL_ERROR;
    RCC_OscInitTypeDef rcc_osc_init = {0};
    RCC_ClkInitTypeDef rcc_clk_init = {0};

    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;       /* 选择要配置HSE */
    rcc_osc_init.HSEState = RCC_HSE_ON;                         /* 打开HSE */
    rcc_osc_init.HSEPredivValue = RCC_HSE_PREDIV_DIV1;          /* HSE预分频系数 */
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;                     /* 打开PLL */
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;             /* PLL时钟源选择HSE */
    rcc_osc_init.PLL.PLLMUL = plln;                             /* PLL倍频系数 */
    ret = HAL_RCC_OscConfig(&rcc_osc_init);                     /* 初始化 */

    if (ret != HAL_OK)
    {
        while (1);                                              /* 时钟初始化失败，之后的程序将可能无法正常执行，可以在这里加入自己的处理 */
    }

    /* 选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2*/
    rcc_clk_init.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;        /* 设置系统时钟来自PLL */
    rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;               /* AHB分频系数为1 */
    rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV2;                /* APB1分频系数为2 */
    rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV1;                /* APB2分频系数为1 */
    ret = HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_2);  /* 同时设置FLASH延时周期为2WS，也就是3个CPU周期。 */

    if (ret != HAL_OK)
    {
        while (1);                                              /* 时钟初始化失败，之后的程序将可能无法正常执行，可以在这里加入自己的处理 */
    }
}


