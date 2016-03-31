/*
 * Remote Station Code
 */

 /* number of iterations taken to smooth out sensor data */
#define AVERAGE_RUN 10

#include <Servo.h>
#include <scheduler.h>

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

void write_servo(){
  //while(1){
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
  //  task_sleep(task_getarg(write_servo));
  //}
}

void write_laser(){
  //while(1){
  if(!laser_val){
    digitalWrite(laser_pin, HIGH);
  }else{
    digitalWrite(laser_pin, LOW);
  }
  //  task_sleep(task_getarg(write_laser)));
  //}
}

void read_bt(){
  //while(1){
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
  //  task_sleep(task_getarg(write_laser)));
  //}
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
void setup(){
  //Setup connnection to the roomba here
  Serial2.begin(9600);  // Set up baud rate for usart communications

  pinMode(laser_pin, OUTPUT); // PORTB = 0x00, DDRB = 0xFF
  pinMode(30, OUTPUT);        // PORTB = 0x00, DDRB = 0xFF
  pinMode(31, OUTPUT);        // PORTB = 0x00, DDRB = 0xFF
  pinMode(32, OUTPUT);        // PORTB = 0x00, DDRB = 0xFF
  pinMode(33, OUTPUT);        // PORTB = 0x00, DDRB = 0xFF

  // Create each task with their function, priority, and frequency
  Scheduler_StartTask(70, 10, write_servo); //task_create(write_servo, 1, unknown)
  Scheduler_StartTask(80, 10, write_laser); //task_create(write_laser, 2, unknown)
                                            //task_create(avoid, 3, unknown)
                                            //task_create(auto, 4, unknown)
  Scheduler_StartTask(60, 90, read_bt);     //task_create(read_bt, 5, unknown)
                                            //task_create(loop, 6, 0)
  
  //  Include subsumption architecture with the following precidence 
  //    - avoid
  //    - read_bt
  //    - auto
  //for control of write_servo
}
