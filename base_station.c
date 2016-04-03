#define AVERAGE_RUN 10
#define F_CPU 16000000UL
#define BAUD 19200


#include "os.h"
#include <avr/io.h>
#include <stdio.h>
#include "adc.h"
#include "uart.h"

int poll_count  = 0;
int sensor_pin  = 0; 

int VRx_avg[10] = {0,0,0,0,0,0,0,0,0,0};
int VRy_avg[10] = {0,0,0,0,0,0,0,0,0,0};
int LS_avg[10]  = {0,0,0,0,0,0,0,0,0,0};

int servo_x     = 1500;
int servo_y     = 1500;
int laser_val   = 0;

int read(int pin, int avg[]) {
  // Change this
  avg[poll_count] = readadc(pin);
  
  int sum = 0;
  int i   = 0;
  for(i = 0; i < AVERAGE_RUN; i++){ 
    sum += avg[i];
  }

  return (sum/AVERAGE_RUN);
}

void read_joystick(){
  int event = Task_GetArg();
  
  //Rewrite map functionality
  servo_x = read(0, VRx_avg);
  servo_y = read(1, VRx_avg);
  
  //Use analog read functionality
  laser_val = readadc(2);
  poll_count++;

  if(poll_count == AVERAGE_RUN){
    poll_count = 0;
  }
 
  Event_Signal(event);
}

void write_bt(){
  int event = Task_GetArg();
  //char buffer[50];
  //sprintf(buffer, "#%d %d %d#", servo_x, servo_y, laser_val);
  BT_UART_Send_Str("hello world");

  Event_Signal(event);
}

void action(){
  int read_joystick_eid = Event_Init();
  int write_bt_eid      = Event_Init();

  for(;;){
    Task_Create(read_joystick, 2, read_joystick_eid);
    Event_Wait(read_joystick_eid);

    Task_Create(write_bt, 3, write_bt_eid);
    Event_Wait(write_bt_eid);
    PORTC = 0x0F;
    Task_Sleep(200); // sleep for 0.2 seconds
    PORTC = 0x00;
    Task_Sleep(200); // sleep for 0.2 seconds
  }
}

void loop(){
  for(;;);
}

void a_main(){
  DDRC    = 0x0F;
  
  InitADC();
  BT_UART_Init();

  Task_Create(action, 1, 0);
  Task_Create(loop, 8, 0);
  Task_Terminate();
}
