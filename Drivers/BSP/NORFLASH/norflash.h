/**
 ****************************************************************************************************
 * @file        norflash.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       NOR FLASH(25QXX) 驱动代码
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

#ifndef __norflash_H
#define __norflash_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* NORFLASH 片选 引脚 定义 */

#define NORFLASH_CS_GPIO_PORT           GPIOB
#define NORFLASH_CS_GPIO_PIN            GPIO_PIN_12
#define NORFLASH_CS_GPIO_CLK_ENABLE()   do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* PB口时钟使能 */

/******************************************************************************************/

/* NORFLASH 片选信号 */
#define NORFLASH_CS(x)      do{ x ? \
                                  HAL_GPIO_WritePin(NORFLASH_CS_GPIO_PORT, NORFLASH_CS_GPIO_PIN, GPIO_PIN_SET) : \
                                  HAL_GPIO_WritePin(NORFLASH_CS_GPIO_PORT, NORFLASH_CS_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)

/* FLASH芯片列表 */
#define W25Q80      0XEF13          /* W25Q80   芯片ID */
#define W25Q16      0XEF14          /* W25Q16   芯片ID */
#define W25Q32      0XEF15          /* W25Q32   芯片ID */
#define W25Q64      0XEF16          /* W25Q64   芯片ID */
#define W25Q128     0XEF17          /* W25Q128  芯片ID */
#define W25Q256     0XEF18          /* W25Q256  芯片ID */
#define BY25Q64     0X6816          /* BY25Q64  芯片ID */
#define BY25Q128    0X6817          /* BY25Q128 芯片ID */
#define NM25Q64     0X5216          /* NM25Q64  芯片ID */
#define NM25Q128    0X5217          /* NM25Q128 芯片ID */

extern uint16_t norflash_TYPE;      /* 定义FLASH芯片型号 */
 
/* 指令表 */
#define FLASH_WriteEnable           0x06 
#define FLASH_WriteDisable          0x04 
#define FLASH_ReadStatusReg1        0x05 
#define FLASH_ReadStatusReg2        0x35 
#define FLASH_ReadStatusReg3        0x15 
#define FLASH_WriteStatusReg1       0x01 
#define FLASH_WriteStatusReg2       0x31 
#define FLASH_WriteStatusReg3       0x11 
#define FLASH_ReadData              0x03 
#define FLASH_FastReadData          0x0B 
#define FLASH_FastReadDual          0x3B 
#define FLASH_FastReadQuad          0xEB  
#define FLASH_PageProgram           0x02 
#define FLASH_PageProgramQuad       0x32 
#define FLASH_BlockErase            0xD8 
#define FLASH_SectorErase           0x20 
#define FLASH_ChipErase             0xC7 
#define FLASH_PowerDown             0xB9 
#define FLASH_ReleasePowerDown      0xAB 
#define FLASH_DeviceID              0xAB 
#define FLASH_ManufactDeviceID      0x90 
#define FLASH_JedecDeviceID         0x9F 
#define FLASH_Enable4ByteAddr       0xB7
#define FLASH_Exit4ByteAddr         0xE9
#define FLASH_SetReadParam          0xC0 
#define FLASH_EnterQPIMode          0x38
#define FLASH_ExitQPIMode           0xFF

/* 静态函数 */
static void norflash_wait_busy(void);               /* 等待空闲 */
static void norflash_send_address(uint32_t address);/* 发送地址 */
static void norflash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen);    /* 写入page */
static void norflash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen); /* 写flash,不带擦除 */

/* 普通函数 */
void norflash_init(void);                   /* 初始化25QXX */
uint16_t norflash_read_id(void);            /* 读取FLASH ID */
void norflash_write_enable(void);           /* 写使能 */
uint8_t norflash_read_sr(uint8_t regno);    /* 读取状态寄存器 */
void norflash_write_sr(uint8_t regno,uint8_t sr);   /* 写状态寄存器 */

void norflash_erase_chip(void);             /* 整片擦除 */
void norflash_erase_sector(uint32_t saddr); /* 扇区擦除 */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen);     /* 读取flash */
void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen);    /* 写入flash */

#endif















