#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include "Online_data.h"

// WiFi Credentials
const char* WIFI_SSID = "VEGAMAIN";
const char* WIFI_PASSWORD = "vegapcpass";
const char* GOOGLE_SCRIPT_ID = "AKfycbwyZRJuiAoDd0sCBEsPzddmVY3jeH4pFpQ3uZATCW59DHTOJvO03QgJ2kIC6h7aKhRngg";

// NTP Server Settings
const char* NTP_SERVER = "pool.ntp.org";
const long  GMT_OFFSET_SEC = 19800; // GMT+5:30
const int   DAYLIGHT_OFFSET_SEC = 0;

// Pin definitions
const uint8_t HUMIDITY_PIN = 33; 
const uint8_t TEMP_PIN = 35;
const uint8_t SOIL_MOISTURE_PIN = 32;
const uint8_t FAN_PIN = 26;
const uint8_t WATER_PIN = 27;

// Global Sensor Data
struct SensorData {
    float humidity;
    float temperature;
    String waterState;
    String fanStatus;
    String imageLink;
} currentData;

OnlineData online(WIFI_SSID, WIFI_PASSWORD, GOOGLE_SCRIPT_ID);

// FreeRTOS Task Handles
TaskHandle_t xSensorTaskHandle = NULL;
TaskHandle_t xUploadTaskHandle = NULL;
TaskHandle_t xWiFiTaskHandle = NULL;

// Function Prototypes
void vSensorTask(void *pvParameters);
void vUploadTask(void *pvParameters);
void vWiFiTask(void *pvParameters);

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Simplified Plant Automation Starting...");
    
    // Initialize data with defaults
    currentData = {0.0f, 0.0f, "off", "on", "Not idintify"};
    
    pinMode(FAN_PIN, OUTPUT);
    pinMode(WATER_PIN, OUTPUT);
    digitalWrite(FAN_PIN, LOW);
    digitalWrite(WATER_PIN, LOW);
    
    // Create Tasks
    xTaskCreatePinnedToCore(vWiFiTask, "WiFiTask", 8192, NULL, 1, &xWiFiTaskHandle, 1);
    xTaskCreatePinnedToCore(vSensorTask, "SensorTask", 2048, NULL, 2, &xSensorTaskHandle, 1);
    xTaskCreatePinnedToCore(vUploadTask, "UploadTask", 8192, NULL, 1, &xUploadTaskHandle, 0);
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}

void vWiFiTask(void *pvParameters) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (true) {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            int retry = 0;
            while (WiFi.status() != WL_CONNECTED && retry < 20) {
                vTaskDelay(pdMS_TO_TICKS(500));
                retry++;
            }
            if (WiFi.status() == WL_CONNECTED) {
                configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void vSensorTask(void *pvParameters) {
    while (true) {
        // No sensors connected - using defaults
        // Temprature - 0, Humidity - 0, Watering states - off, Fan status - on, Image link - Not idintify
        currentData.temperature = 0.0f;
        currentData.humidity = 0.0f;
        currentData.waterState = "off";
        currentData.fanStatus = "on";
        currentData.imageLink = "Not idintify";

        Serial.printf("[SENSOR] H:%.1f, T:%.1f, W:%s, F:%s\n", 
                      currentData.humidity, currentData.temperature, 
                      currentData.waterState.c_str(), 
                      currentData.fanStatus.c_str());

        vTaskDelay(pdMS_TO_TICKS(30000)); // Every 30 seconds
    }
}

void vUploadTask(void *pvParameters) {
    vTaskDelay(pdMS_TO_TICKS(15000));
    while (true) {
        if (WiFi.status() == WL_CONNECTED) {
            struct tm timeinfo;
            char timestamp[32];
            if(!getLocalTime(&timeinfo)) strcpy(timestamp, "No-Sync");
            else strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

            if (online.sendData(timestamp, currentData.temperature, 
                               currentData.humidity, currentData.waterState.c_str(), 
                               currentData.fanStatus.c_str(), currentData.imageLink.c_str())) {
                Serial.println("[UPLOAD] Success");
            } else {
                Serial.println("[UPLOAD] Failed");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(120000)); // Every 2 minutes
    }
}