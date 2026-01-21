/*
 * ESP32 Robot - Stop at 10cm Test (Safety Fixed)
 * Logic: 
 * 1. Move Forward ONLY if path is clear (> 10cm).
 * 2. Stop if object is close (<= 10cm).
 * 3. Stop if Sensor reads 0 (Error/Disconnected).
 * * Hardware Setup:
 * - Sensor Trig: Pin 15
 * - Sensor Echo: Pin 2
 * - Left Motor:  Pins 27 (Fwd), 26 (Rev)
 * - Right Motor: Pins 33 (Fwd), 32 (Rev)
 */

// --- CONFIGURATION ---
const int OBSTACLE_LIMIT = 10;  // Stop when closer than 10cm
const int SPEED_DRIVE = 100;    // Motor Speed (0-255)

// --- PIN DEFINITIONS ---
const int trigPin = 15;
const int echoPin = 2;

const int pinLeft_Fwd = 27;
const int pinLeft_Rev = 26;
const int pinRight_Fwd = 33;
const int pinRight_Rev = 32;

// Variables
long duration;
int distance;

void setup() {
  // Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Motor Pins
  pinMode(pinLeft_Fwd, OUTPUT);
  pinMode(pinLeft_Rev, OUTPUT);
  pinMode(pinRight_Fwd, OUTPUT);
  pinMode(pinRight_Rev, OUTPUT);

  Serial.begin(115200);
  Serial.println("--- Stop-at-10cm Test Online ---");
  delay(2000); // 2 second delay to put robot on floor
}

void loop() {
  // 1. Measure Distance
  distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // 2. Drive Logic (Safety First)
  if (distance == 0) {
    // CASE A: Sensor Error or Disconnected
    Serial.println("Sensor Error (0). Stopping for safety.");
    stopMotors();
  } 
  else if (distance <= OBSTACLE_LIMIT) {
    // CASE B: Object Detected
    Serial.println("Obstacle! Stopping.");
    stopMotors();
  } 
  else {
    // CASE C: Path Clear (> 10cm)
    Serial.println("Path Clear. Moving Forward.");
    moveForward(SPEED_DRIVE);
  }

  delay(100); // Small delay for stability
}

// --- HELPER FUNCTIONS ---

int getDistance() {
  // Clear trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Send 10us pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in cm
  return duration * 0.0343 / 2;
}

void moveForward(int speed) {
  analogWrite(pinLeft_Fwd, speed);
  analogWrite(pinLeft_Rev, 0);
  analogWrite(pinRight_Fwd, speed);
  analogWrite(pinRight_Rev, 0);
}

void stopMotors() {
  analogWrite(pinLeft_Fwd, 0);
  analogWrite(pinLeft_Rev, 0);
  analogWrite(pinRight_Fwd, 0);
  analogWrite(pinRight_Rev, 0);
}