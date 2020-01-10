// ----------------------------------------------------------------------------
// Copyright 2019 ARM Ltd.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------
#ifdef MBED_CONF_MBED_CLOUD_CLIENT_SECURE_ELEMENT_ATCA_SUPPORT

#ifndef __STORAGE_SE_ATMEL_H__
#define __STORAGE_SE_ATMEL_H__

#include "psa_driver_se_atmel.h"
#include "kcm_status.h"

/* === Definitions and Prototypes === */
#define STORAGE_ATCA_SIGNER_CHAIN_DEPTH          2

/* === APIs === */
/** Initializes Atmel Secure Element peripheral and loads pre-provisioned items.
*
* @returns ::KCM_STATUS_SUCCESS in case of success or one of the `::kcm_status_e` errors otherwise.
*/
kcm_status_e storage_psa_se_atmel_init(void);


#endif //__STORAGE_SE_ATMEL_H__
#endif //#ifdef MBED_CONF_MBED_CLOUD_CLIENT_SECURE_ELEMENT_ATCA_SUPPORT
