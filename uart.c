#include "uart.h"
#include <avr/io.h>
#define BT_BAUDRATE 9600
#define F_CPU 16000000UL
#define BT_UBRR (F_CPU/(16UL*BT_BAUDRATE)) - 1

void RMB_UART_Init(){  
   PRR0 &= ~(1 << PRUSART0);
   UBRR0 = 51;

   // Clear USART Transmit complete flag, normal USART transmission speed
   UCSR0A = (1 << TXC0) | (0 << U2X0);
   // Enable receiver, transmitter, rx complete interrupt and tx complete interupt.
   UCSR0B = (1<<TXEN0);
   // 8-bit data
   UCSR0C = ((1<<UCSZ01)|(3<<UCSZ00));
   // disable 2x speed
   UCSR0A &= ~(1<<U2X0);
}

void BT_UART_Init(){  
   PRR1 &= ~(1 << PRUSART1);
   UBRR1 = BT_UBRR;

   // Clear USART Transmit complete flag, normal USART transmission speed
   UCSR1A = (1 << TXC1) | (0 << U2X1);
   // Enable receiver, transmitter, rx complete interrupt and tx complete interupt.
   UCSR1B = (1<<TXEN1);
   // 8-bit data
   UCSR1C = ((1<<UCSZ11)|(3<<UCSZ10));
   // disable 2x speed
   UCSR1A &= ~(1<<U2X1);
}

void RMB_UART_Send_Byte(uint8_t data_out){
   while(!( UCSR0A & (1<<UDRE0)));

   UDR0 = data_out;
}

void BT_UART_Send_Byte(uint8_t data_out){
   while(!( UCSR1A & (1<<UDRE1)));

   UDR1 = data_out;
}

void RMB_UART_Send_String(char *string_out){
   for(; *string_out; string_out++){
      RMB_UART_Send_Byte(*string_out);
   }
}

void BT_UART_Send_String(char *string_out){
   for(; *string_out; string_out++){
      BT_UART_Send_Byte(*string_out);
   }
}