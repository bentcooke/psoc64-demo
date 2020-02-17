#include "mbed.h"
#include "key_config_manager.h"
#include <stdio.h>

uint8_t get_user_wifi_input(char*, char*);

/* Mbed OS WiFi Credential Handler by Mac Lobdell 2020
   Currently uses KVStore for storage of credentials.  Future versions may support alternative storage.
   If values are already set in the non-volatile memory, use them. If no values are set, then ask the 
   user to provide them over th serial port. To clear the values after they are set in the non-volatile 
   memory, you will need to reset the storage back to factory settings.  This is done by flashing an 
   application that was compiled with the -DRESET_STORAGE flag.  Alternatively, it may be possible to 
   reset the storage from within the normal applicaiton, but this is not proven to work yet.
*/

uint8_t get_wifi_credentials(char *ssid_return_buf, char *pswd_return_buf)
{
     char ssid_get_buf[40];
     char pswd_get_buf[40];
     char ssid_set_buf[40];
     char pswd_set_buf[40];
     size_t item_size = 0;
     kcm_status_e status;
     kcm_status_e status2;
 
     //#ifdef RESET_STORAGE
      #if 1
         // if about to reset the storage anyway, then just get user input, but don't save it
         // this allows the system to connect the wifi if the credentials are corrupted in 
         // the storage
         get_user_wifi_input(ssid_set_buf, pswd_set_buf);
         //copy ssid and password back to buffers from calling function
         strncpy(pswd_return_buf, pswd_set_buf,strlen(pswd_set_buf)+1);  
         strncpy(ssid_return_buf, ssid_set_buf,strlen(ssid_set_buf)+1);
         return MBED_SUCCESS;
     #endif   
                  
     kcm_init();
 
     status = kcm_item_get_data_size((const uint8_t*) "ssid", 4, KCM_CONFIG_ITEM, &item_size);
     if(status == KCM_STATUS_ITEM_NOT_FOUND) 
     {
          printf("Existing WiFi credentials not found\r\n");          

          get_user_wifi_input(ssid_set_buf, pswd_set_buf);
          
          printf("Writing WiFi credentials to KCM\r\n");

          size_t ssid_set_buf_len = strlen(ssid_set_buf)+1;
          size_t pswd_set_buf_len = strlen(pswd_set_buf)+1;
          
          printf("SSID length: %d\r\n", ssid_set_buf_len);
          printf("PSWD length: %d\r\n", pswd_set_buf_len);
          
          //store it - 0 for not factory, so it can be erased when storage is reset
          status2 = kcm_item_store((const uint8_t*) "ssid", 4, KCM_CONFIG_ITEM, 0, (uint8_t*) ssid_set_buf, ssid_set_buf_len, NULL);
          if(status2 != KCM_STATUS_SUCCESS) {
              printf("KCM Error %d\r\n", status);
              return status2;
          }

          status2 = kcm_item_store((const uint8_t*) "pswd", 4, KCM_CONFIG_ITEM, 0, (uint8_t*) pswd_set_buf, pswd_set_buf_len, NULL);
          if(status2 != KCM_STATUS_SUCCESS) {
              printf("KCM Error %d\r\n", status);
              return status2;
          }

     }

     //read it out
      status = kcm_item_get_data((const uint8_t*) "ssid", 4, KCM_CONFIG_ITEM, (uint8_t*) ssid_get_buf, 40, &item_size);
      if(status == KCM_STATUS_SUCCESS) {
          printf("Read from KCM...SSID: %s\r\n",ssid_get_buf);
          printf("SSID length: %d\r\n", strlen(ssid_set_buf));            
          strncpy(ssid_return_buf, ssid_get_buf,strlen(ssid_get_buf)+1);
      }else {
          printf("KCM Error %d\r\n", status);
          return status;
      }
      
      status = kcm_item_get_data((const uint8_t*) "pswd", 4, KCM_CONFIG_ITEM, (uint8_t*) pswd_get_buf, 40, &item_size);
      if(status == KCM_STATUS_SUCCESS) {
          printf("Read from KCM...PSWD: ");
          for(int p=0;p<strlen(pswd_get_buf); p++)
          {
            //print * for the number of password characters
            printf("*");
          }
          printf("\r\n");          
          printf("PSWD length: %d\r\n", strlen(pswd_get_buf));
          strncpy(pswd_return_buf, pswd_get_buf,strlen(pswd_get_buf)+1);  
      }else {              
          printf("KCM Error %d\r\n", status);
          return status;
      }
     
     if((strlen(pswd_return_buf) > 0) && (strlen(pswd_return_buf) > 0))
     {
         printf("Success\r\n");
         return MBED_SUCCESS;            
     }else{
         printf("Error\r\n");
         return 1;
     }

     return MBED_SUCCESS;            
}

uint8_t get_user_wifi_input(char *ssid_buf, char *pswd_buf)
{
  uint8_t pos = 0;
  int in_char = 0;
      
    if (0) {
    //alternative for testing
       snprintf(ssid_buf, 40, "%s", "myssid"); 
       snprintf(pswd_buf, 40, "%s", "mypassword");  
    }

    printf("Enter WiFi SSID (2.4 GHz Only): \r\n");

    //loop until user presses enter
    pos = 0;
    while(1)
    {    
        in_char = getchar();
        //echo it back
        putchar(in_char);
        
        if (in_char == EOF || in_char == '\n') {
              // If EOF or end line, replace with null termination, and break out
             ssid_buf[pos] = '\0';
             break;
        } else {
          //collect characters
          ssid_buf[pos] = in_char;
        }
        pos++;
        
        //if .
        if (pos >= 40) {
          printf("Maximum number of characters exceeded!\r\n");
          break;
        }
    }

    printf("Enter WiFi Password: \r\n");

    //loop until user presses enter    
    pos = 0; 
    while(1)
    {   
        in_char = getchar();
        //echo * back
        putchar('*');
        
        if (in_char == EOF || in_char == '\n') {
              // If EOF or end line, replace with null termination, and break out
             pswd_buf[pos] = '\0';
             break;
        } else {
          //collect characters
          pswd_buf[pos] = in_char;
        }
        pos++;
        
        //if .
        if (pos >= 40) {
          printf("Maximum number of characters exceeded!\r\n");
          break;
        }
    }  
    printf("\r\n");
    printf("SSID: %s\r\n",ssid_buf);
    printf("PASSWORD: %s\r\n",pswd_buf);
    return MBED_SUCCESS;  
  
}
