/*
 * Object Avoidance Robot (Multithreaded / FreeRTOS)
 * Hardware Setup:
 * - HC-SR04 Trig -> Pin 15
 * - HC-SR04 Echo -> Pin 2
 * - MX1508 Motor A -> Pins 27, 26 (Left)
 * - MX1508 Motor B -> Pins 33, 32 (Right)
 */

#include <Arduino.h>

// --- PIN DEFINITIONS ---
const int trigPin = 15;
const int echoPin = 2;

// Motor A (Left)
const int MOTOR_A_IN1 = 27; 
const int MOTOR_A_IN2 = 26;

// Motor B (Right)
const int MOTOR_B_IN1 = 33; 
const int MOTOR_B_IN2 = 32;

// --- SETTINGS ---
const int stopDistance = 30; // Stop if closer than 30cm
const int motorSpeed = 70;   // Speed (0-255)

// --- SHARED GLOBAL VARIABLES ---
// 'volatile' tells the compiler that this variable can change 
// at any time (by another thread), so it must always read the fresh value.
volatile int sharedDistance = 0;

// --- FUNCTION PROTOTYPES ---
void moveForward();
void stopMotors();
void TaskMeasure(void *pvParameters);
void TaskMotors(void *pvParameters);

void setup() {
  Serial.begin(115200);

  // Setup Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Setup Motor Pins
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_B_IN1, OUTPUT);
  pinMode(MOTOR_B_IN2, OUTPUT);

  Serial.println("MX1508 Multithreaded Robot Initialized...");

  // --- CREATE TASKS ---
  
  // Task 1: Measure Distance
  xTaskCreatePinnedToCore(
    TaskMeasure,      // Function to call
    "MeasureDist",    // Name for debugging
    2048,             // Stack size (bytes)
    NULL,             // Parameter to pass
    1,                // Priority (1 = low, higher numbers = higher priority)
    NULL,             // Task handle
    0                 // Core ID (0 or 1). We put sensor on Core 0.
  );

  // Task 2: Motor Logic
  xTaskCreatePinnedToCore(
    TaskMotors,       // Function to call
    "MotorLogic",     // Name for debugging
    2048,             // Stack size
    NULL,             // Parameter
    1,                // Priority
    NULL,             // Task handle
    1                 // Core ID (0 or 1). We put motors on Core 1.
  );
}

void loop() {
  // The loop is now empty because the work is done in the tasks.
  // We just delay to prevent the loop from eating CPU cycles unnecessarily.
  delay(1000); 
}

// --- TASK 1: SENSOR READING ---
// This runs on Core 0 independently
// void TaskMeasure(void *pvParameters) {
//   long duration;
  
//   for (;;) { // Infinite loop for the task
//     // 1. Trigger Pulse
//     digitalWrite(trigPin, LOW);
//     delayMicroseconds(2);
//     digitalWrite(trigPin, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(trigPin, LOW);

//     // 2. Read Echo
//     // Note: pulseIn is blocking, but since this is in its own task,
//     // it won't freeze the motor logic!
//     duration = pulseIn(echoPin, HIGH);
    
//     // 3. Update Shared Variable
//     // We update the volatile variable so the other task can see it.
//     sharedDistance = duration * 0.0343 / 2;

//     // Optional: Debug Print (Don't print too fast or serial buffers fill up)
//     // Serial.print("Distance: ");
//     // Serial.println(sharedDistance);

//     // IMPORTANT: Allow other tasks to run / prevent Watchdog Timer crash
//     vTaskDelay(50 / portTICK_PERIOD_MS); // Wait 50ms before next measurement
//   }
// }




// --- TASK 1: SENSOR READING ---
void TaskMeasure(void *pvParameters) {
  long duration;
  
  for (;;) { 
    // 1. Trigger Pulse
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // 2. Read Echo
    duration = pulseIn(echoPin, HIGH);
    
    // 3. Update Shared Variable
    int newDistance = duration * 0.0343 / 2;
    sharedDistance = newDistance;

    // --- DEBUGGING SECTION ---
    // Only print if the distance changed or just print every time.
    // We print it here to see raw sensor data.
    Serial.print("Distance: ");
    Serial.print(newDistance);
    Serial.println(" cm");
    // ------------------------

    // Wait 100ms so the Serial monitor isn't flooded too fast
    vTaskDelay(100 / portTICK_PERIOD_MS); 
  }
}





// --- TASK 2: MOTOR CONTROL ---
// This runs on Core 1 independently
void TaskMotors(void *pvParameters) {
  for (;;) { // Infinite loop
    
    int currentDist = sharedDistance; // Read the global variable locally

    // Logic: Check valid distance
    if (currentDist > 0 && currentDist < stopDistance) {
      stopMotors();
      // Serial.println("Status: STOPPED");
    } else {
      moveForward();
      // Serial.println("Status: MOVING");
    }

    // IMPORTANT: Small delay to ensure stability and allow CPU context switch
    vTaskDelay(10 / portTICK_PERIOD_MS); // Update motors every 10ms
  }
}

// --- HELPER FUNCTIONS ---

void moveForward() {
  // Left Motor Forward
  analogWrite(MOTOR_A_IN1, motorSpeed);
  analogWrite(MOTOR_A_IN2, LOW);

  // Right Motor Forward
  analogWrite(MOTOR_B_IN1, motorSpeed);
  analogWrite(MOTOR_B_IN2, LOW);
}

void stopMotors() {
  analogWrite(MOTOR_A_IN1, LOW);
  analogWrite(MOTOR_A_IN2, LOW);
  
  analogWrite(MOTOR_B_IN1, LOW);
  analogWrite(MOTOR_B_IN2, LOW);
}