/****************************************?***********************************//**
* \file cy_jwt_cert_policy.h
* \version 1.0
*
* \brief
*  This is the header file for functions to extract certificate
*  from the Provisioning packet.
*
********************************************************************************
* \copyright
* Copyright 2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#ifndef __CY_JWT_BNU_POLICY__
#define __CY_JWT_BNU_POLICY__

#include "cJSON.h"

#if defined(__cplusplus)
extern "C" {
#endif

/***************************************
* Macro definitions
***************************************/
#define JSON_MAX_DEPTH  (15)

/** Provisioning details SysCall opcode */
#define PROVDETAIL_SYSCALL_OPCODE       (0x37UL << 24UL)

/** SysCall return codes */
#define CY_FB_SYSCALL_SUCCESS           (0xA0000000UL)

/** SysCall timeout value */
#define SYSCALL_TIMEOUT                 (15000UL)

/** Error codes for policy processing functions */
enum cy_jwt_error_codes
{
    CY_JWT_SUCCESS = 0,
    /* generic JSON - [-1, -20) */
    CY_JWT_ERR_JKEY_NOT_FOUND = -1,
    CY_JWT_ERR_JSN_NONOBJ = -2,
    CY_JWT_ERR_JWT_PACKET_PARSE = -3,
    CY_JWT_ERR_JSN_BIG_NESTING = -4,
    CY_JWT_ERR_JSN_PARSE_FAIL = -5,
    /* ECC KEYs [-10, -20) */
    CY_JWT_ERR_EC_KEY_CRV_INV = -10,
    CY_JWT_ERR_EC_KEY_NON_COMPLT = -11,
    CY_JWT_ERR_UNSUPPORTED_ALG = -12,
    /* JWT [-20, -30) */
    CY_JWT_ERR_B64DECODE_FAIL = -20,
    CY_JWT_ERR_JWT_TOO_BIG = -21,
    CY_JWT_ERR_JWT_BROKEN_FORMAT = -22,
    /* Templates [-30, -40) */
    CY_JWT_ERR_TEMPL_VALID_FAIL = -30,
    CY_JWT_ERR_JWT_MALLOC_FAIL = -100,
    CY_JWT_ERR_OTHER = -101
};

/** IDs of the parts of the provisioning packet */
#define FB_POLICY_JWT               (0x100u)
#define FB_POLICY_TEMPL_BOOT        (0x101u)
#define FB_POLICY_TEMPL_DEBUG       (0x102u)
#define FB_POLICY_TEMPL_WOUNDING    (0x103u)

/*
 * Looks over given JWT packet, find where JWT body is located
 * and what length of it is.
 * */
int Cy_JWT_GetStrDataBody(char *jwt_str, char **body_ptr, uint32_t *body_len);

/*
 * Function calculates the buffer size required for decoded base64 string storage.
 * */
uint32_t Cy_JWT_GetB64DecodeLen(uint32_t base64_size);

/*
 * Finds item by key (name) in whole JSON.
 * */
cJSON *Cy_JWT_FindJsonItemByKey(const char* key, cJSON *json);

/*
 * Parses provisioning packet (JWT) and looks for chain_of_trust token
 * */
int Cy_JWT_ParseProvisioningPacket(char *provPacket, char *certBuff, uint32_t certBuffLen, uint8_t certificateId);

/*
 * Read provisioning packet (JWT), policy templates or public keys strings in
 * JSON format.
 * */
int Cy_JWT_GetProvisioningDetails(uint32_t id, char **ptr, uint32_t *len);

		
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CY_JWT_BNU_POLICY__ */
