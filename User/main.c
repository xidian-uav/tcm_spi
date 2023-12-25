#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/KEY/key.h"
#include "./BSP/NORFLASH/norflash.h"

/* 要写入到FLASH的字符串数组 */
const uint8_t g_text_buf[] = {"STM32 SPI TEST"};

#define TEXT_SIZE sizeof(g_text_buf) /* TEXT字符串长度 */

int main(void)
{
    uint8_t key;
    uint16_t i = 0;
    uint8_t datatemp[TEXT_SIZE];
    uint32_t flashsize;
    uint16_t id = 0;

    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    delay_init(72);                     /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    usmart_dev.init(72);                /* 初始化USMART */
    led_init();                         /* 初始化LED */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    norflash_init();                    /* 初始化NORFLASH */

    lcd_show_string(30,  50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30,  70, 200, 16, 16, "SPI TEST", RED);
    lcd_show_string(30,  90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY1:Write  KEY0:Read", RED); /* 显示提示信息 */

    id = norflash_read_id(); /* 读取FLASH ID */

    while ((id == 0) || (id == 0XFFFF)) /* 检测不到FLASH芯片 */
    {
        lcd_show_string(30, 130, 200, 16, 16, "FLASH Check Failed!", RED);
        delay_ms(500);
        lcd_show_string(30, 130, 200, 16, 16, "Please Check!      ", RED);
        delay_ms(500);
        LED0_TOGGLE(); /* LED0闪烁 */
    }

    lcd_show_string(30, 130, 200, 16, 16, "SPI FLASH Ready!", BLUE);
    flashsize = 16 * 1024 * 1024; /* FLASH 大小为16M字节 */

    while (1)
    {
        key = key_scan(0);

        if (key == KEY1_PRES) /* KEY1按下,写入 */
        {
            lcd_fill(0, 150, 239, 319, WHITE); /* 清除半屏 */
            lcd_show_string(30, 150, 200, 16, 16, "Start Write FLASH....", BLUE);
            sprintf((char *)datatemp, "%s%d", (char *)g_text_buf, i);
            norflash_write((uint8_t *)datatemp, flashsize - 100, TEXT_SIZE);      /* 从倒数第100个地址处开始,写入SIZE长度的数据 */
            lcd_show_string(30, 150, 200, 16, 16, "FLASH Write Finished!", BLUE); /* 提示传送完成 */
        }

        if (key == KEY0_PRES) /* KEY0按下,读取字符串并显示 */
        {
            lcd_show_string(30, 150, 200, 16, 16, "Start Read FLASH... . ", BLUE);
            norflash_read(datatemp, flashsize - 100, TEXT_SIZE);                   /* 从倒数第100个地址处开始,读出SIZE个字节 */
            lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:   ", BLUE); /* 提示传送完成 */
            lcd_show_string(30, 170, 200, 16, 16, (char *)datatemp, BLUE);         /* 显示读到的字符串 */
        }

        i++;

        if (i == 20)
        {
            LED0_TOGGLE(); /* LED0闪烁 */
            i = 0;
        }

        delay_ms(10);
    }
}
