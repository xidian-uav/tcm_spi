#include "./trust_interface.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/TCM/tcm.h"

uint16_t respSize = 0;
uint8_t commandBuf[512];

void writeUint16ToBuffer(uint8_t *buffer, uint16_t index, uint16_t data)
{
    // 小端序存储：先存低字节，再存高字节
    buffer[index + 1] = data & 0xFF;    // 低字节
    buffer[index] = (data >> 8) & 0xFF; // 高字节
}

void writeUint32ToBuffer(uint8_t *buffer, uint16_t index, uint32_t data)
{
    // 小端序存储：依次存低字节到高字节
    buffer[index + 3] = data & 0xFF; // 存储最低字节
    buffer[index + 2] = (data >> 8) & 0xFF;
    buffer[index + 1] = (data >> 16) & 0xFF;
    buffer[index] = (data >> 24) & 0xFF; // 存储最高字节
}

int lp_tcm_startup(uint8_t *respBuf)
{
    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 12);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_Startup);
    writeUint16ToBuffer(commandBuf, 10, TCM_SU_CLEAR);

    respSize = sendCommand(commandBuf, 12, respBuf);

    return respSize;
}

/* sm3哈希接口 */
int lp_tcm_hash_sm3(uint8_t *data, uint16_t dataSize, uint8_t *respBuf)
{
    uint16_t commandSize = 2 + 4 + 4 + 2 + dataSize + 2 + 4; /* 计算命令长度 */

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, commandSize);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_Hash);
    writeUint16ToBuffer(commandBuf, 10, dataSize);

    uint16_t i, j;
    for (i = 12, j = 0; j < dataSize; i++, j++)
    {
        commandBuf[i] = data[j];
    }

    writeUint16ToBuffer(commandBuf, 12 + dataSize, TCM_ALG_SHA256);
    writeUint32ToBuffer(commandBuf, 12 + dataSize + 2, TCM_RH_NULL);

    respSize = sendCommand(commandBuf, commandSize, respBuf);

    return respSize;
}