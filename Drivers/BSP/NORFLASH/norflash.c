/**
 ****************************************************************************************************
 * @file        norflash.c
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-24
 * @brief       NOR FLASH(25QXX) ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200424
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#include "./BSP/SPI/spi.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/NORFLASH/norflash.h"


uint16_t g_norflash_type = NM25Q128;     /* Ĭ����NM25Q128 */

/**
 * @brief       ��ʼ��SPI NOR FLASH
 * @param       ��
 * @retval      ��
 */
void norflash_init(void)
{
    uint8_t temp;

    NORFLASH_CS_GPIO_CLK_ENABLE();      /* NORFLASH CS�� ʱ��ʹ�� */

    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Pin = NORFLASH_CS_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_PULLUP;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(NORFLASH_CS_GPIO_PORT, &gpio_init_struct); /* CS����ģʽ����(�������) */

    NORFLASH_CS(1);                     /* ȡ��Ƭѡ */

    spi_init();                        /* ��ʼ��SPI2 */
    spi_set_speed(SPI_SPEED_2);        /* SPI2 �л�������״̬ 18Mhz */
    
    g_norflash_type = norflash_read_id();   /* ��ȡFLASH ID. */
    
    if (g_norflash_type == W25Q256)     /* SPI FLASHΪW25Q256, ����ʹ��4�ֽڵ�ַģʽ */
    {
        temp = norflash_read_sr(3);     /* ��ȡ״̬�Ĵ���3���жϵ�ַģʽ */

        if ((temp & 0X01) == 0)         /* �������4�ֽڵ�ַģʽ,�����4�ֽڵ�ַģʽ */
        {
            norflash_write_enable();    /* дʹ�� */
            temp |= 1 << 1;             /* ADP=1, �ϵ�4λ��ַģʽ */
            norflash_write_sr(3, temp); /* дSR3 */
            
            NORFLASH_CS(0);
            spi_read_write_byte(FLASH_Enable4ByteAddr);    /* ʹ��4�ֽڵ�ַָ�� */
            NORFLASH_CS(1);
        }
    }

    //printf("ID:%x\r\n", g_norflash_type);
}

/**
 * @brief       �ȴ�����
 * @param       ��
 * @retval      ��
 */
static void norflash_wait_busy(void)
{
    while ((norflash_read_sr(1) & 0x01) == 0x01);   /* �ȴ�BUSYλ��� */
}

/**
 * @brief       25QXXдʹ��
 *   @note      ��S1�Ĵ�����WEL��λ
 * @param       ��
 * @retval      ��
 */
void norflash_write_enable(void)
{
    NORFLASH_CS(0);
    spi_read_write_byte(FLASH_WriteEnable);   /* ����дʹ�� */
    NORFLASH_CS(1);
}

/**
 * @brief       25QXX���͵�ַ
 *   @note      ����оƬ�ͺŵĲ�ͬ, ����24ibt / 32bit��ַ
 * @param       address : Ҫ���͵ĵ�ַ
 * @retval      ��
 */
static void norflash_send_address(uint32_t address)
{
    if (g_norflash_type == W25Q256) /*  ֻ��W25Q256֧��4�ֽڵ�ַģʽ */
    {
        spi_read_write_byte((uint8_t)((address)>>24)); /* ���� bit31 ~ bit24 ��ַ */
    } 
    spi_read_write_byte((uint8_t)((address)>>16));     /* ���� bit23 ~ bit16 ��ַ */
    spi_read_write_byte((uint8_t)((address)>>8));      /* ���� bit15 ~ bit8  ��ַ */
    spi_read_write_byte((uint8_t)address);             /* ���� bit7  ~ bit0  ��ַ */
}

/**
 * @brief       ��ȡ25QXX��״̬�Ĵ�����25QXXһ����3��״̬�Ĵ���
 *   @note      ״̬�Ĵ���1��
 *              BIT7  6   5   4   3   2   1   0
 *              SPR   RV  TB BP2 BP1 BP0 WEL BUSY
 *              SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
 *              TB,BP2,BP1,BP0:FLASH����д��������
 *              WEL:дʹ������
 *              BUSY:æ���λ(1,æ;0,����)
 *              Ĭ��:0x00
 *
 *              ״̬�Ĵ���2��
 *              BIT7  6   5   4   3   2   1   0
 *              SUS   CMP LB3 LB2 LB1 (R) QE  SRP1
 *
 *              ״̬�Ĵ���3��
 *              BIT7      6    5    4   3   2   1   0
 *              HOLD/RST  DRV1 DRV0 (R) (R) WPS ADP ADS
 *
 * @param       regno: ״̬�Ĵ����ţ���Χ:1~3
 * @retval      ״̬�Ĵ���ֵ
 */
uint8_t norflash_read_sr(uint8_t regno)
{
    uint8_t byte = 0, command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_ReadStatusReg1;  /* ��״̬�Ĵ���1ָ�� */
            break;

        case 2:
            command = FLASH_ReadStatusReg2;  /* ��״̬�Ĵ���2ָ�� */
            break;

        case 3:
            command = FLASH_ReadStatusReg3;  /* ��״̬�Ĵ���3ָ�� */
            break;

        default:
            command = FLASH_ReadStatusReg1;
            break;
    }

    NORFLASH_CS(0);
    spi_read_write_byte(command);      /* ���Ͷ��Ĵ������� */
    byte = spi_read_write_byte(0Xff);  /* ��ȡһ���ֽ� */
    NORFLASH_CS(1);
    
    return byte;
}

/**
 * @brief       д25QXX״̬�Ĵ���
 *   @note      �Ĵ���˵����norflash_read_sr����˵��
 * @param       regno: ״̬�Ĵ����ţ���Χ:1~3
 * @param       sr   : Ҫд��״̬�Ĵ�����ֵ
 * @retval      ��
 */
void norflash_write_sr(uint8_t regno, uint8_t sr)
{
    uint8_t command = 0;

    switch (regno)
    {
        case 1:
            command = FLASH_WriteStatusReg1;  /* д״̬�Ĵ���1ָ�� */
            break;

        case 2:
            command = FLASH_WriteStatusReg2;  /* д״̬�Ĵ���2ָ�� */
            break;

        case 3:
            command = FLASH_WriteStatusReg3;  /* д״̬�Ĵ���3ָ�� */
            break;

        default:
            command = FLASH_WriteStatusReg1;
            break;
    }

    NORFLASH_CS(0);
    spi_read_write_byte(command);  /* ���Ͷ��Ĵ������� */
    spi_read_write_byte(sr);       /* д��һ���ֽ� */
    NORFLASH_CS(1);
}

/**
 * @brief       ��ȡоƬID
 * @param       ��
 * @retval      FLASHоƬID
 *   @note      оƬID�б���: norflash.h, оƬ�б�����
 */
uint16_t norflash_read_id(void)
{
    uint16_t deviceid;

    NORFLASH_CS(0);
    spi_read_write_byte(FLASH_ManufactDeviceID);   /* ���Ͷ� ID ���� */
    spi_read_write_byte(0);    /* д��һ���ֽ� */
    spi_read_write_byte(0);
    spi_read_write_byte(0);
    deviceid = spi_read_write_byte(0xFF) << 8;     /* ��ȡ��8λ�ֽ� */
    deviceid |= spi_read_write_byte(0xFF);         /* ��ȡ��8λ�ֽ� */
    NORFLASH_CS(1);

    return deviceid;
}

/**
 * @brief       ��ȡSPI FLASH
 *   @note      ��ָ����ַ��ʼ��ȡָ�����ȵ�����
 * @param       pbuf    : ���ݴ洢��
 * @param       addr    : ��ʼ��ȡ�ĵ�ַ(���32bit)
 * @param       datalen : Ҫ��ȡ���ֽ���(���65535)
 * @retval      ��
 */
void norflash_read(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;

    NORFLASH_CS(0);
    spi_read_write_byte(FLASH_ReadData);       /* ���Ͷ�ȡ���� */
    norflash_send_address(addr);                /* ���͵�ַ */
    
    for(i=0;i<datalen;i++)
    {
        pbuf[i] = spi_read_write_byte(0XFF);   /* ѭ����ȡ */
    }
    
    NORFLASH_CS(1);
}

/**
 * @brief       SPI��һҳ(0~65535)��д������256���ֽڵ�����
 *   @note      ��ָ����ַ��ʼд�����256�ֽڵ�����
 * @param       pbuf    : ���ݴ洢��
 * @param       addr    : ��ʼд��ĵ�ַ(���32bit)
 * @param       datalen : Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
 * @retval      ��
 */
static void norflash_write_page(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t i;

    norflash_write_enable();    /* дʹ�� */

    NORFLASH_CS(0);
    spi_read_write_byte(FLASH_PageProgram);    /* ����дҳ���� */
    norflash_send_address(addr);                /* ���͵�ַ */

    for(i=0;i<datalen;i++)
    {
        spi_read_write_byte(pbuf[i]);          /* ѭ��д�� */
    }
    
    NORFLASH_CS(1);
    norflash_wait_busy();       /* �ȴ�д����� */
}

/**
 * @brief       �޼���дSPI FLASH
 *   @note      ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
 *              �����Զ���ҳ����
 *              ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
 *
 * @param       pbuf    : ���ݴ洢��
 * @param       addr    : ��ʼд��ĵ�ַ(���32bit)
 * @param       datalen : Ҫд����ֽ���(���65535)
 * @retval      ��
 */
static void norflash_write_nocheck(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint16_t pageremain;
    pageremain = 256 - addr % 256;  /* ��ҳʣ����ֽ��� */

    if (datalen <= pageremain)      /* ������256���ֽ� */
    {
        pageremain = datalen;
    }

    while (1)
    {
        /* ��д���ֽڱ�ҳ��ʣ���ַ���ٵ�ʱ��, һ����д��
         * ��д��ֱ�ӱ�ҳ��ʣ���ַ�����ʱ��, ��д������ҳ��ʣ���ַ, Ȼ�����ʣ�೤�Ƚ��в�ͬ����
         */
        norflash_write_page(pbuf, addr, pageremain);

        if (datalen == pageremain)   /* д������� */
        {
            break;
        }
        else     /* datalen > pageremain */
        {
            pbuf += pageremain;         /* pbufָ���ַƫ��,ǰ���Ѿ�д��pageremain�ֽ� */
            addr += pageremain;         /* д��ַƫ��,ǰ���Ѿ�д��pageremain�ֽ� */
            datalen -= pageremain;      /* д���ܳ��ȼ�ȥ�Ѿ�д���˵��ֽ��� */

            if (datalen > 256)          /* ʣ�����ݻ�����һҳ,����һ��дһҳ */
            {
                pageremain = 256;       /* һ�ο���д��256���ֽ� */
            }
            else     /* ʣ������С��һҳ,����һ��д�� */
            {
                pageremain = datalen;   /* ����256���ֽ��� */
            }
        }
    }
}

/**
 * @brief       дSPI FLASH
 *   @note      ��ָ����ַ��ʼд��ָ�����ȵ����� , �ú�������������!
 *              SPI FLASH һ����: 256���ֽ�Ϊһ��Page, 4KbytesΪһ��Sector, 16������Ϊ1��Block
 *              ��������С��λΪSector.
 *
 * @param       pbuf    : ���ݴ洢��
 * @param       addr    : ��ʼд��ĵ�ַ(���32bit)
 * @param       datalen : Ҫд����ֽ���(���65535)
 * @retval      ��
 */
uint8_t g_norflash_buf[4096];   /* �������� */

void norflash_write(uint8_t *pbuf, uint32_t addr, uint16_t datalen)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t *norflash_buf;

    norflash_buf = g_norflash_buf;
    secpos = addr / 4096;       /* ������ַ */
    secoff = addr % 4096;       /* �������ڵ�ƫ�� */
    secremain = 4096 - secoff;  /* ����ʣ��ռ��С */

    //printf("ad:%X,nb:%X\r\n", addr, datalen); /* ������ */
    if (datalen <= secremain)
    {
        secremain = datalen;    /* ������4096���ֽ� */
    }

    while (1)
    {
        norflash_read(norflash_buf, secpos * 4096, 4096);   /* ������������������ */

        for (i = 0; i < secremain; i++)   /* У������ */
        {
            if (norflash_buf[secoff + i] != 0XFF)
            {
                break;      /* ��Ҫ����, ֱ���˳�forѭ�� */
            }
        }

        if (i < secremain)   /* ��Ҫ���� */
        {
            norflash_erase_sector(secpos);  /* ����������� */

            for (i = 0; i < secremain; i++)   /* ���� */
            {
                norflash_buf[i + secoff] = pbuf[i];
            }

            norflash_write_nocheck(norflash_buf, secpos * 4096, 4096);  /* д���������� */
        }
        else        /* д�Ѿ������˵�,ֱ��д������ʣ������. */
        {
            norflash_write_nocheck(pbuf, addr, secremain);  /* ֱ��д���� */
        }

        if (datalen == secremain)
        {
            break;  /* д������� */
        }
        else        /* д��δ���� */
        {
            secpos++;               /* ������ַ��1 */
            secoff = 0;             /* ƫ��λ��Ϊ0 */

            pbuf += secremain;      /* ָ��ƫ�� */
            addr += secremain;      /* д��ַƫ�� */
            datalen -= secremain;   /* �ֽ����ݼ� */

            if (datalen > 4096)
            {
                secremain = 4096;   /* ��һ����������д���� */
            }
            else
            {
                secremain = datalen;/* ��һ����������д���� */
            }
        }
    }
}

/**
 * @brief       ��������оƬ
 *   @note      �ȴ�ʱ�䳬��...
 * @param       ��
 * @retval      ��
 */
void norflash_erase_chip(void)
{
    norflash_write_enable();    /* дʹ�� */
    norflash_wait_busy();       /* �ȴ����� */
    NORFLASH_CS(0);
    spi_read_write_byte(FLASH_ChipErase);  /* ���Ͷ��Ĵ������� */ 
    NORFLASH_CS(1);
    norflash_wait_busy();       /* �ȴ�оƬ�������� */
}

/**
 * @brief       ����һ������
 *   @note      ע��,������������ַ,�����ֽڵ�ַ!!
 *              ����һ������������ʱ��:150ms
 *
 * @param       saddr : ������ַ ����ʵ����������
 * @retval      ��
 */
void norflash_erase_sector(uint32_t saddr)
{
    //printf("fe:%x\r\n", saddr);   /* ����falsh�������,������ */
    saddr *= 4096;
    norflash_write_enable();        /* дʹ�� */
    norflash_wait_busy();           /* �ȴ����� */

    NORFLASH_CS(0);
    spi_read_write_byte(FLASH_SectorErase);    /* ����дҳ���� */
    norflash_send_address(saddr);   /* ���͵�ַ */
    NORFLASH_CS(1);
    norflash_wait_busy();           /* �ȴ������������ */
}

























