#define F_CPU 16000000UL
#define BAUD 19200
#define AVERAGE_RUN 10

#include "uart.h"
//#include "adc.h"
//#include "roomba_driver.h"
#include <avr/io.h>
#include <stdio.h>
#include "os.h"

int bytes = -1;
char values[25];

int poll_count  = 0;
int laser_pin   = 13;

int servo_x     = 1500;
int servo_y     = 1500;
int laser_val   = 0;

/*initialize current servo values to the center position */
int curr_servo_x= 1500;
int curr_servo_y= 1500;

void auto_move(){
  int event = Task_GetArg();
  Event_Signal(event);
}

void avoid_move(){
  int event = Task_GetArg();
  Event_Signal(event);
}

void write_servo(){
  int event = Task_GetArg();
  /*
  if(abs(curr_servo_x - servo_x) >= 10){
    if((curr_servo_x - servo_x)>0){
      curr_servo_x -= 10;
      myservo.writeMicroseconds(curr_servo_x);
    }else{
      curr_servo_x += 10;
      myservo.writeMicroseconds(curr_servo_x);
    }
  }else{
    curr_servo_x = servo_x;
    myservo.writeMicroseconds(curr_servo_x);
  }
  */
  Event_Signal(event);
}

void write_laser(){
  int event = Task_GetArg();
  /*
  if(!laser_val){
    digitalWrite(laser_pin, HIGH);
  }else{
    digitalWrite(laser_pin, LOW);
  }
  */
  Event_Signal(event);
}

void man_move(){
  char curr;
  int event = Task_GetArg();
  /*
  while(bytes == -1 || curr!='#'){
    curr = (char)uart1_recvbyte();
    bytes++;
    values[bytes] = curr;
  }

  values[++bytes] = '\0';
  sscanf(values, "#%d %d %d#", &servo_x, &servo_y, &laser_val);
  bytes=-1;
  */

  Event_Signal(event);
}

/*
 * action
 * Handles creating the tasks and scheduling the output
 *
 */
void action(){
  int avoid_move_eid  = Event_Init();
  int man_move_eid    = Event_Init();
  
  int write_laser_eid = Event_Init();
  int write_servo_eid = Event_Init();

  for(;;){
    
    Task_Create(avoid_move, 2, avoid_move_eid);
    Task_Create(man_move, 2, man_move_eid);
    
    Event_Wait(avoid_move_eid);
    Event_Wait(man_move_eid);

    Task_Create(write_servo, 3, write_laser_eid);
    Task_Create(write_laser, 3, write_servo_eid);
    
    PORTC = 0x0F;
    Task_Sleep(50); // sleep for 0.2 seconds
    PORTC = 0x00;
    Task_Sleep(50); // sleep for 0.2 seconds
  }
}


/* Create loop function which executes while schedular sleeps
 *
 */
void loop(){
  for(;;);
}


/*  a_main
 * 
 *    Applications main function which initializes pins, and tasks
 */
void a_main(){
  uart0_init();
  
  for(;;){
    uart0_sendstr("hello world");
  }

  DDRC = 0x0F;

  Task_Create(action, 1, 0);
  Task_Create(loop, 8, 0);  

  Task_Terminate();
}