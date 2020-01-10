/******************************************************************************
* File Name: cy_eink_psoc_interface.c
*
* Version: 1.00
*
* Description: This file contains functions that encapsulate PSoC Component APIs
*              or Peripheral Driver Library APIs.
*
* Hardware Dependency: CY8CKIT-028-EPD E-INK Display Shield
*
*******************************************************************************
* Copyright (2019), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress 
* reserves the right to make changes to the Software without notice. Cypress 
* does not assume any liability arising out of the application or use of the 
* Software or any product or circuit described in the Software. Cypress does 
* not authorize its products for use in any products where a malfunction or 
* failure of the Cypress product may reasonably be expected to result in 
* significant property damage, injury or death (“High Risk Product”). By 
* including Cypress’s product in a High Risk Product, the manufacturer of such 
* system or application assumes all risk of such use and in doing so agrees to 
* indemnify Cypress against all liability.
*******************************************************************************/
/******************************************************************************
* This file contains functions that encapsulate PSoC Component APIs or 
* Peripheral Driver Library APIs. Functions defined in this file are used by the 
* cy_eink_hardware_driver.c.
*
* For the details of the E-INK display and library functions, see the code  
* example document of CE218136 - PSoC 6 MCU E-INK Display with CapSense (RTOS)
*
* For the details of E-INK display control and communication protocols, see the
* driver document available at the following website:
* http://www.pervasivedisplays.com/products/271
*******************************************************************************/

/* Header file includes */
#include "cy_eink_psoc_interface.h"

/* Function pointer for EINK delay in milliseconds */
cy_eink_delay_function_t Cy_EINK_Delay;

/* Define SPI interface to communicate with the EInk driver */
SPI spi(MBED_CONF_APP_EINK_MOSI, MBED_CONF_APP_EINK_MISO, MBED_CONF_APP_EINK_SCLK);

/* Define EINK display interface pins */
DigitalOut DispCs(MBED_CONF_APP_EINK_DISPCS);             /* SPI CS output pin */
DigitalOut DispRst(MBED_CONF_APP_EINK_DISPRST);         /* Display reset output pin */
DigitalIn  DispBusy(MBED_CONF_APP_EINK_DISPBUSY);       /* Display busy input pin */
DigitalOut Discharge(MBED_CONF_APP_EINK_DISCHARGE);     /* Display discharge output pin */
DigitalOut DispEn(MBED_CONF_APP_EINK_DISPEN);           /* Display enable output pin */
DigitalOut Border(MBED_CONF_APP_EINK_BORDER);           /* Display border output pin */
DigitalOut DispIoEn(MBED_CONF_APP_EINK_DISPIOEN);       /* Display IO Enable output pin */

/*******************************************************************************
* Function Name: void CY_EINK_SetCsPin(uint8_t value) 
********************************************************************************
*
* Summary:
*  Sets the SPI CS pin
*
* Parameters:
*  uint8 value:    Value to be written to the pin 
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void CY_EINK_SetCsPin(uint8_t value)
{
    DispCs.write(value);
}

/*******************************************************************************
* Function Name: void CY_EINK_SetRstPin(uint8_t value) 
********************************************************************************
*
* Summary:
*  Sets the Display Reset pin
*
* Parameters:
*  uint8 value:    Value to be written to the pin 
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void CY_EINK_SetRstPin(uint8_t value)
{
    DispRst.write(value);
}

/*******************************************************************************
* Function Name: void CY_EINK_SetDischargePin(uint8_t value) 
********************************************************************************
*
* Summary:
*  Sets the Display Discharge pin
*
* Parameters:
*  uint8 value:    Value to be written to the pin 
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void CY_EINK_SetDischargePin(uint8_t value)
{
    Discharge.write(value);
}

/*******************************************************************************
* Function Name: void CY_EINK_SetVccPin(uint8_t value) 
********************************************************************************
*
* Summary:
*  Sets the display enable pin
*
* Parameters:
*  uint8 value:    Value to be written to the pin 
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void CY_EINK_SetVccPin(uint8_t value)
{
    DispEn.write(value);
}

/*******************************************************************************
* Function Name: void CY_EINK_SetBorderPin(uint8_t value) 
********************************************************************************
*
* Summary:
*  Sets the display border pin
*
* Parameters:
*  uint8 value:    Value to be written to the pin 
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void CY_EINK_SetBorderPin(uint8_t value)
{
    Border.write(value);
}

/*******************************************************************************
* Function Name: void CY_EINK_SetIoEnPin(uint8_t value) 
********************************************************************************
*
* Summary:
*  Sets the display IoEn pin
*
* Parameters:
*  uint8 value:    Value to be written to the pin 
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void CY_EINK_SetIoEnPin(uint8_t value)
{
    DispIoEn.write(!value);
}


/*******************************************************************************
* Function Name: void Cy_EINK_RegisterDelayFunction(cy_eink_delay_function_t 
*                                                   delayFunction)
********************************************************************************
*
* Summary:
*  Registers the callback function for EINK delay
*
* Parameters:
*  cy_eink_delay_function_t:    Function pointer to a delay function
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void Cy_EINK_RegisterDelayFunction(cy_eink_delay_function_t delayFunction)
{
    /* Register the delay function */
    Cy_EINK_Delay = delayFunction;
}

/*******************************************************************************
* Function Name: void Cy_EINK_InitSPI(void)
********************************************************************************
*
* Summary:
*  Initializes the SPI block that communicates with the E-INK display.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void Cy_EINK_InitSPI(void)
{
    /* Start the SPI master */
    spi.frequency(10000000);
    spi.format(8, 0);
    
    /* Make the chip select HIGH */
    CY_EINK_SetCsPin(1);
}

/*******************************************************************************
* Function Name: void Cy_EINK_AttachSPI(void)
********************************************************************************
*
* Summary:
*  Attaches the SPI master to the E-INK display driver.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void Cy_EINK_AttachSPI(void)
{
    /* Make the chip select HIGH */
    CY_EINK_SetCsPin(1);
}

/*******************************************************************************
* Function Name: void Cy_EINK_DetachSPI(void)
********************************************************************************
*
* Summary:
*  Detaches the SPI master from the E-INK display driver.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void Cy_EINK_DetachSPI(void)
{
    /* Stop the SPI master */
}

/*******************************************************************************
* Function Name: void Cy_EINK_WriteSPI(uint8_t data)
********************************************************************************
*
* Summary:
*  Send a byte of data to the E-INK display driver via SPI.
*
* Parameters:
*  uint8_t data : data byte that need to be transmitted
*
* Return:
*  None
*
* Side Effects:
*  None
*******************************************************************************/
void Cy_EINK_WriteSPI(uint8_t data)
{
    /* Send one byte of data */
    spi.write(data);
}

/*******************************************************************************
* Function Name: void Cy_EINK_WriteSPIBuffer(uint8_t* data, uint16 dataLength)
********************************************************************************
*
* Summary:
*  Send a multiple bytes of data to the E-INK display driver via SPI.
*
* Parameters:
*  uint8_t* data : pointer to the buffer that holds the data
*  uint16 dataLength : number of bytes to be transmitted
*
* Return:
*  int : Number of bytes written to SPI
*
* Side Effects:
*  None
*******************************************************************************/
int Cy_EINK_WriteSPIBuffer(uint8_t* data, uint16 dataLength)
{
    return spi.write((const char*)data, dataLength, NULL, 0);
}

/*******************************************************************************
* Function Name: Cy_EINK_ReadSPI(uint8_t data)
********************************************************************************
*
* Summary:
*  Read a byte of data from the E-INK display driver via SPI.
*
* Parameters:
*  uint8_t data : command that need to be transmitted
*
* Return:
*  uint8_t : received data
*
* Side Effects:
*  None
*******************************************************************************/
uint8_t Cy_EINK_ReadSPI(uint8_t data)
{
    /* Read one byte of the RX data */
    /* Return received byte */
    return spi.write(data);
}

/*******************************************************************************
* Function Name: bool CY_EINK_IsBusy(void)
********************************************************************************
*
* Summary:
*  Check if the E-INK display is busy.
*
* Parameters:
*  None
*
* Return:
*  bool : True if the E-INK display is buy, False otherwise
*
* Side Effects:
*  None
*******************************************************************************/
bool Cy_EINK_IsBusy(void)
{
    /* Return the status of  pin */
    if (DispBusy)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/* [] END OF FILE */
