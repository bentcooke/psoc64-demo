// ----------------------------------------------------------------------------
// Copyright 2016-2019 ARM Ltd.
//
// SPDX-License-Identifier: Apache-2.0
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
#ifndef MBED_TEST_MODE
#include "mbed.h"
#include "kv_config.h"
#include "mbed-cloud-client/MbedCloudClient.h" // Required for new MbedCloudClient()
#include "factory_configurator_client.h"       // Required for fcc_* functions and FCC_* defines
#include "m2mresource.h"                       // Required for M2MResource

#include "mbed-trace/mbed_trace.h"             // Required for mbed_trace_*

#include "eink_display_app.h"
#include "app_version.h"
#include "cypress_capsense.h"
#include "blinker_app.h"
#include "wifi_credential_handler.h"

#define APP_USE_CAPSENSE 1

#if (defined(TARGET_CY8CKIT_064S2_4343W) || defined(TARGET_CYESKIT_064B0S2_4343W)) && !defined(DISABLE_CY_FACTORY_FLOW)
    extern "C" fcc_status_e cy_factory_flow(void);
#endif

// Pointers to the resources that will be created in main_application().
static MbedCloudClient *cloud_client;
static bool cloud_client_running = true;
//static NetworkInterface *network = NULL;
static WiFiInterface *network = NULL;

static M2MResource* m2m_get_res;
static M2MResource* m2m_get_res_led_rate;
static M2MResource* m2m_get_res_led_rate_min;
static M2MResource* m2m_get_res_led_rate_max;
static M2MResource* m2m_put_res;
static M2MResource* m2m_post_res;
static M2MResource* m2m_deregister_res;
  
Thread res_thread;
EventQueue res_queue;
char ssid_buf[40];
char pswd_buf[40];     

void print_client_ids(void)
{
    printf("Account ID: %s\n", cloud_client->endpoint_info()->account_id.c_str());
    printf("Endpoint name: %s\n", cloud_client->endpoint_info()->endpoint_name.c_str());
    printf("Device ID: %s\n\n", cloud_client->endpoint_info()->internal_endpoint_name.c_str());
}

void button_press(void)
{
    m2m_get_res->set_value(m2m_get_res->get_value_int() + 1);
    printf("Counter %" PRIu64 "\n", m2m_get_res->get_value_int());
}

void put_update(const char* /*object_name*/)
{
    printf("PUT update %d\n", (int)m2m_put_res->get_value_int());
}

void execute_post(void* /*arguments*/)
{
    printf("POST executed\n");
}

void deregister_client(void)
{
  #if EINK_DISPLAY
    set_pelion_state(DEREGISTERED);
  #endif
    printf("Unregistering and disconnecting from the network.\n");
    cloud_client->close();
}

void deregister(void* /*arguments*/)
{
    printf("POST deregister executed\n");
    m2m_deregister_res->send_delayed_post_response();

    deregister_client();
}

void client_registered(void)
{
  #if EINK_DISPLAY  
    set_pelion_state(REGISTERED);
  #endif
    printf("Client registered.\n");
    print_client_ids();
}

void client_unregistered(void)
{
    printf("Client unregistered.\n");
    (void) network->disconnect();
    cloud_client_running = false;
}

void client_error(int err)
{
    printf("client_error(%d) -> %s\n", err, cloud_client->error_description());
}

void update_progress(uint32_t progress, uint32_t total)
{
    static uint8_t start_flag;
    
    uint8_t percent = (uint8_t)((uint64_t)progress * 100 / total);
    printf("Update progress = %" PRIu8 "%%\n", percent);
    
    if(percent == 0)
    {
      start_flag = 0;
    }
    if((percent == 1) & (start_flag == 0)) 
    {
      #if EINK_DISPLAY
        set_pelion_state(DOWNLOADING);
      #endif
        start_flag = 1;
    }
}

void update_resources(void)
{
    m2m_get_res_led_rate->set_value(blinker_rate_get());
    //printf("Blink Rate %" PRIu64 "\n", m2m_get_res_led_rate->get_value_int());
}

int main(void)
{
    int status;

    status = mbed_trace_init();
    if (status != 0) {
        printf("mbed_trace_init() failed with %d\n", status);
        return -1;
    }

  #if EINK_DISPLAY
    status = eink_display_app_start();
    if (status != 0) {
        printf("eink display init failed with %d\n", status);
        return -1;
    }
  #endif 
   
    printf("App Version = %d.%d.%d\n\r", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);
#if EINK_DISPLAY
    set_fw_version(APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);

    set_pelion_state(CONNECTING);
#endif 
     
    // Mount default kvstore
    printf("Application ready\n");
    status = kv_init_storage_config();
    if (status != MBED_SUCCESS) {
        printf("kv_init_storage_config() - failed, status %d\n", status);
        return -1;
    }

  #if EINK_DISPLAY
    set_pelion_state(CONNECTING);
  #endif
    status = get_wifi_credentials(ssid_buf, pswd_buf);
    if (status != MBED_SUCCESS) {
        printf("Failed to get wifi credentials from storage or user input.  Error \n");
        return -1;
    } else {
       printf("\n\r");
       printf("SSID: %s\r\n",ssid_buf);
       printf("PASSWORD: ");
       for(int p=0;p<strlen(pswd_buf); p++) { 
         //print * for the number of password characters
         printf("*");
       }
       printf("\r\n");
    }

    // Connect with NetworkInterface
    printf("Connect to network\n");
    //network = NetworkInterface::get_default_instance();
    network = WiFiInterface::get_default_instance();
    if (network == NULL) {
        printf("Failed to get default NetworkInterface\n");
        return -1;
    }
    printf("\nConnecting to [%s]...\n", (const char*) ssid_buf);
    //status = network->connect();
    status = network->connect( (const char*) ssid_buf, (const char*) pswd_buf, NSAPI_SECURITY_WPA_WPA2);
    if (status != NSAPI_ERROR_OK) {
        printf("NetworkInterface failed to connect with %d\n", status);
        printf("Do you want to delete the saved SSID & Password? [y/n]: \n\r");
        int in_char = getchar();
        if (in_char == 'y') {
            status = fcc_init();
            if (status != FCC_STATUS_SUCCESS) {
                printf("fcc_init() failed with %d\n", status);
                return -1;
            }
            printf("Reset storage to an empty state.\n");
            int fcc_status = fcc_storage_delete();
            if (fcc_status != FCC_STATUS_SUCCESS) {
                printf("Failed to delete storage - %d\n", status);
                return -1;
            }
            printf("Reset storage done...Reseting device.\n");
            ThisThread::sleep_for(10000);
            NVIC_SystemReset();
        }
        return -1;
    }
   
  #if EINK_DISPLAY
   set_pelion_state(CONNECTED);
  #endif

 SocketAddress addr;
    status = network->get_ip_address(&addr);
    if (status != NSAPI_ERROR_OK) {
        printf("NetworkInterface failed to get IP address %d\n", status);
        return -1;
    } 

 printf("Network initialized, connected with IP %s\n\n",addr.get_ip_address());

#if defined(DISABLE_CY_FACTORY_FLOW)
    // Run developer flow
    printf("Start developer flow\n");
#endif
    status = fcc_init();
    if (status != FCC_STATUS_SUCCESS) {
        printf("fcc_init() failed with %d\n", status);
        return -1;
    }

#ifdef RESET_STORAGE
    printf("Resets storage to an empty state.\n");
    status = fcc_storage_delete();
    if (status != FCC_STATUS_SUCCESS) {
        printf("Failed to delete storage - %d\n", status);
        return -1;
    }
    printf("Reset storage done.\n");
#endif

#if (defined(TARGET_CY8CKIT_064S2_4343W) || defined(TARGET_CYESKIT_064B0S2_4343W)) && !defined(DISABLE_CY_FACTORY_FLOW)
    status = cy_factory_flow();
#else
    status = fcc_developer_flow();
#endif
    if (status != FCC_STATUS_SUCCESS && status != FCC_STATUS_KCM_FILE_EXIST_ERROR && status != FCC_STATUS_CA_ERROR) {
        printf("fcc_developer_flow() failed with %d\n", status);
        return -1;
    }

#ifdef VERIFY_DEVICE_CONFIG
    status = fcc_verify_device_configured_4mbed_cloud();
    if (status != 0) {
        printf("fcc_verify_device_configured_4mbed_cloud() failed with %d\n", status);
        return -1;
    }
#endif
    printf("Initializing Capacitive Touch Sensors\n");

#if APP_USE_CAPSENSE
    if(capsense_main() != 0)
    {
        printf("Capacitive Touch Sensors failed to initialize \n");
        return -1;
    }
#endif

    blinker_start();

    printf("Create resources\n");
    M2MObjectList m2m_obj_list;

    // GET resource 3200/0/5501
    m2m_get_res = M2MInterfaceFactory::create_resource(m2m_obj_list, 3200, 0, 5501, M2MResourceInstance::INTEGER, M2MBase::GET_ALLOWED);
    if (m2m_get_res->set_value(0) != true) {
        printf("m2m_get_res->set_value() failed\n");
        return -1;
    }

    // Resource for storing led blink rate
    // GET resource current rate
    m2m_get_res_led_rate = M2MInterfaceFactory::create_resource(m2m_obj_list, 3346, 0, 5700, M2MResourceInstance::INTEGER, M2MBase::GET_ALLOWED);
    if (m2m_get_res_led_rate->set_value(0) != true) {
        printf("m2m_get_res_led_rate->set_value() failed\n");
        return -1;
    }

    // GET resource min rate
    m2m_get_res_led_rate_min = M2MInterfaceFactory::create_resource(m2m_obj_list, 3346, 0, 5603, M2MResourceInstance::INTEGER, M2MBase::GET_ALLOWED);
    if (m2m_get_res_led_rate_min->set_value(0) != true) {
        printf("m2m_get_res_led_rate_min->set_value() failed\n");
        return -1;
    }

    // GET resource max rate
    m2m_get_res_led_rate_max = M2MInterfaceFactory::create_resource(m2m_obj_list, 3346, 0, 5604, M2MResourceInstance::INTEGER, M2MBase::GET_ALLOWED);
    if (m2m_get_res_led_rate_max->set_value(7) != true) {
        printf("m2m_get_res_led_rate_max->set_value() failed\n");
        return -1;
    }

    // Resource for generic variable
    // PUT resource 3201/0/5853
    m2m_put_res = M2MInterfaceFactory::create_resource(m2m_obj_list, 3201, 0, 5853, M2MResourceInstance::INTEGER, M2MBase::GET_PUT_ALLOWED);
    if (m2m_put_res->set_value(0) != true) {
        printf("m2m_led_res->set_value() failed\n");
        return -1;
    }
    if (m2m_put_res->set_value_updated_function(put_update) != true) {
        printf("m2m_put_res->set_value_updated_function() failed\n");
        return -1;
    }

    // POST resource 3201/0/5850
    m2m_post_res = M2MInterfaceFactory::create_resource(m2m_obj_list, 3201, 0, 5850, M2MResourceInstance::INTEGER, M2MBase::GET_POST_ALLOWED);
    if (m2m_post_res->set_execute_function(execute_post) != true) {
        printf("m2m_post_res->set_execute_function() failed\n");
        return -1;
    }

    // POST resource 5000/0/1 to trigger deregister.
    m2m_deregister_res = M2MInterfaceFactory::create_resource(m2m_obj_list, 5000, 0, 1, M2MResourceInstance::INTEGER, M2MBase::GET_POST_ALLOWED);

    // Use delayed response
    m2m_deregister_res->set_delayed_response(true);

    if (m2m_deregister_res->set_execute_function(deregister) != true) {
        printf("m2m_post_res->set_execute_function() failed\n");
        return -1;
    }

    printf("Register Pelion Device Management Client\n\n");
    cloud_client = new MbedCloudClient(client_registered, client_unregistered, client_error, NULL, update_progress);
    cloud_client->add_objects(m2m_obj_list);
    cloud_client->setup(network); // cloud_client->setup(NULL); -- https://jira.arm.com/browse/IOTCLT-3114
    //start a thread to update resources once every 2 seconds
    res_thread.start(callback(&res_queue, &EventQueue::dispatch_forever));
    res_queue.call_every(2000, update_resources);

    while(cloud_client_running) {
        int in_char = getchar();
        if (in_char == 'i') {
            print_client_ids(); // When 'i' is pressed, print endpoint info
            continue;
        } else if (in_char == 'r') {
             // When 'r' is pressed, erase storage and reboot the board.            
            printf("Reset storage to an empty state.\n");
            int fcc_status = fcc_storage_delete();
            if (fcc_status != FCC_STATUS_SUCCESS) {
                printf("Failed to delete storage - %d\n", status);
                return -1;
            }
            printf("Reset storage done...Reseting device.\n");
            ThisThread::sleep_for(10000);
            NVIC_SystemReset();
        } else if (in_char > 0 && in_char != 0x18) { // Ctrl+X is 0x18
            button_press(); // Simulate button press
            continue;
        }
        // if Ctrl+X, then reach this point and trigger deregister
        deregister_client();
        break;
    }
    return 0;
}

#endif /* MBED_TEST_MODE */
