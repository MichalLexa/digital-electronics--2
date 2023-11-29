
#ifndef F_CPU
# define F_CPU 16000000  // CPU frequency in Hz required for UART_BAUD_SELECT
#endif

#define HUM_SEN PC0
#define LED_GREEN PB5

#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <UART.h>           // Peter Fleury's UART library
#include <stdlib.h>         // C library. Needed for number conversions
#include <gpio.h>
#include <oled.h>
// Vzduch 920 - max
//voda 730 - min
 uint16_t hum;
int main(void)
{
  GPIO_mode_output(&DDRB, LED_GREEN);

  uart_init(UART_BAUD_SELECT(9600, F_CPU));
  
  
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

  TIM1_OVF_1SEC;
  TIM1_OVF_ENABLE;
  
  oled_init(OLED_DISP_ON);
  
  

  sei();
  
  while (1)
  {


  }
  return 0;
}

ISR(TIMER1_OVF_vect)
{
  ADCSRA = ADCSRA | (1<<ADSC);
 
  
}
ISR(ADC_vect)
{

  char string[8];
  hum = ADC;
  hum = (920-hum)*100/270;

  itoa(hum, string, 10);  
  uart_puts(string);
  uart_puts("\n");
   oled_clrscr();
  oled_gotoxy(0,2);
  oled_puts("Humidity = ");
  //oled_gotoxy(1,2);
  oled_puts(string);
  oled_puts("%");
  oled_gotoxy(0,5);
  oled_puts("Watering: ");
  if (hum<40)
  {
    oled_puts("ON");
    
  }
   else{
     oled_puts("OFF"); 
    }

  oled_display();
    if (hum < 40) 
  {
    PORTB = PORTB ^ (1<<LED_GREEN);
  }


}