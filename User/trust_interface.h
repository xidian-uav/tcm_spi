#ifndef __TRUST_INTERFACE_H
#define __TRUST_INTERFACE_H

#include "./BSP/TCM/tcm.h"

/* Definition of TCMB_MAX_BUFFER Structure */
typedef struct TCMB_MAX_BUFFER TCMB_MAX_BUFFER;
struct TCMB_MAX_BUFFER {
    uint16_t size;
    uint8_t buffer[TCM_MAX_DIGEST_BUFFER];
};

/* 定义哈希哈希命令结构体 */
typedef struct
{
    TCM_ST tag;
    uint32_t commandSize;
    TCM_CC commandCode;
    TCMB_MAX_BUFFER data;
    TCM_ALG_ID hashAlg;
    TCM_RH hierarchy;
}TCM_HASH;

int lp_tcm_hash_sm3(uint8_t *data, uint16_t data_size, uint8_t *respBuf);
#endif