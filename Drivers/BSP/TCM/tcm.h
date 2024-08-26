#ifndef __TCM_H
#define __TCM_H

#include "./SYSTEM/sys/sys.h"

extern SPI_HandleTypeDef tcmHSpi; /* TCMSPI句柄 */

#define TCM_SPI                             SPI1
#define TCM_SPI_BUFF_SIZE                   512

/* TCM命令宏定义 */

/* From TPM 2.0 Part 2: Definition of TCM_CC Constants */
typedef uint32_t                               TCM_CC;
#define TCM_CC_NV_UndefineSpaceSpecial       ((TCM_CC) 0x0000011f)
#define TCM_CC_FIRST TCM_CC_NV_UndefineSpaceSpecial
#define TCM_CC_EvictControl                  ((TCM_CC) 0x00000120)
#define TCM_CC_HierarchyControl              ((TCM_CC) 0x00000121)
#define TCM_CC_NV_UndefineSpace              ((TCM_CC) 0x00000122)
#define TCM_CC_ChangeEPS                     ((TCM_CC) 0x00000124)
#define TCM_CC_ChangePPS                     ((TCM_CC) 0x00000125)
#define TCM_CC_Clear                         ((TCM_CC) 0x00000126)
#define TCM_CC_ClearControl                  ((TCM_CC) 0x00000127)
#define TCM_CC_ClockSet                      ((TCM_CC) 0x00000128)
#define TCM_CC_HierarchyChangeAuth           ((TCM_CC) 0x00000129)
#define TCM_CC_NV_DefineSpace                ((TCM_CC) 0x0000012a)
#define TCM_CC_PCR_Allocate                  ((TCM_CC) 0x0000012b)
#define TCM_CC_PCR_SetAuthPolicy             ((TCM_CC) 0x0000012c)
#define TCM_CC_PP_Commands                   ((TCM_CC) 0x0000012d)
#define TCM_CC_SetPrimaryPolicy              ((TCM_CC) 0x0000012e)
#define TCM_CC_FieldUpgradeStart             ((TCM_CC) 0x0000012f)
#define TCM_CC_ClockRateAdjust               ((TCM_CC) 0x00000130)
#define TCM_CC_CreatePrimary                 ((TCM_CC) 0x00000131)
#define TCM_CC_NV_GlobalWriteLock            ((TCM_CC) 0x00000132)
#define TCM_CC_GetCommandAuditDigest         ((TCM_CC) 0x00000133)
#define TCM_CC_NV_Increment                  ((TCM_CC) 0x00000134)
#define TCM_CC_NV_SetBits                    ((TCM_CC) 0x00000135)
#define TCM_CC_NV_Extend                     ((TCM_CC) 0x00000136)
#define TCM_CC_NV_Write                      ((TCM_CC) 0x00000137)
#define TCM_CC_NV_WriteLock                  ((TCM_CC) 0x00000138)
#define TCM_CC_DictionaryAttackLockReset     ((TCM_CC) 0x00000139)
#define TCM_CC_DictionaryAttackParameters    ((TCM_CC) 0x0000013a)
#define TCM_CC_NV_ChangeAuth                 ((TCM_CC) 0x0000013b)
#define TCM_CC_PCR_Event                     ((TCM_CC) 0x0000013c)
#define TCM_CC_PCR_Reset                     ((TCM_CC) 0x0000013d)
#define TCM_CC_SequenceComplete              ((TCM_CC) 0x0000013e)
#define TCM_CC_SetAlgorithmSet               ((TCM_CC) 0x0000013f)
#define TCM_CC_SetCommandCodeAuditStatus     ((TCM_CC) 0x00000140)
#define TCM_CC_FieldUpgradeData              ((TCM_CC) 0x00000141)
#define TCM_CC_IncrementalSelfTest           ((TCM_CC) 0x00000142)
#define TCM_CC_SelfTest                      ((TCM_CC) 0x00000143)
#define TCM_CC_Startup                       ((TCM_CC) 0x00000144)
#define TCM_CC_Shutdown                      ((TCM_CC) 0x00000145)
#define TCM_CC_StirRandom                    ((TCM_CC) 0x00000146)
#define TCM_CC_ActivateCredential            ((TCM_CC) 0x00000147)
#define TCM_CC_Certify                       ((TCM_CC) 0x00000148)
#define TCM_CC_PolicyNV                      ((TCM_CC) 0x00000149)
#define TCM_CC_CertifyCreation               ((TCM_CC) 0x0000014a)
#define TCM_CC_Duplicate                     ((TCM_CC) 0x0000014b)
#define TCM_CC_GetTime                       ((TCM_CC) 0x0000014c)
#define TCM_CC_GetSessionAuditDigest         ((TCM_CC) 0x0000014d)
#define TCM_CC_NV_Read                       ((TCM_CC) 0x0000014e)
#define TCM_CC_NV_ReadLock                   ((TCM_CC) 0x0000014f)
#define TCM_CC_ObjectChangeAuth              ((TCM_CC) 0x00000150)
#define TCM_CC_PolicySecret                  ((TCM_CC) 0x00000151)
#define TCM_CC_Rewrap                        ((TCM_CC) 0x00000152)
#define TCM_CC_Create                        ((TCM_CC) 0x00000153)
#define TCM_CC_ECDH_ZGen                     ((TCM_CC) 0x00000154)
#define TCM_CC_HMAC                          ((TCM_CC) 0x00000155)
#define TCM_CC_MAC                           ((TCM_CC) 0x00000155)
#define TCM_CC_Import                        ((TCM_CC) 0x00000156)
#define TCM_CC_Load                          ((TCM_CC) 0x00000157)
#define TCM_CC_Quote                         ((TCM_CC) 0x00000158)
#define TCM_CC_RSA_Decrypt                   ((TCM_CC) 0x00000159)
#define TCM_CC_HMAC_Start                    ((TCM_CC) 0x0000015b)
#define TCM_CC_MAC_Start                     ((TCM_CC) 0x0000015b)
#define TCM_CC_SequenceUpdate                ((TCM_CC) 0x0000015c)
#define TCM_CC_Sign                          ((TCM_CC) 0x0000015d)
#define TCM_CC_Unseal                        ((TCM_CC) 0x0000015e)
#define TCM_CC_PolicySigned                  ((TCM_CC) 0x00000160)
#define TCM_CC_ContextLoad                   ((TCM_CC) 0x00000161)
#define TCM_CC_ContextSave                   ((TCM_CC) 0x00000162)
#define TCM_CC_ECDH_KeyGen                   ((TCM_CC) 0x00000163)
#define TCM_CC_EncryptDecrypt                ((TCM_CC) 0x00000164)
#define TCM_CC_FlushContext                  ((TCM_CC) 0x00000165)
#define TCM_CC_LoadExternal                  ((TCM_CC) 0x00000167)
#define TCM_CC_MakeCredential                ((TCM_CC) 0x00000168)
#define TCM_CC_NV_ReadPublic                 ((TCM_CC) 0x00000169)
#define TCM_CC_PolicyAuthorize               ((TCM_CC) 0x0000016a)
#define TCM_CC_PolicyAuthValue               ((TCM_CC) 0x0000016b)
#define TCM_CC_PolicyCommandCode             ((TCM_CC) 0x0000016c)
#define TCM_CC_PolicyCounterTimer            ((TCM_CC) 0x0000016d)
#define TCM_CC_PolicyCpHash                  ((TCM_CC) 0x0000016e)
#define TCM_CC_PolicyLocality                ((TCM_CC) 0x0000016f)
#define TCM_CC_PolicyNameHash                ((TCM_CC) 0x00000170)
#define TCM_CC_PolicyOR                      ((TCM_CC) 0x00000171)
#define TCM_CC_PolicyTicket                  ((TCM_CC) 0x00000172)
#define TCM_CC_ReadPublic                    ((TCM_CC) 0x00000173)
#define TCM_CC_RSA_Encrypt                   ((TCM_CC) 0x00000174)
#define TCM_CC_StartAuthSession              ((TCM_CC) 0x00000176)
#define TCM_CC_VerifySignature               ((TCM_CC) 0x00000177)
#define TCM_CC_ECC_Parameters                ((TCM_CC) 0x00000178)
#define TCM_CC_FirmwareRead                  ((TCM_CC) 0x00000179)
#define TCM_CC_GetCapability                 ((TCM_CC) 0x0000017a)
#define TCM_CC_GetRandom                     ((TCM_CC) 0x0000017b)
#define TCM_CC_GetTestResult                 ((TCM_CC) 0x0000017c)
#define TCM_CC_Hash                          ((TCM_CC) 0x0000017d)
#define TCM_CC_PCR_Read                      ((TCM_CC) 0x0000017e)
#define TCM_CC_PolicyPCR                     ((TCM_CC) 0x0000017f)
#define TCM_CC_PolicyRestart                 ((TCM_CC) 0x00000180)
#define TCM_CC_ReadClock                     ((TCM_CC) 0x00000181)
#define TCM_CC_PCR_Extend                    ((TCM_CC) 0x00000182)
#define TCM_CC_PCR_SetAuthValue              ((TCM_CC) 0x00000183)
#define TCM_CC_NV_Certify                    ((TCM_CC) 0x00000184)
#define TCM_CC_EventSequenceComplete         ((TCM_CC) 0x00000185)
#define TCM_CC_HashSequenceStart             ((TCM_CC) 0x00000186)
#define TCM_CC_PolicyPhysicalPresence        ((TCM_CC) 0x00000187)
#define TCM_CC_PolicyDuplicationSelect       ((TCM_CC) 0x00000188)
#define TCM_CC_PolicyGetDigest               ((TCM_CC) 0x00000189)
#define TCM_CC_TestParms                     ((TCM_CC) 0x0000018a)
#define TCM_CC_Commit                        ((TCM_CC) 0x0000018b)
#define TCM_CC_PolicyPassword                ((TCM_CC) 0x0000018c)
#define TCM_CC_ZGen_2Phase                   ((TCM_CC) 0x0000018d)
#define TCM_CC_EC_Ephemeral                  ((TCM_CC) 0x0000018e)
#define TCM_CC_PolicyNvWritten               ((TCM_CC) 0x0000018f)
#define TCM_CC_PolicyTemplate                ((TCM_CC) 0x00000190)
#define TCM_CC_CreateLoaded                  ((TCM_CC) 0x00000191)
#define TCM_CC_PolicyAuthorizeNV             ((TCM_CC) 0x00000192)
#define TCM_CC_EncryptDecrypt2               ((TCM_CC) 0x00000193)
#define TCM_CC_AC_GetCapability              ((TCM_CC) 0x00000194)
#define TCM_CC_AC_Send                       ((TCM_CC) 0x00000195)
#define TCM_CC_Policy_AC_SendSelect          ((TCM_CC) 0x00000196)
#define TCM_CC_CertifyX509                   ((TCM_CC) 0x00000197)
#define TCM_CC_ACT_SetTimeout                ((TCM_CC) 0x00000198)
#define TCM_CC_LAST                          ((TCM_CC) 0x00000198)
#define TCM_CC_Vendor_TCG_Test               ((TCM_CC) 0x20000000)

/* Definition of UINT16 TCM_ST Constants <INOUT S> */
typedef uint16_t TCM_ST;

#define TCM_ST_RSP_COMMAND          ((TCM_ST) 0x00C4) /* Tag value for a response used when there is an error in the tag. This is also the value returned from a TPM 1.2 when an error occurs. This value is used in this specification because an error in the command tag may prevent determination of the family. When this tag is used in the response the response code will be TCM_RC_BAD_TAG 0 1E16 which has the same numeric value as the TPM 1.2 response code for TPM_BADTAG. NOTE In a previously published version of this specification TCM_RC_BAD_TAG was incorrectly assigned a value of 0x030 instead of 30 0x01e. Some implementations my return the old value instead of the new value. */
#define TCM_ST_NULL                 ((TCM_ST) 0X8000) /* no structure type specified */
#define TCM_ST_NO_SESSIONS          ((TCM_ST) 0x8001) /* tag value for a command response for a command defined in this specification indicating that the command response has no attached sessions and no authorizationSizeparameterSize value is present. If the responseCode from the TPM is not TCM_RC_SUCCESS then the response tag shall have this value. */
#define TCM_ST_SESSIONS             ((TCM_ST) 0x8002) /* tag value for a command response for a command defined in this specification indicating that the command response has one or more attached sessions and the authorizationSizeparameterSize field is present */
#define TCM_ST_RESERVED1            ((TCM_ST) 0x8003) /* When used between application software and the TPM resource manager, this tag indicates that the command has no sessions and the handles are using the Name format rather than the 32-bit handle format. NOTE 1 The response to application software will have a tag of TCM_ST_NO_SESSIONS. Between the TRM and TPM, this tag would occur in a response from a TPM that overlaps the tag parameter of a request with the tag parameter of a response when the response has no associated sessions. NOTE 2 This tag is not used by all TPM or TRM implementations. */
#define TCM_ST_RESERVED2            ((TCM_ST) 0x8004) /* When used between application software and the TPM resource manager. This tag indicates that the command has sessions and the handles are using the Name format rather than the 32-bit handle format. NOTE 1 If the command completes successfully the response to application software will have a tag of TCM_ST_SESSIONS. Between the TRM and TPM would occur in a response from a TPM that overlaps the tag parameter of a request with the tag parameter of a response when the response has authorization sessions. NOTE 2 This tag is not used by all TPM or TRM implementations. */
#define TCM_ST_ATTEST_NV            ((TCM_ST) 0x8014) /* tag for an attestation structure */
#define TCM_ST_ATTEST_COMMAND_AUDIT ((TCM_ST) 0x8015) /* tag for an attestation structure */
#define TCM_ST_ATTEST_SESSION_AUDIT ((TCM_ST) 0x8016) /* tag for an attestation structure */
#define TCM_ST_ATTEST_CERTIFY       ((TCM_ST) 0x8017) /* tag for an attestation structure */
#define TCM_ST_ATTEST_QUOTE         ((TCM_ST) 0x8018) /* tag for an attestation structure */
#define TCM_ST_ATTEST_TIME          ((TCM_ST) 0x8019) /* tag for an attestation structure */
#define TCM_ST_ATTEST_CREATION      ((TCM_ST) 0x801A) /* tag for an attestation structure */
#define TCM_ST_RESERVED3            ((TCM_ST) 0x801B) /* do not use . NOTE This was previously assigned to TCM_ST_ATTEST_NV. The tag is changed because the structure has changed */
#define TCM_ST_CREATION             ((TCM_ST) 0x8021) /* tag for a ticket type */
#define TCM_ST_VERIFIED             ((TCM_ST) 0x8022) /* tag for a ticket type */
#define TCM_ST_AUTH_SECRET          ((TCM_ST) 0x8023) /* tag for a ticket type */
#define TCM_ST_HASHCHECK            ((TCM_ST) 0x8024) /* tag for a ticket type */
#define TCM_ST_AUTH_SIGNED          ((TCM_ST) 0x8025) /* tag for a ticket type */
#define TCM_ST_FU_MANIFEST          ((TCM_ST) 0x8029) /* tag for a structure describing a Field Upgrade Policy */

/* Definition of UINT16 TPM2_SU Constants <IN> */
typedef uint16_t TCM_SU;
#define TCM_SU_CLEAR    ((TCM_SU) 0x0000) /* On TPM2_Shutdown indicates that the TPM should prepare for loss of power and save state required for an orderly startup TPM Reset. On TPM2_Startup indicates that the TPM should perform TPM Reset or TPM Restart */
#define TCM_SU_STATE    ((TCM_SU) 0x0001) /* On TPM2_Shutdown indicates that the TPM should prepare for loss of power and save state required for an orderly startup. TPM Restart or TPM Resume on TPM2_Startup indicates that the TPM should restore the state saved by TPM2_Shutdown TPM2_SU_STATE */


/* From TCG Algorithm Registry: Definition of TCM_ALG_ID Constants */
typedef uint16_t TCM_ALG_ID;

#define TCM_ALG_ERROR               ((TCM_ALG_ID) 0x0000)
#define TCM_ALG_RSA                 ((TCM_ALG_ID) 0x0001)
#define TCM_ALG_TDES                ((TCM_ALG_ID) 0x0003)
#define TCM_ALG_SHA                 ((TCM_ALG_ID) 0x0004)
#define TCM_ALG_SHA1                ((TCM_ALG_ID) 0x0004)
#define TCM_ALG_HMAC                ((TCM_ALG_ID) 0x0005)
#define TCM_ALG_AES                 ((TCM_ALG_ID) 0x0006)
#define TCM_ALG_MGF1                ((TCM_ALG_ID) 0x0007)
#define TCM_ALG_KEYEDHASH           ((TCM_ALG_ID) 0x0008)
#define TCM_ALG_XOR                 ((TCM_ALG_ID) 0x000A)
#define TCM_ALG_SHA256              ((TCM_ALG_ID) 0x000B)
#define TCM_ALG_SHA384              ((TCM_ALG_ID) 0x000C)
#define TCM_ALG_SHA512              ((TCM_ALG_ID) 0x000D)
#define TCM_ALG_NULL                ((TCM_ALG_ID) 0x0010)
#define TCM_ALG_SM3_256             ((TCM_ALG_ID) 0x0012)
#define TCM_ALG_SM4                 ((TCM_ALG_ID) 0x0013)
#define TCM_ALG_RSASSA              ((TCM_ALG_ID) 0x0014)
#define TCM_ALG_RSAES               ((TCM_ALG_ID) 0x0015)
#define TCM_ALG_RSAPSS              ((TCM_ALG_ID) 0x0016)
#define TCM_ALG_OAEP                ((TCM_ALG_ID) 0x0017)
#define TCM_ALG_ECDSA               ((TCM_ALG_ID) 0x0018)
#define TCM_ALG_ECDH                ((TCM_ALG_ID) 0x0019)
#define TCM_ALG_ECDAA               ((TCM_ALG_ID) 0x001A)
#define TCM_ALG_SM2                 ((TCM_ALG_ID) 0x001B)
#define TCM_ALG_ECSCHNORR           ((TCM_ALG_ID) 0x001C)
#define TCM_ALG_ECMQV               ((TCM_ALG_ID) 0x001D)
#define TCM_ALG_KDF1_SP800_56A      ((TCM_ALG_ID) 0x0020)
#define TCM_ALG_KDF2                ((TCM_ALG_ID) 0x0021)
#define TCM_ALG_KDF1_SP800_108      ((TCM_ALG_ID) 0x0022)
#define TCM_ALG_ECC                 ((TCM_ALG_ID) 0x0023)
#define TCM_ALG_SYMCIPHER           ((TCM_ALG_ID) 0x0025)
#define TCM_ALG_CAMELLIA            ((TCM_ALG_ID) 0x0026)
#define TCM_ALG_CMAC                ((TCM_ALG_ID) 0x003F)
#define TCM_ALG_CTR                 ((TCM_ALG_ID) 0x0040)
#define TCM_ALG_SHA3_256            ((TCM_ALG_ID) 0x0027)
#define TCM_ALG_SHA3_384            ((TCM_ALG_ID) 0x0028)
#define TCM_ALG_SHA3_512            ((TCM_ALG_ID) 0x0029)
#define TCM_ALG_OFB                 ((TCM_ALG_ID) 0x0041)
#define TCM_ALG_CBC                 ((TCM_ALG_ID) 0x0042)
#define TCM_ALG_CFB                 ((TCM_ALG_ID) 0x0043)
#define TCM_ALG_ECB                 ((TCM_ALG_ID) 0x0044)
#define TCM_ALG_FIRST               ((TCM_ALG_ID) 0x0001)
#define TCM_ALG_LAST                ((TCM_ALG_ID) 0x0044)

/* Definition of TCM_HANDLE TCM_RH Constants <S> */
typedef uint32_t            TCM_HANDLE;
typedef TCM_HANDLE          TCM_RH;
#define TCM_RH_FIRST       ((TCM_RH) 0x40000000) /* R */
#define TCM_RH_SRK         ((TCM_RH) 0x40000000) /* R */
#define TCM_RH_OWNER       ((TCM_RH) 0x40000001) /* K A P */
#define TCM_RH_REVOKE      ((TCM_RH) 0x40000002) /* R */
#define TCM_RH_TRANSPORT   ((TCM_RH) 0x40000003) /* R */
#define TCM_RH_OPERATOR    ((TCM_RH) 0x40000004) /* R */
#define TCM_RH_ADMIN       ((TCM_RH) 0x40000005) /* R */
#define TCM_RH_EK          ((TCM_RH) 0x40000006) /* R */
#define TCM_RH_NULL        ((TCM_RH) 0x40000007) /* K A P */
#define TCM_RH_UNASSIGNED  ((TCM_RH) 0x40000008) /* R */
#define TCM_RH_PW          ((TCM_RH) 0x40000009) /* S */
#define TCM_RS_PW          ((TCM_RH) 0x40000009) /* S; This was a bug; to be deprecated*/
#define TCM_RH_LOCKOUT     ((TCM_RH) 0x4000000A) /* A */
#define TCM_RH_ENDORSEMENT ((TCM_RH) 0x4000000B) /* K A P */
#define TCM_RH_PLATFORM    ((TCM_RH) 0x4000000C) /* K A P */
#define TCM_RH_PLATFORM_NV ((TCM_RH) 0x4000000D) /* C */
#define TCM_RH_AUTH_00     ((TCM_RH) 0x40000010) /* A */
#define TCM_RH_AUTH_FF     ((TCM_RH) 0x4000010F) /* A */
#define TCM_RH_ACT_0       ((TCM_RH) 0x40000110) /* A P */
#define TCM_RH_ACT_1       ((TCM_RH) 0x40000111)
#define TCM_RH_ACT_2       ((TCM_RH) 0x40000112)
#define TCM_RH_ACT_3       ((TCM_RH) 0x40000113)
#define TCM_RH_ACT_4       ((TCM_RH) 0x40000114)
#define TCM_RH_ACT_5       ((TCM_RH) 0x40000115)
#define TCM_RH_ACT_6       ((TCM_RH) 0x40000116)
#define TCM_RH_ACT_7       ((TCM_RH) 0x40000117)
#define TCM_RH_ACT_8       ((TCM_RH) 0x40000118)
#define TCM_RH_ACT_9       ((TCM_RH) 0x40000119)
#define TCM_RH_ACT_A       ((TCM_RH) 0x4000011A)
#define TCM_RH_ACT_B       ((TCM_RH) 0x4000011B)
#define TCM_RH_ACT_C       ((TCM_RH) 0x4000011C)
#define TCM_RH_ACT_D       ((TCM_RH) 0x4000011D)
#define TCM_RH_ACT_E       ((TCM_RH) 0x4000011E)
#define TCM_RH_ACT_F       ((TCM_RH) 0x4000011F) /* A P */
#define TCM_RH_FW_OWNER             ((TCM_RH) 0x40000140) /* K */
#define TCM_RH_FW_ENDORSEMENT       ((TCM_RH) 0x40000141) /* K */
#define TCM_RH_FW_PLATFORM          ((TCM_RH) 0x40000142) /* K */
#define TCM_RH_FW_NULL              ((TCM_RH) 0x40000143) /* K */
#define TCM_RH_SVN_OWNER_BASE       ((TCM_RH) 0x40010000) /* K */
#define TCM_RH_SVN_ENDORSEMENT_BASE ((TCM_RH) 0x40020000) /* K */
#define TCM_RH_SVN_PLATFORM_BASE    ((TCM_RH) 0x40030000) /* K */
#define TCM_RH_SVN_NULL_BASE        ((TCM_RH) 0x40040000) /* K */
#define TCM_RH_LAST                 ((TCM_RH) 0x4004FFFF) /* R */

void tcm_init();
void target_reset();
void tcm_read_register(uint8_t addr_byte1, uint8_t addr_byte2, uint8_t *dest_buf, uint8_t read_size);
void tcm_write_register(uint8_t addr_byte1, uint8_t addr_byte2, uint8_t *data, uint8_t data_size);
int sendCommand(uint8_t *cmd_frame, int cmd_size, uint8_t *resp_buf);
#endif