// Turbidity Test Script

#define TURBIDITY_PIN 35
#define NUM_SAMPLES 5

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
}

void loop() {
  long totalSensorValue = 0;
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    totalSensorValue += analogRead(TURBIDITY_PIN);
    delay(500);
  }

  float averageSensorValue = (float)totalSensorValue / NUM_SAMPLES;
  float averageVoltage = averageSensorValue * (3.3 / 4095.0);

  Serial.print("Average Analog Value: "); Serial.print(averageSensorValue, 2);
  
  Serial.print("  |  Average Voltage: "); Serial.println(averageVoltage, 3);

  delay(1000); 
}