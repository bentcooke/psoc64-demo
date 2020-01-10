/*
 *
 */

#if defined(TARGET_CY8CKIT_064S2_4343W)
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "cy_syslib.h"
#include "cy_ipc_drv.h"

#include "../mbed-os/features/mbedtls/mbed-crypto/inc/psa/crypto.h"
#include "flashboot_psacrypto.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PSACRYPTO_SYSCALL_OPCODE        (0x35UL << 24UL)

/** PSA crypto function code */
#define CY_FB_SYSCALL_PSA_ASYMETRIC_VERIFY          (0)
#define CY_FB_SYSCALL_PSA_EXPORT_PUBLIC_KEY         (1)
#define CY_FB_SYSCALL_PSA_GET_KEY_INFO              (2)
#define CY_FB_SYSCALL_PSA_KEY_AGREEMENT             (3)
#define CY_FB_SYSCALL_PSA_GENERATOR_READ            (4)
#define CY_FB_SYSCALL_PSA_GENERATOR_ABORT           (5)
#define CY_FB_SYSCALL_PSA_KEY_POLICY_INIT           (6)
#define CY_FB_SYSCALL_PSA_KEY_POLICY_SET_USAGE      (7)
#define CY_FB_SYSCALL_PSA_SET_KEY_POLICY            (8)
#define CY_FB_SYSCALL_PSA_IMPORT_KEY                (9)
#define CY_FB_SYSCALL_PSA_DESTROY_KEY               (10)
#define CY_FB_SYSCALL_PSA_CIPHER_DECRYPT_SETUP      (11)
#define CY_FB_SYSCALL_PSA_CIPHER_IV                 (12)
#define CY_FB_SYSCALL_PSA_CIPHER_UPDATE             (13)
#define CY_FB_SYSCALL_PSA_CIPHER_FINISH             (14)
#define CY_FB_SYSCALL_PSA_GENERATE_RANDOM           (15)
#define CY_FB_SYSCALL_PSA_HASH_SETUP                (16)
#define CY_FB_SYSCALL_PSA_HASH_UPDATE               (17)
#define CY_FB_SYSCALL_PSA_HASH_FINISH               (18)
#define CY_FB_SYSCALL_PSA_ASYMETRIC_SIGN            (19)
#define CY_FB_SYSCALL_PSA_ALLOCATE_KEY              (20)
#define CY_FB_SYSCALL_KS_CREATE_KEY_HANDLE          (21)
#define CY_FB_SYSCALL_KS_OPEN_KEY_HANDLE            (22)
#define CY_FB_SYSCALL_KS_CLOSE_KEY_HANDLE           (23)

/** PSA crypto SysCall return codes */
#define CY_FB_SYSCALL_SUCCESS           (0xA0000000UL)

/** Timeout values */
#define PSACRYPTO_SYSCALL_TIMEOUT_SHORT (150000UL)
#define PSACRYPTO_SYSCALL_TIMEOUT_LONG  (12000000UL)

/**
 *
 */
psa_status_t Cy_SysCall_Psa(uint32_t *syscallCmd)
{
    psa_status_t status = PSA_SUCCESS;
    uint32_t timeout = 0;

    bool is_crypto_enabled = Cy_Crypto_Core_IsEnabled(CRYPTO);

    IPC->STRUCT[CY_IPC_CHAN_SYSCALL].DATA0 =  (uint32_t)syscallCmd;

    while(((IPC->STRUCT[CY_IPC_CHAN_SYSCALL].ACQUIRE &
            IPC_STRUCT_ACQUIRE_SUCCESS_Msk) == 0) &&
            (timeout < PSACRYPTO_SYSCALL_TIMEOUT_SHORT))
    {
        ++timeout;
    }

    if(timeout < PSACRYPTO_SYSCALL_TIMEOUT_SHORT)
    {
        timeout = 0;

        IPC->STRUCT[CY_IPC_CHAN_SYSCALL].NOTIFY = 1;

        while(((IPC->STRUCT[CY_IPC_CHAN_SYSCALL].LOCK_STATUS &
                IPC_STRUCT_ACQUIRE_SUCCESS_Msk) != 0) &&
                (timeout < PSACRYPTO_SYSCALL_TIMEOUT_LONG))
        {
            ++timeout;
        }

        if(timeout < PSACRYPTO_SYSCALL_TIMEOUT_LONG)
        {
            if(CY_FB_SYSCALL_SUCCESS != syscallCmd[0])
            {
                status = syscallCmd[0];
            }
        }
        else
        {
            status = PSA_ERROR_UNKNOWN_ERROR;
        }
    }
    else
    {
        status = PSA_ERROR_UNKNOWN_ERROR;
    }

    if (is_crypto_enabled)
    {
        Cy_Crypto_Core_Enable(CRYPTO);
    }

    return status;
}

/**
 *
 */
psa_status_t fb_psa_asymmetric_verify(psa_key_handle_t handle,
                                    psa_algorithm_t alg,
                                    const uint8_t *hash,
                                    size_t hash_length,
                                    const uint8_t *signature,
                                    size_t signature_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_param[6];
    uint32_t syscall_cmd[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_ASYMETRIC_VERIFY<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = handle;
    syscall_param[1] = alg;
    syscall_param[2] = (uint32_t)hash;
    syscall_param[3] = hash_length;
    syscall_param[4] = (uint32_t)signature;
    syscall_param[5] = signature_length;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_cipher_decrypt_setup(psa_cipher_operation_t *operation,
                                      psa_key_handle_t handle,
                                      psa_algorithm_t alg)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_param[6];
    uint32_t syscall_cmd[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_CIPHER_DECRYPT_SETUP<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)operation;
    syscall_param[1] = handle;
    syscall_param[2] = alg;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_cipher_finish(psa_cipher_operation_t *operation,
                               uint8_t *output,
                               size_t output_size,
                               size_t *output_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_param[6];
    uint32_t syscall_cmd[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_CIPHER_FINISH<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)operation;
    syscall_param[1] = (uint32_t)output;
    syscall_param[2] = output_size;
    syscall_param[3] = (uint32_t)output_length;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_cipher_set_iv(psa_cipher_operation_t *operation,
                               const unsigned char *iv,
                               size_t iv_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_param[6];
    uint32_t syscall_cmd[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_CIPHER_IV<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)operation;
    syscall_param[1] = (uint32_t)iv;
    syscall_param[2] = iv_length;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_cipher_update(psa_cipher_operation_t *operation,
                               const uint8_t *input,
                               size_t input_length,
                               unsigned char *output,
                               size_t output_size,
                               size_t *output_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_param[6];
    uint32_t syscall_cmd[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_CIPHER_UPDATE<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)operation;
    syscall_param[1] = (uint32_t)input;
    syscall_param[2] = input_length;
    syscall_param[3] = (uint32_t)output;
    syscall_param[4] = output_size;
    syscall_param[5] = (uint32_t)output_length;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_destroy_key(psa_key_handle_t handle)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_param[6];
    uint32_t syscall_cmd[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_DESTROY_KEY<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = handle;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}


psa_status_t fb_psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_HASH_SETUP<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)operation;
    syscall_param[1] = alg;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[3];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_HASH_UPDATE<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)operation;
    syscall_param[1] = (uint32_t)input;
    syscall_param[2] = input_length;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash,
                             size_t hash_size,
                             size_t *hash_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[4];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_HASH_FINISH<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)operation;
    syscall_param[1] = (uint32_t)hash;
    syscall_param[2] = hash_size;
    syscall_param[3] = (uint32_t)hash_length;

    status = Cy_SysCall_Psa(syscall_cmd);
    /* TBD */
    return status;
}

psa_status_t fb_psa_import_key(psa_key_handle_t handle,
                            psa_key_type_t type,
                            const uint8_t *data,
                            size_t data_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[4];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_IMPORT_KEY<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = handle;
    syscall_param[1] = type;
    syscall_param[2] = (uint32_t)data;
    syscall_param[3] = data_length;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_key_policy_t fb_psa_key_policy_init(void)
{
    uint32_t syscall_cmd[2];
    uint32_t syscall_param[3];
    psa_key_policy_t policy;

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_KEY_POLICY_INIT<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)&policy;

    (void)Cy_SysCall_Psa(syscall_cmd);

    return policy;
}

void fb_psa_key_policy_set_usage(psa_key_policy_t *policy,
                              psa_key_usage_t usage,
                              psa_algorithm_t alg)
{
    uint32_t syscall_cmd[2];
    uint32_t syscall_param[3];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_KEY_POLICY_SET_USAGE<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)policy;
    syscall_param[1] = usage;
    syscall_param[2] = alg;

    (void)Cy_SysCall_Psa(syscall_cmd);
}

psa_status_t fb_psa_set_key_policy(psa_key_handle_t handle,
                                const psa_key_policy_t *policy)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[3];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_SET_KEY_POLICY<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = handle;
    syscall_param[1] = (uint32_t)policy;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}


psa_status_t fb_psa_get_key_information(psa_key_handle_t handle,
                                     psa_key_type_t *type,
                                     size_t *bits)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[3];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_GET_KEY_INFO<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = handle;
    syscall_param[1] = (uint32_t)type;
    syscall_param[2] = (uint32_t)bits;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_export_public_key(psa_key_handle_t handle,
                                   uint8_t *data,
                                   size_t data_size,
                                   size_t *data_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[4];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_EXPORT_PUBLIC_KEY<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = handle;
    syscall_param[1] = (uint32_t)data;
    syscall_param[2] = data_size;
    syscall_param[3] = (uint32_t)data_length;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}


psa_status_t fb_psa_allocate_key(psa_key_handle_t *handle)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param;

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_ALLOCATE_KEY<<8);
    syscall_cmd[1] = (uint32_t)&syscall_param;

    syscall_param = (uint32_t)handle;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_keys_create_key(cy_key_slot_t key_id, psa_key_handle_t *handle)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_KS_CREATE_KEY_HANDLE<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)key_id;
    syscall_param[1] = (uint32_t)handle;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_keys_open_key(cy_key_slot_t key_id, psa_key_handle_t *handle)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_KS_OPEN_KEY_HANDLE<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)key_id;
    syscall_param[1] = (uint32_t)handle;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_keys_close_key(cy_key_slot_t key_id)
{
#if 1
    (void)key_id;
    /* Skip closing key because some bug in the HW */
    return PSA_SUCCESS;
#else
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_cmd[2];
    uint32_t syscall_param;

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_KS_CLOSE_KEY_HANDLE<<8);
    syscall_cmd[1] = (uint32_t)&syscall_param;

    syscall_param = (uint32_t)key_id;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
#endif
}

psa_status_t fb_psa_generate_random(uint8_t *output,
                                    size_t output_size)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_param[6];
    uint32_t syscall_cmd[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_GENERATE_RANDOM<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;

    syscall_param[0] = (uint32_t)output;
    syscall_param[1] = output_size;

    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}

psa_status_t fb_psa_asymmetric_sign( psa_key_handle_t key,
                                    psa_algorithm_t alg,
                                    const uint8_t *hash,
                                    size_t hash_length,
                                    const uint8_t *signature,
                                    size_t signature_size,
									size_t *signature_length)
{
    psa_status_t status = PSA_SUCCESS;

    uint32_t syscall_param[7];
    uint32_t syscall_cmd[2];

    syscall_cmd[0] = PSACRYPTO_SYSCALL_OPCODE + (CY_FB_SYSCALL_PSA_ASYMETRIC_SIGN<<8);
    syscall_cmd[1] = (uint32_t)syscall_param;
    //REG32(0x0802C200-4) = key;
    syscall_param[0] = key;
    syscall_param[1] = alg;
    syscall_param[2] = (uint32_t)hash;
    syscall_param[3] = hash_length;
    syscall_param[4] = (uint32_t)signature;
    syscall_param[5] = signature_size;
    syscall_param[6] = (uint32_t)signature_length;
    status = Cy_SysCall_Psa(syscall_cmd);

    return status;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
