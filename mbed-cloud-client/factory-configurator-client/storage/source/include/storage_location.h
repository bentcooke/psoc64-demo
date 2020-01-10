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

#ifndef __STORAGE_LOCATION_H__
#define __STORAGE_LOCATION_H__

typedef enum {
    STORAGE_NON_PSA_TYPE_LOCATION,
    STORAGE_PSA_TYPE_LOCATION,
    STORAGE_SE_PRE_PROVISIONED_TYPE_LOCATION,
    STORAGE_SE_GENERATED_TYPE_LOCATION,
    STORAGE_MAX_TYPE_LOCATION,
} storage_type_location_e;

#endif //__STORAGE_LOCATION_H__
