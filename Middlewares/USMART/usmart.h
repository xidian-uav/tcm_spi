/**
 ****************************************************************************************************
 * @file        usmart.h
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
 *
 * V3.5 20201220
 * 1，修改部分代码以支持AC6编译器
 ****************************************************************************************************
 */

#ifndef __USMART_H
#define __USMART_H

#include "./USMART/usmart_port.h"


#define USMART_OK               0       /* 无错误 */
#define USMART_FUNCERR          1       /* 函数错误 */
#define USMART_PARMERR          2       /* 参数错误 */
#define USMART_PARMOVER         3       /* 参数溢出 */
#define USMART_NOFUNCFIND       4       /* 未找到匹配函数 */

#define SP_TYPE_DEC             0       /* 10进制参数显示 */
#define SP_TYPE_HEX             1       /* 16进制参数显示 */


/* 函数名列表 */
struct _m_usmart_nametab
{
    void *func;             /* 函数指针 */
    const char *name;       /* 函数名(查找串) */
};

/* usmart控制管理器 */
struct _m_usmart_dev
{
    struct _m_usmart_nametab *funs;     /* 函数名指针 */

    void (*init)(uint16_t tclk);        /* 初始化 */
    uint8_t (*cmd_rec)(char *str);   /* 识别函数名及参数 */
    void (*exe)(void);                  /* 执行  */
    void (*scan)(void);                 /* 扫描 */
    uint8_t fnum;                       /* 函数数量 */
    uint8_t pnum;                       /* 参数数量 */
    uint8_t id;                         /* 函数id */
    uint8_t sptype;                     /* 参数显示类型(非字符串参数):0,10进制;1,16进制; */
    uint16_t parmtype;                  /* 参数的类型 */
    uint8_t  plentbl[MAX_PARM];         /* 每个参数的长度暂存表 */
    uint8_t  parm[PARM_LEN];            /* 函数的参数 */
    uint8_t runtimeflag;                /* 0,不统计函数执行时间;1,统计函数执行时间,注意:此功能必须在USMART_ENTIMX_SCAN使能的时候,才有用 */
    uint32_t runtime;                   /* 运行时间,单位:0.1ms,最大延时时间为定时器CNT值的2倍*0.1ms */
};

extern struct _m_usmart_nametab usmart_nametab[];   /* 在usmart_config.c里面定义 */
extern struct _m_usmart_dev usmart_dev;             /* 在usmart_config.c里面定义 */


void usmart_init(uint16_t tclk);        /* 初始化 */
uint8_t usmart_cmd_rec(char*str);    	/* 识别 */
void usmart_exe(void);                  /* 执行 */
void usmart_scan(void);                 /* 扫描 */
uint32_t read_addr(uint32_t addr);      /* 读取指定地址的值 */
void write_addr(uint32_t addr,uint32_t val);/* 在指定地址写入指定的值 */

#endif






























