#include "./trust_interface.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/TCM/tcm.h"

/* 本文件仅处理命令封包与响应解析，不涉及底层 SPI 细节 */
static uint16_t respSize = 0;
static uint8_t commandBuf[TCM_COMMAND_BUFF_SIZE];

/* TCM/TPM 指令字段为大端序 */
static void writeUint16ToBuffer(uint8_t *buffer, uint16_t index, uint16_t data)
{
    buffer[index] = (uint8_t)((data >> 8) & 0xFF);
    buffer[index + 1] = (uint8_t)(data & 0xFF);
}

static void writeUint32ToBuffer(uint8_t *buffer, uint16_t index, uint32_t data)
{
    buffer[index] = (uint8_t)((data >> 24) & 0xFF);
    buffer[index + 1] = (uint8_t)((data >> 16) & 0xFF);
    buffer[index + 2] = (uint8_t)((data >> 8) & 0xFF);
    buffer[index + 3] = (uint8_t)(data & 0xFF);
}

static uint16_t readUint16FromBuffer(const uint8_t *buffer, uint16_t index)
{
    return (uint16_t)(((uint16_t)buffer[index] << 8) | buffer[index + 1]);
}

static uint32_t readUint32FromBuffer(const uint8_t *buffer, uint16_t index)
{
    return ((uint32_t)buffer[index] << 24) |
           ((uint32_t)buffer[index + 1] << 16) |
           ((uint32_t)buffer[index + 2] << 8) |
           buffer[index + 3];
}

void lp_tcm_startup(uint8_t *respBuf)
{
    if (respBuf == NULL)
    {
        return;
    }

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 12);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_Startup);
    writeUint16ToBuffer(commandBuf, 10, TCM_SU_CLEAR);

    respSize = sendCommand(commandBuf, 12, respBuf, 100);
}

int lp_tcm_shutdown(uint8_t *respBuf)
{
    if (respBuf == NULL)
    {
        return -1;
    }

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 12);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_Shutdown);
    writeUint16ToBuffer(commandBuf, 10, TCM_SU_CLEAR);

    respSize = sendCommand(commandBuf, 12, respBuf, 0);

    return respSize;
}

int lp_tcm_selftest(uint8_t *respBuf)
{
    if (respBuf == NULL)
    {
        return -1;
    }

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 11);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_SelfTest);
    writeUint16ToBuffer(commandBuf, 10, TCM_YES);

    respSize = sendCommand(commandBuf, 11, respBuf, 0);

    return respSize;
}

int lp_tcm_clearup(uint8_t *respBuf)
{
    if (respBuf == NULL)
    {
        return -1;
    }

    /* Clear 需要平台授权，默认使用空口令的 PW 会话 */
    writeUint16ToBuffer(commandBuf, 0, TCM_ST_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 0x1b);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_Clear);
    writeUint32ToBuffer(commandBuf, 10, TCM_RH_PLATFORM);

    writeUint32ToBuffer(commandBuf, 14, 9);          /* authorizationSize */
    writeUint32ToBuffer(commandBuf, 18, 0x40000009); /* TCM_RS_PW */
    commandBuf[22] = 0x00;                           /* sessionAttributes */
    writeUint32ToBuffer(commandBuf, 23, 0);          /* nonceSize=0, authSize=0 */

    respSize = sendCommand(commandBuf, 0x1b, respBuf, 0);

    return respSize;
}

int lp_tcm_getrandom(uint16_t bytesRequested, uint8_t *respBuf)
{
    if (respBuf == NULL)
    {
        return -1;
    }

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 12);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_GetRandom);
    writeUint16ToBuffer(commandBuf, 10, bytesRequested);

    respSize = sendCommand(commandBuf, 12, respBuf, 0);

    return respSize;
}

uint32_t lp_tcm_create_rsa(uint8_t *respBuf)
{
    if (respBuf == NULL)
    {
        return 0;
    }

    /* 创建 RSA Primary Key（Owner Hierarchy） */
    // //uint8_t tcmCreatePrimary[] = "\x80\x02\x00\x00\x00\x41\x00\x00\x01\x31\x40\x00\x00\x01\x00\x00\x00\x09\x40\x00\x00\x09\x00\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x18\x00\x23\x00\x0b\x00\x06\x00\x72\x00\x00\x00\x10\x00\x18\x00\x04\x00\x20\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    // //respSize = sendCommand(tcmCreatePrimary, 65, respBuf);

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 95);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_CreatePrimary);
    writeUint32ToBuffer(commandBuf, 10, TCM_RH_OWNER);

    writeUint32ToBuffer(commandBuf, 14, 9); /* authorizationSize */
    writeUint32ToBuffer(commandBuf, 18, 0x40000009);
    commandBuf[22] = 0x00;
    writeUint32ToBuffer(commandBuf, 23, 0);

    writeUint16ToBuffer(commandBuf, 27, 4); /* TCMB_SENSITIVE_CREATE */
    writeUint16ToBuffer(commandBuf, 29, 0);
    writeUint16ToBuffer(commandBuf, 31, 0);

    writeUint16ToBuffer(commandBuf, 33, 54);             /* TCM2B_PUBLIC_size */
    writeUint16ToBuffer(commandBuf, 35, TCM_ALG_RSA);    /* TCMI_ALG_PUBLIC */
    writeUint16ToBuffer(commandBuf, 37, TCM_ALG_SHA256); /* TCMI_ALG_HASH */

    writeUint32ToBuffer(commandBuf, 39, 0x00060072); /* TCMA_OBJECT */

    writeUint16ToBuffer(commandBuf, 43, 0); /* TCM2B_DIGEST */

    writeUint16ToBuffer(commandBuf, 45, TCM_ALG_NULL); /* TCMT_SYM_DEF_OBJECT+ */
    writeUint16ToBuffer(commandBuf, 47, TCM_ALG_NULL); /* TCMT_RSA_SCHEME+ */
    writeUint16ToBuffer(commandBuf, 49, 2048);         /* TCMI_RSA_KEY_BITS+ */
    writeUint32ToBuffer(commandBuf, 51, 0);            /* public exponent */

    writeUint16ToBuffer(commandBuf, 55, TCM_ALG_KDF1_SP800_56A);

    /* 固定的 public digest（此处示例为 SHA256 空哈希） */
    {
        const uint8_t rsaPublic[] = "\xe3\xb0\xc4\x42\x98\xfc\x1c\x14\x9a\xfb\xf4\xc8\x99\x6f\xb9\x24\x27\xae\x41\xe4\x64\x9b\x93\x4c\xa4\x95\x99\x1b\x78\x52\xb8\x55";
        for (uint8_t i = 0, j = 57; i < 32; i++, j++)
        {
            commandBuf[j] = rsaPublic[i];
        }
    }

    writeUint16ToBuffer(commandBuf, 89, 0);
    writeUint32ToBuffer(commandBuf, 91, 0);

    respSize = sendCommand(commandBuf, 95, respBuf, 100);

    if (respSize < 14)
    {
        return 0;
    }

    return readUint32FromBuffer(respBuf, 10);
}

int lp_tcm_rsaencrypt(uint32_t key_hanle, uint8_t *data, uint16_t dataSize, uint8_t *respBuf)
{
    //"\x00\x05\x11\x22\x33\x44\x55\x00\x10\x00\x00";
    if (data == NULL || respBuf == NULL)
    {
        return -1;
    }

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 14 + 2 + dataSize + 2 + 2);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_RSA_Encrypt);
    writeUint32ToBuffer(commandBuf, 10, key_hanle);

    writeUint16ToBuffer(commandBuf, 14, dataSize); /* data size */
    for (uint16_t i = 16, j = 0; j < dataSize; i++, j++)
    {
        commandBuf[i] = data[j];
    }

    writeUint16ToBuffer(commandBuf, 16 + dataSize, TCM_ALG_NULL);
    writeUint16ToBuffer(commandBuf, 16 + dataSize + 2, 0);

    respSize = sendCommand(commandBuf, 14 + 2 + dataSize + 2 + 2, respBuf, 0);

    if (respSize < 12)
    {
        return -1;
    }

    uint16_t encrypt_size = readUint16FromBuffer(respBuf, 10);
    if (encrypt_size > (uint16_t)(respSize - 12))
    {
        return -1;
    }

    for (uint16_t i = 0, j = 12; i < encrypt_size; i++, j++)
    {
        respBuf[i] = respBuf[j];
    }

    return encrypt_size;
}

int lp_tcm_rsadecrypt(uint32_t key_hanle, uint8_t *encryptData, uint16_t dataSize, uint8_t *respBuf)
{
    if (encryptData == NULL || respBuf == NULL)
    {
        return -1;
    }

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, 14 + 13 + 2 + dataSize + 2 + 2);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_RSA_Decrypt);
    writeUint32ToBuffer(commandBuf, 10, key_hanle);

    writeUint32ToBuffer(commandBuf, 14, 9); /* authorizationSize */
    writeUint32ToBuffer(commandBuf, 18, 0x40000009);
    commandBuf[22] = 0x00;
    writeUint32ToBuffer(commandBuf, 23, 0);

    writeUint16ToBuffer(commandBuf, 27, dataSize); /* data size */
    for (uint16_t i = 29, j = 0; j < dataSize; i++, j++)
    {
        commandBuf[i] = encryptData[j];
    }

    writeUint16ToBuffer(commandBuf, 29 + dataSize, TCM_ALG_NULL);
    writeUint16ToBuffer(commandBuf, 29 + dataSize + 2, 0);

    respSize = sendCommand(commandBuf, 14 + 13 + 2 + dataSize + 2 + 2, respBuf, 20);

    if (respSize < 12)
    {
        return -1;
    }

    uint16_t decrypt_size = readUint16FromBuffer(respBuf, 10);
    if (decrypt_size > (uint16_t)(respSize - 12))
    {
        return -1;
    }

    for (uint16_t i = 0, j = 12; i < decrypt_size; i++, j++)
    {
        respBuf[i] = respBuf[j];
    }

    return decrypt_size;
}

/* SHA256 哈希接口 */
int lp_tcm_hash_sha256(uint8_t *data, uint16_t dataSize, uint8_t *respBuf)
{
    if (data == NULL || respBuf == NULL)
    {
        return -1;
    }

    uint16_t commandSize = 2 + 4 + 4 + 2 + dataSize + 2 + 4; /* 计算命令总长度 */

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, commandSize);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_Hash);
    writeUint16ToBuffer(commandBuf, 10, dataSize);

    for (uint16_t i = 12, j = 0; j < dataSize; i++, j++)
    {
        commandBuf[i] = data[j];
    }

    writeUint16ToBuffer(commandBuf, 12 + dataSize, TCM_ALG_SHA256);
    writeUint32ToBuffer(commandBuf, 12 + dataSize + 2, TCM_RH_NULL);

    respSize = sendCommand(commandBuf, commandSize, respBuf, 0);

    return respSize;
}

int lp_tcm_hash_sm3(uint8_t *data, uint16_t dataSize, uint8_t *respBuf)
{
    if (data == NULL || respBuf == NULL)
    {
        return -1;
    }

    uint16_t commandSize = 2 + 4 + 4 + 2 + dataSize + 2 + 4; /* 计算命令总长度 */

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS);
    writeUint32ToBuffer(commandBuf, 2, commandSize);
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_Hash);
    writeUint16ToBuffer(commandBuf, 10, dataSize);

    for (uint16_t i = 12, j = 0; j < dataSize; i++, j++)
    {
        commandBuf[i] = data[j];
    }

    writeUint16ToBuffer(commandBuf, 12 + dataSize, TCM_ALG_SM3_256);
    writeUint32ToBuffer(commandBuf, 12 + dataSize + 2, TCM_RH_NULL);

    respSize = sendCommand(commandBuf, commandSize, respBuf, 0);

    return respSize;
}

int lp_tcm_pcrread(uint8_t pcrIndex, uint8_t *respBuf)
{
    if (respBuf == NULL)
    {
        return -1;
    }
    if (pcrIndex >= 24)
    {
        return -1;
    }

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_NO_SESSIONS); /* 命令头 */
    writeUint32ToBuffer(commandBuf, 2, 20);                 /* 命令总长度 */
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_PCR_Read);    /* 命令码 */
    writeUint32ToBuffer(commandBuf, 10, (uint32_t)1);       /* 选择 PCR 的个数 */

    writeUint16ToBuffer(commandBuf, 14, TCM_ALG_SHA256); /* PCR 所属算法 */
    commandBuf[16] = TCM_PCR_SELECT_MAX;
    commandBuf[17] = 0x00;
    commandBuf[18] = 0x00;
    commandBuf[19] = 0x00;

    if (pcrIndex < 8)
    {
        commandBuf[17] = (uint8_t)(1 << (pcrIndex % 8));
    }
    else if (pcrIndex < 16)
    {
        commandBuf[18] = (uint8_t)(1 << (pcrIndex % 8));
    }
    else
    {
        commandBuf[19] = (uint8_t)(1 << (pcrIndex % 8));
    }

    respSize = sendCommand(commandBuf, 20, respBuf, 0);

    return respSize;
}

int lp_tcm_pcrextend(uint8_t pcrIndex, uint8_t *extendData, uint8_t extendDataSize, uint8_t *respBuf)
{
    if (extendData == NULL || respBuf == NULL)
    {
        return -1;
    }
    if (pcrIndex >= 24 || extendDataSize != 32)
    {
        return -1;
    }

    uint16_t commandSize = (uint16_t)(33 + extendDataSize);

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_SESSIONS);   /* 命令头 */
    writeUint32ToBuffer(commandBuf, 2, commandSize);       /* 命令总长度 */
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_PCR_Extend); /* 命令码 */

    writeUint32ToBuffer(commandBuf, 10, pcrIndex);   /* pcrHandle */
    writeUint32ToBuffer(commandBuf, 14, 9);          /* authorizationSize */
    writeUint32ToBuffer(commandBuf, 18, 0x40000009); /* TCM_RS_PW */
    commandBuf[22] = 0x00;
    writeUint32ToBuffer(commandBuf, 23, 0);              /* nonceSize=0, authSize=0 */
    writeUint32ToBuffer(commandBuf, 27, 1);              /* digest count */
    writeUint16ToBuffer(commandBuf, 31, TCM_ALG_SHA256); /* PCR 所属算法 */

    for (uint16_t i = 0, j = 33; i < extendDataSize; i++, j++)
    {
        commandBuf[j] = extendData[i];
    }

    respSize = sendCommand(commandBuf, commandSize, respBuf, 0);

    return respSize;
}

int lp_tcm_pcrreset(uint8_t pcrIndex, uint8_t *respBuf)
{
    if (respBuf == NULL)
    {
        return -1;
    }
    if (pcrIndex >= 24)
    {
        return -1;
    }

    writeUint16ToBuffer(commandBuf, 0, TCM_ST_SESSIONS);  /* 命令头 */
    writeUint32ToBuffer(commandBuf, 2, 0x1b);             /* 命令总长度 */
    writeUint32ToBuffer(commandBuf, 6, TCM_CC_PCR_Reset); /* 命令码 */

    writeUint32ToBuffer(commandBuf, 10, pcrIndex);   /* pcrHandle */
    writeUint32ToBuffer(commandBuf, 14, 9);          /* authorizationSize */
    writeUint32ToBuffer(commandBuf, 18, 0x40000009); /* TCM_RS_PW */
    commandBuf[22] = 0x00;
    writeUint32ToBuffer(commandBuf, 23, 0); /* nonceSize=0, authSize=0 */

    respSize = sendCommand(commandBuf, 0x1b, respBuf, 0);

    return respSize;
}
