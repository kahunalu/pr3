#include "roomba_driver.h"
#include "os.h"

void a_main() {
	roomba_init();

	/* start cleaning */
	usart_write(DOCK);
	Task_Terminate();
}