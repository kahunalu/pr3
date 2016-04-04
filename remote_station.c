#define F_CPU 16000000UL
#define AVERAGE_RUN 10

#include "uart.h"
//#include "adc.h"
//#include "roomba_driver.h"
#include <avr/io.h>
#include <stdio.h>
#include "os.h"
#include "roomba_driver.h"

extern int bt_bytes;
extern int bt_recv_eid;

int bytes_read = 0;
int read_index = 0;

int laser_pin   = 13;

int x     = 0;
int y     = 0;
int laser_val   = 0;
int rv;
int lv;

void auto_move(){
  int event = Task_GetArg();
  Event_Signal(event);
}

void avoid_move(){
  int event = Task_GetArg();
  Event_Signal(event);
}


/*void write_servo(){
  int event = Task_GetArg();
  
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
  
  Event_Signal(event);
}
*/


void write_laser(){
  int event = Task_GetArg();

  if(!laser_val) {
    /* if the button was pressed shoot the laser */
  }
  Event_Signal(event);
}

void man_move(){
  int event = Task_GetArg();

  x = x - 127;
  y = y- 127;

  x = x*4;
  y = y*4;

  rv = y+x;
  lv = y-x;

  /* [145] [Right velocity high byte] [Right velocity low byte] [Left velocity high byte]
[Left velocity low byte]  */

  RMB_UART_Send_Byte(DRIVE_D);
  RMB_UART_Send_Byte((uint8_t)(rv>>8));
  RMB_UART_Send_Byte((uint8_t)rv);
  RMB_UART_Send_Byte((uint8_t)(lv>>8));
  RMB_UART_Send_Byte((uint8_t)lv);




  Event_Signal(event);
}

void read_bt() {
  int event       = Task_GetArg();
  


  uint8_t *buff, curr;
  

    /* wait for next byte*/

    buff = BT_UART_Recv(); 

    while(read_index != bt_bytes) {

      curr = buff[read_index];

      if(bytes_read == 0 && curr == 255) {
        bytes_read++;

      } else if(bytes_read != 0 && curr == 255) {

        bytes_read = 0;
        /* signal that packet is read */
        Event_Signal(event);    

      } else if(bytes_read != 0 && curr != 255) {

        if(bytes_read == 1) x = curr;
        if(bytes_read == 2) y = curr;
        if(bytes_read == 3) laser_val = curr;
        bytes_read++;

      }

      read_index = (read_index + 1) % UART_BUFFER_SIZE;
    }

}

/*
 * action
 * Handles creating the tasks and scheduling the output
 *
 */
void action(){
  //int avoid_move_eid  = Event_Init();
  int man_move_eid    = Event_Init();
  int packet_recv_eid = Event_Init();
  int mapped_eid = Event_Init();
  bt_recv_eid     = Event_Init();
  
  int write_laser_eid = Event_Init();
  //int write_servo_eid = Event_Init();

  

  for(;;){
    //Task_Create(avoid_move, 2, avoid_move_eid);
    
    //Event_Wait(avoid_move_eid);

    RMB_UART_Send_Byte(0);
    RMB_UART_Send_Byte(1);
    RMB_UART_Send_Byte(2);



    Event_Wait(bt_recv_eid);
    /*RMB_UART_Send_Byte(1);
    Task_Create(read_bt, 2, packet_recv_eid);
    Event_Wait(packet_recv_eid);

    Task_Create(write_laser, 3, write_laser_eid);
    Task_Create(man_move, 2, man_move_eid);

    Event_Wait(write_laser_eid);
    Event_Wait(man_move_eid);

    //Task_Create(write_servo, 3, write_servo_eid);
  }
}


/* Create loop function which executes while scheduler sleeps
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
  RMB_UART_Init();
  BTRemote_UART_Init(); 

  roomba_init(); /* works */
  
  DDRC = 0x0F;

  Task_Create(action, 1, 0);
  Task_Create(loop, 8, 0);  

  Task_Terminate();
}