# PSoC64 Pelion Client Mbed OS Example

This is a simplified example with the following features:
- Mbed OS 5.15
- Custom version of Pelion Device Management Client 4 for PSoC 64
- Support for FW Update

It is based on the standard mbed-os-example-pelion example, with the following additional features 
- Support for e-ink display shield (CY8CKIT-028-EPD)
- Support for colorful blinking LED

The above new features, show the Pelion status, connecting, connected, registered, downloading new firmware, etc.

Note this application is considered **alpha**

## Supported platforms

This table shows a list of platforms that are supported.

Platform                          |  Connectivity     | Storage for credentials  | Storage for FW candidate | Notes
----------------------------------| ------------------| -------------------------| -----------------------  | --------------
Cypress CY8CKIT_064S2_4343W       | WiFi              | Internal Flash           |  Internal Flash          | 

<span class="notes">**(*) Note**: the platforms require further testing</span>

# Developer guide

This section is intended for developers to get started, import the example application, compile and get it running on their device.

## Requirements

- Mbed CLI >= 1.10.0
  
  For instructions on installing and using Mbed CLI, please see our [documentation](https://os.mbed.com/docs/mbed-os/latest/tools/developing-mbed-cli.html).
  
- Install the `CLOUD_SDK_API_KEY`

   `mbed config -G CLOUD_SDK_API_KEY ak_1MDE1...<snip>`

   You should generate your own API key. Pelion Device Management is available for any Mbed developer. Create a [free trial](https://os.mbed.com/pelion-free-tier).

   For instructions on how to generate your API key, please see our [documentation](https://cloud.mbed.com/docs/current/integrate-web-app/api-keys.html#generating-an-api-key). 

## Deploying

This repository is in the process of being updated and depends on few enhancements being deployed in mbed-cloud-client. In the meantime, follow these steps to import and apply the patches before compiling.

```
    mbed import https://github.com/maclobdell/psoc64-demo
    cd psoc64-demo
```
## Compiling

```
    mbed target CY8CKIT_064S2_4343W
    mbed toolchain ARM
    mbed device-management init -d arm.com --model-name example-app --force -q
    mbed compile --profile release
```
## Program Flow

1. Initialize, connect and register to Pelion DM
1. Interact with the user through the serial port (115200 bauds)
   - Press enter through putty/minicom to simulate button
   - Press 'i' to print endpoint name
   - Press Ctrl-C to to unregister
   - Press 'r' to reset storage and reboot (warning: it generates a new device ID!)


## Special Demo Setup instructions
   
   Follow latest workshop slides, plus these extra instructions 
   
   1. Update mbed cli to the latest (from command line)
```   
        pip install -U mbed-cli
```   
   2. Set compiler path in mbed cli for arm compiler 6
```   
       mbed config -G ARMC6_PATH "<path to compiler>" 
```        
   3. download the example application
```   
       mbed import https://github.com/maclobdell/psoc64-demo
       
       cd psoc64-demo
```       
   4. download the script
   
       Go to https://github.com/maclobdell/fota-runner
       
       Download fota-runner.py, place it in the top level of the example application.  
     
   5. deploy libraries
```   
      mbed deploy  
```   
   6.  Install the custom Pelion manifest tool for PSoC 64
```
      pip install manifest-tool-1.5.3.tar.gz
```
   7. Initialize manifest tool and create update certificate
```
      mbed device-management init -d <any domain you want>.com --model-name <any model name you want> --force -q   
```
   8.  Add user application key and policy file that were used to provision your PSoC 64 board

   Place key file in: 
```
   \example-pelion\mbed-os\targets\TARGET_Cypress\TARGET_PSOC6\sb-tools\keys\
```
   Place policy file in: 
```
   \example-pelion\mbed-os\targets\TARGET_Cypress\TARGET_PSOC6\sb-tools\policy\
```
   9. Set the WiFi SSID and Password in `mbed_app.json`

   10. Compile
```   
       mbed compile -m CY8CKIT_064S2_4343W -t ARM --profile release
```   
   11. Flash the memory
   
        Drag and drop the BUILD/ CY8CKIT_064S2_4343W/ARM-RELEASE/psoc64-demo.hex file to the DAPLINK drive
   
   12. Open a terminal for the board serial port
   
   13. If using first to claim, when prompted, type a “c” (first time only), to continue with the claiming.
   
   14. Run the script to automatically update the version and launch a fota campaign.
```   
       python fota-runner.py    
```
      The script will update the version of the application, recompile it, and launch a firmware update campaign
    
# Known-issues

Please review existing issues on github and report any problem you may see.
