/**
 ****************************************************************************************************
 * @file        usmart.c
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
 *
 ****************************************************************************************************
 */

#include "./USMART/usmart.h"
#include "./USMART/usmart_str.h"
#include "./USMART/usmart_port.h"


/* 系统命令 */
char *sys_cmd_tab[] =
{
    "?",
    "help",
    "list",
    "id",
    "hex",
    "dec",
    "runtime",
};

/**
 * @brief       处理系统指令
 * @param       str : 字符串指针
 * @retval      0,成功处理;其他,错误代码;
 */
uint8_t usmart_sys_cmd_exe(char *str)
{
    uint8_t i;
    char sfname[MAX_FNAME_LEN];                  /* 存放本地函数名 */
    uint8_t pnum;
    uint8_t rval;
    uint32_t res;
    res = usmart_get_cmdname(str, sfname, &i, MAX_FNAME_LEN);   /* 得到指令及指令长度 */

    if (res)return USMART_FUNCERR;                  /* 错误的指令 */

    str += i;

    for (i = 0; i < sizeof(sys_cmd_tab) / 4; i++)   /* 支持的系统指令 */
    {
        if (usmart_strcmp(sfname, sys_cmd_tab[i]) == 0)break;
    }

    switch (i)
    {
        case 0:
        case 1: /* 帮助指令 */
            USMART_PRINTF("\r\n");
#if USMART_USE_HELP
            USMART_PRINTF("------------------------USMART V3.5------------------------ \r\n");
            USMART_PRINTF("    USMART是由ALIENTEK开发的一个灵巧的串口调试互交组件,通过 \r\n");
            USMART_PRINTF("它,你可以通过串口助手调用程序里面的任何函数,并执行.因此,你可\r\n");
            USMART_PRINTF("以随意更改函数的输入参数(支持数字(10/16进制,支持负数)、字符串\r\n"),
            USMART_PRINTF("、函数入口地址等作为参数),单个函数最多支持10个输入参数,并支持\r\n"),
            USMART_PRINTF("函数返回值显示.支持参数显示进制设置功能,支持进制转换功能.\r\n");
            USMART_PRINTF("技术支持:www.openedv.com\r\n");
            USMART_PRINTF("USMART有7个系统命令(必须小写):\r\n");
            USMART_PRINTF("?:      获取帮助信息\r\n");
            USMART_PRINTF("help:   获取帮助信息\r\n");
            USMART_PRINTF("list:   可用的函数列表\r\n\n");
            USMART_PRINTF("id:     可用函数的ID列表\r\n\n");
            USMART_PRINTF("hex:    参数16进制显示,后跟空格+数字即执行进制转换\r\n\n");
            USMART_PRINTF("dec:    参数10进制显示,后跟空格+数字即执行进制转换\r\n\n");
            USMART_PRINTF("runtime:1,开启函数运行计时;0,关闭函数运行计时;\r\n\n");
            USMART_PRINTF("请按照程序编写格式输入函数名及参数并以回车键结束.\r\n");
            USMART_PRINTF("--------------------------正点原子------------------------- \r\n");
#else
            USMART_PRINTF("指令失效\r\n");
#endif
            break;

        case 2: /* 查询指令 */
            USMART_PRINTF("\r\n");
            USMART_PRINTF("-------------------------函数清单--------------------------- \r\n");

            for (i = 0; i < usmart_dev.fnum; i++)USMART_PRINTF("%s\r\n", usmart_dev.funs[i].name);

            USMART_PRINTF("\r\n");
            break;

        case 3: /* 查询ID */
            USMART_PRINTF("\r\n");
            USMART_PRINTF("-------------------------函数 ID --------------------------- \r\n");

            for (i = 0; i < usmart_dev.fnum; i++)
            {
                usmart_get_fname((char *)usmart_dev.funs[i].name, sfname, &pnum, &rval); /* 得到本地函数名 */
                USMART_PRINTF("%s id is:\r\n0X%08X\r\n", sfname, (unsigned int)usmart_dev.funs[i].func);  /* 显示ID */
            }

            USMART_PRINTF("\r\n");
            break;

        case 4: /* hex指令 */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0) /* 参数正常 */
            {
                i = usmart_str2num(sfname, &res);       /* 记录该参数 */

                if (i == 0) /* 进制转换功能 */
                {
                    USMART_PRINTF("HEX:0X%X\r\n", res); /* 转为16进制 */
                }
                else if (i != 4)return USMART_PARMERR;  /* 参数错误. */
                else        /* 参数显示设定功能 */
                {
                    USMART_PRINTF("16进制参数显示!\r\n");
                    usmart_dev.sptype = SP_TYPE_HEX;
                }

            }
            else return USMART_PARMERR; /* 参数错误. */

            USMART_PRINTF("\r\n");
            break;

        case 5: /* dec指令 */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0)     /* 参数正常 */
            {
                i = usmart_str2num(sfname, &res);       /* 记录该参数 */

                if (i == 0) /* 进制转换功能 */
                {
                    USMART_PRINTF("DEC:%lu\r\n", (unsigned long)res);  /* 转为10进制 */
                }
                else if (i != 4)
                {
                    return USMART_PARMERR;  /* 参数错误. */
                }
                else        /* 参数显示设定功能 */
                {
                    USMART_PRINTF("10进制参数显示!\r\n");
                    usmart_dev.sptype = SP_TYPE_DEC;
                }

            }
            else 
            {
                return USMART_PARMERR;  /* 参数错误. */
            }
                
            USMART_PRINTF("\r\n");
            break;

        case 6: /* runtime指令,设置是否显示函数执行时间 */
            USMART_PRINTF("\r\n");
            usmart_get_aparm(str, sfname, &i);

            if (i == 0) /* 参数正常 */
            {
                i = usmart_str2num(sfname, &res);   /* 记录该参数 */

                if (i == 0) /* 读取指定地址数据功能 */
                {
                    if (USMART_ENTIMX_SCAN == 0)
                    {
                        USMART_PRINTF("\r\nError! \r\nTo EN RunTime function,Please set USMART_ENTIMX_SCAN = 1 first!\r\n");/* 报错 */
                    }
                    else
                    {
                        usmart_dev.runtimeflag = res;

                        if (usmart_dev.runtimeflag)
                        {
                            USMART_PRINTF("Run Time Calculation ON\r\n");
                        }
                        else 
                        {
                            USMART_PRINTF("Run Time Calculation OFF\r\n");
                        }
                    }
                }
                else 
                {
                    return USMART_PARMERR;  /* 未带参数,或者参数错误 */
                }
            }
            else 
            {
                return USMART_PARMERR;      /* 参数错误. */
            }
            
            USMART_PRINTF("\r\n");
            break;

        default:/* 非法指令 */
            return USMART_FUNCERR;
    }

    return 0;
}

/**
 * @brief       初始化USMART
 * @param       tclk: 定时器的工作频率(单位:Mhz)
 * @retval      无
 */
void usmart_init(uint16_t tclk)
{
#if USMART_ENTIMX_SCAN == 1
    usmart_timx_init(1000, tclk * 100 - 1);
#endif
    usmart_dev.sptype = 1;  /* 十六进制显示参数 */
}

/**
 * @brief       从str中获取函数名,id,及参数信息
 * @param       str: 字符串指针.
 * @retval      0,识别成功;其他,错误代码.
 */
uint8_t usmart_cmd_rec(char *str)
{
    uint8_t sta, i, rval;   /* 状态 */
    uint8_t rpnum, spnum;
    char rfname[MAX_FNAME_LEN];  /* 暂存空间,用于存放接收到的函数名 */
    char sfname[MAX_FNAME_LEN];  /* 存放本地函数名 */
    sta = usmart_get_fname(str, rfname, &rpnum, &rval); /* 得到接收到的数据的函数名及参数个数 */

    if (sta)return sta; /* 错误 */

    for (i = 0; i < usmart_dev.fnum; i++)
    {
        sta = usmart_get_fname((char *)usmart_dev.funs[i].name, sfname, &spnum, &rval); /* 得到本地函数名及参数个数 */

        if (sta)return sta; /* 本地解析有误 */

        if (usmart_strcmp(sfname, rfname) == 0) /* 相等 */
        {
            if (spnum > rpnum)return USMART_PARMERR;/* 参数错误(输入参数比源函数参数少) */

            usmart_dev.id = i;  /* 记录函数ID. */
            break;  /* 跳出. */
        }
    }

    if (i == usmart_dev.fnum)return USMART_NOFUNCFIND;  /* 未找到匹配的函数 */

    sta = usmart_get_fparam(str, &i);   /* 得到函数参数个数 */

    if (sta)return sta;     /* 返回错误 */

    usmart_dev.pnum = i;    /* 参数个数记录 */
    return USMART_OK;
}

/**
 * @brief       USMART执行函数
 *   @note
 *              该函数用于最终执行从串口收到的有效函数.
 *              最多支持10个参数的函数,更多的参数支持也很容易实现.不过用的很少.一般5个左右的参数的函数已经很少见了.
 *              该函数会在串口打印执行情况.以:"函数名(参数1，参数2...参数N)=返回值".的形式打印.
 *              当所执行的函数没有返回值的时候,所打印的返回值是一个无意义的数据.
 *
 * @param       无
 * @retval      无
 */
void usmart_exe(void)
{
    uint8_t id, i;
    uint32_t res;
    uint32_t temp[MAX_PARM];        /* 参数转换,使之支持了字符串 */
    char sfname[MAX_FNAME_LEN];  /* 存放本地函数名 */
    uint8_t pnum, rval;
    id = usmart_dev.id;

    if (id >= usmart_dev.fnum)return;   /* 不执行. */

    usmart_get_fname((char *)usmart_dev.funs[id].name, sfname, &pnum, &rval);    /* 得到本地函数名,及参数个数 */
    USMART_PRINTF("\r\n%s(", sfname);   /* 输出正要执行的函数名 */

    for (i = 0; i < pnum; i++)      /* 输出参数 */
    {
        if (usmart_dev.parmtype & (1 << i)) /* 参数是字符串 */
        {
            USMART_PRINTF("%c", '"');
            USMART_PRINTF("%s", usmart_dev.parm + usmart_get_parmpos(i));
            USMART_PRINTF("%c", '"');
            temp[i] = (uint32_t) & (usmart_dev.parm[usmart_get_parmpos(i)]);
        }
        else    /* 参数是数字 */
        {
            temp[i] = *(uint32_t *)(usmart_dev.parm + usmart_get_parmpos(i));

            if (usmart_dev.sptype == SP_TYPE_DEC)
            {
                USMART_PRINTF("%ld", (long)temp[i]);  /* 10进制参数显示 */
            }
            else 
            {
                USMART_PRINTF("0X%X", temp[i]); /* 16进制参数显示 */
            }
        }

        if (i != pnum - 1)USMART_PRINTF(",");
    }

    USMART_PRINTF(")");
#if USMART_ENTIMX_SCAN==1
    usmart_timx_reset_time();   /* 计时器清零,开始计时 */
#endif

    switch (usmart_dev.pnum)
    {
        case 0: /* 无参数(void类型) */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)();
            break;

        case 1: /* 有1个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0]);
            break;

        case 2: /* 有2个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1]);
            break;

        case 3: /* 有3个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2]);
            break;

        case 4: /* 有4个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3]);
            break;

        case 5: /* 有5个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4]);
            break;

        case 6: /* 有6个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5]);
            break;

        case 7: /* 有7个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6]);
            break;

        case 8: /* 有8个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7]);
            break;

        case 9: /* 有9个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7], temp[8]);
            break;

        case 10:/* 有10个参数 */
            res = (*(uint32_t(*)())usmart_dev.funs[id].func)(temp[0], temp[1], temp[2], temp[3], temp[4], \
                    temp[5], temp[6], temp[7], temp[8], temp[9]);
            break;
    }

#if USMART_ENTIMX_SCAN==1
    usmart_timx_get_time(); /* 获取函数执行时间 */
#endif

    if (rval == 1)  /* 需要返回值. */
    {
        if (usmart_dev.sptype == SP_TYPE_DEC)USMART_PRINTF("=%lu;\r\n", (unsigned long)res);   /* 输出执行结果(10进制参数显示) */
        else USMART_PRINTF("=0X%X;\r\n", res);  /* 输出执行结果(16进制参数显示) */
    }
    else USMART_PRINTF(";\r\n");    /* 不需要返回值,直接输出结束 */

    if (usmart_dev.runtimeflag)     /* 需要显示函数执行时间 */
    {
        USMART_PRINTF("Function Run Time:%d.%1dms\r\n", usmart_dev.runtime / 10, usmart_dev.runtime % 10);  /* 打印函数执行时间 */
    }
}

/**
 * @brief       USMART扫描函数
 *   @note
 *              通过调用该函数,实现USMART的各个控制.该函数需要每隔一定时间被调用一次
 *              以及时执行从串口发过来的各个函数.
 *              本函数可以在中断里面调用,从而实现自动管理.
 *              如果非正点原子用户,则USART_RX_STA和USART_RX_BUF[]需要用户自己实现
 *
 * @param       无
 * @retval      无
 */
void usmart_scan(void)
{
    uint8_t sta, len;
    char *pbuf = 0;

    pbuf = usmart_get_input_string();   /* 获取数据数据流 */
    if (pbuf == 0) return ; /* 数据流空, 直接返回 */
     
    sta = usmart_dev.cmd_rec(pbuf);     /* 得到函数各个信息 */

    if (sta == 0)
    {
        usmart_dev.exe();  /* 执行函数 */
    }
    else
    {
        len = usmart_sys_cmd_exe(pbuf);

        if (len != USMART_FUNCERR)sta = len;

        if (sta)
        {
            switch (sta)
            {
                case USMART_FUNCERR:
                    USMART_PRINTF("函数错误!\r\n");
                    break;

                case USMART_PARMERR:
                    USMART_PRINTF("参数错误!\r\n");
                    break;

                case USMART_PARMOVER:
                    USMART_PRINTF("参数太多!\r\n");
                    break;

                case USMART_NOFUNCFIND:
                    USMART_PRINTF("未找到匹配的函数!\r\n");
                    break;
            }
        }
    } 
 
}

#if USMART_USE_WRFUNS == 1  /* 如果使能了读写操作 */

/**
 * @brief       读取指定地址的值 
 * @param       无
 * @retval      无
 */ 
uint32_t read_addr(uint32_t addr)
{
    return *(uint32_t *)addr;
}

/**
 * @brief       在指定地址写入指定的值
 * @param       无
 * @retval      无
 */ 
void write_addr(uint32_t addr, uint32_t val)
{
    *(uint32_t *)addr = val;
}

#endif





















