#include <Arduino.h>
#include "AM1001.h"

// Pin definitions for ESP32-CAM (Avoid pins used by Flash/PSRAM/Camera if possible)
// GPIO 14 and 15 are often available on the header
const uint8_t HUMIDITY_PIN = 14; 
const uint8_t TEMP_PIN = 15;

AM1001 sensor(HUMIDITY_PIN, TEMP_PIN);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("AM1001 Sensor Reading Started");
    
    sensor.begin();
}

void loop() {
    float humidity = sensor.readHumidity();
    float temperature = sensor.readTemperature();

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print(" %RH\t");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    delay(2000); // Read every 2 seconds
}
