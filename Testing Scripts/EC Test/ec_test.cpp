// EC Test Code
// Note: This version applies a correction factor to low EC readings (< 1 ms/cm) 
// This is because low EC readings were preventing calibration at 1.413 ms/cm.

#include "DFRobot_EC.h"  // Note that EC10 is from the official DFRobot Product Wiki
#include <EEPROM.h>

#define EC_PIN 34
DFRobot_EC ec;
float voltage, ecValue, temperature = 23.2;
float correction = 1.5;  // correction 

void setup()
{
  Serial.begin(115200);
  EEPROM.begin(32);    // Initialize EEPROM for ESP32
  ec.begin();
}

void loop()
{
  static unsigned long timepoint = millis();
  if(millis() - timepoint > 5000U)  // time interval: 1s
  {
    timepoint = millis();
    // ESP32 ADC: 12-bit (0-4095), 3.3V reference  
    voltage = analogRead(EC_PIN) / 4095.0 * 3300;  // voltage in mV
    
    // raw measured EC from sensor
    float ec_value = ec.readEC(voltage, temperature);
    
    if (ec_value < 1)  {
      voltage *= correction;
    }
    
    ec_value = ec.readEC(voltage * correction, temperature);  // re-calculate EC with corrected voltage

    Serial.print("Temperature: ");
    Serial.print(temperature, 1);
    Serial.print("°C  EC: ");
    Serial.print(ec_value, 8);
    Serial.println(" ms/cm");
  }



    ec.calibration(voltage, temperature);  // calibration via Serial commands

}