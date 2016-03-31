/*
 * Remote Station Code
 */

 /* number of iterations taken to smooth out sensor data */
#define AVERAGE_RUN 10

#include <Servo.h>
#include <scheduler.h>

Servo myservo;

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
  digitalWrite(32, HIGH);
  digitalWrite(32, LOW);
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
  digitalWrite(32, HIGH);
  digitalWrite(32, LOW);
}

void write_laser(){
  digitalWrite(31, HIGH);
  digitalWrite(31, LOW);
  if(!laser_val){
    digitalWrite(laser_pin, HIGH);
  }else{
    digitalWrite(laser_pin, LOW);
  } 
  digitalWrite(31, HIGH);
  digitalWrite(31, LOW);
}

void read_bt(){
  digitalWrite(30, HIGH);
  digitalWrite(30, LOW);
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
  digitalWrite(30, HIGH);
  digitalWrite(30, LOW);
}

void idle(uint32_t idle_period){
  delay(idle_period);
}

void setup(){
  pinMode(laser_pin, OUTPUT);
  myservo.attach(2);
  Serial2.begin(9600);
  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);

  Scheduler_Init();

  //start offset in ms, period in ms, function callback
  Scheduler_StartTask(70, 10, write_servo);
  Scheduler_StartTask(80, 10, write_laser);
  Scheduler_StartTask(60, 90, read_bt);
}

void loop(){
  uint32_t idle_period = Scheduler_Dispatch();
  if(idle_period){
    idle(idle_period);
  }
}