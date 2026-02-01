# TCM SPI Demo (STM32F1)

This project is a STM32F1 + SPI demo for a TCM (Trusted Computing Module). It includes a basic startup flow and a set of high-level interfaces for RSA, hashing, and PCR operations.

## Layout

- `User/`: Application code (`main.c`, `trust_interface.c/.h`)
- `Drivers/`: Low-level drivers and BSP
- `Middlewares/`: Middleware
- `Projects/`: Project files

## Quick Start

1. Open the project with STM32CubeIDE / Keil.
2. Build and flash to the target board.
3. Monitor the UART logs (default 115200).
4. Enable the demo blocks in `User/main.c` to test the high-level APIs.

## Implemented High-Level APIs

> Location: `User/trust_interface.c`, `User/trust_interface.h`

- `lp_tcm_startup`: Start up the TCM (first step in the flow).
- `lp_tcm_shutdown`: Orderly shutdown.
- `lp_tcm_selftest`: Run self-test.
- `lp_tcm_clearup`: Clear operation (platform authorization, empty PW session).
- `lp_tcm_getrandom`: Get random bytes.
- `lp_tcm_create_rsa`: Create an RSA Primary Key and return its handle.
- `lp_tcm_rsaencrypt`: RSA encryption.
- `lp_tcm_rsadecrypt`: RSA decryption.
- `lp_tcm_hash_sha256`: SHA256 hash.
- `lp_tcm_hash_sm3`: SM3 hash.
- `lp_tcm_pcrread`: Read PCR.
- `lp_tcm_pcrextend`: Extend PCR (SHA256 digest, 32-byte input expected).
- `lp_tcm_pcrreset`: Reset PCR.

## Notes

- `User/main.c` keeps a full test template guarded by `#if 0`.
- For more command codes and algorithm IDs, see `Drivers/BSP/TCM/tcm.h`.

## License

This demo is provided for learning and validation. Please ensure it matches your hardware platform and protocol specification before use.
