/*
 * Object Avoidance Robot (MX1508 Driver + ESP32)
 * * Hardware Setup:
 * - HC-SR04 Trig -> Pin 15
 * - HC-SR04 Echo -> Pin 2
 * - MX1508 Motor A -> Pins 14, 27
 * - MX1508 Motor B -> Pins 26, 25
 */

// --- PIN DEFINITIONS ---

// Ultrasonic Sensor
const int trigPin = 15;
const int echoPin = 2;

// Motor A (Left) - MX1508 has no Enable pin, just 2 inputs
const int MOTOR_A_IN1 = 27; 
const int MOTOR_A_IN2 = 26;

// Motor B (Right)
const int MOTOR_B_IN1 = 33; 
const int MOTOR_B_IN2 = 32;

// --- SETTINGS ---
const int stopDistance = 30; // Stop if closer than 15cm
const int motorSpeed = 70;  // Speed (0-255)

// Variables
long duration;
int distance;

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

  Serial.println("MX1508 Robot Initialized...");
}

void loop() {
  // --- 1. Measure Distance ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0343 / 2;

  // Debug Print
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // --- 2. Logic ---
  // Check for valid distance (0 usually means timeout/error)
  if (distance > 0 && distance < stopDistance) {
    stopMotors();
  } else {
    moveForward();
  }

  delay(100); 
}

// --- FUNCTIONS ---

void moveForward() {
  // For MX1508: One pin gets PWM (Speed), the other gets LOW (Ground)
  
  // Left Motor Forward
  analogWrite(MOTOR_A_IN1, motorSpeed);
  analogWrite(MOTOR_A_IN2, LOW);

  // Right Motor Forward
  analogWrite(MOTOR_B_IN1, motorSpeed);
  analogWrite(MOTOR_B_IN2, LOW);
}

void stopMotors() {
  // To stop MX1508, set both pins LOW
  analogWrite(MOTOR_A_IN1, LOW);
  analogWrite(MOTOR_A_IN2, LOW);
  
  analogWrite(MOTOR_B_IN1, LOW);
  analogWrite(MOTOR_B_IN2, LOW);
}