#ifndef __TRUST_INTERFACE_H
#define __TRUST_INTERFACE_H



int lp_tcm_startup(uint8_t *respBuf);
int lp_tcm_hash_sm3(uint8_t *data, uint16_t data_size, uint8_t *respBuf);
#endif