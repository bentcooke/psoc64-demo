/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "ThisThread.h"
#include "blinker_app.h"

#define MAX_INDEX 7
#define MAX_COLOR 7
#define LED_OFF 1
#define LED_ON 0

Thread blinker;

DigitalOut led_red(LED_RED, LED_OFF);
DigitalOut led_green(LED_GREEN, LED_OFF);
DigitalOut led_blue(LED_BLUE, LED_ON);

uint8_t led_color = LED_COLOR_WHITE;
uint8_t blinker_mode = BLINKER_MODE_OFF;
void led_off(void);
void led_on(void);

uint32_t blink_period[8] = {1000,750,500,250,100,50,25,5};

volatile int32_t blink_index = 0;

void blinker_thread(void)
{
    while(true){
        //if(blink_period[blink_index] !=0)
        if(blinker_mode == BLINKER_MODE_BLINKING) {
			/*blink the led */
			led_on();			
            ThisThread::sleep_for(blink_period[blink_index]/2);
			led_off();
            ThisThread::sleep_for(blink_period[blink_index]/2);			
        } else if(blinker_mode == BLINKER_MODE_SOLID) {
			/*turn the led on and leave it */
			led_on();
            ThisThread::sleep_for(200);
        } else {
			/*turn the led off */
			led_off();
            ThisThread::sleep_for(200);
        }
    }
}

void blinker_start(void)
{

    blinker.start(blinker_thread);
}

void blinker_rate_increase(void)
{
    blink_index++;

    if (blink_index > MAX_INDEX)
    {
        blink_index = MAX_INDEX;
    }
}

void blinker_rate_decrease(void)
{
    blink_index--;

    if (blink_index < 0)
    {
        blink_index = 0;
    }
}

void blinker_rate_set(uint32_t index)
{
    blink_index = (int32_t) index;

    if (blink_index > MAX_INDEX)
    {
        blink_index = MAX_INDEX;
    }
    if (blink_index < 0)
    {
        blink_index = 0;
    }
}

uint32_t blinker_rate_get(void)
{
    uint32_t ret = (uint32_t) blink_index;
    return ret;
}

void blinker_color_set(uint8_t color)
{
    if(color > 0 && color <= MAX_COLOR){
       led_color = color;
    }
}

void blinker_mode_set(uint8_t mode)
{
    blinker_mode = mode;
	
}

void led_off(void)
{
  led_red = LED_OFF;
  led_green = LED_OFF;
  led_blue = LED_OFF;
}

void led_on(void)
{   
   if(led_color == LED_COLOR_RED) {
      led_red = LED_ON;
      led_green = LED_OFF;
      led_blue = LED_OFF;
    }
    else if(led_color == LED_COLOR_GREEN) {
      led_red = LED_OFF;
      led_green = LED_ON;
      led_blue = LED_OFF;
    } 
    else if(led_color == LED_COLOR_BLUE) {
      led_red = LED_OFF;
      led_green = LED_OFF;
      led_blue = LED_ON;
    }
    else if(led_color == LED_COLOR_MAGENTA) {
      led_red = LED_ON;
      led_green = LED_OFF;
      led_blue = LED_ON;
    }
    else if(led_color == LED_COLOR_CYAN) {
      led_red = LED_OFF;
      led_green = LED_ON;
      led_blue = LED_ON;
    }
    else if(led_color == LED_COLOR_YELLOW) {
      led_red = LED_ON;
      led_green = LED_ON;
      led_blue = LED_OFF;
    }                       
    else if(led_color == LED_COLOR_WHITE) {
      led_red = LED_ON;
      led_green = LED_ON;
      led_blue = LED_ON;
    }                           
}

void leds_test(void)
{
  led_red = LED_ON;
  led_green = LED_OFF;
  led_blue = LED_OFF;
  ThisThread::sleep_for(2000);
  led_red = LED_OFF;
  led_green = LED_ON;
  led_blue = LED_OFF;
  ThisThread::sleep_for(2000);
  led_red = LED_OFF;
  led_green = LED_OFF;
  led_blue = LED_ON;
  ThisThread::sleep_for(2000);  
}
