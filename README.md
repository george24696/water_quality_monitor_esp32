# water_quality_monitor_esp32
Final year E&amp;E project: Water health monitoring system with microbial material capture

The table below shows a list of the specific sensor I used when working on this project:

| Component          | Model           | Manufacturer | 
|--------------------|-----------------|--------------|
| Temperature sensor  | DS18B20         | Maxim        |
| pH Sensor          | SEN0161-V2      | DFRobot      | 
| EC Sensor          | DFR0300 K=1     | DFRobot      | 
| Turbidity Sensor   | SEN0189        | DFRobot      | 

## Credentials

Before building and uploading the firmware you must add your network and Firebase credentials so the device can connect to WiFi and your Firebase Realtime Database. Edit the credentials in the project's source (src/main.cpp) and replace the example values with your own:

```cpp
// Network and Firebase credentials (adjust as needed)
#define WIFI_SSID "wif_name"
#define WIFI_PASSWORD "wifi_password"
#define Web_API_KEY "your_web_api_key"
#define DATABASE_URL "your_database_url"
#define USER_EMAIL "your_user_example_email@gmail.com"
#define USER_PASS "your_user_password"
```

You can obtain a Firebase Realtime Database URL and create user credentials by following this tutorial:

https://randomnerdtutorials.com/esp32-firebase-realtime-database/

Make sure to keep your keys and passwords private and do not commit them to public repositories.







