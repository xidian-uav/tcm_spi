#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/SPI/spi.h"
#include "./BSP/TCM/tcm.h"

SPI_HandleTypeDef hspi; /* SPI句柄 */
uint8_t CFG_NOWAIT = 0;

void tcm_init()
{    
    hspi.Instance = TCM_SPI;                                 /* SPI1 */
    hspi.Init.Mode = SPI_MODE_MASTER;                        /* 设置SPI工作模式，设置为主模式 */
    hspi.Init.Direction = SPI_DIRECTION_2LINES;              /* 设置SPI单向或者双向的数据模式:SPI设置为双线模式 */
    hspi.Init.DataSize = SPI_DATASIZE_8BIT;                  /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
    hspi.Init.CLKPolarity = SPI_POLARITY_LOW;                /* 串行同步时钟的空闲状态为低电平 */
    hspi.Init.CLKPhase = SPI_PHASE_1EDGE;                    /* 串行同步时钟的第一个跳变沿（上升或下降）数据被采样 */
    hspi.Init.NSS = SPI_NSS_SOFT;                            /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
    hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; /* 定义波特率预分频的值:波特率预分频值为256 */
    hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;                   /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
    hspi.Init.TIMode = SPI_TIMODE_DISABLE;                   /* 关闭TI模式 */
    hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;   /* 关闭硬件CRC校验 */
    hspi.Init.CRCPolynomial = 7;                             /* CRC值计算的多项式 */
    
    spi_init(&hspi);                                          /* 初始化SPI */
    
    
    GPIO_InitTypeDef gpio_init_struct;                       // 初始化片选引脚
    
    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpio_init_struct.Pin = GPIO_PIN_4 | GPIO_PIN_0;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpio_init_struct);
    
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);
    printf("tcm init successfully!\r\n");
}

void target_reset()
{
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
    HAL_Delay(500);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,1);
    HAL_Delay(500);
    return;
}

void tcm_write_register(uint8_t addr_byte1, uint8_t addr_byte2, uint8_t *data, uint8_t data_size)
{
    uint8_t tx_data_buff[TCM_RX_BUFF_SIZE];         /* 发送缓冲区 */
    uint8_t rx_data_buff[TCM_RX_BUFF_SIZE];         /* 接收缓冲区 */
    
    uint16_t tx_package_size = 4 + data_size;       /* 数据包大小 */
    tx_data_buff[0] = data_size - 1;                /* 载荷大小 */
    tx_data_buff[1] = 0xD4;                         /* 地址偏移 */
    tx_data_buff[2] = addr_byte1;                   /* 寄存器地址高位 */
    tx_data_buff[3] = addr_byte2;                   /* 寄存器地址低位 */
    
    int i = 0;                                      /* 连接数据包头和载荷 */
    for(i = 0;i < data_size;i++)
    {
        tx_data_buff[4+i] = data[i];
    }
    
    spi_read_write_data(&hspi, tx_data_buff, tx_package_size, rx_data_buff);  /* 发送数据包 */
    
    printf("TX: ");                             /* 串口输出发送的数据 */
    for(i = 0; i < tx_package_size; i++)
    {
        printf("%02x ", tx_data_buff[i]);
    }
    
    printf("\r\n");
    
    printf("RX: ");                             /* 串口输出接收的数据 */
    for(i = 0; i < tx_package_size; i++)
    {
        printf("%02x ", rx_data_buff[i]);
    }
    
    printf("\r\n");
    
    return;
}

void tcm_read_register(uint8_t addr_byte1, uint8_t addr_byte2, uint8_t *dest_buf, uint8_t read_size)
{
    uint8_t tx_data_buff[TCM_RX_BUFF_SIZE];         /* 发送缓冲区 */
    uint8_t rx_data_buff[TCM_RX_BUFF_SIZE];         /* 接收缓冲区 */
    
    uint16_t tx_package_size = 4 + read_size;       /* 数据包大小 */
    tx_data_buff[0] = 0x80 | read_size - 1;         /* 载荷大小 */
    tx_data_buff[1] = 0xD4;                         /* 地址偏移 */
    tx_data_buff[2] = addr_byte1;                   /* 寄存器地址高位 */
    tx_data_buff[3] = addr_byte2;                   /* 寄存器地址低位 */

    int i = 0;                                      /* 连接数据包头和载荷 */
    for(i = 0;i < read_size;i++)
    {
        tx_data_buff[4 + i] = 0x00;
    }

    spi_read_write_data(&hspi, tx_data_buff, tx_package_size, rx_data_buff);    /* 发送数据包 */
    
    printf("TX: ");                                 /* 串口输出发送的数据 */
    for(i = 0; i < tx_package_size; i++)
    {
        printf("%02x ",tx_data_buff[i]);
    }
    printf("\r\n");
    
    printf("RX: ");                                 /* 串口输出接收的数据 */
    for(i = 0; i < tx_package_size; i++)
    {
        printf("%02x ",rx_data_buff[i]);
    }
    printf("\r\n");


    if(dest_buf != NULL)                            /* 拷贝接收的数据 */
    {
        for(i = 0; i < tx_package_size; i++)
        {
            dest_buf[i] = rx_data_buff[i];
    }
    }

    return;
}

#define CS_LOW HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0)
#define CS_HIGH HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1)

int sendCommand(uint8_t *cmd_frame, int cmd_size, uint8_t *resp_buf)
{
    printf("sendCommand Called\r\n");
    
    uint8_t read_buf[TCM_RX_BUFF_SIZE];
    
    CS_LOW;
    // printf("Siezing locality...\r\n");
    tcm_read_register(0x00, 0x00, NULL, 1);
    CS_HIGH;
    
    CS_LOW;
    tcm_write_register(0x00, 0x00, "\x02", 1);
    CS_HIGH;
    
    CS_LOW;
    tcm_read_register(0x00, 0x00, NULL, 1);
    CS_HIGH;
    
    HAL_Delay(10);
    read_buf[4] = '\x00';

    int errcount = 0;

    while((read_buf[4] & 0x40) == 0)
    {
        errcount += 1;
        if(errcount > 5)
        {
          printf("Failure (errcount > 5, precmd)...\r\n");
          while(1){};
        }
        printf("Waiting for cmdReady...\r\n");
        CS_LOW;
        tcm_read_register(0x00, 0x18, read_buf, 1);
        CS_HIGH;
    }


    printf("Checking TPM_STS_0.burstCount\r\n");
    CS_LOW;
    tcm_read_register(0x00, 0x18, read_buf, 4);
    CS_HIGH;

    if((cmd_size - 1) < 63)
    {
        printf("Writing all-but-one to 0x0024...\r\n");
        CS_LOW;
        tcm_write_register(0x00, 0x24, cmd_frame, cmd_size - 1);// "\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x44\x00");
        CS_HIGH;

        // NOTE: DELAY.
        printf("Checking Expect bit\r\n");
        CS_LOW;
        tcm_read_register(0x00, 0x18, read_buf, 1);
        CS_HIGH;
        if((read_buf[4] & 0x08) != 0x08)
        {
            printf("Failure (expect bit incorrect)...\r\n");
            while(1){};
        }
    }
    else
    {
        printf("Entering extra long command frame\r\n");
        int remainingSize = cmd_size - 1;
        int writeHead = 0;
        int writeSize = 0;
        while(remainingSize > 0)
        {
            if(remainingSize >= 30)
            {
                remainingSize -= 30;
                writeSize = 30;
            }
            else
            {
                writeSize = remainingSize;
                remainingSize = 0;
            }
            
            printf("REMAIN:%d, EXPCT:%d\r\n",remainingSize,cmd_size - 1);
            printf("Transferring %d bytes to 0x0024\r\n",writeSize);
            CS_LOW;
            tcm_write_register(0x00, 0x24, cmd_frame + writeHead, writeSize);// "\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x44\x00");
            CS_HIGH;

            HAL_Delay(10);
            printf("Checking Expect bit\r\n");
            CS_LOW;
            tcm_read_register(0x00, 0x18, read_buf, 1);
            CS_HIGH;
            if((read_buf[4] & 0x08) != 0x08)
            {
                printf("Failure (expect bit incorrect)...\r\n");
                while(1){};
            }
            
            writeHead += 30;
        }
    }

    printf("Completing command...\r\n");
    CS_LOW;
    tcm_write_register(0x00, 0x24, cmd_frame + cmd_size - 1, 1);
    CS_HIGH;

    printf("Checking Expect bit (again)\r\n");
    CS_LOW;
    tcm_read_register(0x00, 0x18, read_buf, 1);
    CS_HIGH;
    if((read_buf[4] & 0x08) != 0x00)
    {
        printf("Failure (expect bit incorrect, post-command)...\r\n");
        while(1){};
    }

    printf("Setting go bit\r\n");
    CS_LOW;
    tcm_write_register(0x00, 0x18, "\x20", 1);
    CS_HIGH;

    HAL_Delay(100);

    errcount = 0;
    read_buf[4] = '\x00';
    while((read_buf[4] & 0x10) == 0)
    {
        errcount += 1;
        if(errcount > 10)
        {
            printf("Failure (errcount > 10, postcmd)...\r\n");
            while(1){};
        }
        HAL_Delay(5000);
        printf("Waiting for dataAvail...\r\n");
        CS_LOW;
        tcm_read_register(0x00, 0x18, read_buf, 1);
        CS_HIGH;
    }

    printf("Checking burst count\r\n");
    CS_LOW;
    tcm_read_register(0x00, 0x18, NULL, 4);
    CS_HIGH;

    int i = 0;
    printf("Reading reply header...\r\n");
    CS_LOW;
    tcm_read_register(0x00, 0x24, read_buf, 10);
    CS_HIGH;
    for(i = 0;i < 10;i++)
    {
        resp_buf[i] = read_buf[i + 4];
    }

    printf("Checking burst count\r\n");
    CS_LOW;
    tcm_read_register(0x00, 0x18, NULL, 4);
    CS_HIGH;

    int respsize = read_buf[8] * 0x100 + (read_buf[9] - 10);
    if(respsize == 0)
    {
        printf("Condition: response OK, do not need further reading...\r\n");
    }
    else
    {
        if(respsize <= 63)
        {
            printf("Reading %d resp bytes...\r\n",respsize);
            CS_LOW;
            tcm_read_register(0x00, 0x24, read_buf, respsize);
            CS_HIGH;
            for(i = 0;i < respsize;i++)
            {
              resp_buf[10+i] = read_buf[i + 4];
            }
        }
        else
        {
            printf("Entering read cycle for respsize %d\r\n",respsize);
            int writeHead = 0;
            int remainingRead = respsize;
            int readSize = 0;
            while(remainingRead > 0)
            {
                if(remainingRead > 30)
                {
                    readSize = 30;
                    remainingRead -= 30;
                    printf("Reading 30 bytes...\r\n");
                    CS_LOW;
                    tcm_read_register(0x00, 0x24, read_buf, readSize);
                    CS_HIGH;
                    for(i = 0;i < 30;i++)
                    {
                      resp_buf[10+writeHead+i] = read_buf[i + 4];
                    }
                    writeHead += 30;
                    HAL_Delay(50);
                    /*
                    readbuf[4] = '\x00';

                    while((read_buf[4] & 0x80) != 0x10 || (read_buf[4] & 0x10) != 0x10)
                    {
                    errcount += 1;
                    if(errcount > 5)
                    {
                       printf("Failure (errcount > 5)...\r\n");
                       while(1){};
                    }
                    TIS_DEBUG_LV2("Long Read: Waiting for stsValid == 1 && dataAvail == 1\r\n");
                    CS_LOW;
                    tcm_read_register(0x00,0x18,1,read_buf);
                    CS_HIGH;
                    }
                    */
                }
                else
                {
                    readSize = remainingRead;
                    remainingRead = 0;
                    printf("Reading %d bytes...\r\n",readSize);
                    CS_LOW;
                    tcm_read_register(0x00, 0x24, read_buf, readSize);
                    CS_HIGH;
                    
                    for(i = 0;i < readSize;i++)
                    {
                        resp_buf[10+writeHead+i] = read_buf[i + 4];
                    }
                    HAL_Delay(50);
                    // writeHead += 30;
                }
            }
        }
    }

    if(CFG_NOWAIT == 0)
    {
        errcount = 0;
        read_buf[4] = '\x00';
        while((read_buf[4] & 0x80) == 0 || (read_buf[4] & 0x10) != 0)
        {
            errcount += 1;
            if(errcount > 10)
            {
                printf("Failure (errcount > 10, waiting for stsvalid 1 dataavail 0)...\r\n");
                while(1){};
            }
            HAL_Delay(2000);
            printf("Waiting for stsValid == 1 && dataAvail == 0\r\n");
            CS_LOW;
            tcm_read_register(0x00, 0x18, read_buf, 1);
            CS_HIGH;
        }
    }

    printf("Resetting state machine\r\n");
    CS_LOW;
    tcm_write_register(0x00, 0x18, "\x40", 1);
    CS_HIGH;

    CS_LOW;
    tcm_write_register(0x00, 0x00, "\x20", 1);
    CS_HIGH;
    return 10 + respsize;
}

