
#ifndef F_CPU
# define F_CPU 16000000  // CPU frequency in Hz required for UART_BAUD_SELECT
#define HUM_SEN PC0
#endif



#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <UART.h>           // Peter Fleury's UART library
#include <stdlib.h>         // C library. Needed for number conversions
#include <gpio.h>
// Vzduch 920 - max
//voda 730 - min
int main(void)
{
  uart_init(UART_BAUD_SELECT(9600, F_CPU));
  GPIO_mode_input_pullup(&PORTC, PC0);
  
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
   uint16_t hum;
  char string[8];
  hum =ADC;
  itoa(hum, string, 10);  
  uart_puts(string);
  uart_puts("\n");
}