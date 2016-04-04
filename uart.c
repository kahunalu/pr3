#include "uart.h"
#include <avr/io.h>
#define BT_BAUDRATE 9600
#define F_CPU 16000000UL
#define BT_UBRR (F_CPU/(16UL*BT_BAUDRATE)) - 1
#include <avr/interrupt.h>
#include "os.h"

static volatile int rmb_rxn = 0;
static volatile int rmb_bytes = 0;
static volatile uint8_t rmb_buffer[UART_BUFFER_SIZE];

int bt_recv_eid;                 //BT uart UDx
volatile int bt_bytes = 0;
static volatile uint8_t bt_buffer[UART_BUFFER_SIZE];


/*
 * Initialize the ROOMBA Uart
 */
void RMB_UART_Init(){
  UBRR2 = 51;

  // Clear USART Transmit complete flag, normal USART transmission speed
  UCSR2A = (1 << TXC2) | (0 << U2X2);
  // Enable receiver, transmitter, rx complete interrupt and tx complete interupt.
  UCSR2B |= (1<<RXEN2)|(1<<TXEN2)/*|(1<<RXCIE2)|(1<<TXCIE2)*/;
  // 8-bit data
  UCSR2C = ((1<<UCSZ21)|(3<<UCSZ20));
  // disable 2x speed
  UCSR2A &= ~(1<<U2X2);
}

/*
 * Initialize the BASE Bt Uart without interrupts
 */
void BTBase_UART_Init(){  
   PRR1 &= ~(1 << PRUSART1);
   UBRR1 = BT_UBRR;

  // Clear USART Transmit complete flag, normal USART transmission speed
  UCSR1A = (1 << TXC1) | (0 << U2X1);
  // Enable receiver, transmitter without interrupts
  UCSR1B |= (1<<RXEN1)|(1<<TXEN1);
  // 8-bit data
  UCSR1C = ((1<<UCSZ11)|(3<<UCSZ10));
  // disable 2x speed
  UCSR1A &= ~(1<<U2X1);
}

/*
 * Initialize the REMOTE Bt Uart with interrupts
 */
void BTRemote_UART_Init(){  
   PRR1 &= ~(1 << PRUSART1);
   UBRR1 = BT_UBRR;

  // Clear USART Transmit complete flag, normal USART transmission speed
  UCSR1A = (1 << TXC1) | (0 << U2X1);
  // Enable receiver, transmitter, rx complete interrupt and tx complete interupt.
  UCSR1B |= (1<<RXEN1)|(1<<TXEN1)|(1<<RXCIE1)|(1<<TXCIE1);
  // 8-bit data
  UCSR1C = ((1<<UCSZ11)|(3<<UCSZ10));
  // disable 2x speed
  UCSR1A &= ~(1<<U2X1);
}


/*
 * Send byte accross RMB
 */
void RMB_UART_Send_Byte(uint8_t data_out){
  while(!( UCSR2A & (1<<UDRE2)));
  UDR2 = data_out;
}

/*
 * Send byte accross BT
 */
void BT_UART_Send_Byte(uint8_t data_out){
  while(!( UCSR1A & (1<<UDRE1)));
  UDR1 = data_out;
}


/*
 * Send String accross RMB
 */
void RMB_UART_Send_String(char *string_out){
  for(; *string_out; string_out++){
    RMB_UART_Send_Byte(*string_out);
  }
}


/*
 * Send String across BT
 */
void BT_UART_Send_String(char *string_out){
  for(; *string_out; string_out++){
    BT_UART_Send_Byte(*string_out);
  }
}


/*
 * Return the RMB buffer
 */
char * RMB_UART_Recv(void){
return (char*) rmb_buffer;
}


/*
 * Return the BT buffer
 */
char * BT_UART_Recv(void){
  return (uint8_t*) bt_buffer;
}


/*
 * Fill ROOMBA buffer with data
 */
ISR(USART2_RX_vect){
  while (!(UCSR2A & (1<<RXC2)));

  rmb_buffer[rmb_bytes] = UDR2;
  rmb_bytes = (rmb_bytes + 1) % UART_BUFFER_SIZE;
  rmb_rx = 1;
}


/*
 * Fill BT buffer with data
 */
ISR(USART1_RX_vect){
  /*while (!(UCSR1A & (1<<RXC1)));

  bt_buffer[bt_bytes] = UDR1;
  bt_bytes = (bt_bytes + 1) % UART_BUFFER_SIZE;*/
  Event_Signal(bt_recv_eid);
}
