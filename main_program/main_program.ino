#include <Arduino.h>


// Transducer structure, used both for the actuator and the sensor
struct Transducer{
  int pin_1;
  int pin_2;
};


// actuator pin initialization
const Transducer top_left_motor = {27, 26};
const Transducer bottom_right_motor = {33, 32};
const int list_of_all_motor_pins[] = {26, 27, 32, 33};


// sensor pin initialization
// pin_1(15)->trigger pin
// pin_2(2)->echo pin
const Transducer ultrasound_sensor = {15, 2}; 


// --- Thresholds, Constants & shared variables ---
const int DISTANCE_THRESHOLD = 25;
const int NUMBER_OF_MOTOR_PINS = 4;

volatile int shared_distance = 999;
volatile bool pause_backgound_sensing = false; 

// --- Settings ---
const int motor_speed_while_turning = 100; // range of speed(0-255)
const int motor_speed_while_moving = 70; // range of speed(0-255)
const int rotation_time = 300; // in millisecond
const int pause_time = 2000; // time the bot pauses between turns(in milliseconds)
const int pwm_freq = 5000; // 5kHz
const int pwm_resolution = 8; // 8-bits resolution(0-255)


// --- Function Prototypes --- 
void setup_pwm_for_motors();
void rotate_right();
void rotate_left();
void move_forward();
void move_backward();
void stop_motors();
int read_distance();
int read_filtered_distance();
void avoid_obstacle();


// main setup function that is called when the computer boots
void setup(){
  // setup the serial port with baud rate of 115200
  Serial.begin(115200);

  //initializing pins for sensor
  pinMode(ultrasound_sensor.pin_1, OUTPUT); // Trigger-> pin_1-> 15-> output
  pinMode(ultrasound_sensor.pin_2, INPUT_PULLUP); //Echo-> pin_2-> 2->Input


  // initalizing the pwm motors by calling the initialization function
  setup_pwm_for_motors();

  // start the motors at stopped state
  stop_motors();

  // printing initialization statements
  Serial.println("H49D4R bot initialized!");
  Serial.println("Core computer: ESP32");
  Serial.println("Program by: MrRobot");
  Serial.println("-------------------------");
  delay(2000); // start scanning and all that fun stuff after 2 seconds of initalization.

}

// the main loop function
void loop(){
  int distance = read_filtered_distance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if(distance<DISTANCE_THRESHOLD && distance!=999){
    Serial.println("obstacle");
    stop_motors();
    avoid_obstacle();
  }else{
    Serial.println("clear");
    move_forward();
  }

  delay(100);
}



// pwm_setup function configures pwm for the motors
void setup_pwm_for_motors(){
  for(int i = 0;i<NUMBER_OF_MOTOR_PINS;i++){
    ledcAttach(list_of_all_motor_pins[i], pwm_freq, pwm_resolution);
  }
}

// function that halts all the motors
void stop_motors(){
  for(int i = 0;i<NUMBER_OF_MOTOR_PINS;i++){
    ledcWrite(list_of_all_motor_pins[i], 0);
  }
}

void move_forward(){
  // front motor forward
  ledcWrite(top_left_motor.pin_1, motor_speed_while_moving);
  ledcWrite(top_left_motor.pin_2, 0);
  //back motor forward
  ledcWrite(bottom_right_motor.pin_1, motor_speed_while_moving);
  ledcWrite(bottom_right_motor.pin_2, 0);

}

void move_backward(){
  //front wheel backward
  ledcWrite(top_left_motor.pin_1, 0);
  ledcWrite(top_left_motor.pin_2, motor_speed_while_moving);

  //back motor backward
  ledcWrite(bottom_right_motor.pin_1, 0);
  ledcWrite(bottom_right_motor.pin_2, motor_speed_while_moving);
}


void rotate_right(){
  // front motor forward
  ledcWrite(top_left_motor.pin_1, motor_speed_while_turning);
  ledcWrite(top_left_motor.pin_2, 0);

  //back motor backward
  ledcWrite(bottom_right_motor.pin_1, 0);
  ledcWrite(bottom_right_motor.pin_2, motor_speed_while_turning);
}

void rotate_left(){
   // front motor backward
  ledcWrite(top_left_motor.pin_1, 0);
  ledcWrite(top_left_motor.pin_2, motor_speed_while_turning);

  //back motor forword
  ledcWrite(bottom_right_motor.pin_1, motor_speed_while_turning);
  ledcWrite(bottom_right_motor.pin_2, 0);
}

// function that reads the distance using ultrasound sensor
int read_distance(){
  //clear the trigger
  digitalWrite(ultrasound_sensor.pin_1, LOW);
  delayMicroseconds(2);

  // send 10 microsecond pulse
  digitalWrite(ultrasound_sensor.pin_1, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasound_sensor.pin_1, LOW);

  // get the duration it took for the pulses to make back
  long duration = pulseIn(ultrasound_sensor.pin_2, HIGH, 30000);

  if(!duration){
    return 999;
  }
  return (int)(duration*0.0343/2);
}

// function to remove noise from raw sensor data
int read_filtered_distance(){
  int reading_1 = read_distance();
  delay(2);
  int reading_2 = read_distance();
  delay(2);
  int reading_3 = read_distance();

  // sorting it mannually to get the mid value
  if(reading_1 > reading_2){ int temp = reading_1; reading_1 = reading_2; reading_2 = temp;}
  if(reading_2 > reading_3){ int temp = reading_2; reading_2 = reading_3; reading_3 = temp;}
  if(reading_1 > reading_2){ int temp = reading_1; reading_1 = reading_2; reading_2 = temp;}
  
  // sorted vals reading_1< reading_2< reading_3
  return reading_2;
}

void avoid_obstacle(){
  int pause_time = 200;
  Serial.println("Engaging in avoidance maneuver");

  // safety backup
  stop_motors();
  delay(200);
  move_backward();
  delay(300);
  stop_motors();

  // scan left
  rotate_left();
  delay(rotation_time);
  stop_motors();
  delay(pause_time);
  int left_dist = read_filtered_distance();

  //rotate right
  // first rotation makes it face the initial direction
  rotate_right();
  delay(rotation_time);
  stop_motors();
  delay(pause_time);
  //second rotation looks to the right
  rotate_right();
  delay(rotation_time);
  stop_motors();
  delay(pause_time);
  int right_dist = read_filtered_distance();

  if(left_dist>right_dist){
    rotate_left();
    delay(rotation_time);
    stop_motors();
    delay(pause_time);

    // second time to actually rotate to left
    rotate_left();
    delay(rotation_time);
    stop_motors();
    delay(pause_time);
  }

  delay(500);

}


