#include "./stm32f1xx_it.h"
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/TCM/tcm.h"

int main(void)
{
    uint8_t len;
    uint16_t times = 0;

    HAL_Init();                             /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9);     /* 设置时钟为72Mhz */
    delay_init(72);                         /* 延时初始化 */
    usart_init(115200);                     /* 串口初始化为115200 */
    led_init();                             /* 初始化LED */
    tcm_init();                             /* 初始化TCM */
    
    printf("Entering test routine 1.\r\n");
    
    target_reset();
    printf("Waiting...\r\n");
    HAL_Delay(1000);
    
    uint8_t readbuf[TCM_RX_BUFF_SIZE];
    uint8_t tpm2_startup[] = "\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x44\x00\x00";
    
    // TPM2_STARTUP
    sendCommand(tpm2_startup, 12, readbuf);
    printf("OK: ");
    
    uint16_t i;
    for(i = 0; i < 10; i++ )
    {
      printf("%02x ", readbuf[i]);
    }
    printf("\r\n");
    
    printf("tpm startup successfully!\r\n");
    
    // TPM2_GETRANDOM
    uint16_t respsize = 0;
    uint8_t tpm2_getrandom[] = "\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x7b\x00\x10";
    respsize = sendCommand(tpm2_getrandom, 12, readbuf);
    
    printf("OK: ");
    for(i = 0; i < respsize; i++ )
    {
      printf("%02x ", readbuf[i]);
    }
    printf("\r\n");
              
    // while (1)
    // {
    //     if (g_usart_rx_sta & 0x8000)        /* 接收到了数据? */
    //     {
    //         len = g_usart_rx_sta & 0x3fff;  /* 得到此次接收到的数据长度 */
    //         printf("\r\n您发送的消息为:\r\n");

    //         HAL_UART_Transmit(&g_uart1_handle,(uint8_t*)g_usart_rx_buf, len, 1000);    /* 发送接收到的数据 */
    //         while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != SET);           /* 等待发送结束 */
            
    //         printf("\r\n\r\n");             /* 插入换行 */
    //         g_usart_rx_sta = 0;
    //     }
    //     else
    //     {
    //         times++;

    //         if (times % 5000 == 0)
    //         {
    //             printf("\r\n正点原子 STM32开发板 串口实验\r\n");
    //             printf("正点原子@ALIENTEK\r\n\r\n\r\n");
    //         }

    //         if (times % 200 == 0) 
    //         {
    //             printf("请输入数据,以回车键结束\r\n");
    //             HAL_UART_Transmit(&g_uart1_handle, data, sizeof(data) - 1, 1000);    /* 发送接收到的数据 */
    //             while(__HAL_UART_GET_FLAG(&g_uart1_handle,UART_FLAG_TC) != SET);     /* 等待发送结束 */
    //         }
    //         if (times % 30  == 0) LED0_TOGGLE(); /* 闪烁LED,提示系统正在运行. */

    //         delay_ms(10);
    //     }
    // }
}