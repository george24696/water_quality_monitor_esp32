// Peristaltic Pump Test Code

#include <Arduino.h>

int motorPin = 25;      // GPIO 25

void setup() {
  Serial.begin(115200);
  pinMode(motorPin, OUTPUT);
}

void loop() {
  // Simple on/off test for the peristaltic pump motor
  digitalWrite(motorPin, HIGH);
  delay(1000);
  digitalWrite(motorPin, LOW);
  delay(1000);
}