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
#ifdef MBED_CONF_MBED_CLOUD_CLIENT_SECURE_ELEMENT_SUPPORT
#ifndef __STORAGE_SE_DATA_DEFS_H__
#define __STORAGE_SE_DATA_DEFS_H__

#include "kcm_defs.h"
#include "fcc_defs.h"


/* Structure that represents SE pre provisioned data*/
typedef struct storage_se_preprovisioned_item_data_ {
    kcm_item_type_e  kcm_item_type;
    const char *kcm_item_name;
    uint16_t se_slot_num;
} storage_se_preprovisioned_item_data_s;


/*#ifdef MBED_CONF_MBED_CLOUD_CLIENT_SECURE_ELEMENT_ATCA_SUPPORT

#define PSA_DRV_ATCA_DEVICE_PRV_KEY_SLOT_ID          0x0
#define STORAGE_ATCA_NUMBER_OF_PREPROVISIONED_ITEMS  0x1
#define STORAGE_SE_NUMBER_OF_PROVISIONED_ITEMS STORAGE_ATCA_NUMBER_OF_PREPROVISIONED_ITEMS


storage_se_preprovisioned_item_data_s storage_se_items_data[STORAGE_SE_NUMBER_OF_PROVISIONED_ITEMS] = {
    {   .kcm_item_type = KCM_PRIVATE_KEY_ITEM,
        .kcm_item_name = g_fcc_bootstrap_device_private_key_name,
        .se_slot_num = PSA_DRV_ATCA_DEVICE_PRV_KEY_SLOT_ID
    }
};*/

//#endif//MBED_CONF_MBED_CLOUD_CLIENT_SECURE_ELEMENT_ATCA_SUPPORT
#endif //__STORAGE_SE_DATA_DEFS_H__
#endif //#ifdef MBED_CONF_MBED_CLOUD_CLIENT_SECURE_ELEMENT_SUPPORT
