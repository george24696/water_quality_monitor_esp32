// pH Test for ESP32 using DFRobot_PH library
#include "DFRobot_PH.h"

#define PH_PIN 36     // Use one of ESP32's ADC pins (e.g. 36 = VP)
float voltage, phValue, temperature = 25;
DFRobot_PH ph;

void setup() {
  Serial.begin(115200);
  ph.begin();
}

void loop() {
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) {
    timepoint = millis();
    int16_t adc = analogRead(PH_PIN);
    voltage = adc / 4095.0 * 3300;   // ESP32 ADC: 0-4095; 3.3V reference
    phValue = ph.readPH(voltage, temperature);
    Serial.print("temperature:");
    Serial.print(temperature, 1);
    Serial.print("°C\tpH:");
    Serial.print(phValue, 2);
    Serial.print("\tVoltage:");
    Serial.print(voltage, 2);
    Serial.println(" mV");
  }
  ph.calibration(voltage, temperature); // calibration by Serial CMD
}

float readTemperature() {
  // Optional: Add code to read from DS18B20 or other temperature sensor
  return 25.0; // placeholder
}
