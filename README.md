# PSoC64 Pelion Client Mbed OS Example

This is a simplified example with the following features:
- Mbed OS 6
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
Cypress CYESKIT_064B0S2_4343W       | WiFi              | Internal Flash           |  Internal Flash          |

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
    mbed target CYESKIT_064B0S2_4343W
    mbed toolchain ARM
    mbed device-management init -d arm.com --model-name example-app --force -q
    mbed compile --profile release
```
## Program Flow

1. Initialize, connect and register to Pelion DM
1. Interact with the user through the serial port (115200 bauds)
   - Press enter through putty/minicom to simulate button
   - Press 'i' to print endpoint name
   - Press Ctrl-X to to unregister
   - Press 'r' to reset storage and reboot (warning: it generates a new device ID!)


## Special Demo Setup instructions
   
   Follow latest PSoC 64 Pelion workshop slides
    
# Known-issues

Please review existing issues on github and report any problem you may see.
