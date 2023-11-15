
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

int main(void)
{
  uart_init(UART_BAUD_SELECT(9600, F_CPU));
  //GPIO_mode_input_pullup(&PORTC, PC0);
  uart_puts("test");
  

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
  uint8_t hum;
  char string[8];
  hum = GPIO_read(&DDRC, PC0);

  
    itoa(hum, string, 2);
    uart_puts(hum);
  
}