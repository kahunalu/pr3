#define F_CPU 16000000UL
#define BAUD 19200
#include "uart.h"
#include "os.h"

void a_main(){
	uart0_init();
  
	for(;;){
		uart0_sendstr("Hello World");
	}
}