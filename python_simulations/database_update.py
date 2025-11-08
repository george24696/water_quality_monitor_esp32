import time
import random
import firebase_admin
from firebase_admin import credentials, db
from datetime import datetime
import json

class SensorSimulator:
    def __init__(self):
        # Initialize with realistic starting values and ranges
        self.current_values = {
            'ph': 6.8,
            'ec': 550.0,
            'turbidity': 15.0,
            'temperature': 23.5
        }
        
        # Realistic ranges for each sensor (min, max, typical variation per reading)
        self.sensor_ranges = {
            'ph': (6.0, 8.0, 0.05),      # pH typically varies slowly
            'ec': (400.0, 800.0, 2.0),   # EC in μS/cm
            'turbidity': (10.0, 50.0, 0.5),  # NTU
            'temperature': (20.0, 28.0, 0.1) # °C
        }
        
        # Trends to simulate realistic behavior
        self.trends = {
            'ph': random.choice([-0.01, 0, 0.01]),
            'ec': random.choice([-1, 0, 1]),
            'turbidity': random.choice([-0.2, 0, 0.2]),
            'temperature': random.choice([-0.05, 0, 0.05])
        }
        
        # Trend duration in readings
        self.trend_counter = 0
        self.max_trend_duration = 20  # Change trend every ~3 minutes

    def generate_reading(self):
        """Generate a new sensor reading with realistic variations"""
        new_reading = {}
        
        # Update trend occasionally
        self.trend_counter += 1
        if self.trend_counter >= self.max_trend_duration:
            self._update_trends()
            self.trend_counter = 0
        
        for sensor, current_value in self.current_values.items():
            min_val, max_val, variation = self.sensor_ranges[sensor]
            trend = self.trends[sensor]
            
            # Calculate new value with trend and random variation
            base_change = trend + random.uniform(-variation, variation)
            new_value = current_value + base_change
            
            # Keep within realistic bounds
            new_value = max(min_val, min(max_val, new_value))
            
            new_reading[sensor] = round(new_value, 2)
            self.current_values[sensor] = new_value
        
        return new_reading
    
    def _update_trends(self):
        """Update the trends for more realistic sensor behavior"""
        for sensor in self.trends:
            # Occasionally change trend direction
            if random.random() < 0.3:  # 30% chance to change trend
                min_val, max_val, variation = self.sensor_ranges[sensor]
                current = self.current_values[sensor]
                
                # Bias trend towards middle of range
                if current < (min_val + max_val) / 2:
                    self.trends[sensor] = abs(variation) * 0.5
                else:
                    self.trends[sensor] = -abs(variation) * 0.5
                
                # Add some randomness
                self.trends[sensor] += random.uniform(-variation * 0.3, variation * 0.3)

class FirebaseSensorSimulator:
    def __init__(self, credential_path, database_url):
        """
        Initialize Firebase connection
        
        Args:
            credential_path: Path to Firebase service account JSON file
            database_url: Your Firebase Realtime Database URL
        """
        # Initialize Firebase Admin SDK
        cred = credentials.Certificate(credential_path)
        firebase_admin.initialize_app(cred, {
            'databaseURL': database_url
        })
        
        self.sensor_simulator = SensorSimulator()
        self.db_ref = db.reference('/readings')
        
        print("Firebase Sensor Simulator initialized")
        print("Sending data every 10 seconds...")
    
    def send_sensor_data(self):
        """Generate and send sensor data to Firebase"""
        # Get current timestamp in milliseconds (matching your ESP32 format)
        timestamp = int(time.time() * 1000)
        
        # Generate realistic sensor readings
        sensor_data = self.sensor_simulator.generate_reading()
        
        # Send to Firebase under timestamp key
        self.db_ref.child(str(timestamp)).set(sensor_data)
        
        # Print for monitoring
        print(f"[{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}] Sent data:")
        print(f"  Timestamp: {timestamp}")
        print(f"  pH: {sensor_data['ph']}")
        print(f"  EC: {sensor_data['ec']} μS/cm")
        print(f"  Turbidity: {sensor_data['turbidity']} NTU")
        print(f"  Temperature: {sensor_data['temperature']} °C")
        print("-" * 50)
    
    def run(self):
        """Main loop to run the simulator"""
        try:
            while True:
                self.send_sensor_data()
                time.sleep(60)  # Wait 60 seconds
                
        except KeyboardInterrupt:
            print("\nSimulator stopped by user")
        except Exception as e:
            print(f"Error: {e}")

def main():
    # Configuration - UPDATE THESE WITH YOUR FIREBASE CREDENTIALS
    FIREBASE_CREDENTIAL_PATH = "secret_keys\skripsie-357ed-firebase-adminsdk-fbsvc-a428e812d1.json"
    DATABASE_URL = "https://skripsie-357ed-default-rtdb.europe-west1.firebasedatabase.app/"
    
    # Instructions for setting up Firebase:
    # 1. Go to Firebase Console: https://console.firebase.google.com/
    # 2. Create a new project or select existing one
    # 3. Go to Project Settings > Service Accounts
    # 4. Generate new private key and save the JSON file
    # 5. Update FIREBASE_CREDENTIAL_PATH with the path to your JSON file
    # 6. Update DATABASE_URL with your Realtime Database URL
    
    try:
        simulator = FirebaseSensorSimulator(FIREBASE_CREDENTIAL_PATH, DATABASE_URL)
        simulator.run()
    except Exception as e:
        print(f"Failed to initialize simulator: {e}")
        print("Please check your Firebase configuration")

if __name__ == "__main__":
    main()