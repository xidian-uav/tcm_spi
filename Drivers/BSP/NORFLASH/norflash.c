/**
 ****************************************************************************************************
 * @file        norflash.c
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

#include "./BSP/SPI/spi.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/NORFLASH/norflash.h"


uint16_t g_norflash_type = NM25Q128;     /* 默认是NM25Q128 */

/**
 * @brief       初始化SPI NOR FLASH
 * @param       无
 * @retval      无
 */
void norflash_init(void)
{
    uint8_t temp;

    NORFLASH_CS_GPIO_CLK_ENABLE();      /* NORFLASH CS脚 时钟使能 */

    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = NORFLASH_CS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(NORFLASH_CS_GPIO_PORT, &gpio_init_struct); /* CS引脚模式设置(复用输出) */

    NORFLASH_CS(1);                     /* 取消片选 */

    spi2_init();                        /* 初始化SPI2 */
    spi2_set_speed(SPI_SPEED_2);        /* SPI2 切换到高速状态 18Mhz */
    
    g_norflash_type = norflash_read_id();   /* 读取FLASH ID. */
    
    if (g_norflash_type == W25Q256)     /* SPI FLASH为W25Q256, 必须使能4字节地址模式 */
    {
        temp = norflash_read_sr(3);     /* 读取状态寄存器3，判断地址模式 */

        if ((temp & 0X01) == 0)         /* 如果不是4字节地址模式,则进入4字节地址模式 */
        {
            norflash_write_enable();    /* 写使能 */
            temp |= 1 << 1;             /* ADP=1, 上电4位地址模式 */
            norflash_write_sr(3, temp); /* 写SR3 */
            
            NORFLASH_CS(0);
            spi2_read_write_byte(FLASH_Enable4ByteAddr);    /* 使能4字节地址指令 */
            NORFLASH_CS(1);
        }
    }

    //printf("ID:%x\r\n", g_norflash_type);
}

/**
 * @brief       等待空闲
 * @param       无
 * @retval      无
 */
static void norflash_wait_busy(void)
{
    while ((norflash_read_sr(1) & 0x01) == 0x01);   /* 等待BUSY位清空 */
}

/**
 * @brief       25QXX写使能
 *   @note      将S1寄存器的WEL置位
 * @param       无
 * @retval      无
 */
void norflash_write_enable(void)
{
    NORFLASH_CS(0);
    spi2_read_write_byte(FLASH_WriteEnable);   /* 发送写使能 */
    NORFLASH_CS(1);
}

/**
 * @brief       25QXX发送地址
 *   @note      根据芯片型号的不同, 发送24ibt / 32bit地址
 * @param       address : 要发送的地址
 * @retval      无
 */
static void norflash_send_address(uint32_t address)
{
    if (g_norflash_type == W25Q256) /*  只有W25Q256支持4字节地址模式 */
    {
        spi2_read_write_byte((uint8_t)((address)>>24)); /* 发送 bit31 ~ bit24 地址 */
    } 
    spi2_read_write_byte((uint8_t)((address)>>16));     /* 发送 bit23 ~ bit16 地址 */
    spi2_read_write_byte((uint8_t)((address)>>8));      /* 发送 bit15 ~ bit8  地址 */
    spi2_read_write_byte((uint8_t)address);             /* 发送 bit7  ~ bit0  地址 */
}

/**
 * @brief       读取25QXX的状态寄存器，25QXX一共有3个状态寄存器
 *   @note      状态寄存器1：
 *              BIT7  6   5   4   3   2   1   0
 *              SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *              SPR:默认0,状态寄存器保护位,配合WP使用
 *              TB,BP2,BP1,BP0:FLASH区域写保护设置
 *              WEL:写使能锁定
 *              BUSY:忙标记位(1,忙;0,空闲)
 *              默认:0x00
 *
 *              状态寄存器2：
 *              BIT7  6   5   4   3   2   1   0
 *              SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *              状态寄存器3：
 *              BIT7      6    5    4   3   2   1   0
 *              HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param       regno: 状态寄存器号，范围:1~3
 * @retval      状态寄存器值
 */
uint8_t norflash_read_sr(uint8_t regno)
{
    uint8_t byte = 0, command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_ReadStatusReg1;  /* 读状态寄存器1指令 */
            break;

        case 2:
            command = FLASH_ReadStatusReg2;  /* 读状态寄存器2指令 */
            break;

        case 3:
            command = FLASH_ReadStatusReg3;  /* 读状态寄存器3指令 */
            break;

        default:
            command = FLASH_ReadStatusReg1;
            break;
    }

    NORFLASH_CS(0);
    spi2_read_write_byte(command);      /* 发送读寄存器命令 */
    byte = spi2_read_write_byte(0Xff);  /* 读取一个字节 */
    NORFLASH_CS(1);
    
    return byte;
}

/**
 * @brief       写25QXX状态寄存器
 *   @note      寄存器说明见norflash_read_sr函数说明
 * @param       regno: 状态寄存器号，范围:1~3
 * @param       sr   : 要写入状态寄存器的值
 * @retval      无
 */
void norflash_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_WriteStatusReg1;  /* 写状态寄存器1指令 */
            break;

        case 2:
            command = FLASH_WriteStatusReg2;  /* 写状态寄存器2指令 */
            break;

        case 3:
            command = FLASH_WriteStatusReg3;  /* 写状态寄存器3指令 */
            break;

        default:
            command = FLASH_WriteStatusReg1;
            break;
    }

    NORFLASH_CS(0);
    spi2_read_write_byte(command);  /* 发送读寄存器命令 */
    spi2_read_write_byte(sr);       /* 写入一个字节 */
    NORFLASH_CS(1);
}

/**
 * @brief       读取芯片ID
 * @param       无
 * @retval      FLASH芯片ID
 *   @note      芯片ID列表见: norflash.h, 芯片列表部分
 */
uint16_t norflash_read_id(void)
{
    uint16_t deviceid;

    NORFLASH_CS(0);
    spi2_read_write_byte(FLASH_ManufactDeviceID);   /* 发送读 ID 命令 */
    spi2_read_write_byte(0);    /* 写入一个字节 */
    spi2_read_write_byte(0);
    spi2_read_write_byte(0);
    deviceid = spi2_read_write_byte(0xFF) << 8;     /* 读取高8位字节 */
    deviceid |= spi2_read_write_byte(0xFF);         /* 读取低8位字节 */
    NORFLASH_CS(1);

    return deviceid;
}

/**
 * @brief       读取SPI FLASH
 *   @note      在指定地址开始读取指定长度的数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始读取的地址(最大32bit)
 * @param       datalen : 要读取的字节数(最大65535)
 * @retval      无
 */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;

    NORFLASH_CS(0);
    spi2_read_write_byte(FLASH_ReadData);       /* 发送读取命令 */
    norflash_send_address(addr);                /* 发送地址 */
    
    for(i=0;i<datalen;i++)
    {
        pbuf[i] = spi2_read_write_byte(0XFF);   /* 循环读取 */
    }
    
    NORFLASH_CS(1);
}

/**
 * @brief       SPI在一页(0~65535)内写入少于256个字节的数据
 *   @note      在指定地址开始写入最大256字节的数据
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
 * @retval      无
 */
static void norflash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;

    norflash_write_enable();    /* 写使能 */

    NORFLASH_CS(0);
    spi2_read_write_byte(FLASH_PageProgram);    /* 发送写页命令 */
    norflash_send_address(addr);                /* 发送地址 */

    for(i=0;i<datalen;i++)
    {
        spi2_read_write_byte(pbuf[i]);          /* 循环写入 */
    }
    
    NORFLASH_CS(1);
    norflash_wait_busy();       /* 等待写入结束 */
}

/**
 * @brief       无检验写SPI FLASH
 *   @note      必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
 *              具有自动换页功能
 *              在指定地址开始写入指定长度的数据,但是要确保地址不越界!
 *
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大65535)
 * @retval      无
 */
static void norflash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t pageremain;
    pageremain = 256 - addr % 256;  /* 单页剩余的字节数 */

    if (datalen <= pageremain)      /* 不大于256个字节 */
    {
        pageremain = datalen;
    }

    while (1)
    {
        /* 当写入字节比页内剩余地址还少的时候, 一次性写完
         * 当写入直接比页内剩余地址还多的时候, 先写完整个页内剩余地址, 然后根据剩余长度进行不同处理
         */
        norflash_write_page(pbuf, addr, pageremain);

        if (datalen == pageremain)   /* 写入结束了 */
        {
            break;
        }
        else     /* datalen > pageremain */
        {
            pbuf += pageremain;         /* pbuf指针地址偏移,前面已经写了pageremain字节 */
            addr += pageremain;         /* 写地址偏移,前面已经写了pageremain字节 */
            datalen -= pageremain;      /* 写入总长度减去已经写入了的字节数 */

            if (datalen > 256)          /* 剩余数据还大于一页,可以一次写一页 */
            {
                pageremain = 256;       /* 一次可以写入256个字节 */
            }
            else     /* 剩余数据小于一页,可以一次写完 */
            {
                pageremain = datalen;   /* 不够256个字节了 */
            }
        }
    }
}

/**
 * @brief       写SPI FLASH
 *   @note      在指定地址开始写入指定长度的数据 , 该函数带擦除操作!
 *              SPI FLASH 一般是: 256个字节为一个Page, 4Kbytes为一个Sector, 16个扇区为1个Block
 *              擦除的最小单位为Sector.
 *
 * @param       pbuf    : 数据存储区
 * @param       addr    : 开始写入的地址(最大32bit)
 * @param       datalen : 要写入的字节数(最大65535)
 * @retval      无
 */
uint8_t g_norflash_buf[4096];   /* 扇区缓存 */

void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *norflash_buf;

    norflash_buf = g_norflash_buf;
    secpos = addr / 4096;       /* 扇区地址 */
    secoff = addr % 4096;       /* 在扇区内的偏移 */
    secremain = 4096 - secoff;  /* 扇区剩余空间大小 */

    //printf("ad:%X,nb:%X\r\n", addr, datalen); /* 测试用 */
    if (datalen <= secremain)
    {
        secremain = datalen;    /* 不大于4096个字节 */
    }

    while (1)
    {
        norflash_read(norflash_buf, secpos * 4096, 4096);   /* 读出整个扇区的内容 */

        for (i = 0; i < secremain; i++)   /* 校验数据 */
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break;      /* 需要擦除, 直接退出for循环 */
            }
        }

        if (i < secremain)   /* 需要擦除 */
        {
            norflash_erase_sector(secpos);  /* 擦除这个扇区 */

            for (i = 0; i < secremain; i++)   /* 复制 */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            norflash_write_nocheck(norflash_buf, secpos * 4096, 4096);  /* 写入整个扇区 */
        }
        else        /* 写已经擦除了的,直接写入扇区剩余区间. */
        {
            norflash_write_nocheck(pbuf, addr, secremain);  /* 直接写扇区 */
        }

        if (datalen == secremain)
        {
            break;  /* 写入结束了 */
        }
        else        /* 写入未结束 */
        {
            secpos++;               /* 扇区地址增1 */
            secoff = 0;             /* 偏移位置为0 */

            pbuf += secremain;      /* 指针偏移 */
            addr += secremain;      /* 写地址偏移 */
            datalen -= secremain;   /* 字节数递减 */

            if (datalen > 4096)
            {
                secremain = 4096;   /* 下一个扇区还是写不完 */
            }
            else
            {
                secremain = datalen;/* 下一个扇区可以写完了 */
            }
        }
    }
}

/**
 * @brief       擦除整个芯片
 *   @note      等待时间超长...
 * @param       无
 * @retval      无
 */
void norflash_erase_chip(void)
{
    norflash_write_enable();    /* 写使能 */
    norflash_wait_busy();       /* 等待空闲 */
    NORFLASH_CS(0);
    spi2_read_write_byte(FLASH_ChipErase);  /* 发送读寄存器命令 */ 
    NORFLASH_CS(1);
    norflash_wait_busy();       /* 等待芯片擦除结束 */
}

/**
 * @brief       擦除一个扇区
 *   @note      注意,这里是扇区地址,不是字节地址!!
 *              擦除一个扇区的最少时间:150ms
 *
 * @param       saddr : 扇区地址 根据实际容量设置
 * @retval      无
 */
void norflash_erase_sector(uint32_t saddr)
{
    //printf("fe:%x\r\n", saddr);   /* 监视falsh擦除情况,测试用 */
    saddr *= 4096;
    norflash_write_enable();        /* 写使能 */
    norflash_wait_busy();           /* 等待空闲 */

    NORFLASH_CS(0);
    spi2_read_write_byte(FLASH_SectorErase);    /* 发送写页命令 */
    norflash_send_address(saddr);   /* 发送地址 */
    NORFLASH_CS(1);
    norflash_wait_busy();           /* 等待扇区擦除完成 */
}

























