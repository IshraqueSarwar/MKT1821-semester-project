/*
 * Simple Rotation Test for 2-Motor Robot (ESP32 Core 3.x Compatible)
 * Hardware Setup:
 * - MX1508 Motor A (Left) -> Pins 27, 26
 * - MX1508 Motor B (Right) -> Pins 33, 32
 * 
 * Rotation Logic:
 * - Rotate Right: Left motor forward, Right motor backward
 * - Rotate Left: Left motor backward, Right motor forward
 */

#include <Arduino.h>

// --- PIN DEFINITIONS ---
// Motor A (Left)
const int MOTOR_A_IN1 = 27; 
const int MOTOR_A_IN2 = 26;

// Motor B (Right)
const int MOTOR_B_IN1 = 33; 
const int MOTOR_B_IN2 = 32;

// PWM Settings
const int pwmFreq = 5000;      // 5 KHz frequency
const int pwmResolution = 8;   // 8-bit resolution (0-255)

// --- SETTINGS ---
const int motorSpeed = 100;      // Speed (0-255), adjust as needed
const int rotationTime = 300;   // How long to rotate (milliseconds)
const int pauseTime = 2000;      // Pause between rotations (milliseconds)

// --- FUNCTION PROTOTYPES ---
void rotateRight();
void rotateLeft();
void stopMotors();

void setup() {
  Serial.begin(115200);
  
  // Configure PWM for each motor pin (ESP32 Core 3.x API)
  ledcAttach(MOTOR_A_IN1, pwmFreq, pwmResolution);
  ledcAttach(MOTOR_A_IN2, pwmFreq, pwmResolution);
  ledcAttach(MOTOR_B_IN1, pwmFreq, pwmResolution);
  ledcAttach(MOTOR_B_IN2, pwmFreq, pwmResolution);
  
  // Make sure motors start stopped
  stopMotors();
  
  Serial.println("Robot Rotation Test Started!");
  Serial.println("Using ESP32 Core 3.x PWM API");
  Serial.println("----------------------------");
  delay(2000); // Initial 2 second delay before starting
}

void loop() {
  // // Rotate Right
  // Serial.println("MOVING RIGHT");
  // rotateRight();
  // delay(rotationTime);
  
  // // Stop briefly
  // stopMotors();
  // delay(pauseTime);
  
  // // Rotate Left
  // Serial.println("MOVING LEFT");
  // rotateLeft();
  // delay(rotationTime);
  
  // // Stop briefly
  // stopMotors();
  // delay(pauseTime);
  
  // Serial.println("---"); // Separator for readability


  // sweep function
  sweep180();
}

// --- ROTATION FUNCTIONS ---

void rotateRight() {
  // Left Motor Forward (to rotate right)
  ledcWrite(MOTOR_A_IN1, motorSpeed);
  ledcWrite(MOTOR_A_IN2, 0);
  
  // Right Motor Backward (to rotate right)
  ledcWrite(MOTOR_B_IN1, 0);
  ledcWrite(MOTOR_B_IN2, motorSpeed);
}

void rotateLeft() {
  // Left Motor Backward (to rotate left)
  ledcWrite(MOTOR_A_IN1, 0);
  ledcWrite(MOTOR_A_IN2, motorSpeed);
  
  // Right Motor Forward (to rotate left)
  ledcWrite(MOTOR_B_IN1, motorSpeed);
  ledcWrite(MOTOR_B_IN2, 0);
}

void sweep180(){
  // Rotate Right
  Serial.println("MOVING RIGHT");
  rotateRight();
  delay(rotationTime);
  
  // Stop briefly
  stopMotors();
  delay(pauseTime);
  
  // Rotate Left
  Serial.println("MOVING LEFT");
  rotateLeft();
  delay(rotationTime);
  
  // Stop briefly
  stopMotors();
  delay(pauseTime);

  // Rotate Left
  Serial.println("MOVING LEFT");
  rotateLeft();
  delay(rotationTime);

  // Stop briefly
  stopMotors();
  delay(pauseTime);


  Serial.println("MOVING RIGHT");
  rotateRight();
  delay(rotationTime);

  // Stop briefly
  stopMotors();
  delay(pauseTime);
  
  Serial.println("---"); // Separator for readability
}

void stopMotors() {
  ledcWrite(MOTOR_A_IN1, 0);
  ledcWrite(MOTOR_A_IN2, 0);
  ledcWrite(MOTOR_B_IN1, 0);
  ledcWrite(MOTOR_B_IN2, 0);
}