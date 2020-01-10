#include "mbed.h"
#include "blinker_app.h"
#include "eink_display_app.h"

#include "GUI.h"
#include "pervasive_eink_hardware_driver.h"
#include "cy_cy8ckit_028_epd.h"
#include "LCDConf.h"

/* External global references */
extern GUI_CONST_STORAGE GUI_BITMAP bmCypressLogo_1bpp;

/* Display frame buffer cache */
uint8 imageBufferCache[CY_EINK_FRAME_SIZE] = {0};

pelion_state_t pelion_state = CONNECTING;
char version_str[7];  /* XX.Y.Z */
char update_percent_str[4]; /* XXX% */

/* Function prototypes */
void ShowPelionState(pelion_state_t);

/*******************************************************************************
* Function Name: void UpdateDisplay(void)
********************************************************************************
*
* Summary: This function updates the display with the data in the display
*            buffer.  The function first transfers the content of the EmWin
*            display buffer to the primary EInk display buffer.  Then it calls
*            the Cy_EINK_ShowFrame function to update the display, and then
*            it copies the EmWin display buffer to the Eink display cache buffer
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  It takes about a second to refresh the display.  This is a blocking function
*  and only returns after the display refresh
*
*******************************************************************************/
void UpdateDisplay(cy_eink_update_t updateMethod, bool powerCycle)
{
    cy_eink_frame_t* pEmwinBuffer;

    /* Get the pointer to Emwin's display buffer */
    pEmwinBuffer = (cy_eink_frame_t*)LCD_GetDisplayBuffer();

    /* Update the EInk display */
    Cy_EINK_ShowFrame(imageBufferCache, pEmwinBuffer, updateMethod, powerCycle);

    /* Copy the EmWin display buffer to the imageBuffer cache*/
    memcpy(imageBufferCache, pEmwinBuffer, CY_EINK_FRAME_SIZE);

}

/*******************************************************************************
* Function Name: void ShowStartupScreen(void)
********************************************************************************
*
* Summary: This function displays the startup screen with Cypress Logo and
*            the demo description text
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ShowStartupScreen(void)
{
    /* Set foreground and background color and font size */
    GUI_SetFont(GUI_FONT_24_1);
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();

    GUI_DrawBitmap(&bmCypressLogo_1bpp, 2, 2);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_DispStringAt("CYPRESS", 132, 85);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_DispStringAt("ARM PELION", 132, 115);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_DispStringAt("FOTA DEMO", 132, 145);

    /* Send the display buffer data to display*/
    UpdateDisplay(CY_EINK_FULL_4STAGE, true);
}


void ShowPelionState(pelion_state_t state)
{
    char state_str[12];
    char fw_ver_str[20];

		snprintf(fw_ver_str, 20, "FW Version %s ", version_str);
    	  
	  switch(state){
			case CONNECTING:
				snprintf(state_str, 12, "%s", "CONNECTING  "); 			
				break;			
			case CONNECTED:
				snprintf(state_str, 12, "%s", "CONNECTED   ");
				break;			
			case REGISTERED:
				snprintf(state_str, 12, "%s", "REGISTERED  ");
				break;					
			case REQUESTED:
				snprintf(state_str, 12, "%s", "REQUESTED   ");
				break;					
			case PROCESSING:
				snprintf(state_str, 12, "%s", "PROCESSING  ");
				break;					
			case DOWNLOADING:
				snprintf(state_str, 12, "%s", "DOWNLOADING ");
				break;					
			case INSTALLING:
				snprintf(state_str, 12, "%s", "INSTALLING  ");
				break;					
			case COMPLETE:
				snprintf(state_str, 12, "%s", "COMPLETE    ");
				break;
      case DEREGISTERED:
				snprintf(state_str, 12, "%s", "DEREGISTERED");
				break;        					
			default:
				snprintf(state_str, 12, "%s", "AMUSED      ");
    }
	
    /* Set font size, background color and text mode */
    GUI_SetFont(GUI_FONT_24_1);
    GUI_SetBkColor(GUI_BLACK);
    GUI_SetColor(GUI_WHITE);
    GUI_SetTextMode(GUI_TM_NORMAL);

    /* Clear the display */
    GUI_Clear();

    if(state >= CONNECTED)
    {
      /* Display page title */
      GUI_SetTextAlign(GUI_TA_HCENTER);
      GUI_DispStringAt("Connected to", 132, 5);
    }
  	GUI_SetTextAlign(GUI_TA_HCENTER);
	  GUI_DispStringAt("Arm Pelion", 132, 30);

    /* Font24_1*/
    GUI_SetFont(GUI_FONT_24_1);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_DispStringAt("FW Update Status", 132, 70);

    /* Font32_1*/
    GUI_SetFont(GUI_FONT_32_1);
    GUI_SetTextAlign(GUI_TA_HCENTER);	
    GUI_DispStringAt(state_str, 132, 97);
	
/*    if(state == DOWNLOADING)
    {
      GUI_SetFont(GUI_FONT_32_1);
      GUI_SetTextAlign(GUI_TA_RIGHT);	
      GUI_DispStringAt(update_percent_str, 250, 97);
    }
*/  
    /* Font24_1*/
    GUI_SetFont(GUI_FONT_24_1);
    GUI_SetTextAlign(GUI_TA_HCENTER);
    GUI_DispStringAt(fw_ver_str, 132, 144);
  
    //
    //UpdateDisplay(CY_EINK_FULL_4STAGE, true);
    UpdateDisplay(CY_EINK_FULL_2STAGE, true);
}

/*******************************************************************************
* Function Name: void ClearScreen(void)
********************************************************************************
*
* Summary: This function clears the screen
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void ClearScreen(void)
{
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);
    GUI_Clear();
    UpdateDisplay(CY_EINK_FULL_4STAGE, true);
}

/*****************************************************************************
* Function Name: eink_display_app_start
******************************************************************************
* Summary:
*   This functions initializes the display
*
*****************************************************************************/

int eink_display_app_start()
{   
  
    //leds_test();

    /* start the blinker thread */
		blinker_start();  
    
    /* Initialize EmWin driver*/
    GUI_Init();
    
    /* Initialize EInk Controller */
    Cy_EINK_Start(15u, wait_ms);
    //Cy_EINK_Start(15u, ThisThread::sleep_for);
        
    /* Power on the display and check the status of the operation */
    if (Cy_EINK_Power(CY_EINK_ON) == CY_EINK_SUCCESS)
    {
        /* If powering of EINK display successful, continue with the demo */
        ShowStartupScreen();
        ThisThread::sleep_for(2000);
				return 0;	
		}else{
			  /*return error*/
			  return 1;
		}		
}
        

void set_pelion_state(pelion_state_t state)
{

  switch(state){
    case CONNECTING:
      ShowPelionState(CONNECTING);
      blinker_color_set(LED_COLOR_GREEN);
      blinker_rate_set(2);
      blinker_mode_set(BLINKER_MODE_BLINKING);  
      break;			
    case CONNECTED:
      ShowPelionState(CONNECTED);
      blinker_color_set(LED_COLOR_GREEN);
      blinker_mode_set(BLINKER_MODE_SOLID);  
      break;			
    case REGISTERED:
      ShowPelionState(REGISTERED);
      blinker_color_set(LED_COLOR_BLUE);
      blinker_mode_set(BLINKER_MODE_SOLID);  
      break;					
    case REQUESTED:
      ShowPelionState(REQUESTED);
      blinker_color_set(LED_COLOR_BLUE);		
      blinker_rate_set(2);
      blinker_mode_set(BLINKER_MODE_BLINKING);  
      break;					
    case PROCESSING:
      ShowPelionState(PROCESSING);
      blinker_color_set(LED_COLOR_BLUE);		
      blinker_rate_set(3);
      blinker_mode_set(BLINKER_MODE_BLINKING);  
      break;					
    case DOWNLOADING:
      ShowPelionState(DOWNLOADING);
      blinker_color_set(LED_COLOR_CYAN);		
      blinker_rate_set(3);		
      blinker_mode_set(BLINKER_MODE_BLINKING);  
      break;					
    case INSTALLING:
      ShowPelionState(INSTALLING);
      blinker_color_set(LED_COLOR_CYAN);		
      blinker_rate_set(3);		
      blinker_mode_set(BLINKER_MODE_BLINKING);
      break;					
    case COMPLETE:
      ShowPelionState(COMPLETE);
      blinker_color_set(LED_COLOR_GREEN);
      blinker_mode_set(BLINKER_MODE_SOLID);  
      break;					
      case DEREGISTERED:
        ShowPelionState(DEREGISTERED);
        blinker_color_set(LED_COLOR_GREEN);
        blinker_mode_set(BLINKER_MODE_BLINKING);  
        break;					      
    default:
      ShowPelionState(COMPLETE);
      blinker_color_set(LED_COLOR_GREEN);
      blinker_mode_set(BLINKER_MODE_SOLID);
  }
  
}

void set_pelion_download_percent(uint8_t p)
{
    snprintf(update_percent_str, 4, "%2d", p);  
}

void set_fw_version(uint8_t maj, uint8_t min, uint8_t pat)
{
    snprintf(version_str, 7, "%2d.%1d.%1d", maj, min, pat);
}
