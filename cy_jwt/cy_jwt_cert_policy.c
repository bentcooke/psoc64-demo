/***************************************************************************//**
* \file cy_jwt_cert_policy.c
* \version 1.0
*
* \brief
*  This is the source code for functions to extract certificate
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


#if defined(TARGET_CY8CKIT_064S2_4343W)


#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "cy_syslib.h"
#include "cy_ipc_drv.h"

#include "base64.h"
#include "cy_jwt_cert_policy.h"

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*******************************************************************************
* Function Name: Cy_JWT_GetB64DecodeLen
****************************************************************************//**
* Function calculates the buffer size required for decoded base64 string storage.
*
* \param base64_size Size of base64 string to be decoded.

* \return Returns required buffer size to store decoded Base64 string.
*******************************************************************************/
uint32_t Cy_JWT_GetB64DecodeLen(uint32_t base64_size)
{
    uint32_t out_len, rem;

    out_len = (base64_size * 3);
    out_len /= 4;

    rem = (4 - out_len % 4);

    out_len = out_len + rem;

    return out_len;
}


/*******************************************************************************
* Function Name: Cy_JWT_GetStrDataBody
****************************************************************************//**
* Looks over given JWT packet, find where JWT body is located 
* and what length of it is.
* 
* \param jwt_str  JWT packet to analyse.
* \param body_ptr Pointer to JWT body section.
* \param body_len Length of JWT header section.
* \return         Result of operation: 0 if success, error code otherwise.
*******************************************************************************/
int Cy_JWT_GetStrDataBody(char *jwt_str, char **body_ptr, uint32_t *body_len)
{
    int ret = CY_JWT_SUCCESS;
    char *ptrStart;
    char *ptrEnd;

    ptrStart = (char *)strchr(jwt_str, '.');
    if(NULL != ptrStart)
    {
        ptrEnd = (char *)strchr(ptrStart+1, '.');
        if(NULL != ptrEnd)
        {
            *body_len = ptrEnd-ptrStart-1;
            *body_ptr = ptrStart+1;
        }
        else
        {
            ret = CY_JWT_ERR_JWT_BROKEN_FORMAT;
        }
    }
    else
    {
        ret = CY_JWT_ERR_JWT_BROKEN_FORMAT;
    }
    return ret;
}

/*******************************************************************************
* Function Name: Cy_JWT_FindJsonItemByKey
****************************************************************************//**
* Finds item by key (name) in whole JSON.
* Key can be not unique so function returns first item with this key.
* 
* \param key      String key to lookup after.
* \param json     JSON object to check.
* \return         Returns pointer to JSON object found.
*******************************************************************************/
cJSON *Cy_JWT_FindJsonItemByKey(const char* key, cJSON *json)
{
    cJSON *item = NULL;
    cJSON *parent[JSON_MAX_DEPTH];
    uint32_t depth = 0;

    while(NULL != json)
    {
        if(0 != json->string && 0 == strcmp(key, json->string))
        {
            item = json;
            break;
        }

        if(NULL != json->child)
        {
            if(depth < JSON_MAX_DEPTH)
            {
                parent[depth++] = json;
                json = json->child;
            }
            else
            {
                break;
            }
        }
        else if(NULL != json->next)
        {
            json = json->next;
        }
        else if(0 == depth)
        {
            break;
        }
        else
        {
            while(depth > 0)
            {
                json = parent[--depth]->next;
                if(NULL != json)
                {
                    break;
                }
            }
        }
    }

    return item;
}


/*******************************************************************************
* Function Name: Cy_JWT_ParseProvisioningPacket
****************************************************************************//**
* Parses provisioning packet (JWT) and looks for chain_of_trust token.
*
* \param provPacket    JWT provisioning packet (0 terminated).
* \param certBuff      The buffer where certificate will be copied.
* \param certBuffLen   The length of certBuffLen buffer.
* \param certificateId ID of the requested certificate.
* \return              Result of operation: 0 if success, error code otherwise.
*******************************************************************************/
int Cy_JWT_ParseProvisioningPacket(char *provPacket, char *certBuff, uint32_t certBuffLen, uint8_t certificateId)
{
    int rc = CY_JWT_SUCCESS;
    char *bodyStart = 0;
    uint32_t bodySize = 0;
    char *jsonStr = 0;
    uint32_t jsonLen = 0;
    cJSON *json = 0;
    cJSON *provReq = 0;
    char *certStr = 0;

    rc = Cy_JWT_GetStrDataBody(provPacket, &bodyStart, &bodySize);
    if(CY_JWT_SUCCESS == rc)
    {
        jsonLen = Cy_JWT_GetB64DecodeLen(bodySize);
        jsonStr = malloc(jsonLen);
        if(jsonStr != 0)
        {
            rc = base64_decode((uint8_t*)bodyStart, bodySize,
                    (uint8_t*)jsonStr, jsonLen, BASE64_URL_SAFE_CHARSET);
            if(rc > 0)
            {
                json = cJSON_Parse(jsonStr);
                if(0 != json)
                {
                    provReq = Cy_JWT_FindJsonItemByKey("chain_of_trust", json);
                    if(0 != provReq) 
                    {
						if(provReq->type == cJSON_Array)
						{
							provReq = provReq->child;
							while((certificateId > 0) && (0 != provReq))
							{
								provReq = provReq->next;
								certificateId--;
							}
						}							
						if((0 != provReq) && (certificateId == 0))
						{
							if(provReq->type == cJSON_String)
							{
								certStr = provReq->valuestring;
							}
						}
					}
					if(0 != certStr)
					{
						if((uint32_t)strlen(certStr) < certBuffLen) 
						{
							strcpy(certBuff, certStr);
							rc = CY_JWT_SUCCESS;
						}
						else
						{
							rc = CY_JWT_ERR_JWT_TOO_BIG;
						}
					}
					else
					{
						rc = CY_JWT_ERR_JKEY_NOT_FOUND;
					}
                    cJSON_Delete(json);
                }
                else
                {
                    rc = CY_JWT_ERR_JSN_PARSE_FAIL;
                }
            }
            else
            {
                rc = CY_JWT_ERR_B64DECODE_FAIL;
            }
            free(jsonStr);
        }
        else
        {
            rc = CY_JWT_ERR_JWT_MALLOC_FAIL;
        }
    }

    return rc;
}


/*******************************************************************************
* Function Name: Cy_JWT_GetProvisioningDetails
****************************************************************************//**
* Read provisioning packet (JWT), policy templates or public keys strings in
* JSON format.
*
* \param id          Item id (provisioning packet, templates or public keys).
* \param ptr         Pointer to the response string.
* \param len         Length of the response string.
* \return            Result of operation: 0 if success, error code otherwise.
*******************************************************************************/
int Cy_JWT_GetProvisioningDetails(uint32_t id, char **ptr, uint32_t *len)
{
    int rc = -1;
    volatile uint32_t syscallCmd[2];
    volatile uint32_t syscallParam[2];
    uint32_t timeout = 0;

    syscallCmd[0] = PROVDETAIL_SYSCALL_OPCODE;
    syscallCmd[1] = (uint32_t)syscallParam;

    syscallParam[0] = id;
    syscallParam[1] = 0;

    IPC->STRUCT[CY_IPC_CHAN_SYSCALL].DATA0 =  (uint32_t)syscallCmd;

    while(((IPC->STRUCT[CY_IPC_CHAN_SYSCALL].ACQUIRE &
            IPC_STRUCT_ACQUIRE_SUCCESS_Msk) == 0) &&
            (timeout < SYSCALL_TIMEOUT))
    {
        ++timeout;
    }

    if(timeout < SYSCALL_TIMEOUT)
    {
        timeout = 0;

        IPC->STRUCT[CY_IPC_CHAN_SYSCALL].NOTIFY = 1;

        while(((IPC->STRUCT[CY_IPC_CHAN_SYSCALL].LOCK_STATUS &
                IPC_STRUCT_ACQUIRE_SUCCESS_Msk) != 0) &&
                (timeout < SYSCALL_TIMEOUT))
        {
            ++timeout;
        }

        if(timeout < SYSCALL_TIMEOUT)
        {
            if(CY_FB_SYSCALL_SUCCESS != syscallCmd[0])
            {
                rc = syscallCmd[0];
            }
            else
            {
                *len = syscallParam[0];
                *ptr = (char*)syscallParam[1];
                rc = 0;
            }
        }
    }

    return rc;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
