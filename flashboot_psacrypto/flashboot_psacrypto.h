/***************************************************************************//**
* \file flashboot_psacrypto.h
* \version 1.0
*
* \brief
*  This is the header file for the flashboot psacrypto syscalls.
*
********************************************************************************
* \copyright
*
* © 2019, Cypress Semiconductor Corporation
* or a subsidiary of Cypress Semiconductor Corporation. All rights
* reserved.
*
* This software, including source code, documentation and related
* materials (“Software”), is owned by Cypress Semiconductor
* Corporation or one of its subsidiaries (“Cypress”) and is protected by
* and subject to worldwide patent protection (United States and foreign),
* United States copyright laws and international treaty provisions.
* Therefore, you may use this Software only as provided in the license
* agreement accompanying the software package from which you
* obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, non-
* exclusive, non-transferable license to copy, modify, and compile the
* Software source code solely for use in connection with Cypress�s
* integrated circuit products. Any reproduction, modification, translation,
* compilation, or representation of this Software except as specified
* above is prohibited without the express written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
* PARTICULAR PURPOSE. Cypress reserves the right to make
* changes to the Software without notice. Cypress does not assume any
* liability arising out of the application or use of the Software or any
* product or circuit described in the Software. Cypress does not
* authorize its products for use in any products where a malfunction or
* failure of the Cypress product may reasonably be expected to result in
* significant property damage, injury or death (“High Risk Product”). By
* including Cypress’s product in a High Risk Product, the manufacturer
* of such system or application assumes all risk of such use and in doing
* so agrees to indemnify Cypress against all liability.
*
******************************************************************************/
#ifndef _FLASHBOOT_PSACRYPTO_H_
#define _FLASHBOOT_PSACRYPTO_H_

#include "fb_crypto.h"
#include "../mbed-os/features/mbedtls/mbed-crypto/inc/psa/crypto.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(CY_DEVICE_PSOC6ABLE2)
    /* For PSoC6A-BLE2, PSoC6A-1M */
    #define TEST_STATUS_RAM_ADDRESS            (0x0802f000)
	#define IPC_STR                            (0x4023006c)
#elif defined(CY_DEVICE_PSOC6A2M)
    /* For PSoC6A-2M */
    #define TEST_STATUS_RAM_ADDRESS            (0x0802c200)
	#define IPC_STR                            (0x4022006c)
#elif defined(CY_DEVICE_PSOC6A512K)
    /* For PSoC6A-512K */
    #define TEST_STATUS_RAM_ADDRESS            (0x08020000)
	#define IPC_STR                            (0x4023006c)
#else
    /* Default value */
    //#error "Device family is invalid"
#endif /* #if defined(PSOC6_01_FAMILY) */

typedef int32_t psa_status_t;

#define REG32(addr)					( *(volatile uint32_t *)(addr))



psa_status_t fb_psa_asymmetric_sign( psa_key_handle_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t *hash,
                                    size_t hash_length,
                                    const uint8_t *signature,
                                    size_t signature_size,
                                    size_t *signature_length);


psa_status_t fb_psa_generate_random(uint8_t *output,
                                    size_t output_size);


psa_status_t fb_psa_asymmetric_verify(psa_key_handle_t handle,
                                   psa_algorithm_t alg,
                                   const uint8_t *hash,
                                   size_t hash_length,
                                   const uint8_t *signature,
                                   size_t signature_length);

psa_status_t fb_psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_handle_t handle,
                                      psa_algorithm_t alg);

psa_status_t fb_psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length);

psa_status_t fb_psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const unsigned char *iv,
                               size_t iv_length);

psa_status_t fb_psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input,
                               size_t input_length,
                               unsigned char *output,
                               size_t output_size,
                               size_t *output_length);

psa_status_t fb_psa_destroy_key(psa_key_handle_t handle);

psa_status_t fb_psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg);

psa_status_t fb_psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length);

psa_status_t fb_psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length);

psa_status_t fb_psa_import_key(psa_key_handle_t handle,
                            psa_key_type_t type,
                            const uint8_t *data,
                            size_t data_length);

psa_key_policy_t fb_psa_key_policy_init(void);

void fb_psa_key_policy_set_usage(psa_key_policy_t *policy,
                              psa_key_usage_t usage,
                              psa_algorithm_t alg);

psa_status_t fb_psa_set_key_policy(psa_key_handle_t handle,
                                const psa_key_policy_t *policy);

psa_status_t fb_psa_get_key_information(psa_key_handle_t handle,
                                     psa_key_type_t *type,
                                     size_t *bits);

psa_status_t fb_psa_export_public_key(psa_key_handle_t handle,
                                   uint8_t *data,
                                   size_t data_size,
                                   size_t *data_length);

psa_status_t fb_psa_allocate_key(psa_key_handle_t *handle);

psa_status_t fb_psa_fb_create_key(cy_key_slot_t key_id, psa_key_handle_t *handle);

psa_status_t fb_keys_create_key(cy_key_slot_t key_id, psa_key_handle_t *handle);

psa_status_t fb_keys_open_key(cy_key_slot_t key_id, psa_key_handle_t *handle);

psa_status_t fb_keys_close_key(cy_key_slot_t key_id);

		
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _FLASHBOOT_PSACRYPTO_H_ */
