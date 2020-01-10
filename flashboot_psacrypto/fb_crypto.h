/***************************************************************************//**
* \file crypto.h
* \version 1.20.1
* Device Family: CY8C6xxx and CY8C6xxx-BLE II
*
* Provides header file for Crypto APIs.
*
* References:
* Sec1 : http://www.secg.org/sec1-v2.pdf
*
********************************************************************************
* \copyright
* Copyright 2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(FB_CRYPTO_H_)
#define FB_CRYPTO_H_

#include "../mbed-os/features/mbedtls/mbed-crypto/inc/psa/crypto.h"

#if defined(__cplusplus)
extern "C" {
#endif

/** The length in bytes of RSA N part of public key (N, E) */
#define CY_FB_RSA_N_LENGTH   	(256u)

/** The length in bytes of SHA-256 hash */
#define CY_FB_SHA_LENGTH     	(32u)

/** The length in bytes of ECC-256 signature size */
#define CY_FB_ECC_SIGN_LENGTH 	(64u) /* 256 *2 / 8 */

/* Key slots 0 is reserved as unused. */
#define FB_KEY_SLOT_NA				(0u)
#define FB_KEY_SLOT_DEVICE_ECDH		(1u)	/* Encrypted programming */
#define FB_KEY_SLOT_DEVICE_ECDSA	(2u)	/* Attestation syscall */
#define FB_KEY_SLOT_CYPRESS			(3u)
#define FB_KEY_SLOT_HSM				(4u)
#define FB_KEY_SLOT_OEM				(5u)
#define FB_KEY_CUSTOM_KEY_NUM		(5u)							/* Maximum number of supported custom keys to store in flash */
#define FB_KEY_KEY_STORAGE_NUM		(FB_KEY_CUSTOM_KEY_NUM + 2u)	/* Maximum number of keys supported by store in flash */
#define FB_KEY_SLOT_CUSTOM_MIN		(6u)
#define FB_KEY_SLOT_CUSTOM_MAX		(FB_KEY_SLOT_CUSTOM_MIN + FB_KEY_CUSTOM_KEY_NUM - 1u) /* 10 */
#define FB_KEY_SLOT_STATIC_MAX		(FB_KEY_SLOT_CUSTOM_MAX)		/* For Key protection in PSA Crypto syscall */
#define FB_KEY_SLOT_UDS				(FB_KEY_SLOT_CUSTOM_MAX + 1u)   	/* Temporary slot for Device Key generation and encryption, protected from export */
#define FB_KEY_SLOT_AES				(FB_KEY_SLOT_UDS)
#define FB_KEY_SLOT_TEST			(FB_KEY_SLOT_UDS)

#define FB_KEY_SLOT_CNT             (FB_KEY_SLOT_UDS)


#define FB_POLICY_JWT				(0x100u)
#define FB_POLICY_TEMPL_BOOT		(0x101u)
#define FB_POLICY_TEMPL_DEBUG		(0x102u)
#define FB_POLICY_TEMPL_WOUNDING	(0x103u)
#define FB_POLICY_CERTIFICATE		(0x200u)
#define FB_POLICY_CERT_INDEX_MASK   (0x0FFu)

#define CY_FB_DEAD_ACCESS_CM0_DISABLE				(0x01)
#define CY_FB_DEAD_ACCESS_CM4_DISABLE				(0x02)
#define CY_FB_DEAD_ACCESS_SYS_DISABLE				(0x04)
#define CY_FB_DEAD_ACCESS_AP_MPU_ENABLE				(0x08)
#define CY_FB_DEAD_ACCESS_SFLASH_ALLOWED_NOTHING	(0x30)
#define CY_FB_DEAD_ACCESS_MMIO_ALLOWED_NOTHING		(0xC0)
#define CY_FB_DEAD_ACCESS_FLASH_ALLOWED_NOTHING		(0x07)
#define CY_FB_DEAD_ACCESS_SRAM_ALLOWED_NOTHING		(0x38)
#define CY_FB_DEAD_ACCESS_DIRECT_EXECUTE_DISABLE	(0x80)
#define CY_FB_DEAD_ACCESS_DEFAULT					(CY_FB_DEAD_ACCESS_CM0_DISABLE | CY_FB_DEAD_ACCESS_CM4_DISABLE | \
													CY_FB_DEAD_ACCESS_AP_MPU_ENABLE | CY_FB_DEAD_ACCESS_SFLASH_ALLOWED_NOTHING )



/* Size of the key derivation keys (applies both to the master key and
 * to intermediate keys). */
#define KEY_SIZE_BYTES 			(40u)	/*256?*/

/* Maximum size of the Key in JSON format */
#define JSON_KEY_SIZE_BYTES 	(172u)

#define FB_ECP_KEY_SIZE				(32u)
#define FB_ECP_TO_OCTED_KEY_OFFSET	(26u)
#define FB_ECP_TO_OCTED_W_VALUE		(04u)	/* Export a point into unsigned binary data (SEC1 2.3.3) */

#define CY_FB_UDS_KEY_SIZE			(16u)
#define CY_FB_AES_KEY_SIZE			(32u)
#define CY_FB_UDS_IV_SIZE			(PSA_BLOCK_CIPHER_BLOCK_SIZE( PSA_KEY_TYPE_AES ))

#define CY_FLASH_OPCODE_GENERATE_HASH         ((0x1EUL) << 24UL)

#define CY_IMG_CRYPTO_BLK_SIZE		(16)

/* Algorithm for key derivation. */
#define CY_FB_KDF_ALG PSA_ALG_HKDF( PSA_ALG_SHA_256 )

/* Label to use when deriving an intermediate key. */
#define CY_FB_DERIVE_KEY_LABEL ( (const uint8_t *) "secure_flash_boot" )
#define CY_FB_DERIVE_KEY_LABEL_LENGTH ( strlen( (const char*) CY_FB_DERIVE_KEY_LABEL ) )

#define CY_FB_CRYPTO_PGM_PPU		(10u)

typedef struct
{
    uint8_t encryptedSecret[3*CY_IMG_CRYPTO_BLK_SIZE];
    uint8_t salt[CY_IMG_CRYPTO_BLK_SIZE]; /* seed */
    uint8_t info[CY_IMG_CRYPTO_BLK_SIZE]; /* label */
}cy_stc_header_crypto_data_t;


psa_status_t Cy_FB_GenerateDeviceKeys(uint32_t avoidBlowEfuses);
psa_status_t Cy_FB_ImportDeviceKey(void);

psa_status_t FB_JWT_Sign(const char *str, uint32_t *signatureLength, uint8_t *signature);
psa_status_t Cy_FB_CalculateSecureHash(uint32_t responce, uint32_t avoidBlowEfuses);
psa_status_t Cy_ECDH_ExtractDerivedKey(cy_stc_header_crypto_data_t *cryptoData, uint8_t *inDecData);
void Cy_FB_CleanCrypto(uint32_t cryptoPC);
uint32_t Cy_FB_StartCrypto(void);


psa_status_t cipher_decrypt( psa_key_handle_t keyHandle,
                                    psa_algorithm_t alg,
                                    const uint8_t * iv,
									uint32_t ivSize,
                                    const uint8_t * input,
									uint32_t inputSize,
									uint32_t partSize,
                                    uint8_t * output,
									uint32_t outputSize,
									uint32_t *outputLen );
psa_status_t cipher_operation( psa_cipher_operation_t *operation,
                                      const uint8_t * input,
									  uint32_t inputSize,
									  uint32_t partSize,
                                      uint8_t * output,
									  uint32_t outputSize,
									  uint32_t *outputLen );

typedef uint32_t cy_key_slot_t;

		
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FB_CRYPTO_H_ */


/* [] END OF FILE */
