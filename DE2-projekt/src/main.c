/***********************************************************************
 * Smart plant watering system and data logger
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * NOTE:
 * This code was created as a school project for Digital Electronics 2
 *   
 **********************************************************************/


// Settings for main page of Doxygen manual
/**
 * @mainpage
 * 
 * Smart plant watering system and data logger school project for DE2
 * 
 * @author Jakub Fojtik, Michal Lexa, Jan Socha, Tomas Fryza, Peter Fleury
 * @copyright (c) 2023 Jakub Fojtik, Michal Lexa, Jan Socha, 
 *              This work is licensed under  the terms of the MIT license
 */

/* Defines -----------------------------------------------------------*/
#ifndef F_CPU
# define F_CPU 16000000  // CPU frequency in Hz required for UART_BAUD_SELECT
#endif

#define HUM_SEN PC0         // PC0 is pin, which we used for connecting with humidity sensor 
#define LED_GREEN PB5       // PB5 is AVR pin, where green on-board LED

/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <UART.h>           // Peter Fleury's UART library
#include <stdlib.h>         // C library. Needed for number conversions
#include <gpio.h>           // GPIO library needed for LED indicator
#include <oled.h>           // OLED library needed for displaying data 

/* Global variables --------------------------------------------------*/
// Declaration of "dht12" variable with structure "DHT_values_structure"
struct DHT_values_structure 
{
  uint8_t hum_rel;
} dht12;

 uint16_t hum;


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function, where the program execution begins
 * Purpose:  ADC conversion for received signal
 **********************************************************************/
int main(void)
{
  GPIO_mode_output(&DDRB, LED_GREEN);  // Set pin where LED is connected as output

  uart_init(UART_BAUD_SELECT(9600, F_CPU)); // Initialize UART 
  

  // Configure Analog-to-Digital Convertion unit
  // Select ADC voltage reference to "AVcc with external capacitor at AREF pin"
  ADMUX = ADMUX | (1<<REFS0);
  // Select input channel ADC0 (voltage divider pin)
  ADMUX = ADMUX & ~(1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
  // Enable ADC module
  ADCSRA = ADCSRA | (1<<ADEN);
  // Enable conversion complete interrupt
  ADCSRA = ADCSRA | (1<<ADIE);
  // Set clock prescaler to 128
  ADCSRA = ADCSRA | (1<<ADPS2 | 1<<ADPS1 | 1<<ADPS0);



  // Set prescaler to 1 sec and enable overflow interrupt
  TIM1_OVF_1SEC;
  TIM1_OVF_ENABLE;
  
  oled_init(OLED_DISP_ON); // Initialize OLED
  

  // Enables interrupts by setting the global interrupt mask
  sei();

  // Infinite loop
  while (1)
  {
        /* Empty loop. All subsequent operations are performed exclusively 
         * inside interrupt service routines ISRs */
  }
  return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer overflow interrupt
 * Purpose:  Use Single Conversion mode and start conversion
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
  ADCSRA = ADCSRA | (1<<ADSC); // Start ADC conversion

  // Condition for lighting green LED, when is relative humidity less than 40 %
  if (dht12.hum_rel < 40) 
  {
    PORTB = PORTB ^ (1<<LED_GREEN);
  }
  else 
  {
    PORTB = PORTB ^ (0<<LED_GREEN);

  }
 
}

/**********************************************************************
 * Function: ADC complete interrupt
 * Purpose:  Display value on OLED
 **********************************************************************/
ISR(ADC_vect)
{

  // 920 refers to the highest value, what we got in air
  // 730 refers to the lowest value, what we got in water

  char string[8];       // String for converted numbers by itoa
  hum = ADC;
  hum = (920-hum)*100/270; // Computing relative humidity from experimentary gained values

  // Displaying values on OLED
  itoa(hum, string, 10);  // Convert decimal value to string
  uart_puts(string);
  uart_puts("\n");
  oled_clrscr();
  oled_gotoxy(0,2);
  oled_puts("Humidity = ");
  oled_puts(string);
  oled_puts("%");
  oled_gotoxy(0,5);
  oled_puts("Watering: ");

  // Condition for displaying information of watering status
  if (hum>40)       
  {
    oled_puts("OFF");
   
  }
   else{
     oled_puts("ON"); 
     
    }
  
  dht12.hum_rel = hum;
  oled_display(); // Copy buffer to display RAM

}
