 /*
 This code will reset the corresponding EEPROM used by DFRobot Gravity EC Meter K=1. SKU:DFR0300
 When uploaded this code, please open the serial monitor of the Ardino IDE. The correct value of the EEPROM block should be 255. 
 */
#include <EEPROM.h>
#define ECADDR 0x0A
void setup()
{
  Serial.begin(115200);
  // On ESP32 (and some other boards) EEPROM emulation requires begin() and commit()
  // Allocate at least the number of bytes you intend to use (use 32 for safety)
  if (!EEPROM.begin(32)) {
    Serial.println("EEPROM.begin failed!");
    for (;;) ;
  }

  Serial.println("Writing 0xFF to EEPROM addresses starting at 0x0A...");
  for (int i = 0; i < 8; i++) {
    EEPROM.write(ECADDR + i, 0xFF); // write default value to the EEPROM buffer
    delay(10);
  }
  // Persist the changes to flash
  if (!EEPROM.commit()) {
    Serial.println("EEPROM.commit failed!");
  } else {
    Serial.println("EEPROM.commit OK");
  }
}
void loop()
{
  static int a = 0;
  int value = EEPROM.read(ECADDR + a);
  // Print address in hex and value in decimal (255 expected) and hex for clarity
  Serial.print("0x");
  if ((ECADDR + a) < 0x10) Serial.print("0");
  Serial.print(ECADDR + a, HEX);
  Serial.print(": ");
  Serial.print(value); // decimal
  Serial.print("  (0x");
  if (value < 0x10) Serial.print("0");
  Serial.print(value, HEX);
  Serial.println(")");
  delay(100);
  a = a + 1;
  if (a == 8) {
    Serial.println("Done. If you see 255 for each address, EEPROM was cleared successfully.");
    while (1);
  }
}
