/*
 * Remote Station Code
 */

 /* number of iterations taken to smooth out sensor data */
#define AVERAGE_RUN 10

#include "os.h"
#include "roomba_driver.h"
#include <avr/io.h>

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
  int event = Task_GetArg();
  /*
  while(Serial2.available()){
    char curr = (char)Serial2.read();
    
    if(bytes == -1 && curr=='#'){
      bytes++;
    }else if(bytes != -1 && curr=='#'){
      values[bytes] = '\0';
      sscanf(values, "%d %d %d", &servo_x, &servo_y, &laser_val);
      bytes=-1;
    }else if(bytes != -1 && curr!='#'){
      values[bytes] = curr;
      bytes++;
    }
  }
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
    
    PORTB = 0x40;
    Task_Sleep(5); // sleep for 0.2 seconds
    PORTB = 0x00;
    Task_Sleep(5); // sleep for 0.2 seconds
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
  roomba_init();

  DDRB = 0xF8;
  PORTB = 0xF8;

  Task_Create(action, 1, 0);
  Task_Create(loop, 8, 0);  

  Task_Terminate();
}
