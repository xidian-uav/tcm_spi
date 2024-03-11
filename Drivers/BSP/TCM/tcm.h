#ifndef __TCM_H
#define __TCM_H

#include "./SYSTEM/sys/sys.h"

#define TCM_SPI                             SPI1


/* TCM命令宏定义 */
#define TPM2_Startup                        "\x80\x01\x00\x00\x00\x0c\x00\x00\x01\x44\x00\x00"

#define TCM_RX_BUFF_SIZE                    256


void tcm_init();
void target_reset();
void tcm_read_register(uint8_t addr_byte1, uint8_t addr_byte2, uint8_t *dest_buf, uint8_t read_size);
void tcm_write_register(uint8_t addr_byte1, uint8_t addr_byte2, uint8_t *data, uint8_t data_size);
int sendCommand(uint8_t *cmd_frame,int cmd_size,uint8_t *resp_buf);

#endif