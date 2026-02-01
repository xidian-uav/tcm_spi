# TCM SPI Demo (STM32F1)

本项目基于 STM32F1 + SPI 接口对接 TCM（可信计算模块）示例工程，包含基础启动流程与一组高级接口封装，可快速验证 TCM 的加解密、哈希和 PCR 操作。

## 目录结构

- `User/`：应用层代码（`main.c`、`trust_interface.c/.h`）
- `Drivers/`：底层驱动与 BSP
- `Middlewares/`：中间件
- `Projects/`：工程文件

## 快速开始

1. 使用 STM32CubeIDE / Keil 打开对应工程。
2. 编译并下载到目标板。
3. 通过串口观察运行日志（默认 115200）。
4. 在 `User/main.c` 中启用示例代码块以验证高级接口。

## 已实现的高级接口

> 位置：`User/trust_interface.c`、`User/trust_interface.h`

- `lp_tcm_startup`：启动 TCM（启动流程的第一步）。
- `lp_tcm_shutdown`：有序关机，保存必要状态。
- `lp_tcm_selftest`：执行自检并返回结果。
- `lp_tcm_clearup`：执行 Clear 操作（平台授权，PW 空口令）。
- `lp_tcm_getrandom`：获取随机数。
- `lp_tcm_create_rsa`：创建 RSA Primary Key，返回 key handle。
- `lp_tcm_rsaencrypt`：RSA 加密。
- `lp_tcm_rsadecrypt`：RSA 解密。
- `lp_tcm_hash_sha256`：SHA256 哈希。
- `lp_tcm_hash_sm3`：SM3 哈希。
- `lp_tcm_pcrread`：读取 PCR。
- `lp_tcm_pcrextend`：扩展 PCR（当前示例为 SHA256，要求 32 字节数据）。
- `lp_tcm_pcrreset`：重置 PCR。

## 说明

- `User/main.c` 中保留了完整的测试用例模板（使用 `#if 0` 控制开关）。
- 如需扩展命令，请参考 `Drivers/BSP/TCM/tcm.h` 中的命令码与算法常量。

## License

该示例工程供学习与验证用途，使用前请确认与硬件平台/协议规范一致。
