/*********
  Instructions on how to set up firebase on https://RandomNerdTutorials.com/esp32-firebase-realtime-database/
*********/
// Minimal sketch: read pH, EC, turbidity (ADC pins) and DS18B20 temp, write readings to Firebase RTDB,
// and watch /pump boolean to actuate a pump on GPIO25 for 10s when true.

// Enable Firebase features used in this sketch. These must be defined before
// including <FirebaseClient.h> so the library exposes the UserAuth and
// RealtimeDatabase types used below.
#define ENABLE_USER_AUTH
#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>
#include <OneWire.h>
#include <EEPROM.h>
#include "DFRobot_PH.h"
#include "DFRobot_EC.h"

// --- Pin Definitions ---
#define PH_PIN 36        // Use GPIO36 for pH sensor (ADC1_CH0)
#define EC_PIN 34        // Use GPIO34 for EC sensor (ADC1_CH6)
#define TURBIDITY_PIN 35 // Use GPIO35 for turbidity sensor (ADC1_CH7)
#define DS18S20_Pin 4    // DS18S20 Signal pin on GPIO4
#define PUMP_PIN 25      // Pump control pin

// Network and Firebase credentials (adjust as needed)
#define WIFI_SSID "wif_name"
#define WIFI_PASSWORD "wifi_password"
#define Web_API_KEY "your_web_api_key"
#define DATABASE_URL "your_database_url"
#define USER_EMAIL "your_user_example_email@gmail.com"
#define USER_PASS "your_user_password"

// Firebase and network objects
UserAuth user_auth(Web_API_KEY, USER_EMAIL, USER_PASS);
FirebaseApp app;
WiFiClientSecure ssl_client;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl_client);
RealtimeDatabase Database;

// DS18B20 OneWire object and sensor libraries
OneWire ds(DS18S20_Pin);

// pH and EC objects
DFRobot_PH ph;
DFRobot_EC ec;

// Timing
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 10000; // 10s

// small helper to map floats
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max){
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// callback required by library (kept minimal)
void processData(AsyncResult &aResult){
  (void)aResult; // ignore in this minimal example
}

// Temperature retrieval using the OneWire logic from your test attachment
float getTemp(){
  // returns the temperature from one DS18S20 in °C
  byte data[12];
  byte addr[8];

  if (!ds.search(addr)){
    ds.reset_search();
    return -1000.0f;
  }

  if (OneWire::crc8(addr, 7) != addr[7]){
    Serial.println("CRC is not valid!");
    return -1000.0f;
  }

  if (addr[0] != 0x10 && addr[0] != 0x28){
    Serial.println("Device is not recognized");
    return -1000.0f;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // start conversion

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) data[i] = ds.read();

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB);
  float TemperatureSum = tempRead / 16.0;
  return TemperatureSum;
}

void connectWiFi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(250);
  }
  Serial.println("\nWi-Fi connected");
}

void setup(){
  Serial.begin(115200);

  // ADC config: ensure full range reading
  analogReadResolution(12); // 0-4095
  analogSetPinAttenuation(PH_PIN, ADC_11db);
  analogSetPinAttenuation(EC_PIN, ADC_11db);
  analogSetPinAttenuation(TURBIDITY_PIN, ADC_11db);

  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);

  connectWiFi();

  // minimal SSL setup
  ssl_client.setInsecure();

  // initialize firebase
  initializeApp(aClient, app, getAuth(user_auth), processData, "authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);

  // DS18B20
  // initialize EEPROM and sensor libs
  EEPROM.begin(32);
  ph.begin();
  ec.begin();
}

void loop(){
  app.loop();

  if (!app.ready()){
    // not authenticated yet
    delay(500);
    return;
  }

  unsigned long now = millis();
  if (now - lastSendTime < sendInterval) return;
  lastSendTime = now;
  // Read temperature using your OneWire routine
  float tempC = getTemp();
  float temperatureForCalc = 25.0f;
  if (tempC != -1000.0f) {
    temperatureForCalc = tempC;
  } else {
    Serial.println("Temperature sensor not found, using 25.0 C for calculations");
  }

  // pH reading using DFRobot_PH
  int rawPH = analogRead(PH_PIN); // 0-4095
  float voltagePH = rawPH / 4095.0f * 3300.0f; // mV
  float phValue = ph.readPH(voltagePH, temperatureForCalc);

  // EC reading using DFRobot_EC (voltage in mV). Apply small correction for low EC like in your test.
  int rawEC = analogRead(EC_PIN);
  float voltageEC = rawEC / 4095.0f * 3300.0f; // mV
  float ecValue = ec.readEC(voltageEC, temperatureForCalc);
  float ecCorrection = 1.5f;
  if (ecValue < 1.0f){
    // apply correction and recalc
    ecValue = ec.readEC(voltageEC * ecCorrection, temperatureForCalc);
  }

  // Turbidity: average multiple samples like in your test
  const int NUM_SAMPLES = 5;
  long totalSensorValue = 0;
  for (int i = 0; i < NUM_SAMPLES; i++){
    totalSensorValue += analogRead(TURBIDITY_PIN);
    delay(500);
  }
  float averageSensorValue = (float)totalSensorValue / NUM_SAMPLES;
  float averageVoltage = averageSensorValue * (3.3f / 4095.0f);
  // Map voltage to NTU (linear approximation 0-3.3V -> 0-1000 NTU)
  float turbidityNTU = mapFloat(averageVoltage, 0.0f, 3.3f, 0.0f, 1000.0f);

  // Log locally
  Serial.printf("pH=%.2f (%.0f mV), EC=%.3f ms/cm, turb=%.1f NTU, temp=%.2f C\n", phValue, voltagePH, ecValue, turbidityNTU, tempC);

  // Write readings to Firebase
  Database.set<float>(aClient, "/readings/ph", phValue);
  Database.set<float>(aClient, "/readings/ec", ecValue);
  Database.set<float>(aClient, "/readings/turbidity", turbidityNTU);
  if (tempC != -1000.0f) Database.set<float>(aClient, "/readings/temperature", tempC);
  Database.set<int32_t>(aClient, "/readings/timestamp", (int32_t)now);

  // Check pump state from DB and actuate if requested
  bool pumpState = false;
  // Database.get returns the value directly when using templated get<T>
  pumpState = Database.get<bool>(aClient, "/pump");
  if (pumpState){
    Serial.println("Pump requested: turning ON for 10s");
    digitalWrite(PUMP_PIN, HIGH);
    delay(10000);
    digitalWrite(PUMP_PIN, LOW);
    // clear pump flag back to false
    Database.set<bool>(aClient, "/pump", false);
    Serial.println("Pump cycle complete");
  }
}