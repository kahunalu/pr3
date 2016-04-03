#ifndef USB_UART_H_
#define UART_H_
 
//#ifndef MEGA_2560
//  #error "Sonar_SEN136B5B.h can only be used with the MEGA2560"
//#endif

#include <stdint.h>

void RMB_UART_Init(void);
void BT_UART_Init(void);

void RMB_UART_Send_Byte(uint8_t);
void BT_UART_Send_Byte(uint8_t);

void RMB_UART_Send_String(char*);
void BT_UART_Send_String(char*);
 
#endif /* USB_UART_H_ */
