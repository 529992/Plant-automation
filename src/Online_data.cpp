#include "Online_data.h"
#include <WiFi.h>
#include <HTTPClient.h>

OnlineData::OnlineData(const char* ssid, const char* password, const char* scriptId) 
    : _ssid(ssid), _password(password), _scriptId(scriptId) {}

void OnlineData::connectWiFi() {
    Serial.print("Connecting to WiFi: ");
    Serial.println(_ssid);
    
    WiFi.begin(_ssid, _password);
    
    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 15000; // 15 seconds timeout
    
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection FAILED (Timeout)");
    }
}

bool OnlineData::sendData(const char* timestamp, float temperature, float humidity, const char* waterState, const char* fanStatus, const char* imageLink) {
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }

    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    String url = "https://script.google.com/macros/s/" + String(_scriptId) + "/exec";
    
    // Helper to encode spaces
    auto encode = [](String s) {
        s.replace(" ", "%20");
        return s;
    };

    // Combine parameters into the URL for a GET request
    // Keys: Timestrap, Temprature, Humidity, Watering states, Fan status, Image link
    url += "?Timestrap=" + encode(String(timestamp));
    url += "&Temprature=" + String(temperature, 1);
    url += "&Humidity=" + String(humidity, 1);
    url += "&Watering%20states=" + encode(String(waterState));
    url += "&Fan%20status=" + encode(String(fanStatus));
    url += "&Image%20link=" + encode(String(imageLink));
    
    Serial.print("Requesting URL: ");
    Serial.println(url);

    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.begin(client, url);
    
    int httpResponseCode = http.GET();
    String response = http.getString();
    
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    
    if (httpResponseCode == 200 || httpResponseCode == 201 || httpResponseCode == 302) {
        http.end();
        return true;
    } else {
        Serial.print("Error Response body: ");
        Serial.println(response);
        http.end();
        return false;
    }
}
