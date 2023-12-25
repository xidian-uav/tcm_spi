#include "retarget.h"
#include "main.h"
#include <stdio.h>

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

#define TIS_DEBUG  printf
#define TIS_DEBUG_LV2 printf
#define TPM2_BUFSIZE 512

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);

#define MAX_RETRIES 10

int CFG_NOWAIT = 0;

// uint8_t *TPM2_CC_CreatePrimary = "\x80\x02\x00\x00\x00\x5b\x00\x00\x01\x31\x40\x00\x00\x01\x00\x00\x00\x09\x40\x00\x00\x09\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x32\x00\x25\x00\x0b\x00\x06\x00\x72\x00\x00\x00\x06\x00\x80\x00\x43\x00\x20\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55\x00\x00\x00\x00\x00\x00";
// uint8_t *TPM2_CC_Encrypt = "\x80\x02\x00\x00\x00\x42\x00\x00\x01\x64\x80\x00\x00\x00\x00\x00\x00\x09\x40\x00\x00\x09\x00\x00\x00\x00\x00\x00\x00\x10\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff";

uint8_t *TPM2_CC_CreatePrimary = "\x80\x02\x00\x00\x00\x5f\x00\x00\x01\x31\x40\x00\x00\x01\x00\x00\x00\x09\x40\x00\x00\x09\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x36\x00\x01\x00\x0b\x00\x06\x00\x72\x00\x00\x00\x10\x00\x10\x08\x00\x00\x00\x00\x00\x00\x20\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55\x00\x00\x00\x00\x00\x00";
#define LEN_TPM2_CC_CreatePrimary 95
uint8_t *TPM2_CC_Encrypt = "\x80\x01\x00\x00\x00\x19\x00\x00\x01\x74\x80\x00\x00\x00\x00\x05\x11\x22\x33\x44\x55\x00\x15\x00\x00";
#define LEN_TPM2_CC_Encrypt 25

uint8_t unhexlify_nibble(uint8_t databyte)
{
	// printf("%d\n",databyte);
	if(databyte >= '0' && databyte <= '9')
	{
		return databyte - '0';
	}
	else if(databyte >= 'a' && databyte <= 'f')
	{
		return databyte - 'a' + 0x0a;
	}
	else if(databyte >= 'A' && databyte <= 'F')
	{
		return databyte - 'A' + 0x0a;
	}
	else
	{
		printf("ERR: unhexlify_nibble called on %02x\r\n", databyte);
		return 0;
	}
}

uint8_t unhexlify(uint8_t *data, int data_offset)
{
	uint8_t intval = unhexlify_nibble(data[data_offset]) * 0x10 + unhexlify_nibble(data[data_offset + 1]);
	// printf("%d\n",intval);
	return intval;
}

void target_reset()
{
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,0);
	  HAL_Delay(500);
	  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,1);
	  HAL_Delay(500);
	  return;
}

void writeRegister(uint8_t addrByte1,uint8_t addrByte2, uint8_t writeSize, uint8_t *data)
{
	uint8_t dbuf[TPM2_BUFSIZE];
	uint8_t rbuf[TPM2_BUFSIZE];
	int tsize = 4 + writeSize;
	dbuf[0] = writeSize - 1;
	dbuf[1] = 0xD4;
	dbuf[2] = addrByte1;
	dbuf[3] = addrByte2;
	int i = 0;
	for(i = 0;i < writeSize;i++)
	{
		dbuf[4+i] = data[i];
	}
	int temp2 = 0;
	HAL_SPI_TransmitReceive(&hspi1,dbuf,rbuf,tsize,HAL_MAX_DELAY);
	TIS_DEBUG("TX: ");
	for(temp2 = 0;temp2 < tsize;temp2++ )
	{
		TIS_DEBUG("%02x ",dbuf[temp2]);
	}
	TIS_DEBUG("\r\n");
	TIS_DEBUG("RX: ");
	for(temp2 = 0;temp2 < tsize;temp2++ )
	{
		TIS_DEBUG("%02x ",rbuf[temp2]);
	}
	TIS_DEBUG("\r\n");
	return;
}

void readRegister(uint8_t addrByte1, uint8_t addrByte2,uint8_t readSize, uint8_t *destbuf)
{
	uint8_t dbuf[TPM2_BUFSIZE];
	uint8_t rbuf[TPM2_BUFSIZE];
	int tsize = 4 + readSize;

	dbuf[0] = 0x80 | readSize - 1;
	dbuf[1] = 0xD4;
	dbuf[2] = addrByte1;
	dbuf[3] = addrByte2;
	int i = 0;
	for(i = 0;i < readSize;i++)
	{
		dbuf[4 + i] = 0x00;
	}
	int temp2 = 0;

	HAL_SPI_TransmitReceive(&hspi1,dbuf,rbuf,tsize,HAL_MAX_DELAY);
	TIS_DEBUG("TX: ");
	for(temp2 = 0;temp2 < tsize;temp2++ )
	{
		TIS_DEBUG("%02x ",dbuf[temp2]);
	}
	TIS_DEBUG("\r\n");
	TIS_DEBUG("RX: ");
	for(temp2 = 0;temp2 < tsize;temp2++ )
	{
		TIS_DEBUG("%02x ",rbuf[temp2]);
	}
	TIS_DEBUG("\r\n");

	// memcpy
	if(destbuf != NULL)
	{
		for(temp2 = 0;temp2 < tsize;temp2++ )
		{
			destbuf[temp2] = rbuf[temp2];
		    // printf("%02x ",rbuf[temp2]);
		}
	}

	return;
}

#define CS_LOW HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0)
#define CS_HIGH HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,1)

int sendCommand(uint8_t *cmdFrame,int cmdSize,uint8_t *respBuf)
{
  uint8_t readbuf[TPM2_BUFSIZE];
  TIS_DEBUG_LV2("sendCommand Called\r\n");
  CS_LOW;
  // printf("Siezing locality...\r\n");
  readRegister(0x00,0x00,1,NULL);
  CS_HIGH;
  CS_LOW;
  writeRegister(0x00,0x00,1,"\x02");
  CS_HIGH;
  CS_LOW;
  readRegister(0x00,0x00,1,NULL);
  CS_HIGH;
  HAL_Delay(10);
  readbuf[4] = '\x00';

  int errcount = 0;

  while((readbuf[4] & 0x40) == 0)
  {
	  errcount += 1;
	  if(errcount > 5)
	  {
		  printf("Failure (errcount > 5, precmd)...\r\n");
		  while(1){};
	  }
	  TIS_DEBUG_LV2("Waiting for cmdReady...\r\n");
	  CS_LOW;
	  readRegister(0x00,0x18,1,readbuf);
	  CS_HIGH;
  }


  TIS_DEBUG_LV2("Checking TPM_STS_0.burstCount\r\n");
  CS_LOW;
  readRegister(0x00,0x18,4,readbuf);
  CS_HIGH;

  if((cmdSize - 1) < 63)
  {
	  TIS_DEBUG_LV2("Writing all-but-one to 0x0024...\r\n");
	  CS_LOW;
	  writeRegister(0x00,0x24,cmdSize - 1,cmdFrame);// "\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x44\x00");
	  CS_HIGH;

	  // NOTE: DELAY.
	  TIS_DEBUG_LV2("Checking Expect bit\r\n");
	  CS_LOW;
	  readRegister(0x00,0x18,1,readbuf);
	  CS_HIGH;
	  if((readbuf[4] & 0x08) != 0x08)
	  {
		  printf("Failure (expect bit incorrect)...\r\n");
		  while(1){};
	  }
  }
  else
  {
	  printf("Entering extra long command frame\r\n");
	  int remainingSize = cmdSize - 1;
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
		  TIS_DEBUG_LV2("REMAIN:%d, EXPCT:%d\r\n",remainingSize,cmdSize - 1);
		  TIS_DEBUG_LV2("Transferring %d bytes to 0x0024\r\n",writeSize);
		  CS_LOW;
		  writeRegister(0x00,0x24,writeSize,cmdFrame + writeHead);// "\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x44\x00");
		  CS_HIGH;

		  HAL_Delay(10);
		  TIS_DEBUG_LV2("Checking Expect bit\r\n");
		  CS_LOW;
		  readRegister(0x00,0x18,1,readbuf);
		  CS_HIGH;
		  if((readbuf[4] & 0x08) != 0x08)
		  {
		     printf("Failure (expect bit incorrect)...\r\n");
		  	 while(1){};
		  }
		  writeHead += 30;
	  }
  }

  TIS_DEBUG_LV2("Completing command...\r\n");
  CS_LOW;
  writeRegister(0x00,0x24,1,cmdFrame + cmdSize - 1);
  CS_HIGH;

  TIS_DEBUG_LV2("Checking Expect bit (again)\r\n");
  CS_LOW;
  readRegister(0x00,0x18,1,readbuf);
  CS_HIGH;
  if((readbuf[4] & 0x08) != 0x00)
  {
	  printf("Failure (expect bit incorrect, post-command)...\r\n");
	  while(1){};
  }

  TIS_DEBUG_LV2("Setting go bit\r\n");
  CS_LOW;
  writeRegister(0x00,0x18,1,"\x20");
  CS_HIGH;

  HAL_Delay(100);

  errcount = 0;
  readbuf[4] = '\x00';
  while((readbuf[4] & 0x10) == 0)
  {
	  errcount += 1;
	  if(errcount > 10)
	  {
		  printf("Failure (errcount > 10, postcmd)...\r\n");
		  while(1){};
	  }
	  HAL_Delay(5000);
	  TIS_DEBUG_LV2("Waiting for dataAvail...\r\n");
	  CS_LOW;
	  readRegister(0x00,0x18,1,readbuf);
	  CS_HIGH;
  }

  TIS_DEBUG_LV2("Checking burst count\r\n");
  CS_LOW;
  readRegister(0x00,0x18,4,NULL);
  CS_HIGH;

  int i = 0;
  TIS_DEBUG_LV2("Reading reply header...\r\n");
  CS_LOW;
  readRegister(0x00,0x24,10,readbuf);
  CS_HIGH;
  for(i = 0;i < 10;i++)
  {
	  respBuf[i] = readbuf[i + 4];
  }

  TIS_DEBUG_LV2("Checking burst count\r\n");
  CS_LOW;
  readRegister(0x00,0x18,4,NULL);
  CS_HIGH;

  int respsize = readbuf[8] * 0x100 + (readbuf[9] - 10);
  if(respsize == 0)
  {
	  TIS_DEBUG_LV2("Condition: response OK, do not need further reading...\r\n");
  }
  else
  {
	  if(respsize <= 63)
	  {
		  TIS_DEBUG_LV2("Reading %d resp bytes...\r\n",respsize);
		  CS_LOW;
		  readRegister(0x00,0x24,respsize,readbuf);
		  CS_HIGH;
		  for(i = 0;i < respsize;i++)
		  {
			  respBuf[10+i] = readbuf[i + 4];
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
				  TIS_DEBUG_LV2("Reading 30 bytes...\r\n");
				  CS_LOW;
				  readRegister(0x00,0x24,readSize,readbuf);
				  CS_HIGH;
				  for(i = 0;i < 30;i++)
				  {
					  respBuf[10+writeHead+i] = readbuf[i + 4];
				  }
				  writeHead += 30;
				  HAL_Delay(50);
				  /*
				  readbuf[4] = '\x00';

				  while((readbuf[4] & 0x80) != 0x10 || (readbuf[4] & 0x10) != 0x10)
				  {
				    errcount += 1;
				    if(errcount > 5)
				    {
				  	   printf("Failure (errcount > 5)...\r\n");
				  	   while(1){};
				  	}
				  	TIS_DEBUG_LV2("Long Read: Waiting for stsValid == 1 && dataAvail == 1\r\n");
				    CS_LOW;
				    readRegister(0x00,0x18,1,readbuf);
				    CS_HIGH;
				  }
				  */

			  }
			  else
			  {
				  readSize = remainingRead;
				  remainingRead = 0;
				  TIS_DEBUG_LV2("Reading %d bytes...\r\n",readSize);
				  CS_LOW;
				  readRegister(0x00,0x24,readSize,readbuf);
				  CS_HIGH;
				  for(i = 0;i < readSize;i++)
				  {
					  respBuf[10+writeHead+i] = readbuf[i + 4];
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
  readbuf[4] = '\x00';
  while((readbuf[4] & 0x80) == 0 || (readbuf[4] & 0x10) != 0)
  {
	  errcount += 1;
	  if(errcount > 10)
	  {
	     printf("Failure (errcount > 10, waiting for stsvalid 1 dataavail 0)...\r\n");
		 while(1){};
	  }
	  HAL_Delay(2000);
	  TIS_DEBUG_LV2("Waiting for stsValid == 1 && dataAvail == 0\r\n");
  	  CS_LOW;
  	  readRegister(0x00,0x18,1,readbuf);
  	  CS_HIGH;
  }
  }

  TIS_DEBUG_LV2("Resetting state machine\r\n");
  CS_LOW;
  writeRegister(0x00,0x18,1,"\x40");
  CS_HIGH;

  CS_LOW;
  writeRegister(0x00,0x00,1,"\x20");
  CS_HIGH;
  return 10 + respsize;
}

int tcm_spi(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();

  uint8_t tbuf[TPM2_BUFSIZE];
  uint8_t rbuf[TPM2_BUFSIZE];
  int tsize = 0;
  uint8_t uartbuf[128];

  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,1); // CS HIGH
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,1); // RST HIGH

  RetargetInit(&huart2);
  int uartHdr = 0;
  int temp2 = 0;
  int respsize = 0;
  uint8_t readbuf[TPM2_BUFSIZE];

  printf("ok\r\n");
  while(1)
  {
	  for(uartHdr = 0;uartHdr < 128;uartHdr++)
	  {
		  uartbuf[uartHdr] = getc(stdin);
		  putc(uartbuf[uartHdr],stdout);
		  if(uartbuf[uartHdr] == '\r' || uartbuf[uartHdr] == '\n' || uartbuf[uartHdr] == 0)
		  {
			  break;
		  }
	  }

	  for(uartHdr = 0;uartHdr < 128; uartHdr++)
	  {
		  if(uartbuf[uartHdr] == '\r' || uartbuf[uartHdr] == '\n' || uartbuf[uartHdr] == 0)
		  {
			  break;
		  }
		  else if(uartbuf[uartHdr] == 'R' || uartbuf[uartHdr] == 'r') // reset
		  {
			  target_reset();
		  }
		  else if(uartbuf[uartHdr] == '-')  // cs low
		  {
			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,0);
		  }
		  else if(uartbuf[uartHdr] == '+') // cs high
		  {
			  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,1);
		  }
		  else if((uartbuf[uartHdr] >= '0' && uartbuf[uartHdr] <= '9') || (uartbuf[uartHdr] >= 'a' && uartbuf[uartHdr] <= 'f') ||  (uartbuf[uartHdr] >= 'A' && uartbuf[uartHdr] <= 'F'))
		  {
			  tbuf[tsize++] = unhexlify(uartbuf,uartHdr);
			  uartHdr += 1;
		  }
		  else if(uartbuf[uartHdr] == 's' || uartbuf[uartHdr] == 'S')
		  {
			  HAL_SPI_TransmitReceive(&hspi1,tbuf,rbuf,tsize,HAL_MAX_DELAY);
			  printf("TX: ");
			  for(temp2 = 0;temp2 < tsize;temp2++ )
			  {
				printf("%02x ",tbuf[temp2]);
			  }
			  printf("\r\n");
			  printf("RX: ");
			  for(temp2 = 0;temp2 < tsize;temp2++ )
			  {
			    printf("%02x ",rbuf[temp2]);
			  }
			  printf("\r\n");
			  tsize = 0;
		  }
		  else if(uartbuf[uartHdr] == 't')
		  {
			  printf("Entering test routine 1.");

			  target_reset();
			  printf("Waiting...\r\n");
			  HAL_Delay(1000);

			  // TPM2_STARTUP
			  sendCommand("\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x44\x00\x00",12,readbuf);
			  printf("OK: ");
			  for(temp2 = 0;temp2 < 10;temp2++ )
			  {
				  printf("%02x ",readbuf[temp2]);
			  }
			  printf("\r\n");

			  // TPM2_GETRANDOM
			  respsize = sendCommand("\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x7b\x00\x10",12,readbuf);
			  printf("OK: ");
			  for(temp2 = 0;temp2 < respsize;temp2++ )
			  {
				  printf("%02x ",readbuf[temp2]);
			  }
			  printf("\r\n");

			  // target_reset();
		  }
		  else if(uartbuf[uartHdr] == 'v' || uartbuf[uartHdr] == 'V')
		  {
			  respsize = sendCommand(tbuf,tsize,readbuf);
			  printf("OK: ");
			  for(temp2 = 0;temp2 < respsize;temp2++ )
			  {
				  printf("%02x ",readbuf[temp2]);
			  }
			  tsize = 0;
			  printf("\r\n");
		  }
		  else if(uartbuf[uartHdr] == 'z')
		  {
			  printf("Entering test routine 2\r\n");
			  uint8_t readbuf[TPM2_BUFSIZE];
			  target_reset();
			  printf("Waiting...\r\n");
			  HAL_Delay(1000);

			  // TPM2_STARTUP
			  sendCommand("\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x44\x00\x00",12,readbuf);
			  /*
			  printf("OK: ");
			  for(temp2 = 0;temp2 < 10;temp2++ )
			  {
				  printf("%02x ",readbuf[temp2]);
			  }
			  printf("\r\n");

			  */
			  HAL_Delay(100);

			  int respsize;

			  respsize = sendCommand(TPM2_CC_CreatePrimary,LEN_TPM2_CC_CreatePrimary,readbuf);
			  /*
			  printf("OK: ");
			  for(temp2 = 0;temp2 < respsize;temp2++ )
			  {
				  printf("%02x ",readbuf[temp2]);
			  }
			  printf("\r\n");


			  */

			  HAL_Delay(100);
			  printf("Sending!\r\n");

			  CFG_NOWAIT = 1;
			  respsize = sendCommand(TPM2_CC_Encrypt,LEN_TPM2_CC_Encrypt,readbuf);
			  CFG_NOWAIT = 0;
			  printf("OK: ");
			  for(temp2 = 0;temp2 < respsize;temp2++ )
			  {
				  printf("%02x ",readbuf[temp2]);
			  }
			  printf("\r\n");



			  // target_reset();
		 		  }
		  else if(uartbuf[uartHdr] == ' ')
		  {
			  // pass
		  }
		  else
		  {
			  printf("(error: bad char)\n");
			  break;
		  }
	  }
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
