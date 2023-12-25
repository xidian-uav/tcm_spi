/**
 ****************************************************************************************************
 * @file        usmart_str.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V3.5
 * @date        2020-12-20
 * @brief       USMART 串口调试组件
 *
 *              USMART是由ALIENTEK开发的一个灵巧的串口调试互交组件,通过 它,你可以通过串口助手调用程
 *              序里面的任何函数,并执行.因此,你可以随意更改函数的输入参数(支持数字(10/16进制,支持负数)
 *              、字符串、函数入口地址等作为参数),单个函数最多支持10个输入参数,并支持函数返 回值显示.
 *              V2.1版本以后新增hex和dec两个指令.他们可以用于设置函数参数的显示格式.也可以用于数据的进
 *              制转换,例如:
 *              输入"hex 100"  会在串口调试助手上看到 HEX 0X64.
 *              输入"dec 0X64" 会在串口调试助手上看到 DEC 100.
 *   @note
 *              USMART资源占用情况@MDK 3.80A@2.0版本：
 *              FLASH:4K~K字节(通过USMART_USE_HELP和USMART_USE_WRFUNS设置)
 *              SRAM:72字节(最少的情况下)
 *              SRAM计算公式:   SRAM=PARM_LEN+72-4  其中PARM_LEN必须大于等于4.
 *              应该保证堆栈不小于100个字节.
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明 
 * 
 * V3.4之前版本详细修改说明见USMART文件夹下的:readme.txt
 * 
 * V3.4 20200324
 * 1, 新增usmart_port.c和usmart_port.h,用于管理USMART的移植,方便修改
 * 2, 修改变量命名方式为: uint8_t, uint16_t, uint32_t
 * 3, 修改usmart_reset_runtime为usmart_timx_reset_time
 * 4, 修改usmart_get_runtime为usmart_timx_get_time
 * 5, 修改usmart_scan函数实现方式,改成由usmart_get_input_string获取数据流
 * 6, 修改printf函数为USMART_PRINTF宏定义
 * 7, 修改定时扫描相关函数,改用宏定义方式,方便移植
 * V3.5 20201220
 * 1，修改部分代码以支持AC6编译器
 *
 ****************************************************************************************************
 */

#ifndef __USMART_STR_H
#define __USMART_STR_H

#include "./USMART/usmart_port.h"


uint8_t usmart_get_parmpos(uint8_t num);                /* 得到某个参数在参数列里面的起始位置 */
uint8_t usmart_strcmp(char *str1, char *str2);    /* 对比两个字符串是否相等 */
uint32_t usmart_pow(uint8_t m, uint8_t n);              /* M^N次方 */
uint8_t usmart_str2num(char *str, uint32_t *res);    /* 字符串转为数字 */
uint8_t usmart_get_cmdname(char *str, char *cmdname, uint8_t *nlen, uint8_t maxlen); /* 从str中得到指令名,并返回指令长度 */
uint8_t usmart_get_fname(char *str, char *fname, uint8_t *pnum, uint8_t *rval); /* 从str中得到函数名 */
uint8_t usmart_get_aparm(char *str, char *fparm, uint8_t *ptype); /* 从str中得到一个函数参数 */
uint8_t usmart_get_fparam(char *str, uint8_t *parn); /* 得到str中所有的函数参数. */

#endif











