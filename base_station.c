/*
 * Base Station Code
 */

 /* number of iterations taken to smooth out sensor data */
#define AVERAGE_RUN 10

#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
#include <scheduler.h>

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

int poll_count  = 0;
int sensor_pin  = 0; 

int VRx_avg[10] = {0,0,0,0,0,0,0,0,0,0};
int VRy_avg[10] = {0,0,0,0,0,0,0,0,0,0};
int LS_avg[10]  = {0,0,0,0,0,0,0,0,0,0};

int servo_x     = 1500;
int servo_y     = 1500;
int laser_val   = 0;

/*returns averaged analog data rounded to an integer*/
int read(int pin, int avg[]) {
  avg[poll_count] = analogRead(pin);
  int sum = 0;
  int i   = 0;
  for(i = 0; i < AVERAGE_RUN; i++){ 
    sum += avg[i];
  }
  return (sum/AVERAGE_RUN);
}

void print_status() {
  digitalWrite(32, HIGH);
  digitalWrite(32, LOW);  
  /* print servo position */
  lcd.print(servo_x);
  lcd.print(", ");
  lcd.print(servo_y);
  lcd.print("                ");
  lcd.setCursor(0,1);

  /* laser fired this round? */
  if(!laser_val){
    lcd.print(" SW, ");
  }else{
    lcd.print("!SW, ");
  }

  /*if a laser is detected print shot */
  if(analogRead(sensor_pin)>100){
    lcd.print(" SHOT");
  }else{
    lcd.print("!SHOT");
  }
  
  lcd.print("                ");
  lcd.setCursor(0,0);
  digitalWrite(32, HIGH);
  digitalWrite(32, LOW);  
}

void read_joystick(){
  digitalWrite(30, HIGH);
  digitalWrite(30, LOW);
  /* mapping joystick values to servo*/
  servo_x = map(read(15, VRx_avg),0,1023,2400,600);
  laser_val = analogRead(13);

  poll_count++;

  /* Each ten iterations update system status on LCD screen */
  if(poll_count == AVERAGE_RUN){
    poll_count = 0;
  }
  digitalWrite(30, HIGH);
  digitalWrite(30, LOW);
}

void write_bt(){
  digitalWrite(31, HIGH);
  digitalWrite(31, LOW);
  char buffer[50];
  sprintf(buffer, "#%d %d %d#", servo_x, servo_y, laser_val);
  Serial1.println(buffer);
  
  digitalWrite(31, HIGH);
  digitalWrite(31, LOW);  
}

void idle(uint32_t idle_period){
  delay(idle_period);
}

void setup(){
  Serial1.begin(9600);
  lcd.begin(16,2);

  pinMode(30, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(32, OUTPUT);
  pinMode(33, OUTPUT);

  Scheduler_Init();
  //start offset in ms, period in ms, function callback
  Scheduler_StartTask(10, 90, write_bt);
  Scheduler_StartTask(0, 90, read_joystick);
  Scheduler_StartTask(30, 180, print_status);
}

void loop(){
  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period){
    idle(idle_period);
  }
}
