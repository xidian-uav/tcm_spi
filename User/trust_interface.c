#include "./trust_interface.h"
#include "./SYSTEM/usart/usart.h"

uint16_t respSize = 0;
uint8_t *commandBuf;
uint16_t i;

// ���ת������
uint16_t uint16Convert(uint16_t host_value) {
    return ((host_value << 8) & 0xFF00) | ((host_value >> 8) & 0x00FF);
}

uint32_t uint32Convert(uint32_t host_value) {
    return ((host_value << 24) & 0xFF000000) |
           ((host_value << 8) & 0x00FF0000) |
           ((host_value >> 8) & 0x0000FF00) |
           ((host_value >> 24) & 0x000000FF);
}


int lp_tcm_hash_sm3(uint8_t *data, uint16_t dataSize, uint8_t *respBuf)
{
    for (i = 0; i < 2; i++)
    {
        
    }
    
    TCM_HASH tcm_hash;
    tcm_hash.tag = uint16Convert(TCM_ST_NO_SESSIONS);
    tcm_hash.commandSize = 2 + 4 + 4 + dataSize + 2 + 4;
    tcm_hash.commandCode = uint32Convert(TCM_CC_Hash);
    tcm_hash.data.size = uint16Convert(dataSize);
    
    
    for(i = 0; i < dataSize; i++)
    {
        tcm_hash.data.buffer[i] = data[i];
    }
    
    tcm_hash.hashAlg = uint16Convert(TCM_ALG_SHA256);
    tcm_hash.hierarchy = uint32Convert(TCM_RH_OWNER);
    
    
    uint8_t *p = (uint8_t *)&tcm_hash;
    
    for(i=0; i<tcm_hash.commandSize; i++, p++)
    {
        printf("%02x", *p);
    }
    
    respSize = sendCommand((uint8_t *)&tcm_hash, tcm_hash.commandSize, respBuf);
    
    if(respSize > 0x0a)
    {
        return respSize;
    }
    else
    {
        return 0;
    }        
}