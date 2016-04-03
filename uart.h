#ifndef USB_UART_H_
#define UART_H_
 
#include <stdint.h>

void RMB_UART_Init(void);
void BT_UART_Init(void);

void RMB_UART_Send_Byte(uint8_t);
void BT_UART_Send_Byte(uint8_t);

uint8_t RMB_UART_Recv_Byte(void);
uint8_t BT_UART_Recv_Byte(void);

void RMB_UART_Send_String(char*);
void BT_UART_Send_String(char*);
 
#endif /* USB_UART_H_ */
