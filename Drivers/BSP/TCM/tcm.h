#ifndef __TCM_H
#define __TCM_H

#include "./SYSTEM/sys/sys.h"


/* TCM命令宏定义 */
#define TPM2_Startup "\x80\x01\x00\x00\x00\x0C\x00\x00\x01\x44\x00\x00";
//extern uint8_t TPM2_Startup[]="\x80\x01\x00\x00\x00\x0C\x00\x00\x01\x44\x00\x00";


void tcm_init();

#endif