#ifndef __TRUST_INTERFACE_H
#define __TRUST_INTERFACE_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/TCM/tcm.h"

/* PCR 选择结构（与 TCM2.0 规范一致） */
typedef struct TCMS_PCR_SELECTION TCMS_PCR_SELECTION;
struct TCMS_PCR_SELECTION
{
    TCM_ALG_ID hash;                         /* 选择的哈希算法 */
    uint8_t sizeofSelect;                    /* pcrSelect 数组长度（字节） */
    uint8_t pcrSelect[TCM_PCR_SELECT_MAX];   /* PCR 选择位图 */
};

/* PCR 选择列表结构 */
typedef struct TCML_PCR_SELECTION TCML_PCR_SELECTION;
struct TCML_PCR_SELECTION
{
    uint32_t count;                                  /* 选择结构数量，可为 0 */
    TCMS_PCR_SELECTION pcrSelections[TPM2_NUM_PCR_BANKS];
};

/* 高级接口：TCM 基础控制 */
void lp_tcm_startup(uint8_t *respBuf);
int lp_tcm_shutdown(uint8_t *respBuf);
int lp_tcm_selftest(uint8_t *respBuf);
int lp_tcm_clearup(uint8_t *respBuf);
int lp_tcm_getrandom(uint16_t bytesRequested, uint8_t *respBuf);

/* 高级接口：非对称密钥与加解密 */
uint32_t lp_tcm_create_rsa(uint8_t *respBuf);
int lp_tcm_rsaencrypt(uint32_t key_hanle, uint8_t *data, uint16_t dataSize, uint8_t *respBuf);
int lp_tcm_rsadecrypt(uint32_t key_hanle, uint8_t *data, uint16_t dataSize, uint8_t *respBuf);

/* 高级接口：哈希 */
int lp_tcm_hash_sha256(uint8_t *data, uint16_t dataSize, uint8_t *respBuf);
int lp_tcm_hash_sm3(uint8_t *data, uint16_t data_size, uint8_t *respBuf);

/* 高级接口：PCR 操作 */
int lp_tcm_pcrread(uint8_t pcrIndex, uint8_t *respBuf);
/* extendDataSize 当前要求为 32（SHA256 摘要长度） */
int lp_tcm_pcrextend(uint8_t pcrIndex, uint8_t *extendData, uint8_t extendDataSize, uint8_t *respBuf);
int lp_tcm_pcrreset(uint8_t pcrIndex, uint8_t *respBuf);

#endif
